/*  Hardware abstraction layer for embedded systems
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 */

#pragma once

#ifdef __AVR
#include <avr/pgmspace.h>
#else
#include <stdint.h>

#define PROGMEM
#define PSTR(x) x

inline uint8_t pgm_read_byte(const uint8_t* ptr)
{
   return *ptr;
}
#endif
