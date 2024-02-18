#include "libyuv.h"
#include "sjpg_decoder.h"
#include <iostream>
void saveRGBTOPPM(uint8_t *data, int linesize, int width, int height,
                  const char *output_name) {
  FILE *pFile;

  // Open file
  pFile = fopen(output_name, "wb");
  if (pFile == NULL) {
    return;
  }

  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);

  // Write pixel data
  const int kBytesPerPixel = 3; // R(8) G(8) B(8)
  for (int y = 0; y < height; y++) {
    uint8_t *img_row = data + y * linesize;
    fwrite(img_row, 1, width * kBytesPerPixel, pFile);
  }

  // Close file
  fclose(pFile);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <jpg_file_path>\n", argv[0]);
    return -1;
  }
  auto file_path = argv[1];

  sjpg_codec::Decoder decoder;
  auto ok = decoder.open(file_path);
  if (!ok) {
    printf("Failed to open file: %s\n", file_path);
    return -1;
  }

  auto ret = decoder.decodeImageFile();
  if (ret != 0) {
    printf("Failed to decode file: %s\n", file_path);
    return -1;
  }

  auto img_width = decoder.getSOF0Segment().width;
  auto img_height = decoder.getSOF0Segment().height;
  auto y_data = decoder.getYDecodedData();
  auto u_data = decoder.getUDecodedData();
  auto v_data = decoder.getVDecodedData();

  auto save_data = [](const std::string &save_path,
                      std::vector<uint8_t> &data) {
    std::fstream out(save_path, std::ios::out);
    std::cout << "data size: " << data.size() << std::endl;
    for (auto i = 0; i < data.size(); i++) {
      out << (int)data[i] << std::endl;
    }
  };

  save_data("y_data.txt", y_data);
  save_data("u_data.txt", u_data);
  save_data("v_data.txt", v_data);

  auto total_pixel = img_width * img_height;
  auto rgb_data_size = total_pixel * 3;
  auto rgb_data = std::vector<uint8_t>(rgb_data_size, 0);
  auto rgb_stride = img_width * 3;

  libyuv::I444ToRGB24(y_data.data(), img_width, u_data.data(), img_width,
                      v_data.data(), img_width, rgb_data.data(), rgb_stride,
                      img_width, img_height);

  saveRGBTOPPM(rgb_data.data(), rgb_stride, img_width, img_height,
               "output.ppm");
}
