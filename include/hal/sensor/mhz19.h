/*  Hardware abstraction layer for embedded systems
 *  MH-Z19(B) CO2 sensor
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

#include <stdint.h>

enum class Mhz19Status
{
   SUCCESS,
   USART_ERROR,
   CHECKSUM_ERROR,
   BAD_RESPONSE_ERROR,
};

struct Mhz19Data
{
   Mhz19Status status;
   uint16_t co2Ppm;
   int16_t temperature;
};

constexpr unsigned long MHZ19_BAUDRATE = 9600;

void mhz19StartMeasurement();
/// Retrieve data; measurement must be triggered by @e mhz19StartMeasurement() first.
/// Returned data are only valid if status == SUCCESS
Mhz19Data mhz19GetMeasurement();
void mhz19enableAutomaticBaselineCorrection();
void mhz19disableAutomaticBaselineCorrection();
