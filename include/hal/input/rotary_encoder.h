/*  Hardware abstraction layer for embedded systems
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

#include <stdint.h>

void rotaryEncoderInit();
/** Return change in rotation since last call.
 *
 *  Negative values indicate a rotation to the left, positive values to the right.
 */
int8_t rotaryEncoderGetDelta();

/// Hardware sampling function. Must be called every 1ms
void rotaryEncoderSample();
