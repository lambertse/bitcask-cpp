#pragma once
#include "DatabaseFile.hpp"
#include "Hint.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {
class StableFile : public DatabaseFile<Key, Hint> {
public:
  StableFile(file::FileHandler file) : DatabaseFile(file) {}
  virtual ~StableFile();

  using Handler = std::shared_ptr<StableFile>;
  static Handler Create(file::FileHandler file) {
    return std::make_shared<StableFile>(file);
  }
  Value Read(const Key &key, Offset offset, size_t size);
};
} // namespace bitcask
