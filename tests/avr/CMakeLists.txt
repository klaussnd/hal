cmake_minimum_required(VERSION 3.11)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../../cmake-avr/generic-gcc-avr.cmake")

set(AVR_MCU atmega168)
set(MCU_FREQ 7372800UL)

project(avr-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/../../include)
set(STDCPP_INC_DIR ${CMAKE_SOURCE_DIR}/../../avr-libstdcpp/include)

add_compile_definitions(F_CPU=${MCU_FREQ})

add_avr_executable(test_usart
    test_usart.cpp
    ${SOURCE_DIR}/hal/avr/usart_irq.cpp
    ${SOURCE_DIR}/hal/avr/usart_stdout.c
)
avr_target_include_directories(test_usart
    PRIVATE
        ${INC_DIR}
)

add_avr_executable(test_adc
    test_adc.cpp
    ${SOURCE_DIR}/hal/avr/adc_irq.cpp
    ${SOURCE_DIR}/hal/avr/usart_irq.cpp
    ${SOURCE_DIR}/hal/avr/usart_stdout.c
)
avr_target_include_directories(test_adc
    PRIVATE
        ${INC_DIR}
)
avr_target_compile_definitions(test_adc
    PRIVATE
        ADC_REF_AVCC=1
)

add_avr_executable(test_rotary_encoder
   test_rotary_encoder.cpp
   ${SOURCE_DIR}/hal/avr/input/rotary_encoder.cpp
   ${SOURCE_DIR}/hal/avr/input/button.cpp
   ${SOURCE_DIR}/hal/avr/usart_irq.cpp
   ${SOURCE_DIR}/hal/avr/usart_stdout.c
)
avr_target_include_directories(test_rotary_encoder
    PRIVATE
        ${INC_DIR}
        ${STDCPP_INC_DIR}
)
avr_target_compile_definitions(test_rotary_encoder
    PRIVATE
        ROTARY_PHASE_A_PIN=B,0
        ROTARY_PHASE_B_PIN=B,1
        BUTTON_PORT=D
        BUTTON_MASK=0x40
)

add_avr_executable(test_button_toggle
   test_button_toggle.cpp
   ${SOURCE_DIR}/hal/avr/input/button.cpp
   ${SOURCE_DIR}/hal/avr/usart_irq.cpp
   ${SOURCE_DIR}/hal/avr/usart_stdout.c
)
avr_target_include_directories(test_button_toggle
    PRIVATE
        ${INC_DIR}
        ${STDCPP_INC_DIR}
)
avr_target_compile_definitions(test_button_toggle
    PRIVATE
        BUTTON_PORT=D
        BUTTON_MASK=0x40
        BUTTON_HAVE_TOGGLE=1
        BUTTON_HAVE_NO_PRESS
)

add_avr_executable(test_i2c_si7021
   test_i2c_si7021.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_general.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_twi.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_detail.h
   ${SOURCE_DIR}/hal/avr/usart_irq.cpp
   ${SOURCE_DIR}/hal/avr/usart_stdout.c
   ${SOURCE_DIR}/hal/sensor/si7021.cpp
)
avr_target_include_directories(test_i2c_si7021
    PRIVATE
        ${INC_DIR}
        ${STDCPP_INC_DIR}
)

add_avr_executable(test_i2c_si1145
   test_i2c_si1145.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_general.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_twi.cpp
   ${SOURCE_DIR}/hal/avr/i2c_master_detail.h
   ${SOURCE_DIR}/hal/avr/usart_irq.cpp
   ${SOURCE_DIR}/hal/avr/usart_stdout.c
   ${SOURCE_DIR}/hal/sensor/si1145.cpp
)
avr_target_include_directories(test_i2c_si1145
    PRIVATE
        ${INC_DIR}
        ${STDCPP_INC_DIR}
)
