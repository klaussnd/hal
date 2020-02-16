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
// ADC GAIN DIV
#define SI114X_ADC_GAIN_DIV1 0X00
#define SI114X_ADC_GAIN_DIV2 0X01
#define SI114X_ADC_GAIN_DIV4 0X02
#define SI114X_ADC_GAIN_DIV8 0X03
#define SI114X_ADC_GAIN_DIV16 0X04
#define SI114X_ADC_GAIN_DIV32 0X05
// LED CURRENT
#define SI114X_LED_CURRENT_5MA 0X01
#define SI114X_LED_CURRENT_11MA 0X02
#define SI114X_LED_CURRENT_22MA 0X03
#define SI114X_LED_CURRENT_45MA 0X04
// Recovery period the  ADC takes before making a PS measurement
#define SI114X_ADC_COUNTER_1ADCCLK 0X00
#define SI114X_ADC_COUNTER_7ADCCLK 0X01
#define SI114X_ADC_COUNTER_15ADCCLK 0X02
#define SI114X_ADC_COUNTER_31ADCCLK 0X03
#define SI114X_ADC_COUNTER_63ADCCLK 0X04
#define SI114X_ADC_COUNTER_127ADCCLK 0X05
#define SI114X_ADC_COUNTER_255ADCCLK 0X06
#define SI114X_ADC_COUNTER_511ADCCLK 0X07
// ADC MISC
#define SI114X_ADC_MISC_LOWRANGE 0X00
#define SI114X_ADC_MISC_HIGHRANGE 0X20
#define SI114X_ADC_MISC_ADC_NORMALPROXIMITY 0X00
#define SI114X_ADC_MISC_ADC_RAWADC 0X04
// INT OE
#define SI114X_INT_CFG_INTOE 0X01
// IRQ ENABLE
#define SI114X_IRQEN_ALS 0x01
#define SI114X_IRQEN_PS1 0x04
#define SI114X_IRQEN_PS2 0x08
#define SI114X_IRQEN_PS3 0x10

#define SI114X_ADDR 0X60

namespace
{
uint8_t readByte(uint8_t reg);
uint16_t readWord(uint8_t reg);
bool writeByte(uint8_t reg, uint8_t value);
uint8_t writeParamData(uint8_t reg, uint8_t value);

void reset();
}  // namespace

bool si1145Init()
{
   if (readByte(SI114X_PART_ID) != 0x45)
   {
      return false;
   }

   reset();

   // ENABLE UV reading
   // these reg must be set to the fixed value
   writeByte(SI114X_UCOEFF0, 0x29);
   writeByte(SI114X_UCOEFF1, 0x89);
   writeByte(SI114X_UCOEFF2, 0x02);
   writeByte(SI114X_UCOEFF3, 0x00);
   writeParamData(SI114X_CHLIST, SI114X_CHLIST_ENUV | SI114X_CHLIST_ENALSIR
                                    | SI114X_CHLIST_ENALSVIS | SI114X_CHLIST_ENPS1);

   // PS ADC SETTING
   writeParamData(SI114X_PS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
   writeParamData(SI114X_PS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
   writeParamData(SI114X_PS_ADC_MISC,
                  SI114X_ADC_MISC_HIGHRANGE | SI114X_ADC_MISC_ADC_RAWADC);

   // VIS ADC SETTING
   writeParamData(SI114X_ALS_VIS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
   writeParamData(SI114X_ALS_VIS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
   writeParamData(SI114X_ALS_VIS_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);

   // IR ADC SETTING
   writeParamData(SI114X_ALS_IR_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
   writeParamData(SI114X_ALS_IR_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
   writeParamData(SI114X_ALS_IR_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);

   // interrupt enable
   writeByte(SI114X_INT_CFG, SI114X_INT_CFG_INTOE);
   writeByte(SI114X_IRQ_ENABLE, SI114X_IRQEN_ALS);

   // AUTO RUN
   writeByte(SI114X_MEAS_RATE0, 0xFF);
   writeByte(SI114X_COMMAND, SI114X_PSALS_AUTO);
   return true;
}

std::optional<Si1145Data> si1145Measure()
{
   const uint16_t vis = readWord(SI114X_ALS_VIS_DATA0);
   const uint16_t ir = readWord(SI114X_ALS_IR_DATA0);
   const uint16_t uv = readWord(SI114X_AUX_DATA0_UVINDEX0);
   return Si1145Data{vis, ir, uv};
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

uint8_t readByte(uint8_t reg)
{
   uint8_t value;
   if (i2cMasterWriteThenRead(SI114X_ADDR, &reg, 1, &value, 1) == I2cStatus::SUCCESS)
   {
      return value;
   }
   return 0xff;
}

uint16_t readWord(uint8_t reg)
{
   uint8_t buf[2];
   if (i2cMasterWriteThenRead(SI114X_ADDR, &reg, 1, buf, 2) == I2cStatus::SUCCESS)
   {
      return buf[0] | (static_cast<uint16_t>(buf[1]) << 8);
   }
   return 0xffff;
}

bool writeByte(uint8_t reg, uint8_t value)
{
   const uint8_t buf[] = {reg, value};
   return i2cMasterWrite(SI114X_ADDR, buf, sizeof(buf)) == I2cStatus::SUCCESS;
}

uint8_t writeParamData(uint8_t reg, uint8_t value)
{
   // write Value into PARAMWR reg first
   writeByte(SI114X_WR, value);
   writeByte(SI114X_COMMAND, reg | SI114X_SET);
   // SI114X writes value out to PARAM_RD,read and confirm its right
   return readByte(SI114X_RD);
}
}  // namespace
