//
// Created by user on 2/7/24.
//
#include "sjpg_segments.h"
#include <fstream>
#include <numeric>
#include <vector>

#include <gmock/gmock.h>

using namespace testing;

namespace sjpg_codec {
using namespace segments;

class Decoder {
public:
  bool open(const std::string &filepath) {
    in_file_.open(filepath, std::ios::in | std::ios::binary);
    if (!in_file_.is_open() || !in_file_.good()) {
      LOG_ERROR("Failed to open file %s\n", filepath.c_str());
      return false;
    }

    filepath_ = filepath;
    return true;
  }

  int decodeImageFile() {
    if (!isFileOpened()) {
      LOG_ERROR("File not opened, you cannot decode it\n");
      return -1;
    }

    for (;;) {
      if (in_file_.eof()) {
        LOG_INFO("End of file\n");
        break;
      }

      auto b = readByte();
      if (b == 0xFF) {
        b = readByte();
        parseSegment(b);
      }
    }

    return 0;
  }

  const std::string &getFilePath() const { return filepath_; }
  const SOISegment &getSOISegment() const { return soi_; }
  const APP0Segment &getAPP0Segment() const { return app0_; }
  const COMSegment &getCOMSegment() const { return com_; }
  const std::vector<DQTSegment> &getDQTSegments() const { return dqt_; }
  const SOF0Segment &getSOF0Segment() const { return sof0_; }
  const std::vector<DHTSegment> &getDHTSegments() const { return dht_; }
  const SOSSegment &getSOSSegment() const { return sos_; }
  const EOISegment &getEOISegment() const { return eoi_; }

private:
  bool isFileOpened() { return in_file_.is_open(); }

  void parseSOISegment() {
    soi_.file_pos = in_file_.tellg();
    soi_.print();
  }
  void parseSegment(uint8_t b) {
    switch (b) {
    case SOISegment::marker:
      parseSOISegment();
      break;
    case APP0Segment::marker:
      parseAPP0Segment();
      break;
    case COMSegment::marker:
      parseCOMSegment();
      break;
    case DQTSegment::marker:
      parseDQTSegment();
      break;
    case SOF0Segment::marker:
      parseSOF0Segment();
      break;
    case DHTSegment::marker:
      parseDHTSegment();
      break;
    case SOSSegment::marker:
      parseSOSSegment();
      break;
    case EOISegment::marker:
      parseEOISegment();
      break;
    default:
      break;
    }
  }
  void parseAPP0Segment() {
    app0_.file_pos = in_file_.tellg();

    auto length = read2BytesBigEndian();
    app0_.length = length;
    in_file_.read(app0_.identifier, 5);

    app0_.major_version = readByte();
    app0_.minor_version = readByte();
    app0_.pixel_units = readByte();
    app0_.x_density = read2BytesBigEndian();
    app0_.y_density = read2BytesBigEndian();
    app0_.thumbnail_width = readByte();
    app0_.thumbnail_height = readByte();

    auto thumbnail_data_length =
        3 * app0_.thumbnail_width * app0_.thumbnail_height;
    if (thumbnail_data_length != 0) {
      app0_.thumbnail_data.resize(thumbnail_data_length);
      in_file_.read(reinterpret_cast<char *>(app0_.thumbnail_data.data()),
                    thumbnail_data_length);
    }

    app0_.print();
  }
  void parseEOISegment() {
    eoi_.file_pos = in_file_.tellg();
    eoi_.print();
  }
  void parseCOMSegment() {
    com_.file_pos = in_file_.tellg();
    auto length = read2BytesBigEndian();
    com_.length = length;
    com_.comment.resize(length - 2);
    in_file_.read(reinterpret_cast<char *>(com_.comment.data()), length - 2);
    com_.print();
  }

  void parseDQTSegment() {
    DQTSegment dqt;
    dqt.file_pos = in_file_.tellg();
    dqt.length = read2BytesBigEndian();
    auto tmp = readByte();
    dqt.precision = tmp >> 4;
    dqt.q_table_id = tmp & 0x0F;

    const static int q_table_size = 64;
    dqt.q_table.resize(q_table_size);
    in_file_.read(reinterpret_cast<char *>(dqt.q_table.data()), q_table_size);

    dqt_.push_back(dqt);

    dqt.print();
  }

  void parseSOF0Segment() {
    sof0_.file_pos = in_file_.tellg();
    sof0_.length = read2BytesBigEndian();
    sof0_.precision = readByte();
    sof0_.height = read2BytesBigEndian();
    sof0_.width = read2BytesBigEndian();
    sof0_.num_components = readByte();

    for (int i = 0; i < sof0_.num_components; i++) {
      auto b0 = readByte();
      auto b1 = readByte();
      auto b2 = readByte();

      sof0_.component_id.push_back(b0);
      sof0_.sampling_factor.push_back(b1);
      sof0_.quantization_table_id.push_back(b2);
    }

    sof0_.print();
  }

  void parseDHTSegment() {
    DHTSegment dht;
    dht.file_pos = in_file_.tellg();
    dht.length = read2BytesBigEndian();
    auto b = readByte();
    dht.ac_or_dc = b >> 4;
    dht.table_id = b & 0x0F;

    const static int huffman_table_size = 16;
    dht.symbol_counts.resize(huffman_table_size);
    in_file_.read(reinterpret_cast<char *>(dht.symbol_counts.data()),
                  huffman_table_size);

    auto num_symbols =
        std::accumulate(dht.symbol_counts.begin(), dht.symbol_counts.end(), 0);
    dht.symbols.resize(num_symbols);
    in_file_.read(reinterpret_cast<char *>(dht.symbols.data()), num_symbols);

    dht.print();
    dht_.push_back(dht);
  }

  void parseSOSSegment() {
    sos_.file_pos = in_file_.tellg();
    sos_.length = read2BytesBigEndian();
    sos_.num_components = readByte();
    for (int i = 0; i < sos_.num_components; i++) {
      auto b0 = readByte();
      auto b1 = readByte();
      sos_.component_id.push_back(b0);

      auto huffman_table_id_dc = b1 >> 4;
      auto huffman_table_id_ac = b1 & 0x0F;
      sos_.huffman_table_id_ac.push_back(huffman_table_id_ac);
      sos_.huffman_table_id_dc.push_back(huffman_table_id_dc);
    }

    // skip 3 bytes
    for (int i = 0; i < 3; i++) {
      readByte();
    }
    sos_.print();
  }

  uint16_t read2BytesBigEndian() {
    uint8_t byte1;
    uint8_t byte2;
    in_file_.read(reinterpret_cast<char *>(&byte1), sizeof(byte1));
    in_file_.read(reinterpret_cast<char *>(&byte2), sizeof(byte2));
    return (byte1 << 8) | byte2;
  }
  uint8_t readByte() {
    uint8_t value;
    in_file_.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
  }

  std::ifstream in_file_;
  std::string filepath_;

  // header segments
  SOISegment soi_;
  APP0Segment app0_;
  COMSegment com_;
  std::vector<DQTSegment> dqt_;
  SOF0Segment sof0_;
  std::vector<DHTSegment> dht_;
  SOSSegment sos_;
  EOISegment eoi_;
};
} // namespace sjpg_codec

class AJPEGDecoder : public Test {
public:
  std::string filepath =
      "/Users/user/Documents/develop/simple_jpeg_codec/resources/lenna.jpg";
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