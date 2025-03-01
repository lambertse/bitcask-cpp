#pragma once

#include "File.hpp"
#include "bitcask/Type.hpp"
#include <functional>

namespace bitcask {

struct RecordInf {
  uint32_t size;
  Offset keyOffset;
  Offset valueOffset;
};

using RecordFoundCallback =
    std::function<void(const Key &, const Value &, RecordInf)>;

void ReadAllRecordFromFile(file::FileHandler file,
                           const RecordFoundCallback &callback);

RecordInf WriteRecord(file::FileHandler file, const Key &key,
                      const Value &value);

} // namespace bitcask
