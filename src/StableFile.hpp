#pragma once
#include "DatabaseFile.hpp"
#include "Hint.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {
class StableFile : public DatabaseFile<Key, Hint> {
public:
  virtual ~StableFile();

  Value Read(const Key &key, Offset offset, size_t size);
};
} // namespace bitcask
