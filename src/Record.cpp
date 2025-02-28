#include "Record.hpp"
#include "CRC32.hpp"
#include "File.hpp"
#include "log/Logger.hpp"
namespace bitcask {

// Record structure:
//
// | crc32 | keySize | valueSize | key | value |

struct RecordHeader {
  uint32_t crc32;
  uint32_t keySize;
  uint32_t valueSize;
};

namespace {
namespace internal {
uint32_t getCRC32(RecordHeader &header, const Key &key, const Value &value);
size_t fillHeader(RecordHeader &header, const Key &key, const Value &value);
} // namespace internal
} // namespace

RecordInf WriteRecord(file::FileHandler file, const Key &key,
                      const Value &value) {
  RecordHeader header;
  size_t size = internal::fillHeader(header, key, value);

  file::WriteFile(file, &header, sizeof(header));

  RecordInf recordInf;
  recordInf.keyOffset = file->tellp();
  file::WriteFile(file, key.c_str(), key.size());
  recordInf.valueOffset = file->tellp();
  file::WriteFile(file, value.c_str(), value.size());
  recordInf.size = size;

  BITCASK_LOGGER_DEBUG("Write: Header: crc32: {}, keySize: {}, valueSize: {}",
                       header.crc32, header.keySize, header.valueSize);
  BITCASK_LOGGER_DEBUG("Write: keyOffset: {}, valueOffset: {}, size: {}",
                       recordInf.keyOffset, recordInf.valueOffset,
                       recordInf.size);

  return recordInf;
}

bool ReadRecord(file::FileHandler file, RecordInf &record) {
  RecordHeader header;
  if (!file::ReadFile(file, &header, sizeof(header)))
    return false;
  record.keyOffset = file->tellg();
  record.key = file::ReadFile(file, record.keyOffset, header.keySize);
  record.valueOffset = record.keyOffset + header.keySize;
  record.value = file::ReadFile(file, record.valueOffset, header.valueSize);
  record.size = sizeof(header) + header.keySize + header.valueSize;

  if (header.crc32 != internal::getCRC32(header, record.key, record.value)) {
    BITCASK_LOGGER_ERROR("CRC32 not match");
    return false;
  }
  return true;
}

void ReadAllRecordFromFile(file::FileHandler file,
                           std::function<void(RecordInf)> callback) {
  RecordInf info;
  uint32_t count = 0;

  file->seekg(0, std::ios::beg);
  while (ReadRecord(file, info)) {
    callback(info);
    count++;
    BITCASK_LOGGER_INFO("Read {}, key: {}, value: {}", count, info.key,
                        info.value);
  }
  if (!file->eof()) {
    BITCASK_LOGGER_ERROR("Read file error");
  }
  file->close();
}

namespace {
namespace internal {
uint32_t getCRC32(RecordHeader &header, const Key &key, const Value &value) {
  uint32_t crc = 0;
  crc = crc32(crc, reinterpret_cast<const uint8_t *>(&header.keySize),
              sizeof(header.keySize));
  crc = crc32(crc, reinterpret_cast<const uint8_t *>(&header.valueSize),
              sizeof(header.valueSize));
  crc = crc32(crc, key);
  crc = crc32(crc, value);
  return crc;
}
size_t fillHeader(RecordHeader &header, const Key &key, const Value &value) {
  header.keySize = key.size();
  header.valueSize = value.size();
  header.crc32 = getCRC32(header, key, value);
  return sizeof(header) + header.keySize + header.valueSize;
  ;
}
} // namespace internal
} // namespace
} // namespace bitcask
