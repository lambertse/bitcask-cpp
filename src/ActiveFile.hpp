#pragma once
#include "DatabaseFile.hpp"
#include "Record.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {

class ActiveFile : public DatabaseFile<Key, Value> {
public:
  using Handler = std::shared_ptr<ActiveFile>;

  ActiveFile(file::FileHandler file) : DatabaseFile(file) {}
  virtual ~ActiveFile();
  static file::FileHandler Restore(const std::string &filename,
                                   const RecordFoundCallback &callback);

  static Handler Create(file::FileHandler file);
  static Handler Create(const std::string &path);
  Offset Write(const Key &key, const Value &value);

  file::FileHandler Rotate();
};

} // namespace bitcask
