cmake_minimum_required(VERSION 3.7)
project(sensor)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7")
   set(CMAKE_CXX_STANDARD 14)
else()
   set(CMAKE_CXX_STANDARD 17)
endif()

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/../../include)

add_executable(test_usart
    test_usart.cpp
    ${SOURCE_DIR}/hal/linux/usart.cpp
)
target_include_directories(test_usart
   PRIVATE
      ${INC_DIR}
)

add_executable(test_sensor_i2c
    test_sensor_i2c.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
    ${SOURCE_DIR}/hal/sensor/hyt939.cpp
    ${SOURCE_DIR}/hal/sensor/si7021.cpp
)
target_include_directories(test_sensor_i2c
   PRIVATE
      ${INC_DIR}
)
target_compile_definitions(test_sensor_i2c
   PRIVATE
      I2C_DEV="/dev/i2c-1"
)

add_executable(test_sensor_light
    test_sensor_light.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
    ${SOURCE_DIR}/hal/sensor/si1145.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
)
target_include_directories(test_sensor_light
   PRIVATE
      ${INC_DIR}
)

add_executable(socketcan_read
   socketcan_read.cpp
   ${SOURCE_DIR}/hal/linux/socketcan.cpp
)
target_include_directories(socketcan_read
   PRIVATE
      ${INC_DIR}
)
