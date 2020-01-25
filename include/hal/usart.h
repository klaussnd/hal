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

#include <stdint.h>

/** Initialise the usart */
template <unsigned long baudrate>
void usartInit();

/** Return true if a complete line is available to be read in text mode */
bool usartAvailReadLine();

/** Read from serial line (binary mode) */
uint8_t usartRead(char* dstbuf, uint8_t maxlength);

/** Read one line from serial line (string mode) */
uint8_t usartReadLine(char* dstbuf, uint8_t maxlength);

/** Write data to serial line
 *  @param buf Pointer to the buffer containing the data
 *  @param length Number of characters to be written
 */
uint8_t usartWrite(const char* buf, uint8_t length);

#ifdef __AVR
/** @see usartWrite
 *  This version operates on data in program space */
inline uint8_t usartWrite_P(const char* buf, uint8_t length);
#endif

/** Write string to serial line
 *  @see usartWrite
 *  Writes data until null character
 *  @param str String to be written */
uint8_t usartWriteString(const char* buf);

#ifdef __AVR
/** @see usartWriteString
 *  This version operates on data in program space */
uint8_t usartWriteString_P(const char* buf);

#include "avr/usart_priv.h"
#endif
