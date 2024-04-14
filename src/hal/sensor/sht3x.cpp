#include <hal/sensor/sht3x.h>

#include <hal/delay.h>
#include <hal/i2c_master.h>

namespace
{
constexpr uint8_t SHT35_ADDR = 0x44;

uint8_t computeCrcWord(uint8_t data[2]);
}  // namespace

#define SHT3X_CMD_READ_SERIAL_NUMBER 0x3780  // Read the chip serial number
#define SHT3X_CMD_GETDATA_POLLING_H 0x2400   // measurement: polling, high repeatability
#define SHT3X_CMD_GETDATA_POLLING_M 0x240B   // measurement: polling, medium repeatability
#define SHT3X_CMD_GETDATA_POLLING_L 0x2416   // measurement: polling, low repeatability

#define makeCommand(cmd)                                                                 \
   {                                                                                     \
      static_cast<uint8_t>(cmd >> 8), static_cast<uint8_t>(cmd & 0xff)                   \
   }

std::variant<uint32_t, SHT3xError> sht3xReadSerialNumber()
{
   const uint8_t cmd[2] = makeCommand(SHT3X_CMD_READ_SERIAL_NUMBER);
   uint8_t buf[6];
   if (i2cMasterWriteThenRead(SHT35_ADDR, cmd, sizeof(cmd), buf, sizeof(buf))
       != I2cStatus::SUCCESS)
   {
      return SHT3xError::I2C;
   }

   if (computeCrcWord(buf) != buf[2] || computeCrcWord(buf + 3) != buf[5])

   {
      return SHT3xError::CRC;
   }

   uint32_t result;
   result = buf[0];
   result = (result << 8) | buf[1];
   result = (result << 8) | buf[3];
   result = (result << 8) | buf[4];
   return result;
}

std::variant<SHT3xRawData, SHT3xError> sht3xMeasure()
{
   const uint8_t cmd[2] = makeCommand(SHT3X_CMD_GETDATA_POLLING_H);
   uint8_t buf[6];
   if (i2cMasterWrite(SHT35_ADDR, cmd, sizeof(cmd)) != I2cStatus::SUCCESS)
   {
      return SHT3xError::I2C;
   }
   delay_ms(15);  // wait for measurement to complete
   if (i2cMasterRead(SHT35_ADDR, buf, sizeof(buf)) != I2cStatus::SUCCESS)
   {
      return SHT3xError::I2C;
   }
   if (computeCrcWord(buf) != buf[2] || computeCrcWord(buf + 3) != buf[5])

   {
      return SHT3xError::CRC;
   }

   SHT3xRawData raw;
   raw.temperature = (buf[0] << 8) | buf[1];
   raw.humidity = (buf[3] << 8) | buf[4];
   return raw;
}

TemperatureAndHumidity sht3xCompute(const SHT3xRawData& raw)
{
   // Use integer arithmetic and do proper rounding
   TemperatureAndHumidity data;
   data.temperature =
      (static_cast<int16_t>(17500ul * static_cast<uint32_t>(raw.temperature) / 65535ul)
       + 5)
         / 10
      - 450;
   data.humidity = static_cast<uint8_t>(
      (static_cast<uint16_t>(1000ul * static_cast<uint32_t>(raw.humidity) / 65535ul) + 5u)
      / 10);
   return data;
}

namespace
{
uint8_t computeCrcWord(uint8_t data[2])
{
   uint8_t crc = 0xff;

   for (uint8_t dataCounter = 0; dataCounter < 2; dataCounter++)
   {
      crc ^= (data[dataCounter]);
      for (uint8_t bit = 8; bit > 0; --bit)
      {
         if (crc & 0x80)
            crc = (crc << 1) ^ 0x131;
         else
            crc = (crc << 1);
      }
   }
   return crc;
}
}  // namespace
