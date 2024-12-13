#include "gtest/gtest.h"
#include "wolf/sensirion/crc.hpp"

TEST(SensirionCrc, Uint16Beef) {
  const std::uint8_t crc = wolf::sensirion::crc::from_uint16(0xbeef);
  EXPECT_EQ(crc, 0x92);
}

TEST(SensirionCrc, Uint16Value4) {
  const std::uint8_t crc = wolf::sensirion::crc::from_uint16(4);
  EXPECT_EQ(crc, 0x45);
}

TEST(SensirionCrc, Carray) {
  std::uint8_t test[] = {0xbe, 0xef};
  std::size_t size = sizeof(test);
  EXPECT_EQ(size, 2);
  const std::uint8_t crc = wolf::sensirion::crc::from_carray(test, size);
  EXPECT_EQ(crc, 0x92);
}
