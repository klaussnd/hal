cmake_minimum_required(VERSION 3.11)
project(unittests)

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    add_compile_options(-Wall -Wextra)
endif()

set(GOOGLETEST_DIR "/usr/src/googletest" CACHE STRING "Location of googletest sources")

add_subdirectory(${GOOGLETEST_DIR} gtest)

set(tests_unit_sources
    test_fifo.cpp
    test_ringbuffer.cpp
    test_canid.cpp
)

set(INC_DIR ${CMAKE_SOURCE_DIR}/../../include)

add_executable(tests_unit main.cpp ${tests_unit_sources})
target_include_directories(tests_unit
    PRIVATE
        ${INC_DIR}
)
target_link_libraries(tests_unit PRIVATE gmock)
