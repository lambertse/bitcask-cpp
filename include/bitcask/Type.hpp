#pragma once
#include <cstdint>
#include <map>
#include <string>
namespace bitcask {
using Key = std::string;
using Value = std::string;
using Offset = uint32_t;

using Map = std::map<Key, Value>;
} // namespace bitcask
