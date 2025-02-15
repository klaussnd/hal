/*  Hardware abstraction layer for embedded systems
 *  ADT7410 temperature sensor
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

/** Initialise temperature sensor
 *  Select measurement once a second and 16bit accuracy
 */
bool adt7410Init(void);

/** Read temperature sensor
 *  @return Temperature as deci-Celsius on success, an empty optional on failure
 */
std::optional<int16_t> adt7410ReadTemp();

/** Check if the temperature sensor is busy doing measurements
 *  Return If the sensor is busy, or an empty optional on error.
 */
std::optional<bool> adt7410IsBusy();
