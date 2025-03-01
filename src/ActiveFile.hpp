#pragma once
#include "DatabaseFile.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {

class ActiveFile : public DatabaseFile<Key, Value> {
public:
  virtual ~ActiveFile();

  bool Write(const Key &key, const Value &value);
};

} // namespace bitcask
