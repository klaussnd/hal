#include <hal/sensor/veml6075.h>

#include <hal/i2c_master.h>

#include <algorithm>

namespace
{
constexpr uint8_t VEML6075_DEVICE_ADDRESS = 0x10;
constexpr uint16_t VEML6075_DEVICE_ID = 0x26;

constexpr uint8_t SHUTDOWN_BIT = 0;
constexpr uint8_t ACTIVE_FORCE_BIT = 1;
constexpr uint8_t TRIGGER_BIT = 2;
constexpr uint8_t HIGH_DYNAMIC_BIT = 3;
constexpr uint8_t INTEGRATION_TIME_SHIFT = 4;
constexpr uint8_t INTEGRATION_TIME_MASK = 0x70;

enum class Veml6075Register
{
   UV_CONF = 0x00,
   UVA_DATA = 0x07,
   UVB_DATA = 0x09,
   UVCOMP1_DATA = 0x0A,
   UVCOMP2_DATA = 0x0B,
   ID = 0x0C
};

bool setPower(bool on);

std::optional<uint16_t> readI2cRegister(Veml6075Register reg);
bool writeI2cRegister(Veml6075Register reg, uint16_t value);
}  // namespace

bool veml6075Init()
{
   if (!setPower(true))
   {
      return false;
   }
   const auto maybeDeviceId = readI2cRegister(Veml6075Register::ID);
   if (!maybeDeviceId.has_value() || maybeDeviceId.value() != VEML6075_DEVICE_ID)
   {
      return false;
   }

   return veml6075SetIntegrationTime(Veml6075IntegrationTime::_100MS)
          && veml6075SetHighDynamic(false) && veml6075SetActiveForce(false);
}

bool veml6075SetIntegrationTime(Veml6075IntegrationTime integration_time)
{
   const auto conf = readI2cRegister(Veml6075Register::UV_CONF);
   if (!conf.has_value())
   {
      return false;
   }
   const uint16_t new_conf =
      (conf.value() & ~INTEGRATION_TIME_MASK)
      | (static_cast<uint8_t>(integration_time) << INTEGRATION_TIME_SHIFT);
   return writeI2cRegister(Veml6075Register::UV_CONF, new_conf);
}

bool veml6075SetHighDynamic(bool enabled)
{
   const auto conf = readI2cRegister(Veml6075Register::UV_CONF);
   if (!conf.has_value())
   {
      return false;
   }
   const uint16_t new_conf = (conf.value() & ~(1 << HIGH_DYNAMIC_BIT))
                             | (static_cast<uint8_t>(enabled) << HIGH_DYNAMIC_BIT);
   return writeI2cRegister(Veml6075Register::UV_CONF, new_conf);
}

bool veml6075SetActiveForce(bool enabled)
{
   const auto conf = readI2cRegister(Veml6075Register::UV_CONF);
   if (!conf.has_value())
   {
      return false;
   }
   const uint16_t new_conf = (conf.value() & ~(1 << ACTIVE_FORCE_BIT))
                             | (static_cast<uint8_t>(enabled) << ACTIVE_FORCE_BIT);
   return writeI2cRegister(Veml6075Register::UV_CONF, new_conf);
}

bool Veml6075Trigger()
{
   const auto conf = readI2cRegister(Veml6075Register::UV_CONF);
   if (!conf.has_value())
   {
      return false;
   }
   const uint16_t new_conf = conf.value() | (1 << TRIGGER_BIT);
   return writeI2cRegister(Veml6075Register::UV_CONF, new_conf);
}

std::optional<Veml6075Data> veml6075Measure()
{
   const auto uva = readI2cRegister(Veml6075Register::UVA_DATA);
   const auto uvb = readI2cRegister(Veml6075Register::UVB_DATA);
   const auto comp1 = readI2cRegister(Veml6075Register::UVCOMP1_DATA);
   const auto comp2 = readI2cRegister(Veml6075Register::UVCOMP2_DATA);
   if (uva.has_value() && uvb.has_value() && comp1.has_value() && comp2.has_value())
   {
      return Veml6075Data{uva.value(), uvb.value(), comp1.value(), comp2.value()};
   }
   return {};
}

namespace
{
bool setPower(bool on)
{
   const auto conf = readI2cRegister(Veml6075Register::UV_CONF);
   if (!conf.has_value())
   {
      return false;
   }
   const uint8_t power_bit = on ? 0 : 1;
   const uint16_t new_conf = (conf.value() & ~1) | power_bit;
   return writeI2cRegister(Veml6075Register::UV_CONF, new_conf);
}

std::optional<uint16_t> readI2cRegister(Veml6075Register reg)
{
   uint8_t buf[2];
   if (i2cMasterWriteThenRead(VEML6075_DEVICE_ADDRESS, reinterpret_cast<uint8_t*>(&reg),
                              1, buf, sizeof(buf))
       == I2cStatus::SUCCESS)
   {
      return static_cast<uint16_t>(buf[0] | (buf[1] << 8));  // LSB first
   }
   return {};
}

bool writeI2cRegister(Veml6075Register reg, uint16_t value)
{
   uint8_t buf[3] = {static_cast<uint8_t>(reg),
                     static_cast<uint8_t>(value),  // LSB first
                     static_cast<uint8_t>(value >> 8)};
   return i2cMasterWrite(VEML6075_DEVICE_ADDRESS, buf, 3) == I2cStatus::SUCCESS;
}
}  // namespace
