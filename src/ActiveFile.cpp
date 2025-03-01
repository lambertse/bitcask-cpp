#include "ActiveFile.hpp"
#include "Record.hpp"

namespace bitcask {
using namespace file;
ActiveFile::~ActiveFile() {
  // TBD
}

RecordInf ActiveFile::Write(const Key &key, const Value &value) {
  return WriteRecord(_file, key, value);
}
} // namespace bitcask
