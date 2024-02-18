if (NOT TARGET gmock)
    add_subdirectory(googletest EXCLUDE_FROM_ALL)
endif ()

add_subdirectory(libyuv EXCLUDE_FROM_ALL)
target_include_directories(yuv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include)