#pragma once
#include <stdint.h>
namespace bitcask {
struct Hint {
  uint32_t fd;     // file descriptor
  uint32_t offset; // offset
  uint32_t size;   // size
};

} // namespace bitcask
