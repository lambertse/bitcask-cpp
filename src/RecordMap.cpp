#include "RecordMap.hpp"

namespace bitcask {

RecordMap::RecordMap() = default;

RecordMap::~RecordMap() = default;

std::optional<Hint> RecordMap::Get(const Key &key) {
  Hint hint;
  if (_map.find(key, hint)) {
    return hint;
  }
  return std::nullopt;
}

void RecordMap::Put(const Key &key, const Hint &value) {
  _map.insert_or_assign(key, value);
}

void RecordMap::Delete(const Key &key) { _map.erase(key); }

} // namespace bitcask
