#include "BitcaskImpl.hpp"
#include "bitcask/Logger.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <system_error>
#include <thread>

namespace bitcask {
using namespace file;

namespace {
namespace Internal {
std::string ensureEndingSlash(const std::string &path) {
  if (path.empty() || path.back() == '/') {
    return path;
  }
  return path + "/";
}
std::string getActivePathInDir(const std::string &dbDir) {
  std::error_code ec;
  int maxFD = 0;
  for (const auto &entry : std::filesystem::directory_iterator(dbDir, ec)) {
    if (!entry.is_regular_file()) {
      continue;
    }
    try {
      int fd = std::stoul(entry.path().filename().string());
      if (fd > maxFD) {
        maxFD = fd;
      }
    } catch (...) {
      continue;
    }
  }
  return dbDir + std::to_string(maxFD) + ".db";

} // namespace Internal
} // namespace Internal
} // namespace

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

bool BitcaskImpl::RestoreActiveMap() {
  std::error_code ec;
  auto activePath = Internal::getActivePathInDir(_dbDir);
  if (std::filesystem::exists(activePath, ec)) {
    RecordFoundCallback callback = [this](const Key &key, const Value &value,
                                          RecordInf record) {
      Hint hint{getActiveFD(), record.valueOffset, sizeof(value)};
      _recordMap.Put(key, hint);
      _activeMap.Put(key, value);
    };
    FileHandler file = ActiveFile::Restore(activePath, callback);
    _activeFile = ActiveFile::Create(file);
    BITCASK_LOGGER_DEBUG("Restore active file: {}", activePath);
    return true;
  } else {
    _activeFile = ActiveFile::Create(activePath);
    BITCASK_LOGGER_DEBUG("Create new active file: {}", activePath);
  }
  return _activeFile != nullptr;
}

bool BitcaskImpl::RestoreStableMap() {
  std::error_code ec;
  std::string activeFileName =
      std::filesystem::path(Internal::getActivePathInDir(_dbDir))
          .filename()
          .string();

  for (const auto &entry : std::filesystem::directory_iterator(_dbDir, ec)) {
    if (!entry.is_regular_file() ||
        entry.path().filename().string() == activeFileName) {
      continue;
    }

    uint32_t fileID = std::stoul(entry.path().filename().string());
    RecordFoundCallback callback = [&](const Key &key, const Value &value,
                                       RecordInf record) {
      Hint hint{fileID, record.valueOffset, sizeof(value)};
      _recordMap.Put(key, hint);
    };
    FileHandler file = StableFile::Restore(entry.path().string(), callback);
    _stableFiles[fileID] = StableFile::Create(file);
  }
  return true;
}

BitcaskImpl::BitcaskImpl(const std::string &dbDir, const Setting &setting)
    : _running(true), _setting(setting) {
  BITCASK_LOGGER_INFO("Max file size: {}", setting.maxFileSize);
  _dbDir = Internal::ensureEndingSlash(dbDir);

  RestoreActiveMap();
  RestoreStableMap();

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
    BITCASK_LOGGER_DEBUG("Key not found");
    return std::nullopt;
  }
  auto record = recordOpt.value();
  if (record.fd == getActiveFD()) {
    BITCASK_LOGGER_DEBUG("Get from active file");
    return _activeMap.Get(key);
  }
  if (_stableFiles.find(record.fd) == _stableFiles.end()) {
    return std::nullopt;
  }
  BITCASK_LOGGER_DEBUG("Get from stable file");
  return _stableFiles[record.fd]->Read(key, record.offset, record.size);
}

bool BitcaskImpl::Delete(const Key &key) {
  // TBD
  return false;
}

uint32_t BitcaskImpl::getActiveFD() const { return _stableFiles.size(); }

bool BitcaskImpl::CommitWrite(Writes &writes) {
  std::map<uint32_t, std::shared_ptr<StableFile>> newStableFiles;
  std::vector<Hint> records;
  int idx = 0, lastActiveWrite = 0;

  for (const auto &[key, value, promise] : writes) {
    if (value == "37449") {
      BITCASK_LOGGER_INFO("key: {}, value: {}", key, value);
    }
    auto valueOffset = _activeFile->Write(key, value);
    records.push_back(Hint{getActiveFD(), valueOffset, sizeof(value)});
    idx++;
    if (valueOffset < _setting.maxFileSize) {
      continue;
    }
    BITCASK_LOGGER_INFO("Rotate active file");
    // Create new stable file with current active file
    _activeFile->Rotate();
    auto fd = getActiveFD() + newStableFiles.size();
    FileHandler handler = new std::fstream();
    handler->open(_dbDir + std::to_string(fd) + ".db",
                  std::ios::binary | std::ios::in | std::ios::app);
    newStableFiles[fd] = StableFile::Create(handler);
    lastActiveWrite = idx;

    std::string newActiveFileName = _dbDir + std::to_string(fd + 1) + ".db";
    _activeFile = ActiveFile::Create(newActiveFileName);
  }
  {
    std::shared_lock lock(_mtx);
    _stableFiles.insert(newStableFiles.begin(), newStableFiles.end());
  }
  // If have to move to new file, delete current active map
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
