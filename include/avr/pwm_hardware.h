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

#include <timer.h>

/** Set PWM value
 *  @e no_ch  timer number and channel (e.g. 0,A)
 *  @e val    value
 */
#define pwmSet(no_ch, val) TMR_SET_OCR2(no_ch, val)

/** Get PWM value */
#define pwmGet(no_ch) TMR_GET_OCR2(no_ch)

/** Set prescaler for PWM timer */
#define pwmSetPrescaler(no_ch, val) TMR_SET_PRESCALER(no_ch, val)

/** Stop PWM
 *  Equivalent to calling pwmSet(no_ch, 0)
 */
#define pwmStop(no_ch) pwmSet(no_ch, 0)

enum class PwmOutput
{
   OC0A = 0x01,
   OC0B = 0x02,
   OC2A = 0x04,
   OC2B = 0x08,
};

enum class PwmDirection
{
   Normal,
   Inverted
};

/** Initialise hardware PWM */
template <PwmOutput output, PwmDirection direction>
void pwmInit();

#include "pwm_hardware_priv.h"
