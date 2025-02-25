#pragma once

#include "File.hpp"
#include "bitcask/Type.hpp"
#include <memory>

namespace bitcask {

class ActiveFile {
public:
  using Handler = std::shared_ptr<ActiveFile>;
  Handler Create(const std::string &filename);
  // TBD: Restore

  explicit ActiveFile(file::FileHandler &file);
  ~ActiveFile();

  bool Write(const Key &key, const Value &value);

private:
  file::FileHandler _file;
};

} // namespace bitcask
