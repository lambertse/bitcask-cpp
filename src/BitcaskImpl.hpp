#pragma once
#include "ActiveFile.hpp"
#include "ActiveMap.hpp"
#include "RecordMap.hpp"
#include "StableFile.hpp"
#include "bitcask/Type.hpp"

#include <map>
#include <optional>
#include <shared_mutex>
namespace bitcask {
class BitcaskImpl {
public:
  BitcaskImpl(const std::string &dbDir);
  ~BitcaskImpl();

  bool Put(const Key &key, const Value &value);
  std::optional<Value> Get(const Key &key);
  bool Delete(const Key &key);

private:
  bool RestoreActiveMap(const std::string &activePath);
  bool RestoreStableMap(const std::string &dbDir);
  uint32_t GetActiveFileID(const std::string &dbDir);

private:
  std::shared_mutex _mtx;
  uint32_t _activeFileID;

  ActiveMap _activeMap;
  RecordMap _recordMap;

  std::map<uint32_t, std::shared_ptr<StableFile>> _stableFiles;
  ActiveFile::Handler _activeFile;
};
} // namespace bitcask
