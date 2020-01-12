#include <hal/i2c_master.h>

#include "i2c_master_detail.h"

I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length,
                         bool stop_after_transmission);

I2cStatus i2cMasterRead(const uint8_t dev_addr, uint8_t* data, uint8_t length)
{
   auto res = i2cStart(dev_addr, I2cOperation::READ);
   if (res != I2cStatus::SUCCESS)
   {
      return res;
   }

   // accept received data and ack it
   bool ok = true;
   while (ok && length > 1)
   {
      ok = i2cReadByte(data, I2cAck::ACK);
      ++data;
      length--;
   }
   if (ok)
   {
      ok = i2cReadByte(data, I2cAck::NACK);
   }

   i2cStop();

   return ok ? I2cStatus::SUCCESS : I2cStatus::TRANSMISSION_ERROR;
}

I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length)
{
   return i2cMasterWrite(dev_addr, data, length, true);
}

I2cStatus i2cMasterWriteThenRead(uint8_t dev_addr, const uint8_t* write_data,
                                 uint8_t write_length, uint8_t* read_data,
                                 uint8_t read_length)
{
   auto res = i2cMasterWrite(dev_addr, write_data, write_length, false);
   if (res == I2cStatus::SUCCESS)
   {
      res = i2cMasterRead(dev_addr, read_data, read_length);
   }
   return res;
}

I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length,
                         bool stop_after_transmission)
{
   auto res = i2cStart(dev_addr, I2cOperation::WRITE);
   if (res != I2cStatus::SUCCESS)
   {
      return res;
   }

   while (length)
   {
      i2cWriteByte(*data, I2cWriteType::DATA);
      data++;
      length--;
   }

   // transmit stop condition
   if (stop_after_transmission)
   {
      i2cStop();
   }

   return res;
}
