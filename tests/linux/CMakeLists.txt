cmake_minimum_required(VERSION 3.7)
project(hal-test)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/../../include)

find_package(Boost COMPONENTS program_options REQUIRED)

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
    ${SOURCE_DIR}/hal/sensor/adt7410.cpp
    ${SOURCE_DIR}/hal/sensor/hyt939.cpp
    ${SOURCE_DIR}/hal/sensor/si1145.cpp
    ${SOURCE_DIR}/hal/sensor/si1145_autoexp.cpp
    ${SOURCE_DIR}/hal/sensor/si1145_helper.cpp
    ${SOURCE_DIR}/hal/sensor/si7021.cpp
    ${SOURCE_DIR}/hal/sensor/veml6075.cpp

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
    ${SOURCE_DIR}/hal/sensor/si1145_helper.cpp
    ${SOURCE_DIR}/hal/sensor/si1145_autoexp.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
)
target_include_directories(test_sensor_light
   PRIVATE
      ${INC_DIR}
)
target_link_libraries(test_sensor_light
   PRIVATE
      Boost::program_options
)

add_executable(test_sensor_sht3x
    test_sensor_sht3x.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
    ${SOURCE_DIR}/hal/sensor/sht3x.cpp
    ${SOURCE_DIR}/hal/linux/i2c_master.cpp
)
target_include_directories(test_sensor_sht3x
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

add_executable(test_sensor_co2
   test_sensor_co2.cpp
   ${SOURCE_DIR}/hal/sensor/mhz19.cpp
   ${SOURCE_DIR}/hal/linux/usart.cpp
)
target_include_directories(test_sensor_co2
   PRIVATE
      ${INC_DIR}
)
