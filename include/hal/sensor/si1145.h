/*  Hardware abstraction layer for embedded systems
 *  SI1145 light sensor
 *
 *  (c) Klaus Schneider-Zapp klaus underscore snd at web dot de
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 */

#pragma once

#include <optional>
#include <stdint.h>

struct Si1145Data
{
   uint16_t vis;
   uint16_t ir;
   uint16_t uv_index;
};

enum class Si1145Gain
{
   DIV_1,
   DIV_2,
   DIV_4,
   DIV_8,
   DIV_16,
   DIV_32,
   DIV_64,
   DIV_128
};

enum class Si1145IrPhotodiode
{
   SMALL,
   LARGE
};

enum class Si1145Range
{
   NORMAL,
   HIGH
};

/// Initialises the sensor
/// @return false on error
bool si1145Init();
bool si1145StartMeasurement();
/// Check if the measurement is finished.
/// @return true if the measurement is finished, false if it is not finished,
///         or an empty optional if an error occured.
std::optional<bool> si1145IsMeasurementFinished();
/// Read measurements, waiting for measurements to complete if necessary.
/// Must call si1145StartMeasurement() before.
/// @return The measured data, or an empty optional if an error occured.
std::optional<Si1145Data> si1145ReadMeasurement();
bool si1145SetVisMode(Si1145Range range, Si1145Gain gain);
bool si1145SetIrMode(Si1145Range range, Si1145Gain gain, Si1145IrPhotodiode photodiode);
