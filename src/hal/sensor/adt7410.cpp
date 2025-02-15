#include <hal/sensor/adt7410.h>

#include <hal/i2c_master.h>

#include <cstring>

/* i2c address of device
   MSB ...  LSB
   1 0 0 1 0 A1 A0  (4 addresses)
 */
#define ADT7410_ADDR_BASE 0x48
#ifndef ADT7410_ADDR
#define ADT7410_ADDR ADT7410_ADDR_BASE
#endif

/* ADT7410 registers definition */
#define ADT7410_TEMPERATURE 0
#define ADT7410_STATUS 2
#define ADT7410_CONFIG 3
#define ADT7410_T_ALARM_HIGH 4
#define ADT7410_T_ALARM_LOW 6
#define ADT7410_T_CRIT 8
#define ADT7410_T_HYST 0xA

/* ADT7410 status */
#define ADT7410_STAT_T_LOW (1 << 4)
#define ADT7410_STAT_T_HIGH (1 << 5)
#define ADT7410_STAT_T_CRIT (1 << 6)
#define ADT7410_STAT_NOT_RDY (1 << 7)

/* ADT7410 config */
#define ADT7410_FAULT_QUEUE_MASK (1 << 0 | 1 << 1)
#define ADT7410_CT_POLARITY (1 << 2)
#define ADT7410_INT_POLARITY (1 << 3)
#define ADT7410_EVENT_MODE (1 << 4)
#define ADT7410_MODE_MASK (1 << 5 | 1 << 6)
#define ADT7410_MODE_FULL (0 << 5 | 0 << 6)
#define ADT7410_MODE_ONESPS (0 << 5 | 1 << 6)
#define ADT7410_MODE_PD (1 << 5 | 1 << 6)
#define ADT7410_RESOLUTION (1 << 7)

/* ADT7410 masks */
#define ADT7410_T13_VALUE_MASK 0xFFF8
#define ADT7410_T_HYST_MASK 0xF

/* straight from the datasheet */
#define ADT7410_TEMP_MIN (-55000)
#define ADT7410_TEMP_MAX 150000

namespace
{
/** Read ADT7410 register */
bool adt7410ReadRegister(uint8_t reg, uint8_t* data, uint8_t length);
/** Write ADT7410 register */
bool adt7410WriteRegister(uint8_t reg, const uint8_t* data, uint8_t length);
/** Convert raw value to temperature in deci-Celsius */
int16_t adt7410Raw2Temp(uint8_t[2]);
}  // namespace

bool adt7410Init(void)
{
   const uint8_t conf = ADT7410_MODE_ONESPS | ADT7410_RESOLUTION;
   return adt7410WriteRegister(ADT7410_CONFIG, &conf, 1);
}

std::optional<int16_t> adt7410ReadTemp()
{
   bool do_wait = true;

   // wait for status bit to indicate ready
   do
   {
      const auto is_busy = adt7410IsBusy();
      if (!is_busy.has_value())
      {
         return {};
      }
      do_wait = is_busy.value();
   } while (do_wait);

   // read raw temperature
   uint8_t data[2];
   if (!adt7410ReadRegister(ADT7410_TEMPERATURE, data, sizeof(data)))
   {
      return {};
   }
   else
   {
      return adt7410Raw2Temp(data);
   }
}

std::optional<bool> adt7410IsBusy()
{
   uint8_t status;
   if (!adt7410ReadRegister(ADT7410_STATUS, &status, sizeof(status)))
   {
      return {};
   }
   return (status & ADT7410_STAT_NOT_RDY);
}

namespace
{
int16_t adt7410Raw2Temp(uint8_t raw[2])
{
   /* format is 1/128 Celsius
    * MSB comes first, so we need to swap bytes */
   int16_t temp = (int16_t)((((uint16_t)raw[0]) << 8) | (uint16_t)raw[1]);
   /* we want 1/10 degrees
      divide by d = (128/10), add/subtract d/2 to round to nearest */
   temp += (temp > 0) ? 13 : -13;
   temp >>= 3; /* divide by 8 */
   temp *= 10;
   temp >>= 4; /* divide by 16 */
   return temp;
}

bool adt7410ReadRegister(uint8_t reg, uint8_t* data, const uint8_t len)
{
   return i2cMasterWriteThenRead(ADT7410_ADDR, &reg, 1, data, len) == I2cStatus::SUCCESS;
}

bool adt7410WriteRegister(const uint8_t reg, const uint8_t* data, const uint8_t len)
{
   uint8_t buffer[1 + len];
   buffer[0] = reg;
   for (uint8_t i = 0; i < len; ++i)
   {
      buffer[i + 1] = data[i];
   }
   return i2cMasterWrite(ADT7410_ADDR, buffer, 1 + len) == I2cStatus::SUCCESS;
}
}  // namespace
