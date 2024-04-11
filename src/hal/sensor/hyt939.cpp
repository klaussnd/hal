#include <hal/sensor/hyt939.h>

#include <hal/i2c_master.h>

namespace
{
constexpr uint8_t HYT939_ADDR = 0x28;
}

std::optional<Hyt939RawData> hyt939MeasureRaw()
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
   } while (buffer[0] & ~0xbf);

   /* get data */
   if (i2cMasterRead(HYT939_ADDR, buffer, sizeof(buffer)) != I2cStatus::SUCCESS)
   {
      return {};
   }

   const uint16_t humi_raw = ((buffer[0] << 8) | buffer[1]) & 0x3fff;
   const uint16_t temp_raw = ((buffer[2] << 6) | (buffer[3] >> 2)) & 0x3fff;
   return Hyt939RawData{humi_raw, temp_raw};
}

Hyt939Data hyt939Compute(const Hyt939RawData& raw)
{
   Hyt939Data data;
   data.humidity = (static_cast<uint32_t>(raw.humidity) * 100UL)
                   / (static_cast<uint32_t>(2 << 13) - 1UL);
   const uint16_t temp_tmp = static_cast<uint32_t>(raw.temperature) * 1650UL
                             / (static_cast<uint32_t>(2 << 13) - 1UL);
   data.temperature = static_cast<int16_t>(temp_tmp) - 400;

   return data;
}

std::optional<Hyt939Data> hyt939Measure()
{
   const auto raw = hyt939MeasureRaw();
   if (!raw.has_value())
   {
      return {};
   }
   return hyt939Compute(raw.value());
}
