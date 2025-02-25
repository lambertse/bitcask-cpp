#pragma once
#include "File.hpp"
#include "bitcask/Type.hpp"

namespace bitcask {
class StableFile {
public:
  StableFile(file::FileHandler &file);
  ~StableFile();

  Value Read(const Key &key, Offset offset, size_t size);

private:
  file::FileHandler _file;
};
} // namespace bitcask
