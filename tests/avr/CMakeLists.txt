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
)
