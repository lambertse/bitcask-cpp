#include "StableFile.hpp"

namespace bitcask {
StableFile::StableFile(file::FileHandler &file) : _file(file) {}

StableFile::~StableFile() {}

Value StableFile::Read(const Key &key, Offset offset, size_t size) {
  _file->seekg(offset, std::ios::beg);
  Value value;
  if (file::ReadFile(_file, &value, size)) {
    return value;
  }
  return "";
}
} // namespace bitcask
