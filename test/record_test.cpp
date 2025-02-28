#include "Record.hpp"
#include <gtest/gtest.h>

#include "Record.hpp"
#include "log/Logger.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

using namespace bitcask;
using namespace file;

class RecordTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::remove("/tmp/testfile.dat");
    testFile = new std::fstream();
    testFile->open("/tmp/testfile.dat", std::ios::binary | std::ios::out |
                                            std::ios::in | std::ios::trunc);
    ASSERT_TRUE(testFile->is_open());
  }

  void TearDown() override {
    // Close and remove the temporary file
    testFile->close();
    std::remove("/tmp/testfile.dat");
  }

  FileHandler testFile;
};

// TEST_F(RecordTest, WriteRecordTest) {
//   Key key = "testKey";
//   Value value = "testValue";
//   RecordInf recordInf = WriteRecord(testFile, key, value);
//
//   ASSERT_EQ(recordInf.size,
//             sizeof(RecordHeader) + key.size() + value.size());
//   ASSERT_EQ(recordInf.keyOffset, sizeof(RecordHeader));
//   ASSERT_EQ(recordInf.valueOffset, sizeof(RecordHeader) +
//   key.size());
// }

TEST_F(RecordTest, ReadAllRecordFromFileTest) {
  Key key = "anhhai";
  Value value = "trile";
  RecordInf inf1 = WriteRecord(testFile, key, value);

  key = "anhba";
  value = "khoale";
  RecordInf inf2 = WriteRecord(testFile, key, value);

  key = "abcdefgh";
  value = "111111111111111";
  WriteRecord(testFile, key, value);
  testFile->seekg(0, std::ios::beg);

  std::vector<RecordInf> records;
  ReadAllRecordFromFile(testFile,
                        [&](RecordInf record) { records.push_back(record); });
}

int main(int argc, char **argv) {
  logger::init(logger::LOG_LEVEL_FROM_INFO,
               [](std::string msg) { std::cout << msg << std::endl; });
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
