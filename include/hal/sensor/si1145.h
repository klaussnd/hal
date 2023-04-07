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
};

enum class Si1145Gain : uint8_t
{
   DIV_1 = 0,
   DIV_2 = 1,
   DIV_4 = 2,
   DIV_8 = 3,
   DIV_16 = 4,
   DIV_32 = 5,
   DIV_64 = 6,
   DIV_128 = 7,
};

enum class Si1145IrPhotodiode : uint8_t
{
   SMALL,
   LARGE
};

enum class Si1145Range : uint8_t
{
   NORMAL,
   HIGH
};

enum class Si1145AutoExposureResult : uint8_t
{
   CHANGED, ///< range or gain was modified
   KEPT,    ///< all settings were left unchanged
   ERROR,   ///< an error occured while tying to run the auto-exposure
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
/// Convenience function: starts a measurement, waits for its completion,
/// and returns the measured value.
/// @return VIS value, or an empty optional in case of error
std::optional<uint16_t> si1145MeasureVis();

bool si1145SetVisMode(Si1145Range range, Si1145Gain gain);
bool si1145SetIrMode(Si1145Range range, Si1145Gain gain, Si1145IrPhotodiode photodiode);

std::optional<Si1145Range> si1145GetVisRange();
std::optional<Si1145Gain> si1145GetVisGain();
std::optional<Si1145Range> si1145GetIrRange();
std::optional<Si1145Gain> si1145GetIrGain();
std::optional<Si1145IrPhotodiode> si1145GetIrPhotoduiode();

// Auto-exposure functions (implemented in si1145_autoexp.cpp)

/// Run auto-exposure to select gain and range
Si1145AutoExposureResult si1145VisAutoExposure();

// Utility functions (implemented in si1145_helper.cpp)

/// Return the logical value, i.e. the gain as a number
uint8_t logicalValue(Si1145Gain gain);
/// Return a string describing the range value
/// On AVR the string is stored in program memory (flash).
const char* asString(Si1145Range range);
/// Return a string describing the IR photodiode value
/// On AVR the string is stored in program memory (flash).
const char* asString(Si1145IrPhotodiode ir_photodiode);
