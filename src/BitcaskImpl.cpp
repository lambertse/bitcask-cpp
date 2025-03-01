#include "BitcaskImpl.hpp"
#include "log/Logger.hpp"
#include <filesystem>
#include <memory>
#include <system_error>

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
  std::error_code ec;
  if (std::filesystem::exists(activePath, ec)) {
    RecordFoundCallback callback = [this](const Key &key, const Value &value,
                                          RecordInf record) {
      Hint hint;
      hint.fd = _activeFileID;
      hint.offset = record.valueOffset;
      hint.size = sizeof(key);
      _recordMap.Put(key, hint);
      _activeMap.Put(key, value);
    };
    file = ActiveFile::Restore(activePath, callback);
    BITCASK_LOGGER_INFO("Restore active file: {}", activePath);
  } else {
    file = new std::fstream();
    if (!OpenFile(file, activePath,
                  std::ios::binary | std::ios::out | std::ios::in |
                      std::ios::trunc)) {
      BITCASK_LOGGER_ERROR("Cannot create active file: {}", activePath);
      return false;
    }
    BITCASK_LOGGER_INFO("Create new active file: {}", activePath);
  }
  _activeFile = ActiveFile::Create(file);
  return true;
}

bool BitcaskImpl::RestoreStableMap(const std::string &dbDir) {
  std::error_code ec;

  for (const auto &entry : std::filesystem::directory_iterator(dbDir, ec)) {
    if (entry.is_regular_file()) {
      uint32_t fileID = std::stoul(entry.path().filename().string());
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

BitcaskImpl::BitcaskImpl(const std::string &dbDir) {
  _activeFileID = GetActiveFileID(dbDir);
  RestoreActiveMap(dbDir + std::to_string(_activeFileID) + ".db");
  RestoreStableMap(dbDir);
}

BitcaskImpl::~BitcaskImpl() {}

bool BitcaskImpl::Put(const Key &key, const Value &value) {
  std::lock_guard lock(_mtx);
  _activeMap.Put(key, value);
  if (!_activeFile) {
    BITCASK_LOGGER_ERROR("Active file is not initialized");
    return false;
  }
  auto recordInf = _activeFile->Write(key, value);
  _recordMap.Put(key, {_activeFileID, recordInf.valueOffset, sizeof(value)});

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
