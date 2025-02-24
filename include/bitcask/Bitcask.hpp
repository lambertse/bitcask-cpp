#pragma once
#include "Type.hpp"
#include <memory>
#include <optional>
namespace bitcask {
class Bitcask {
public:
  static Bitcask *Create(const std::string &dbDir);
  ~Bitcask();

  bool Put(const Key &key, const Value &value);
  std::optional<Value> Get(const Key &key);
  bool Delete(const Key &key);

private:
  Bitcask(const std::string &dbDir);
  Bitcask(const Bitcask &);
  Bitcask &operator=(const Bitcask &);

  std::shared_ptr<class BitcaskImpl> impl_;
};

} // namespace bitcask
