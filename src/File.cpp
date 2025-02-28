#include "File.hpp"
#include "bitcask/Type.hpp"
#include "log/Logger.hpp"

namespace bitcask::file {

bool OpenFile(FileHandler file, const std::string &filename,
              std::ios::openmode mode) {
  file->open(filename, mode);
  return file->is_open();
}

bool ReadFile(FileHandler file, void *buf, size_t size) {
  if (!file->read(static_cast<char *>(buf), size)) {
    return false;
  }
  return true;
}

std::string ReadFile(FileHandler file, size_t size) {
  std::string buf(size, '\0');
  if (!ReadFile(file, &buf[0], size)) {
    return "";
  }
  return buf;
}

std::string ReadFile(FileHandler file, Offset offset, size_t size) {
  file->seekg(offset, std::ios::beg);
  return ReadFile(file, size);
}

long WriteFile(FileHandler &file, const void *buf, size_t size) {
  file->write(static_cast<const char *>(buf), size);
  return static_cast<long>(file->tellp());
}
} // namespace bitcask::file
