#pragma once

#include <hal/i2c_master.h>

#include <stdint.h>
#include <variant>

enum class Abp2Status
{
   BUSY,         ///< The device is busy, try again later
   I2C_FAILURE,  ///< A failure occured during the i2c communication
};

struct Abp2RawData
{
   uint32_t pressure;
   uint32_t temperature;
};

I2cStatus abp2StartMeasurement();
/** Obtain raw measurement
 *  Compute pressure according to eq. 2 of the datasheet:
 *  pressure = (raw - outputmin) * (Pmax - Pmin) / (outputmax - outputmin) + Pmin
 *  where outputmin = 1677722, outputmax = 15099494, and Pmin and Pmax depend on model.
 *  Compute the temperature according to eq. 3.
 *  @return The raw data, or the status if now data could be obtained
 */
std::variant<Abp2Status, Abp2RawData> abp2GetMeasurement();
