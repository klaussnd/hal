/*  Hardware abstraction layer for embedded systems
 *  Hygrosens Hyt939 temperature and humidity sensor
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
 */

#pragma once

#include <optional>
#include <stdint.h>

struct Hyt939Data
{
   uint8_t humidity;     ///< percent
   int16_t temperature;  ///< decicelsius
};

std::optional<Hyt939Data> hyt939Measure();

struct Hyt939RawData
{
   uint16_t humidity;
   uint16_t temperature;
};
std::optional<Hyt939RawData> hyt939MeasureRaw();
Hyt939Data hyt939Compute(const Hyt939RawData& raw);
