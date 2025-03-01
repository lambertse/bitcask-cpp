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
  RecordInf Write(const Key &key, const Value &value);
};

} // namespace bitcask
