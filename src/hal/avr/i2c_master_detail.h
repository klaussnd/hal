#pragma once

#include <hal/i2c_master.h>

#include <cstdint>
#include <optional>

enum class I2cOperation
{
   WRITE = 0,
   READ = 1,
};

enum class I2cWriteType
{
   ADDRESS,
   DATA,
};

enum class I2cAck
{
   ACK,
   NACK
};

/// Send start condition and transfer the address/direction byte.
I2cStatus i2cStart(uint8_t address, I2cOperation operation);
bool i2cWriteByte(uint8_t byte, I2cWriteType type);
/// @param[out] data Data read
bool i2cReadByte(uint8_t* data, I2cAck ack);
void i2cStop();
