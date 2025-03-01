#include "BitcaskImpl.hpp"
#include <filesystem>
#include <memory>

namespace bitcask {
using namespace file;
constexpr auto kActiveFileName = "active.data";

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
  if (std::filesystem::exists(activePath)) {
    RecordFoundCallback callback = [this](RecordInf record) {
      _activeMap.Put(record.key, record.value);
    };
    file = ActiveFile::Restore(activePath, callback);
  } else {
    file = new std::fstream();
    OpenFile(file, activePath,
             std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
  }
  _activeFile = ActiveFile::Create(file);
  return true;
}

bool BitcaskImpl::RestoreStableMap(const std::string &dbDir) {
  std::error_code ec;

  for (const auto &entry : std::filesystem::directory_iterator(dbDir, ec)) {
    if (entry.is_regular_file()) {
      uint32_t fileID = std::stoul(entry.path().filename().string());
      RecordFoundCallback callback = [&](RecordInf record) {
        Hint hint;
        hint.fd = fileID;
        hint.offset = record.valueOffset;
        hint.size = sizeof(record.value);
        _recordMap.Put(record.key, hint);
      };
      FileHandler file = StableFile::Restore(entry.path().string(), callback);
      _stableFiles[fileID] =
          std::dynamic_pointer_cast<StableFile>(StableFile::Create(file));
    }
  }
  return true;
}

BitcaskImpl::BitcaskImpl(const std::string &dbDir) {
  _activeFileID = GetActiveFileID(dbDir);
  RestoreActiveMap(dbDir);
  RestoreStableMap(dbDir);
}

BitcaskImpl::~BitcaskImpl() {}

bool BitcaskImpl::Put(const Key &key, const Value &value) {
  std::lock_guard lock(_mtx);
  // TBD
  return false;
}

std::optional<Value> BitcaskImpl::Get(const Key &key) {
  std::shared_lock lock(_mtx);
  auto recordOpt = _recordMap.Get(key);
  if (!recordOpt.has_value()) {
    return std::nullopt;
  }
  auto record = recordOpt.value();
  if (record.fd == _activeFileID) {
    return _activeMap.Get(key);
  }
  if (_stableFiles.find(record.fd) == _stableFiles.end()) {
    return std::nullopt;
  }

  return _stableFiles[record.fd]->Read(key, record.offset, record.size);
}

bool BitcaskImpl::Delete(const Key &key) {
  // TBD
  return false;
}

} // namespace bitcask
