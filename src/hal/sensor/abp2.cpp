#include <hal/sensor/abp2.h>

namespace
{
constexpr uint8_t I2C_ADDRESS = 0x28;
constexpr uint8_t ABP2_BUSY = (1 << 5);
}  // namespace

I2cStatus abp2StartMeasurement()
{
   const uint8_t cmd[3] = {0xaa, 0x00, 0x00};
   return i2cMasterWrite(I2C_ADDRESS, cmd, sizeof(cmd));
}

std::variant<Abp2Status, Abp2RawData> abp2GetMeasurement()
{
   uint8_t data[7];
   const auto i2c_status = i2cMasterRead(I2C_ADDRESS, data, sizeof(data));
   if (i2c_status != I2cStatus::SUCCESS)
   {
      return Abp2Status::I2C_FAILURE;
   }
   if (data[0] & ABP2_BUSY)
   {
      return Abp2Status::BUSY;
   }

   Abp2RawData result;
   result.pressure = static_cast<uint32_t>(data[3])
                     | (static_cast<uint32_t>(data[2]) << 8)
                     | (static_cast<uint32_t>(data[1]) << 16);
   result.temperature = static_cast<uint32_t>(data[6])
                        | (static_cast<uint32_t>(data[5]) << 8)
                        | (static_cast<uint32_t>(data[4]) << 16);
   return result;
}
