#pragma once
#include "Setting.hpp"
#include "Type.hpp"
#include <future>

#include <memory>
#include <optional>
namespace bitcask {
class Bitcask {
public:
  static Bitcask *Create(const std::string &dbDir,
                         const Setting &setting = Setting{});
  ~Bitcask();

  void Put(const Key &key, const Value &value);
  std::future<void> PutAsync(const Key &key, const Value &value);

  std::optional<Value> Get(const Key &key);
  bool Delete(const Key &key);

private:
  Bitcask(const std::string &dbDir, const Setting &setting = Setting{});
  Bitcask(const Bitcask &);
  Bitcask &operator=(const Bitcask &);

  std::shared_ptr<class BitcaskImpl> _impl;
};

} // namespace bitcask
