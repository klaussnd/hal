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

#if defined __GNUC__ && __GNUC__ < 7
 #include <experimental/optional>
 namespace std
 {
   template <typename T>
   using optional = experimental::optional<T>;
 }
#else
 #include <optional>
#endif

#include <stdint.h>

struct Hyt939Data
{
   uint8_t humidity; ///< percent
   int16_t temperature; ///< decicelsius
};

std::optional<Hyt939Data> hyt939Measure();
