//
// Created by user on 2/18/24.
//
#include <gmock/gmock.h>
#include "sjpg_bit_stream.h"
using namespace testing;
using namespace sjpg_codec;

class ABitStream : public Test {
public:
  BitStream s;
};

TEST_F(ABitStream, InitPositionIsZero) { ASSERT_THAT(s.getPosition(), Eq(0)); }

TEST_F(ABitStream, InitSizeIsZero) { ASSERT_THAT(s.getSize(), Eq(0)); }

TEST_F(ABitStream, AppendStringIncreaseSize) {
  s.append("1010");

  ASSERT_THAT(s.getSize(), Eq(4));
}

TEST_F(ABitStream, CanGet1Bit) {
  s.append("1010");
  auto b = s.getBit();

  ASSERT_THAT(b, Eq('1'));
}

TEST_F(ABitStream, GetBitIncreasePosition) {
  s.append("1010");
  s.getBit();

  ASSERT_THAT(s.getPosition(), Eq(1));
}

TEST_F(ABitStream, CanGetNBits) {
  s.append("1010");
  auto b = s.getBitN(2);

  ASSERT_THAT(b, Eq("10"));
}

TEST_F(ABitStream, GetNBitsIncreasePosition) {
  s.append("1010");
  s.getBitN(2);

  ASSERT_THAT(s.getPosition(), Eq(2));
}
