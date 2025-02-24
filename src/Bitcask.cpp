#include "bitcask/Bitcask.hpp"

namespace bitcask {
Bitcask *Bitcask::Create(const std::string &db_path) {
  return new Bitcask(db_path);
}

bool Bitcask::Put(const Key &key, const Value &value) {
  // TBD
  return false;
}

std::optional<Value> Bitcask::Get(const Key &key) {
  // TBD
  return std::nullopt;
}

bool Bitcask::Delete(const Key &key) {
  // TBD
  return false;
}

Bitcask::Bitcask(const std::string &db_path) {
  // TBD
}

Bitcask::~Bitcask() {}

} // namespace bitcask
