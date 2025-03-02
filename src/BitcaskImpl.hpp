#pragma once
#include "ActiveFile.hpp"
#include "ActiveMap.hpp"
#include "RecordMap.hpp"
#include "StableFile.hpp"
#include "bitcask/Setting.hpp"
#include "bitcask/Type.hpp"

#include <atomic>
#include <cstdint>
#include <future>
#include <map>
#include <optional>
#include <shared_mutex>
namespace bitcask {

using Writes = std::vector<struct Write>;
class BitcaskImpl {
public:
  explicit BitcaskImpl(const std::string &dbDir,
                       const Setting &setting = Setting{});
  ~BitcaskImpl();

  std::future<void> Put(const Key &key, const Value &value);
  std::optional<Value> Get(const Key &key);
  bool Delete(const Key &key);

private:
  bool RestoreActiveMap();
  bool RestoreStableMap();

  bool CommitWrite(Writes &writes);
  bool CommitWorker();

  uint32_t GetNextFD();

private:
  std::shared_mutex _mtx;
  std::string _dbDir;
  std::atomic_bool _running;
  Setting _setting;

  ActiveMap _activeMap;
  RecordMap _recordMap;

  std::thread _commitProcessor;
  Writes _writes;
  std::map<uint32_t, std::shared_ptr<StableFile>> _stableFiles;
  std::shared_ptr<ActiveFile> _activeFile;
};
} // namespace bitcask
