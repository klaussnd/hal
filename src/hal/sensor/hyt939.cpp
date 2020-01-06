#include <hal/sensor/hyt939.h>

#include <hal/i2c_master.h>

namespace
{
constexpr uint8_t HYT939_ADDR = 0x28;
}

std::optional<Hyt939Data> hyt939Measure()
{
   uint8_t buffer[4] = {0};

   /* measure */
   if (i2cMasterWrite(HYT939_ADDR, buffer, 1) != I2cStatus::SUCCESS)
   {
      return {};
   }

   /* wait for end of measurement */
   do
   {
      if (i2cMasterRead(HYT939_ADDR, buffer, 1) != I2cStatus::SUCCESS)
      {
         return {};
      }
   } while(buffer[0] & ~0xbf);

   /* get data */
   if (i2cMasterRead(HYT939_ADDR, buffer, sizeof(buffer)) != I2cStatus::SUCCESS)
   {
      return {};
   }

   Hyt939Data data;
   const uint16_t humi_raw = ((buffer[0]<<8) | buffer[1]) & 0x3fff;
   const uint16_t temp_raw = ((buffer[2]<<6) | (buffer[3]>>2)) & 0x3fff;
   data.humidity = static_cast<uint32_t>(humi_raw) * 100UL / ((2<<13) - 1);
   const uint16_t temp_tmp = static_cast<uint32_t>(temp_raw) * 1650UL / ((2<<13) - 1);
   data.temperature = static_cast<int16_t>(temp_tmp) - 400;

   return data;
}
