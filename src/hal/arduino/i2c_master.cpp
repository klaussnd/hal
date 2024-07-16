#include <hal/i2c_master.h>

#include <Wire.h>

namespace
{
I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length,
                         bool stop);
}

bool i2cMasterInit()
{
   Wire.begin();
   return true;
}

I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length)
{
   return i2cMasterWrite(dev_addr, data, length, true);
}

I2cStatus i2cMasterRead(uint8_t dev_addr, uint8_t* data, uint8_t length)
{
   const bool request_success = Wire.requestFrom(dev_addr, length) == length;
   uint8_t bytes_read = 0;
   while (Wire.available() && bytes_read < length)  // slave may send less than requested
   {
      *data = Wire.read();
      ++data;
      ++bytes_read;
   }

   return request_success && bytes_read == length ? I2cStatus::SUCCESS
                                                  : I2cStatus::TRANSMISSION_ERROR;
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

namespace
{
I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length, bool stop)
{
   Wire.beginTransmission(dev_addr);
   const bool write_success = Wire.write(data, length) == length;
   const bool send_success = Wire.endTransmission(stop) == 0;
   return write_success && send_success ? I2cStatus::SUCCESS
                                        : I2cStatus::TRANSMISSION_ERROR;
}
}  // namespace
