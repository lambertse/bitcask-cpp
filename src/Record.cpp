#include "Record.hpp"
#include "log/Logger.hpp"
#include <crc32c/crc32c.h>
namespace bitcask {

struct RecordHeader {
  uint32_t crc32;
  uint32_t keySize;
  uint32_t valueSize;
};

bool WriteRecordToFile(file::FileHandler file, const Key &key,
                       const Value &value) {

  return true;
}

void ReadAllRecordFromFile(file::FileHandler file,
                           std::function<void(RecordInf)> callback) {
  // TBD
}

} // namespace bitcask
//
