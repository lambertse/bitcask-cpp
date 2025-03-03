#include "bitcask/Bitcask.hpp"
#include <bitcask/Logger.hpp>
#include <filesystem>
#include <gtest/gtest.h>

using namespace bitcask;

#define DB_PATH "/tmp/bitcask_db/"

class BitCaskTest : public ::testing::Test {
public:
  BitCaskTest() {}
  ~BitCaskTest() override {}
  void SetUp() override {
    if (std::filesystem::exists(DB_PATH)) {
      std::filesystem::remove_all(DB_PATH);
    }
    std::filesystem::create_directory(DB_PATH);
    bitcaskPtr = Bitcask::Create(DB_PATH);
  }
  void TearDown() override {
    std::filesystem::remove_all(DB_PATH);
    if (bitcaskPtr) {
      delete bitcaskPtr;
    }
  }

protected:
  Bitcask *bitcaskPtr = nullptr;
};

TEST_F(BitCaskTest, PutAndGet) {
  bitcaskPtr->PutAsync("key1", "value1");
  bitcaskPtr->PutAsync("key2", "value2");
  bitcaskPtr->Put("key3", "value3");

  EXPECT_EQ(bitcaskPtr->Get("key1").value_or(""), "value1");
  EXPECT_EQ(bitcaskPtr->Get("key2").value_or(""), "value2");
  EXPECT_EQ(bitcaskPtr->Get("key3").value_or(""), "value3");
}
int main(int argc, char *argv[]) {

  logger::init(logger::LOG_LEVEL_FROM_DEBUG,
               [](std::string msg) { std::cout << msg << std::endl; });
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
