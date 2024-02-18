//
// Created by user on 2/18/24.
//

#ifndef SIMPLE_JPEG_CODEC_SJPG_BIT_STREAM_H
#define SIMPLE_JPEG_CODEC_SJPG_BIT_STREAM_H
#include <string>
namespace sjpg_codec {
class BitStream {
public:
  void append(const std::string &s) { data_.append(s); }
  size_t getPosition() const { return pos_; }
  size_t getSize() const { return data_.size(); }
  char getBit() {
    if(pos_ >= data_.size()){
      throw std::out_of_range("BitStream out of range");
    }
    return data_[pos_++];
  }
  std::string getBitN(size_t n) {
    std::string bits;
    for (size_t i = 0; i < n; i++) {
      bits.push_back(getBit());
    }
    return bits;
  }

private:
  std::string data_;
  size_t pos_{0};
};
} // namespace sjpg_codec

#endif // SIMPLE_JPEG_CODEC_SJPG_BIT_STREAM_H
