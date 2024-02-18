add_executable(unit_tests
        test_bit_stream.cpp
        test_jpeg_decoder.cpp
        test_huffman_table.cpp
        )

target_link_libraries(unit_tests PRIVATE sjpg gmock_main)
