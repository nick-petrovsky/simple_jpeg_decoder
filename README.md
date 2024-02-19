# Simple Jpeg Decoder
Simple Jpeg Decoder is a C/C++ implementation that demonstrates how to decode a baseline jpeg file. The code is designed to showcase the decoding process and provide a practical guide for beginners to learn jpeg decoding.

# Purpose
The purpose of this repository is to provide a simple and easy-to-understand example of jpeg decoding. The code is written in a basic style to help beginners understand the decoding process and get started with jpeg decoding.

# References
The code in this repository is based on the following articles:

+ [Let's Write a Simple JPEG Library, Part-I: The Basics](https://koushtav.me/jpeg/tutorial/2017/11/25/lets-write-a-simple-jpeg-library-part-1/)
+ [Let's Write a Simple JPEG Library, Part-II: The Decoder](https://koushtav.me/jpeg/tutorial/c++/decoder/2019/03/02/lets-write-a-simple-jpeg-library-part-2/#start-of-scan-segment-sos)
+ [Understanding and Decoding a JPEG Image using Python](https://yasoob.me/posts/understanding-and-writing-jpeg-decoder-in-python/)
+ [jpeg_tutorial](https://github.com/MROS/jpeg_tutorial)

# Compilation
To compile the code, use the following command:
```shell
git clone https://github.com/jiemojiemo/simple_jpeg_decoder.git
git submodule update --init --recursive
cd simple_jpeg_decoder
cmake -S . -B build
cmake --build build
```

# Usage
The main.cpp file contains an example of how to decode a jpeg file and save the decoded RGB data as a PPM file. The tests directory contains a variety of unit tests that can be used as a reference for specific classes or functions.

# Contributing
Contributions to this repository are welcome. If you find any issues or have suggestions for improvements, please feel free to submit a pull request.

# License
This repository is licensed under the MIT License.