#pragma once
#include "DatabaseFile.hpp"
#include "Hint.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {
class StableFile : public DatabaseFile<Key, Hint> {
public:
  using Handler = std::shared_ptr<StableFile>;
  StableFile(file::FileHandler file) : DatabaseFile(file) {}
  virtual ~StableFile();

  static file::FileHandler Restore(const std::string &filename,
                                   const RecordFoundCallback &callback);
  static Handler Create(file::FileHandler file);
  Value Read(const Key &key, Offset offset, size_t size);
};
} // namespace bitcask
