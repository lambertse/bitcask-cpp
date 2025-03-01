#pragma once

#include "File.hpp"
#include "Record.hpp"
#include "log/Logger.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace bitcask {

template <typename Key, typename Value> class DatabaseFile {
public:
  static file::FileHandler Restore(const std::string &filename,
                                   const RecordFoundCallback &callback) {
    if (!std::filesystem::exists(filename))
      return nullptr;

    file::FileHandler file = new std::fstream();
    if (!file::OpenFile(file, filename,
                        std::ios::binary | std::ios::out | std::ios::in |
                            std::ios::app) ||
        !file->is_open() || !file->good()) {
      BITCASK_LOGGER_ERROR("Failed to open file: {}", filename);
      return nullptr;
    }
    ReadAllRecordFromFile(file, callback);
    return file;
  }
  virtual ~DatabaseFile() {
    if (_file) {
      _file->close();
      delete _file;
    }
  }
  explicit DatabaseFile(file::FileHandler file) : _file(file) {}

protected:
  file::FileHandler _file;
};

} // namespace bitcask
