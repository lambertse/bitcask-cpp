#pragma once
#include "File.hpp"

namespace bitcask {
class StableFile {
public:
  StableFile(file::FileHandler &file) : _file(file) {}

private:
  file::FileHandler _file;
};
} // namespace bitcask
