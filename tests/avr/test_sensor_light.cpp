#include <hal/i2c_master.h>
#include <hal/sensor/si1145.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdlib.h>

enum class MeasurementType
{
   NONE,
   CONTINUOUS,
   AUTOEXPOSURE,
   EXPOSURE_SERIES,
};

struct Settings
{
   MeasurementType measure{MeasurementType::NONE};
};

void makeSingleMeasurement();
void makeAutoExposureMeasurement();
void makeExposureSeries();
void handleInput(Settings& settings);

int main(void)
{
   usartInit<115200>();
   sei();

   i2cMasterInit();
   if (!si1145Init())
   {
      usartWriteString_P(PSTR("Init error\n"));
      return 0;
   }
   usartWriteString_P(PSTR("Init done\n"));

   Settings settings;
   while (1)
   {
      if (usartIsLineAvailableToRead())
      {
         handleInput(settings);
      }

      switch (settings.measure)
      {
      case MeasurementType::CONTINUOUS:
         makeSingleMeasurement();
         break;
      case MeasurementType::AUTOEXPOSURE:
         makeAutoExposureMeasurement();
         settings.measure = MeasurementType::NONE;
         break;
      case MeasurementType::EXPOSURE_SERIES:
         makeExposureSeries();
         settings.measure = MeasurementType::NONE;
         break;
      case MeasurementType::NONE:
         break;
      }

      _delay_ms(500);
   }
}

void makeSingleMeasurement()
{
   if (!si1145StartMeasurement())
   {
      usartWriteString_P(PSTR("Start meas failed\n"));
      return;
   }
   const auto values = si1145ReadMeasurement();
   if (values)
   {
      fprintf_P(usart_stdout, PSTR("VIS %d, IR %d\n"), values->vis, values->ir);
   }
   else
   {
      usartWriteString_P(PSTR("Measurement failed\n"));
   }
}

void makeAutoExposureMeasurement()
{
   const auto autoexp_res = si1145VisAutoExposure();
   if (autoexp_res == Si1145AutoExposureResult::ERROR)
   {
      usartWriteString_P(PSTR("Auto-exposure failed\n"));
      return;
   }
   const auto raw = si1145MeasureVis();
   const auto range = si1145GetVisRange();
   const auto gain = si1145GetVisGain();
   if (raw.has_value() && range.has_value() && gain.has_value())
   {
      usartWriteString_P(asString(range.value()));
      fprintf_P(usart_stdout, PSTR(",%d,%d\n"), logicalValue(gain.value()), raw.value());
   }
   else
   {
      usartWriteString_P(PSTR("Measurement failed\n"));
   }
}

void makeExposureSeries()
{
   const Si1145Gain gains[] PROGMEM = {
      Si1145Gain::DIV_1,  Si1145Gain::DIV_2,  Si1145Gain::DIV_4,  Si1145Gain::DIV_8,
      Si1145Gain::DIV_16, Si1145Gain::DIV_32, Si1145Gain::DIV_64, Si1145Gain::DIV_128};
   constexpr uint8_t gain_count = sizeof(gains) / sizeof(gains[0]);
   constexpr uint8_t range_count = 2;
   constexpr uint8_t ir_photodiode_count = 2;

   usartWriteString_P(PSTR("range,gain,ir_photodiode,vis,ir"));
   for (uint8_t gain_index = 0; gain_index < gain_count; ++gain_index)
   {
      const auto gain = static_cast<Si1145Gain>(pgm_read_byte(gains + gain_index));

      for (uint8_t range_index = 0; range_index < range_count; ++range_index)
      {
         const auto range = static_cast<Si1145Range>(range_index);

         for (uint8_t ir_diode_index = 0; ir_diode_index < ir_photodiode_count;
              ++ir_diode_index)
         {
            const auto ir_photodiode = static_cast<Si1145IrPhotodiode>(ir_diode_index);

            if (!si1145SetVisMode(range, gain))
            {
               usartWriteString_P(PSTR("Unable to set VIS mode!\n"));
               return;
            }
            if (!si1145SetIrMode(range, gain, ir_photodiode))
            {
               usartWriteString_P(PSTR("Unable to set IR mode!\n"));
               return;
            }
            _delay_ms(64);

            for (int i = 0; i < 10; ++i)
            {
               bool ok = false;
               for (int try_count = 0; !(ok = si1145StartMeasurement()) && try_count < 10;
                    ++try_count)
               {
                  usartWriteString_P(PSTR("Unable to start measurement, try again\n"));
                  _delay_ms(64);
               }
               if (!ok)
               {
                  usartWriteString_P(PSTR("Failed\n"));
                  return;
               }
               const auto meas = si1145ReadMeasurement();
               if (meas)
               {
                  usartWriteString_P(asString(range));
                  fprintf_P(usart_stdout, PSTR(",%d,"), logicalValue(gain));
                  usartWriteString_P(asString(ir_photodiode));
                  fprintf_P(usart_stdout, PSTR(",%d,%d"), meas.value().vis,
                            meas.value().ir);
               }
               else
               {
                  usartWriteString_P(PSTR("Unable to read measurements"));
               }
               usartWriteString_P(PSTR("\n"));
               _delay_ms(250);
            }
         }
      }
   }
}

constexpr uint8_t MAX_TOKEN_COUNT = 3;

uint8_t splitArguments(char* buf, char* tokens[]);
std::optional<Si1145Range> rangeFromString(const char* str);
std::optional<Si1145Gain> gainFromLogicalValue(uint8_t value);

#define assertTokenCount(expected_count)                                                 \
   if (token_count != expected_count)                                                    \
   {                                                                                     \
      fprintf_P(usart_stdout, PSTR("Incorrect number of arguments %d\n"), token_count);  \
      return;                                                                            \
   }

void handleInput(Settings& settings)
{
   char buf[16];
   const uint8_t byte_count = usartReadLine(buf, sizeof(buf) - 1);
   buf[byte_count] = '\0';
   char* tokens[MAX_TOKEN_COUNT + 1];
   const uint8_t token_count = splitArguments(buf, tokens);
   if (token_count == 0)
   {
      usartWriteString_P(PSTR("Empty input\n"));
      return;
   }

   switch (*tokens[0])
   {
   case '?':
      usartWriteString_P(PSTR("m - start measurement\n"
                              "s - stop\n"
                              "e - exposure sweep\n"
                              "a - auto exposure measurement\n"
                              "v <range> <gain> - set vis\n"));
      break;
   case 'm':
      settings.measure = MeasurementType::CONTINUOUS;
      break;
   case 'a':
      settings.measure = MeasurementType::AUTOEXPOSURE;
      break;
   case 's':
      settings.measure = MeasurementType::NONE;
      break;
   case 'e':
      settings.measure = MeasurementType::EXPOSURE_SERIES;
      break;
   case 'v':
   {
      assertTokenCount(3);
      const auto range = rangeFromString(tokens[1]);
      const auto gain = gainFromLogicalValue(static_cast<uint8_t>(atoi(tokens[2])));
      if (range && gain)
      {
         if (si1145SetVisMode(range.value(), gain.value()))
         {
            usartWriteString_P(PSTR("Set VIS range "));
            usartWriteString_P(asString(range.value()));
            fprintf_P(usart_stdout, PSTR(" gain %d\n"), logicalValue(gain.value()));
         }
      }
   }
   }
}

uint8_t splitArguments(char* buf, char* tokens[])
{
   const char* delimiter_chars = PSTR(" \t\n");
   char* last;
   uint8_t count = 0;
   tokens[count] = strtok_rP(buf, delimiter_chars, &last);
   while (tokens[count] && count < MAX_TOKEN_COUNT)
   {
      ++count;
      tokens[count] = strtok_rP(nullptr, delimiter_chars, &last);
   }
   for (uint8_t i = count + 1; i <= MAX_TOKEN_COUNT; ++i)
   {
      tokens[i] = nullptr;
   }

   return count;
}

std::optional<Si1145Range> rangeFromString(const char* str)
{
   if (strcasecmp_P(str, PSTR("normal")) == 0)
   {
      return Si1145Range::NORMAL;
   }
   else if (strcasecmp_P(str, PSTR("high")) == 0)
   {
      return Si1145Range::HIGH;
   }
   usartWriteString_P(PSTR("Invalid range "));
   usartWriteString(str);
   usartWriteString_P(PSTR("\n"));
   return {};
}

std::optional<Si1145Gain> gainFromLogicalValue(uint8_t value)
{
   switch (value)
   {
   case 1:
      return Si1145Gain::DIV_1;
   case 2:
      return Si1145Gain::DIV_2;
   case 4:
      return Si1145Gain::DIV_4;
   case 8:
      return Si1145Gain::DIV_8;
   case 16:
      return Si1145Gain::DIV_16;
   case 32:
      return Si1145Gain::DIV_32;
   case 64:
      return Si1145Gain::DIV_64;
   case 128:
      return Si1145Gain::DIV_128;
   default:
      fprintf_P(usart_stdout, PSTR("Invalid gain %d\n"), value);
      return {};
   }
}
