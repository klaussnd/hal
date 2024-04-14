/*  Hardware abstraction layer for embedded systems
 *  SHT3x series digital temperature and humidity sensors
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

#include <hal/sensor/temp_humi.h>

#include <stdint.h>
#include <variant>

enum class SHT3xError : uint8_t
{
   I2C,
   CRC,
};

struct SHT3xRawData
{
   uint16_t temperature;
   uint16_t humidity;
};

std::variant<uint32_t, SHT3xError> sht3xReadSerialNumber();
std::variant<SHT3xRawData, SHT3xError> sht3xMeasure();

TemperatureAndHumidity sht3xCompute(const SHT3xRawData& raw);
