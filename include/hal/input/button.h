/*  Debounced detection of max. 8 buttons, sampling 4 times, with repeat function
 *
 *  Original author: Peter Dannegger <danni@specs.de>
 *  Adjusted by Klaus Schneider-Zapp klaus underscore snd at web dot de
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

void buttonInit(void);

/** Check if a button was pressed short
 *  @param button_mask  Mask of buttons to check, can be a subset of all sampled buttons
 *  @return Mask of buttons that have been pressed (i.e. 0 if none)
 */
uint8_t buttonShort(uint8_t button_mask);

/// Check if a button was pressed long
uint8_t buttonLong(uint8_t button_mask);

/** Check if a button has been pressed long enough such that the
 *  button repeat functionality kicks in. After a small setup delay
 *  the button is reported beeing pressed in subsequent calls
 *  to this function. This simulates the user repeatedly
 *  pressing and releasing the button.
 */
uint8_t buttonRpt(uint8_t button_mask);

/** Check if a button has been pressed. Each pressed button is reported
 *  only once.
 */
uint8_t buttonPress(uint8_t button_mask);

/// Return button state
uint8_t buttonDown(uint8_t button_mask);

/// Sampling function, must be called every 10ms
void buttonSample(void);
