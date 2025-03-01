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
  using Handler = std::shared_ptr<DatabaseFile>;

  static Handler Create(file::FileHandler &file) {
    return std::make_shared<DatabaseFile>(file);
  }
  static file::FileHandler Restore(const std::string &filename,
                                   RecordFoundCallback callback) {
    if (!std::filesystem::exists(filename))
      return nullptr;

    file::FileHandler file = new std::fstream();
    if (!file::OpenFile(file, filename, std::ios::binary | std::ios::in)) {
      BITCASK_LOGGER_ERROR("Failed to open file: {}", filename);
      return nullptr;
    }
    ReadAllRecordFromFile(file, callback);
    return file;
  }
  virtual ~DatabaseFile() {}
  explicit DatabaseFile(file::FileHandler file) : _file(file) {}

protected:
  file::FileHandler _file;
};

} // namespace bitcask
