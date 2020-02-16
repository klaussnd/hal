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

struct Si1145Data
{
   uint16_t vis;
   uint16_t ir;
   uint16_t uv_index;
};

bool si1145Init();
std::optional<Si1145Data> si1145Measure();
