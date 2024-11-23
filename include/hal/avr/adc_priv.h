// private implementation-specific
#pragma once

#include <avr/io.h>

#if AVR_ARCH == 2
constexpr uint8_t ADC_REF_AVCC = 0;
constexpr uint8_t ADC_REF_INTERNAL = (1 << REFS1);  // internal band gap 1.1V
constexpr uint8_t ADC_REF_EXTERNAL = (1 << REFS0);

constexpr uint8_t ADC_MUX_MASK = 0x3f;

#elif defined __AVR_ATmega8__ || defined __AVR_ATmega16__ || defined __AVR_ATmega32__    \
   || defined __AVR_ATmega168__ || defined __AVR_ATmega168P__                            \
   || defined __AVR_ATmega328P__
constexpr uint8_t ADC_REF_AVCC = (1 << REFS0);
constexpr uint8_t ADC_REF_INTERNAL = (1 << REFS1) | (1 << REFS0);
constexpr uint8_t ADC_REF_EXTERNAL = 0;

constexpr uint8_t ADC_MUX_MASK = 0x0f;

#else
#error Unknown MCU
#endif
