#pragma once
#include "DatabaseFile.hpp"
#include "Record.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {

class ActiveFile : public DatabaseFile<Key, Value> {
public:
  ActiveFile(file::FileHandler file) : DatabaseFile(file) {}
  virtual ~ActiveFile();

  using Handler = std::shared_ptr<ActiveFile>;
  static Handler Create(file::FileHandler file) {
    return std::make_shared<ActiveFile>(file);
  }
  static Handler Create(const std::string &path) {
    file::FileHandler file = new std::fstream();
    if (!file::OpenFile(file, path,
                        std::ios::binary | std::ios::out | std::ios::in |
                            std::ios::trunc)) {
      BITCASK_LOGGER_ERROR("Cannot create active file: {}", path);
      return nullptr;
    }
    return Create(file);
  }

  Offset Write(const Key &key, const Value &value);
  file::FileHandler Rotate();
};

} // namespace bitcask
