#pragma once
#include <map>
#include <string>
namespace bitcask {
using Key = std::string;
using Value = std::string;

using Map = std::map<Key, Value>;
} // namespace bitcask
