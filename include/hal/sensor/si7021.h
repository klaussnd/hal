/*  Hardware abstraction layer for embedded systems
 *  SI7021 temperature and humidity sensor
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

bool si7021Init();
/// @return Temperature in decicelsius
uint16_t si7021Temperature();
/// @return Relative humidity in percent
uint16_t si7021Humidity();
