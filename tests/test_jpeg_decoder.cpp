//
// Created by user on 2/7/24.
//
#include <gmock/gmock.h>
#include "sjpg_decoder.h"

using namespace testing;

class AJPEGDecoder : public Test {
public:
  std::string filepath =
      "./resources/lenna.jpg";
  sjpg_codec::Decoder decoder;
};

TEST_F(AJPEGDecoder, OpenFileReturnOKIfFileExists) {
  auto ok = decoder.open(filepath);

  ASSERT_TRUE(ok);
}

TEST_F(AJPEGDecoder, OpenFileReturnFalseIfFileDoesNotExist) {
  auto ok = decoder.open("nonexistent.jpg");

  ASSERT_FALSE(ok);
}

TEST_F(AJPEGDecoder, CanGetCurrentOpenedFile) {
  decoder.open(filepath);
  ASSERT_EQ(filepath, decoder.getFilePath());
}

TEST_F(AJPEGDecoder, DecodeFailedIfFileNotOpened) {
  auto ret = decoder.decodeImageFile();

  ASSERT_THAT(ret, Eq(-1));
}

TEST_F(AJPEGDecoder, CanDecodeFileIfOpened) {
  decoder.open(filepath);
  auto ret = decoder.decodeImageFile();

  ASSERT_THAT(ret, Eq(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetSOIAndEOISegment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  ASSERT_THAT(decoder.getSOISegment().file_pos, Ne(0));
  ASSERT_THAT(decoder.getEOISegment().file_pos, Ne(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetAPP0Segment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  ASSERT_THAT(decoder.getAPP0Segment().file_pos, Ne(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetCOMSegment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  ASSERT_THAT(decoder.getCOMSegment().file_pos, Ne(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetDQTSegment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  auto dqt_count = decoder.getDQTSegments().size();

  ASSERT_THAT(dqt_count, Gt(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetSOF0Segment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  auto sof0_count = decoder.getSOF0Segment();

  ASSERT_THAT(sof0_count.file_pos, Gt(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetDHTSegment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  auto dht_segments = decoder.getDHTSegments();

  ASSERT_THAT(dht_segments.size(), Gt(0));
  ASSERT_THAT(dht_segments[0].file_pos, Gt(0));
}

TEST_F(AJPEGDecoder, DecodeFileGetSOSSegment) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  auto sos = decoder.getSOSSegment();

  ASSERT_THAT(sos.file_pos, Gt(0));
}

TEST_F(AJPEGDecoder, CanGetDeocdecChannelDatas) {
  decoder.open(filepath);
  decoder.decodeImageFile();

  auto y_data = decoder.getYDecodedData();
  auto u_data = decoder.getUDecodedData();
  auto v_data = decoder.getVDecodedData();

  ASSERT_THAT(y_data.size(), Gt(0));
  ASSERT_THAT(u_data.size(), Gt(0));
  ASSERT_THAT(v_data.size(), Gt(0));
}