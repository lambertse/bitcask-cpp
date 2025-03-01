#include "BitcaskImpl.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

using namespace bitcask;

class BitCaskImplTest : public ::testing::Test {
protected:
  BitCaskImplTest() {}
  ~BitCaskImplTest() override {}
  void SetUp() override {
    std::string dbDir = "/tmp/test_db/";
    if (!std::filesystem::exists(dbDir)) {
      std::filesystem::create_directory(dbDir);
    }
    bitcaskPtr = std::make_unique<BitcaskImpl>(dbDir);
  }
  /*  void TearDown() override { std::filesystem::remove_all("/tmp/test_db/"); }
   */

protected:
  std::unique_ptr<BitcaskImpl> bitcaskPtr;
};

TEST_F(BitCaskImplTest, InitFromFolder) {
  bitcaskPtr->Put("key1", "value1");
  bitcaskPtr->Put("key2", "value2");
  bitcaskPtr->Put("key3", "value3");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(bitcaskPtr->Get("key1").value_or(""), "value1");
  EXPECT_EQ(bitcaskPtr->Get("key2").value_or(""), "value2");
  EXPECT_EQ(bitcaskPtr->Get("key3").value_or(""), "value3");
}

int main() {
  logger::init(logger::LOG_LEVEL_FROM_INFO,
               [](std::string msg) { std::cout << msg << std::endl; });
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
