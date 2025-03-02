#include "BitcaskImpl.hpp"
#include "log/Logger.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <system_error>
#include <thread>

namespace bitcask {
using namespace file;
constexpr auto kActiveFileName = "active.data";

struct Write {
  Key key;
  Value value;
  std::promise<void> promise;
  Write(Key key, Value value) : key(key), value(value) {}
  Write(Write &&write)
      : key(std::move(write.key)), value(std::move(write.value)),
        promise(std::move(write.promise)) {}
};

using Writes = std::vector<Write>;
uint32_t BitcaskImpl::GetActiveFileID(const std::string &dbDir) {
  uint32_t maxID = 0;
  std::error_code ec;
  for (const auto &entry : std::filesystem::directory_iterator(dbDir, ec)) {
    if (entry.is_regular_file()) {
      uint32_t fileID = std::stoul(entry.path().filename().string());
      if (fileID > maxID) {
        maxID = fileID;
      }
    }
  }
  return maxID;
}

bool BitcaskImpl::RestoreActiveMap(const std::string &activePath) {
  FileHandler file = nullptr;
  std::error_code ec;
  if (std::filesystem::exists(activePath, ec)) {
    RecordFoundCallback callback = [this](const Key &key, const Value &value,
                                          RecordInf record) {
      Hint hint;
      hint.fd = GetActiveFD();
      hint.offset = record.valueOffset;
      hint.size = sizeof(key);
      _recordMap.Put(key, hint);
      _activeMap.Put(key, value);
    };
    file = ActiveFile::Restore(activePath, callback);
    BITCASK_LOGGER_INFO("Restore active file: {}", activePath);
  } else {
    _activeFile = ActiveFile::Create(activePath);
    BITCASK_LOGGER_INFO("Create new active file: {}", activePath);
    return _activeFile != nullptr;
  }
  _activeFile = ActiveFile::Create(file);
  return true;
}

bool BitcaskImpl::RestoreStableMap(const std::string &dbDir) {
  std::error_code ec;

  for (const auto &entry : std::filesystem::directory_iterator(dbDir, ec)) {
    if (entry.is_regular_file()) {
      uint32_t fileID = std::stoul(entry.path().filename().string());
      if (fileID == GetActiveFD()) {
        continue;
      }
      RecordFoundCallback callback = [&](const Key &key, const Value &value,
                                         RecordInf record) {
        Hint hint;
        hint.fd = fileID;
        hint.offset = record.valueOffset;
        hint.size = sizeof(value);
        _recordMap.Put(key, hint);
      };
      FileHandler file = StableFile::Restore(entry.path().string(), callback);
      _stableFiles[fileID] = StableFile::Create(file);
    }
  }
  return true;
}

BitcaskImpl::BitcaskImpl(const std::string &dbDir, const Setting &setting)
    : _running(true), _dbDir(dbDir), _setting(setting) {

  RestoreActiveMap(dbDir + std::to_string(GetActiveFD()) + ".db");
  RestoreStableMap(dbDir);

  _commitProcessor = std::thread(std::bind(&BitcaskImpl::CommitWorker, this));
}

BitcaskImpl::~BitcaskImpl() {
  _running = false;
  if (_commitProcessor.joinable())
    _commitProcessor.join();
}

std::future<void> BitcaskImpl::Put(const Key &key, const Value &value) {
  std::lock_guard lock(_mtx);
  _writes.emplace_back(Write{key, value});
  return _writes.back().promise.get_future();
}

std::optional<Value> BitcaskImpl::Get(const Key &key) {
  std::shared_lock lock(_mtx);
  auto recordOpt = _recordMap.Get(key);
  if (!recordOpt.has_value()) {
    BITCASK_LOGGER_INFO("Key not found");
    return std::nullopt;
  }
  auto record = recordOpt.value();
  if (record.fd == GetActiveFD()) {
    BITCASK_LOGGER_INFO("Get from active file");
    return _activeMap.Get(key);
  }
  if (_stableFiles.find(record.fd) == _stableFiles.end()) {
    return std::nullopt;
  }
  BITCASK_LOGGER_INFO("Get from stable file");
  return _stableFiles[record.fd]->Read(key, record.offset, record.size);
}

bool BitcaskImpl::Delete(const Key &key) {
  // TBD
  return false;
}

uint32_t BitcaskImpl::GetActiveFD() { return _stableFiles.size(); }

bool BitcaskImpl::CommitWrite(Writes &writes) {
  auto activeFile = _activeFile;
  std::map<uint32_t, std::shared_ptr<StableFile>> newStableFiles;
  std::vector<Hint> records;
  int idx = 0, lastActiveWrite = 0;

  for (const auto &[key, value, promise] : writes) {
    auto valueOffset = activeFile->Write(key, value);
    records.push_back(Hint{GetActiveFD(), valueOffset, sizeof(value)});
    idx++;
    if (valueOffset < _setting.maxFileSize) {
      continue;
    }
    BITCASK_LOGGER_INFO("Rotate active file");
    // Create new stable file with current active file
    auto handler = activeFile->Rotate();
    auto fd = GetActiveFD() + newStableFiles.size();
    newStableFiles[fd] = StableFile::Create(handler);
    lastActiveWrite = idx;

    std::string newActiveFileName = _dbDir + std::to_string(fd) + ".db";
    activeFile = ActiveFile::Create(newActiveFileName);
  }
  {
    std::shared_lock lock(_mtx);
    _stableFiles.insert(newStableFiles.begin(), newStableFiles.end());
  }
  if (!newStableFiles.empty()) {
    _activeMap.Clear();
  }
  for (int i = 0; i < writes.size(); i++) {
    auto &[key, value, promise] = writes[i];
    if (i >= lastActiveWrite) {
      _activeMap.Put(key, value);
    }
    _recordMap.Put(key, records[i]);
    promise.set_value();
  }
  return true;
}
bool BitcaskImpl::CommitWorker() {
  while (true) {
    Writes writes;
    {
      std::shared_lock lock(_mtx);
      swap(writes, _writes);
    }
    if (!CommitWrite(writes)) {
      BITCASK_LOGGER_ERROR("Commit write failed");
    }
    if (!_running) {
      break;
    }
    std::this_thread::yield();
  }
  return true;
}

} // namespace bitcask
