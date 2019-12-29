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
