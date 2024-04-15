#pragma once

#include <stdint.h>

struct TemperatureAndHumidity
{
   uint8_t humidity;     ///< percent
   int16_t temperature;  ///< decicelsius
};
