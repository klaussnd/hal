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

#include <inttypes.h>

#ifdef __AVR
#include "avr/adc_priv.h"

enum class AdcReference : uint8_t
{
   AVCC = ADC_REF_AVCC,                  ///< AVcc with external capacitor on AREF pin
   INTERNAL_BANDGAP = ADC_REF_INTERNAL,  ///< internal band gap 1.1V or 2.56V
   EXTERNAL_AREF = ADC_REF_EXTERNAL,     ///< external reference on AREF
};
/** Set the reference for the analogue-digital converter */
void adcSetReference(AdcReference ref);
#endif

/** Read from the analogue-digital converter (ADC)
 *  @param channel     The ADC channel to read
 *  @param meas_count  Number of measurements to average
 *  @note On AVR, the MCU is set into sleep mode to reduce noise during the conversion.
 *        It is the caller's responsibility to select the correct sleep mode before
 *        calling this function.
 */
uint16_t adcRead(uint8_t channel, uint8_t meas_count = 1);
