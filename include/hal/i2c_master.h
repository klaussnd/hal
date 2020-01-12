#pragma once

#include <inttypes.h>

enum class I2cStatus
{
   SUCCESS,
   START_ERROR,
   ADDRESS_ERROR,
   TRANSMISSION_ERROR,
   ACK_ERROR,
};

/** Initialise i2c interface */
bool i2cMasterInit();

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
