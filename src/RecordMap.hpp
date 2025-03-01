#pragma once

#include "Hint.hpp"
#include "bitcask/Type.hpp"
#include "libcuckoo/cuckoohash_map.hh"
#include <optional>

namespace bitcask {

class RecordMap {
public:
  RecordMap();
  ~RecordMap();

  void Put(const Key &key, const Hint &value);
  std::optional<Hint> Get(const Key &key);
  void Delete(const Key &key);
  void Clear();

private:
  libcuckoo::cuckoohash_map<Key, Hint> _map;
};
} // namespace bitcask
