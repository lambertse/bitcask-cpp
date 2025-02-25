#include "ActiveFile.hpp"

namespace bitcask {
ActiveFile::Handler ActiveFile::Create(const std::string &filename) {}

ActiveFile::ActiveFile(file::FileHandler &file) : _file(file) {}

ActiveFile::~ActiveFile() {}

bool ActiveFile::Write(const Key &key, const Value &value) {
  // TBD
  return false;
}
} // namespace bitcask
