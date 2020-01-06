#include <hal/i2c_master.h>

#define SI7021_DEFAULT_ADDRESS	0x40

#define SI7021_MEASRH_HOLD_CMD      0xE5 // Measure Relative Humidity, Hold Master Mode
#define SI7021_MEASRH_NOHOLD_CMD    0xF5 // Measure Relative Humidity, No Hold Master Mode
#define SI7021_MEASTEMP_HOLD_CMD    0xE3 // Measure Temperature, Hold Master Mode
#define SI7021_MEASTEMP_NOHOLD_CMD  0xF3 // Measure Temperature, No Hold Master Mode
#define SI7021_READPREVTEMP_CMD     0xE0 // Read Temperature Value from Previous RH Measurement
#define SI7021_RESET_CMD            0xFE
#define SI7021_WRITERHT_REG_CMD     0xE6 // Write RH/T User Register 1
#define SI7021_READRHT_REG_CMD      0xE7 // Read RH/T User Register 1
#define SI7021_WRITEHEATER_REG_CMD  0x51 // Write Heater Control Register
#define SI7021_READHEATER_REG_CMD   0x11 // Read Heater Control Register
#define SI7021_ID1_CMD              0xFA0F // Read Electronic ID 1st Byte
#define SI7021_ID2_CMD              0xFCC9 // Read Electronic ID 2nd Byte
#define SI7021_FIRMVERS_CMD         0x84B8 // Read Firmware Revision

#define SI7021_REV_1					0xff
#define SI7021_REV_2					0x20

namespace
{
uint8_t readRegister8(uint8_t reg);
uint16_t readRegister16(uint8_t reg);
void writeRegister8(uint8_t reg, uint8_t value);
}

bool si7021Init()
{
   return readRegister8(SI7021_READRHT_REG_CMD) == 0x3A;
}

int16_t si7021Temperature()
{
   uint8_t buf[3] = {SI7021_MEASTEMP_NOHOLD_CMD};
   i2cMasterWrite(SI7021_DEFAULT_ADDRESS, buf, 1);
   while (i2cMasterRead(SI7021_DEFAULT_ADDRESS, buf, 3) != I2cStatus::SUCCESS)
      ;
   uint16_t temp = buf[0] << 8 | buf[1];
   //uint8_t checksum = buf[2];
   temp = ((uint32_t)temp * 17572UL / 65536UL);
   return ((int16_t)temp - 4685)/10;
}

uint8_t si7021Humidity()
{
   uint8_t buf[3] = {SI7021_MEASRH_NOHOLD_CMD};
   i2cMasterWrite(SI7021_DEFAULT_ADDRESS, buf, 1);
   while (i2cMasterRead(SI7021_DEFAULT_ADDRESS, buf, 3) != I2cStatus::SUCCESS)
      ;
   uint16_t hum = buf[0] << 8 | buf[1];
   //uint8_t checksum = buf[2];
   return static_cast<uint8_t>(static_cast<uint32_t>(hum) * 125UL / 65536UL -6UL);
}

namespace
{
inline uint8_t readRegister8(uint8_t reg)
{
   uint8_t value;
   i2cMasterWriteThenRead(SI7021_DEFAULT_ADDRESS, &reg, sizeof(reg), &value, sizeof(value));
   return value;
}

inline uint16_t readRegister16(uint8_t reg)
{
   uint16_t value;
   i2cMasterWriteThenRead(SI7021_DEFAULT_ADDRESS, &reg, sizeof(reg), (uint8_t*)&value, sizeof(value));
   return value;
}

inline void writeRegister8(uint8_t reg, uint8_t value)
{
   uint8_t buf[] = {reg, value};
   i2cMasterWrite(SI7021_DEFAULT_ADDRESS, buf, sizeof(buf));
}
}
