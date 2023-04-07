#include <hal/sensor/si1145.h>

#include <hal/delay.h>
#include <hal/i2c_master.h>

// Registers, Parameters and commands
//
// commands
//
#define SI114X_QUERY 0X80
#define SI114X_SET 0XA0
#define SI114X_NOP 0X0
#define SI114X_RESET 0X01
#define SI114X_BUSADDR 0X02
#define SI114X_PS_FORCE 0X05
#define SI114X_GET_CAL 0X12
#define SI114X_ALS_FORCE 0X06
#define SI114X_PSALS_FORCE 0X07
#define SI114X_PS_PAUSE 0X09
#define SI114X_ALS_PAUSE 0X0A
#define SI114X_PSALS_PAUSE 0XB
#define SI114X_PS_AUTO 0X0D
#define SI114X_ALS_AUTO 0X0E
#define SI114X_PSALS_AUTO 0X0F
//
// IIC REGISTERS
//
#define SI114X_PART_ID 0X00
#define SI114X_REV_ID 0X01
#define SI114X_SEQ_ID 0X02
#define SI114X_INT_CFG 0X03
#define SI114X_IRQ_ENABLE 0X04
#define SI114X_IRQ_MODE1 0x05
#define SI114X_IRQ_MODE2 0x06
#define SI114X_HW_KEY 0X07
#define SI114X_MEAS_RATE0 0X08
#define SI114X_MEAS_RATE1 0X09
#define SI114X_PS_RATE 0X0A
#define SI114X_PS_LED21 0X0F
#define SI114X_PS_LED3 0X10
#define SI114X_UCOEFF0 0X13
#define SI114X_UCOEFF1 0X14
#define SI114X_UCOEFF2 0X15
#define SI114X_UCOEFF3 0X16
#define SI114X_WR 0X17
#define SI114X_COMMAND 0X18
#define SI114X_RESPONSE 0X20
#define SI114X_IRQ_STATUS 0X21
#define SI114X_ALS_VIS_DATA0 0X22
#define SI114X_ALS_VIS_DATA1 0X23
#define SI114X_ALS_IR_DATA0 0X24
#define SI114X_ALS_IR_DATA1 0X25
#define SI114X_PS1_DATA0 0X26
#define SI114X_PS1_DATA1 0X27
#define SI114X_PS2_DATA0 0X28
#define SI114X_PS2_DATA1 0X29
#define SI114X_PS3_DATA0 0X2A
#define SI114X_PS3_DATA1 0X2B
#define SI114X_AUX_DATA0_UVINDEX0 0X2C
#define SI114X_AUX_DATA1_UVINDEX1 0X2D
#define SI114X_RD 0X2E
#define SI114X_CHIP_STAT 0X30
//
// Parameters
//
#define SI114X_I2C_ADDR 0X00

#define SI114X_CHLIST 0X01
#define SI114X_CHLIST_ENUV 0x80
#define SI114X_CHLIST_ENAUX 0x40
#define SI114X_CHLIST_ENALSIR 0x20
#define SI114X_CHLIST_ENALSVIS 0x10
#define SI114X_CHLIST_ENPS1 0x01
#define SI114X_CHLIST_ENPS2 0x02
#define SI114X_CHLIST_ENPS3 0x04

#define SI114X_PSLED12_SELECT 0X02
#define SI114X_PSLED3_SELECT 0X03

#define SI114X_PS_ENCODE 0X05
#define SI114X_ALS_ENCODE 0X06

#define SI114X_PS1_ADCMUX 0X07
#define SI114X_PS2_ADCMUX 0X08
#define SI114X_PS3_ADCMUX 0X09

#define SI114X_PS_ADC_COUNTER 0X0A
#define SI114X_PS_ADC_GAIN 0X0B
#define SI114X_PS_ADC_MISC 0X0C

#define SI114X_ALS_IR_ADC_MUX 0X0E
#define SI114X_AUX_ADC_MUX 0X0F

#define SI114X_ALS_VIS_ADC_COUNTER 0X10
#define SI114X_ALS_VIS_ADC_GAIN 0X11
#define SI114X_ALS_VIS_ADC_MISC 0X12

#define SI114X_LED_REC 0X1C

#define SI114X_ALS_IR_ADC_COUNTER 0X1D
#define SI114X_ALS_IR_ADC_GAIN 0X1E
#define SI114X_ALS_IR_ADC_MISC 0X1F
//
// USER SETTINGS DEFINE
//
// ADCMUX
#define SI114X_ADCMUX_SMALL_IR 0x00
#define SI114X_ADCMUX_VISIABLE 0x02
#define SI114X_ADCMUX_LARGE_IR 0x03
#define SI114X_ADCMUX_NO 0x06
#define SI114X_ADCMUX_GND 0x25
#define SI114X_ADCMUX_TEMPERATURE 0x65
#define SI114X_ADCMUX_VDD 0x75
// LED SEL
#define SI114X_PSLED12_SELECT_PS1_NONE 0x00
#define SI114X_PSLED12_SELECT_PS1_LED1 0x01
#define SI114X_PSLED12_SELECT_PS1_LED2 0x02
#define SI114X_PSLED12_SELECT_PS1_LED3 0x04
#define SI114X_PSLED12_SELECT_PS2_NONE 0x00
#define SI114X_PSLED12_SELECT_PS2_LED1 0x10
#define SI114X_PSLED12_SELECT_PS2_LED2 0x20
#define SI114X_PSLED12_SELECT_PS2_LED3 0x40
#define SI114X_PSLED3_SELECT_PS2_NONE 0x00
#define SI114X_PSLED3_SELECT_PS2_LED1 0x10
#define SI114X_PSLED3_SELECT_PS2_LED2 0x20
#define SI114X_PSLED3_SELECT_PS2_LED3 0x40
// LED CURRENT
#define SI114X_LED_CURRENT_5MA 0X01
#define SI114X_LED_CURRENT_11MA 0X02
#define SI114X_LED_CURRENT_22MA 0X03
#define SI114X_LED_CURRENT_45MA 0X04
// INT OE
#define SI114X_INT_CFG_INTOE 0X01
// IRQ ENABLE
#define SI114X_IRQEN_ALS 0x01
#define SI114X_IRQEN_PS1 0x04
#define SI114X_IRQEN_PS2 0x08
#define SI114X_IRQEN_PS3 0x10

// PS_ADC_MISC bits
#define SI114X_HIGHRANGE (1 << 5)

// Error status returned in the response register
#define SI114X_ERROR_INVALID_SETTING 0x80
#define SI114X_ERROR_PS1_ADC_OVERFLOW 0x88
#define SI114X_ERROR_PS2_ADC_OVERFLOW 0x89
#define SI114X_ERROR_PS3_ADC_OVERFLOW 0x8a
#define SI114X_ERROR_ALS_VIS_ADC_OVERFLOW 0x8c
#define SI114X_ERROR_ALS_IR_ADC_OVERFLOW 0x8d
#define SI114X_ERROR_AUX_ADC_OVERFLOW 0x8e

// i2c address
#define SI114X_ADDR 0X60

enum class WriteCommandStatus : uint8_t
{
   SUCCESS,
   FAILURE,
   RETRY,
};

namespace
{
void reset();

std::optional<uint8_t> readByte(uint8_t reg);
std::optional<uint16_t> readWord(uint8_t reg);
bool writeByte(uint8_t reg, uint8_t value);
// read/write data from configuration RAM
std::optional<uint8_t> readParamData(uint8_t reg);
bool writeParamData(uint8_t reg, uint8_t value);
bool writeCommand(uint8_t cmd);
WriteCommandStatus writeCommandOnce(uint8_t cmd);

bool waitForMeasurementCompleted();
void clearInterrupt();

uint8_t getGainValue(Si1145Gain gain);
Si1145Gain getGainFromValue(uint8_t gain);
uint8_t getPhotodiodeValue(Si1145IrPhotodiode photodiode);
Si1145IrPhotodiode getPhotodiodeFromValue(uint8_t photodiode);
}  // namespace

bool si1145Init()
{
   if (readByte(SI114X_PART_ID) != 0x45)
   {
      return false;
   }

   reset();

   // set which inputs are enabled
   bool ok = writeParamData(
      SI114X_CHLIST, SI114X_CHLIST_ENUV | SI114X_CHLIST_ENALSIR | SI114X_CHLIST_ENALSVIS);

   // Disable all LEDs
   ok |= writeParamData(SI114X_PSLED12_SELECT, 0);
   ok |= writeParamData(SI114X_PSLED3_SELECT, 0);

   // set ADCs
   ok |= si1145SetVisMode(Si1145Range::HIGH, Si1145Gain::DIV_1);
   ok |= si1145SetIrMode(Si1145Range::HIGH, Si1145Gain::DIV_1, Si1145IrPhotodiode::LARGE);

   // interrupt enable
   ok |= writeByte(SI114X_INT_CFG, SI114X_INT_CFG_INTOE);  // enable interrupt pin
   ok |= writeByte(SI114X_IRQ_ENABLE, SI114X_IRQEN_ALS);

   return ok;
}

bool si1145StartMeasurement()
{
   return writeCommand(SI114X_ALS_FORCE);
}

std::optional<bool> si1145IsMeasurementFinished()
{
   const auto status = readByte(SI114X_IRQ_STATUS);
   if (status.has_value())
   {
      return status.value() & (1 << 0);
   }
   return {};
}

std::optional<Si1145Data> si1145ReadMeasurement()
{
   if (!waitForMeasurementCompleted())
   {
      return {};
   }

   clearInterrupt();

   const auto vis = readWord(SI114X_ALS_VIS_DATA0);
   const auto ir = readWord(SI114X_ALS_IR_DATA0);
   if (vis.has_value() && ir.has_value())
   {
      return Si1145Data{vis.value(), ir.value()};
   }
   else
   {
      return {};
   }
}

std::optional<uint16_t> si1145MeasureVis()
{
   if (!si1145StartMeasurement())
   {
      return {};
   }
   if (!waitForMeasurementCompleted())
   {
      return {};
   }
   clearInterrupt();
   return readWord(SI114X_ALS_VIS_DATA0);
}

bool si1145SetVisMode(Si1145Range range, Si1145Gain gain)
{
   if (gain > Si1145Gain::DIV_128)
   {
      return false;
   }

   const uint8_t gain_value = getGainValue(gain);
   const uint8_t complement = 0x07 ^ gain_value;
   return writeParamData(SI114X_ALS_VIS_ADC_GAIN, gain_value)
          && writeParamData(SI114X_ALS_VIS_ADC_COUNTER,
                            static_cast<uint8_t>(complement << 4))
          && writeParamData(SI114X_ALS_VIS_ADC_MISC,
                            range == Si1145Range::HIGH ? SI114X_HIGHRANGE : 0);
}

bool si1145SetIrMode(Si1145Range range, Si1145Gain gain, Si1145IrPhotodiode photodiode)
{
   const uint8_t gain_value = getGainValue(gain);
   const uint8_t complement = 0x07 ^ gain_value;
   bool isOk =
      writeParamData(SI114X_ALS_IR_ADC_GAIN, gain_value)
      && writeParamData(SI114X_ALS_IR_ADC_COUNTER, static_cast<uint8_t>(complement << 4))
      && writeParamData(SI114X_ALS_IR_ADC_MUX, getPhotodiodeValue(photodiode));
   const auto old_misc_value = readParamData(SI114X_ALS_IR_ADC_MISC);
   isOk &= old_misc_value.has_value();
   if (old_misc_value.has_value())
   {
      const uint8_t new_misc_value =
         (old_misc_value.value() & ~SI114X_HIGHRANGE)
         | (range == Si1145Range::HIGH ? SI114X_HIGHRANGE : 0);
      isOk &= writeParamData(SI114X_ALS_IR_ADC_MISC, new_misc_value);
   }
   return isOk;
}

std::optional<Si1145Range> si1145GetVisRange()
{
   const auto reg = readParamData(SI114X_ALS_VIS_ADC_MISC);
   if (reg.has_value())
   {
      return (reg.value() & SI114X_HIGHRANGE) ? Si1145Range::HIGH : Si1145Range::NORMAL;
   }
   return {};
}

std::optional<Si1145Gain> si1145GetVisGain()
{
   const auto gain_value = readParamData(SI114X_ALS_VIS_ADC_GAIN);
   if (gain_value.has_value())
   {
      return getGainFromValue(gain_value.value());
   }
   return {};
}

std::optional<Si1145Range> si1145GetIrRange()
{
   const auto reg = readParamData(SI114X_ALS_IR_ADC_MISC);
   if (reg.has_value())
   {
      return (reg.value() & SI114X_HIGHRANGE) ? Si1145Range::HIGH : Si1145Range::NORMAL;
   }
   return {};
}

std::optional<Si1145Gain> si1145GetIrGain()
{
   const auto gain_value = readParamData(SI114X_ALS_IR_ADC_GAIN);
   if (gain_value.has_value())
   {
      return getGainFromValue(gain_value.value());
   }
   return {};
}

std::optional<Si1145IrPhotodiode> si1145GetIrPhotoduiode()
{
   const auto mux_value = readParamData(SI114X_ALS_IR_ADC_MUX);
   if (mux_value.has_value())
   {
      return getPhotodiodeFromValue(mux_value.value());
   }
   return {};
}

namespace
{
void reset()
{
   writeByte(SI114X_MEAS_RATE0, 0);
   writeByte(SI114X_MEAS_RATE1, 0);
   writeByte(SI114X_IRQ_ENABLE, 0);
   writeByte(SI114X_IRQ_MODE1, 0);
   writeByte(SI114X_IRQ_MODE2, 0);
   writeByte(SI114X_INT_CFG, 0);
   writeByte(SI114X_IRQ_STATUS, 0xFF);

   writeByte(SI114X_COMMAND, SI114X_RESET);
   delay_ms(10);
   writeByte(SI114X_HW_KEY, 0x17);
   delay_ms(10);
}

std::optional<uint8_t> readByte(uint8_t reg)
{
   uint8_t value;
   if (i2cMasterWriteThenRead(SI114X_ADDR, &reg, 1, &value, 1) == I2cStatus::SUCCESS)
   {
      return value;
   }
   return {};
}

std::optional<uint16_t> readWord(uint8_t reg)
{
   uint8_t buf[2];
   if (i2cMasterWriteThenRead(SI114X_ADDR, &reg, 1, buf, 2) == I2cStatus::SUCCESS)
   {
      return buf[0] | (static_cast<uint16_t>(buf[1]) << 8);
   }
   return {};
}

bool writeByte(uint8_t reg, uint8_t value)
{
   const uint8_t buf[] = {reg, value};
   return i2cMasterWrite(SI114X_ADDR, buf, sizeof(buf)) == I2cStatus::SUCCESS;
}

std::optional<uint8_t> readParamData(uint8_t reg)
{
   if (writeCommand(SI114X_QUERY | reg))  // query command
   {
      // value is written to PARAM_RD
      return readByte(SI114X_RD);
   }
   return {};
}

bool writeParamData(uint8_t reg, uint8_t value)
{
   const bool is_ok = writeByte(SI114X_WR, value)  // write Value into PARAMWR reg first
                      && writeCommand(SI114X_SET | reg);  // then use set command
   if (is_ok)
   {
      // value is written to PARAM_RD, read and confirm it is correct
      const auto set_value = readByte(SI114X_RD);
      return set_value && set_value.value() == value;
   }
   return false;
}

bool writeCommand(uint8_t cmd)
{
   WriteCommandStatus status = WriteCommandStatus::RETRY;
   for (uint8_t try_count = 0; status == WriteCommandStatus::RETRY && try_count < 10;
        ++try_count)
   {
      status = writeCommandOnce(cmd);
   }

   return status == WriteCommandStatus::SUCCESS;
}

WriteCommandStatus writeCommandOnce(uint8_t cmd)
{
   if (!writeByte(SI114X_COMMAND, SI114X_NOP))  // clear response register
   {
      return WriteCommandStatus::FAILURE;
   }
   auto response = readByte(SI114X_RESPONSE);
   const bool is_ok = response.has_value()
                      && response.value() == 0x00  // verify response register cleared
                      && writeByte(SI114X_COMMAND, cmd);  // write to command register
   if (!is_ok)
   {
      return WriteCommandStatus::FAILURE;
   }

   for (uint8_t i = 0; i < 50; ++i)
   {
      response = readByte(SI114X_RESPONSE);
      if (!response.has_value())
      {
         return WriteCommandStatus::FAILURE;
      }
      if (response.value() > 0)
      {
         const bool is_error = (response.value() & 0xf0) != 0;
         const bool is_overflow = is_error && (response.value() & 0x0f) >= 0x08;
         // don't regard overflow as an error
         // it can easily be detected by checking if the value is 0xffff
         return !is_error || is_overflow ? WriteCommandStatus::SUCCESS
                                         : WriteCommandStatus::FAILURE;
      }
      delay_us(500);
   }
   // retry if response remains 0 for more than 25ms
   return WriteCommandStatus::RETRY;
}

bool waitForMeasurementCompleted()
{
   std::optional<bool> is_finished;
   do
   {
      is_finished = si1145IsMeasurementFinished();
   } while (is_finished.has_value() && !is_finished.value());
   return is_finished.has_value();
}

void clearInterrupt()
{
   const auto irq_status = readByte(SI114X_IRQ_STATUS);
   if (irq_status)
   {
      writeByte(SI114X_IRQ_STATUS, irq_status.value());
   }
}

inline uint8_t getGainValue(Si1145Gain gain)
{
   return static_cast<uint8_t>(gain);
}

inline Si1145Gain getGainFromValue(uint8_t gain)
{
   return static_cast<Si1145Gain>(gain);
}

uint8_t getPhotodiodeValue(Si1145IrPhotodiode photodiode)
{
   switch (photodiode)
   {
   case Si1145IrPhotodiode::SMALL:
      return 0x00;
   case Si1145IrPhotodiode::LARGE:
      return 0x03;
   }
   return 0;  // should not happen
}

Si1145IrPhotodiode getPhotodiodeFromValue(uint8_t photodiode)
{
   switch (photodiode)
   {
   case 0x00:
      return Si1145IrPhotodiode::SMALL;
   case 0x03:
      return Si1145IrPhotodiode::LARGE;
   }
   return Si1145IrPhotodiode::SMALL;  // should not happen
}
}  // namespace
