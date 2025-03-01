#include "ActiveFile.hpp"
#include "Record.hpp"

namespace bitcask {
using namespace file;
ActiveFile::~ActiveFile() {}

Offset ActiveFile::Write(const Key &key, const Value &value) {
  return WriteRecord(_file, key, value).valueOffset;
}

file::FileHandler ActiveFile::Rotate() {
  _file->flush();
  _file->close();
  file::FileHandler newFile = new std::fstream();
  swap(_file, newFile);
  return newFile;
}
} // namespace bitcask
