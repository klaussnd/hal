#include <hal/sensor/mhz19.h>
#include <hal/usart.h>
#include <utils/programspace.h>

namespace
{
enum class Mhz19Command
{
   MHZ_CMND_READPPM = 0,
   MHZ_CMND_ABCENABLE,
   MHZ_CMND_ABCDISABLE,
   MHZ_CMND_ZEROPOINT,
   MHZ_CMND_RESET,
   MHZ_CMND_RANGE_1000,
   MHZ_CMND_RANGE_2000,
   MHZ_CMND_RANGE_3000,
   MHZ_CMND_RANGE_5000
};

const uint8_t Mhz19CommandData[][4] PROGMEM = {
   //  2     3    6    7
   {0x86, 0x00, 0x00, 0x00},   // mhz_cmnd_read_ppm
   {0x79, 0xA0, 0x00, 0x00},   // mhz_cmnd_abc_enable
   {0x79, 0x00, 0x00, 0x00},   // mhz_cmnd_abc_disable
   {0x87, 0x00, 0x00, 0x00},   // mhz_cmnd_zeropoint
   {0x8D, 0x00, 0x00, 0x00},   // mhz_cmnd_reset
   {0x99, 0x00, 0x03, 0xE8},   // mhz_cmnd_set_range_1000
   {0x99, 0x00, 0x07, 0xD0},   // mhz_cmnd_set_range_2000
   {0x99, 0x00, 0x0B, 0xB8},   // mhz_cmnd_set_range_3000
   {0x99, 0x00, 0x13, 0x88}};  // mhz_cmnd_set_range_5000

bool sendCommand(Mhz19Command command);
uint8_t computeChecksum(uint8_t* array);
}  // namespace

void mhz19StartMeasurement()
{
   sendCommand(Mhz19Command::MHZ_CMND_READPPM);
}

Mhz19Data mhz19GetMeasurement()
{
   uint8_t response[9];
   if (!usartReadExact(reinterpret_cast<char*>(response), sizeof(response)))
   {
      return Mhz19Data{Mhz19Status::USART_ERROR, 0, 0};
   }

   uint8_t crc = computeChecksum(response);
   if (response[8] != crc)
   {
      return Mhz19Data{Mhz19Status::CHECKSUM_ERROR, 0, 0};
   }
   if (response[0] != 0xff || response[1] != 0x86)
   {
      return Mhz19Data{Mhz19Status::BAD_RESPONSE_ERROR, 0, 0};
   }
   const uint16_t concentrationPpm = (response[2] << 8) | response[3];
   const int16_t temperatureCelsius = response[4] - 40;

   return Mhz19Data{Mhz19Status::SUCCESS, concentrationPpm, temperatureCelsius};
}

void mhz19enableAutomaticBaselineCorrection()
{
   sendCommand(Mhz19Command::MHZ_CMND_ABCENABLE);
}

void mhz19disableAutomaticBaselineCorrection()
{
   sendCommand(Mhz19Command::MHZ_CMND_ABCDISABLE);
}

namespace
{
bool sendCommand(Mhz19Command command)
{
   const auto cmdDataPtr = Mhz19CommandData[static_cast<uint8_t>(command)];
   uint8_t buf[9] = {
      0xff, 0x01, pgm_read_byte(cmdDataPtr),     pgm_read_byte(cmdDataPtr + 1),
      0,    0,    pgm_read_byte(cmdDataPtr + 2), pgm_read_byte(cmdDataPtr + 3),
      0};
   buf[8] = computeChecksum(buf);
   return usartWrite(reinterpret_cast<char*>(buf), 9) == 9;
}

uint8_t computeChecksum(uint8_t* buf)
{
   uint8_t checksum = 0;
   for (uint8_t index = 1; index < 8; ++index)
   {
      checksum += buf[index];
   }
   checksum = 255 - checksum;
   return checksum + 1;
}
}  // namespace
