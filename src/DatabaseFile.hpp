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
  virtual ~DatabaseFile() {
    if (_file) {
      _file->flush();
      _file->close();
      delete _file;
    }
  }
  explicit DatabaseFile(file::FileHandler file) : _file(file) {}

protected:
  file::FileHandler _file;
};

} // namespace bitcask
