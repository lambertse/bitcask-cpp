#pragma once
#include <fstream>

namespace bitcask::file {
using FileHandler = std::fstream *;
static bool OpenFile(FileHandler file, const std::string &filename,
                     std::ios::openmode mode);
static bool ReadFile(FileHandler file, void *buf, size_t size);
static std::string ReadFile(FileHandler file, uint32_t offset, size_t size);
static long WriteFile(const void *buf, size_t size, FileHandler &file);
} // namespace bitcask::file
