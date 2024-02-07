add_executable(unit_tests
        test_jpeg_decoder.cpp
        )

target_link_libraries(unit_tests PRIVATE sjpg gmock_main)
