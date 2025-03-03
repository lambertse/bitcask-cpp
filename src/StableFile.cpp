#include "StableFile.hpp"
#include "File.hpp"
#include "bitcask/Logger.hpp"
#include <filesystem>

namespace bitcask {
using namespace file;
StableFile::~StableFile() {
  if (_file) {
    _file->flush();
    _file->close();
    delete _file;
  }
}

file::FileHandler StableFile::Restore(const std::string &filename,
                                      const RecordFoundCallback &callback) {
  if (!std::filesystem::exists(filename))
    return nullptr;

  file::FileHandler file = new std::fstream();
  if (!file::OpenFile(file, filename, std::ios::binary | std::ios::in) ||
      !file->is_open() || !file->good()) {
    BITCASK_LOGGER_ERROR("Failed to open file: {}", filename);
    return nullptr;
  }
  ReadAllRecordFromFile(file, callback);
  BITCASK_LOGGER_INFO("is good {}, is bad {}, is_open {}, position {}",
                      file->good(), file->bad(), file->is_open(),
                      file->tellp());
  return file;
}
StableFile::Handler StableFile::Create(file::FileHandler file) {
  return std::make_shared<StableFile>(file);
}

Value StableFile::Read(const Key &key, Offset offset, size_t size) {
  _file->seekg(offset, std::ios::beg);
  std::string value = file::ReadValueFromFile(_file, size);
  return value;
}
} // namespace bitcask
