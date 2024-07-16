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

#include <inttypes.h>

enum class I2cStatus : uint8_t
{
   SUCCESS,
   START_ERROR,
   ADDRESS_ERROR,
   TRANSMISSION_ERROR,
   ACK_ERROR,
};

/** Initialise i2c interface */
#if defined __AVR || defined(ESP8266)
bool i2cMasterInit();
#elif defined __linux
bool i2cMasterInit(const char* device);
#else
#warning Undefined platform
#endif

/** Send data to i2c
 */
I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length);

/** Receive data from i2c.
 *  Assert to read exactly \e length bytes.
 */
I2cStatus i2cMasterRead(uint8_t dev_addr, uint8_t* data, uint8_t length);

/** Write data, send a restart condition and receive data.
 */
I2cStatus i2cMasterWriteThenRead(uint8_t dev_addr, const uint8_t* write_data,
                                 uint8_t write_length, uint8_t* read_data,
                                 uint8_t read_length);
