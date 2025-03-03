#include "BitcaskImpl.hpp"
#include "bitcask/Logger.hpp"
#include <filesystem>
#include <gtest/gtest.h>

using namespace bitcask;
#define DB_PATH "/tmp/test_exist_db/"
class BitCaskImplTest : public ::testing::Test {
protected:
  BitCaskImplTest() {}
  ~BitCaskImplTest() override {}
  void SetUp() override {

    if (std::filesystem::exists(DB_PATH)) {
      std::filesystem::remove_all(DB_PATH);
    }
    std::filesystem::create_directory(DB_PATH);
  }

protected:
  std::unique_ptr<BitcaskImpl> bitcaskPtrExisted;
};

TEST_F(BitCaskImplTest, InitFromExistedDB) {
  bitcaskPtrExisted = std::make_unique<BitcaskImpl>(DB_PATH);
  bitcaskPtrExisted->Put("key1", "value1");
  bitcaskPtrExisted->Put("key2", "value2");
  bitcaskPtrExisted->Put("key3", "value3").get();
  EXPECT_EQ(bitcaskPtrExisted->Get("key1").value_or(""), "value1");
  EXPECT_EQ(bitcaskPtrExisted->Get("key2").value_or(""), "value2");
  EXPECT_EQ(bitcaskPtrExisted->Get("key3").value_or(""), "value3");
  bitcaskPtrExisted.reset();

  BITCASK_LOGGER_INFO("Recreate bitcask");
  bitcaskPtrExisted = std::make_unique<BitcaskImpl>(DB_PATH);
  bitcaskPtrExisted->Put("key1", "value1");
}

int main(int argc, char **argv) {
  logger::init(logger::LOG_LEVEL_FROM_DEBUG,
               [](std::string msg) { std::cout << msg << std::endl; });
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
