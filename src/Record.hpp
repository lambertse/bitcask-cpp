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

void ReadAllRecordFromFile(file::FileHandler file,
                           std::function<void(RecordInf)> callback);

bool WriteRecordToFile(file::FileHandler file, const Key &key,
                       const Value &value);

} // namespace bitcask
