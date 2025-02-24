#pragma once
#include "bitcask/Type.hpp"

#include <optional>
namespace bitcask {
class BitcaskImpl {
public:
  BitcaskImpl(const std::string &dbDir);
  ~BitcaskImpl();

  bool Put(const Key &key, const Value &value);
  std::optional<Value> Get(const Key &key);
  bool Delete(const Key &key);
};
} // namespace bitcask
