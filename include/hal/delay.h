/*  Hardware abstraction layer for embedded systems
 *
 *  (c) Klaus Schneider-Zapp klaus underscore snd at web dot de
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
#include <util/delay.h>
#else
#include <unistd.h>
#endif

template <typename T>
void delay_ms(T milliseconds);
template <typename T>
void delay_us(T microseconds);


template <typename T>
void delay_ms(T milliseconds)
{
#ifdef __AVR
   _delay_ms(milliseconds);
#else
   usleep(1000UL * milliseconds);
#endif
}

template <typename T>
void delay_us(T microseconds)
{
#ifdef __AVR
   _delay_us(microseconds);
#else
   usleep(microseconds);
#endif
}
