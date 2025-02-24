#include "BitcaskImpl.hpp"

namespace bitcask {
BitcaskImpl::BitcaskImpl(const std::string &dbDir) {
  // TBD
}

BitcaskImpl::~BitcaskImpl() {}

bool BitcaskImpl::Put(const Key &key, const Value &value) {
  // TBD
}

std::optional<Value> BitcaskImpl::Get(const Key &key) {
  // TBD
  return std::nullopt;
}

bool BitcaskImpl::Delete(const Key &key) {
  // TBD
  return false;
}

} // namespace bitcask
