#ifndef ADC_H
#define ADC_H

#include <inttypes.h>

/** Read from the analogue-digital converter (ADC)
 *  @param channel  The ADC channel to read
 *  @note On AVR, the MCU is set into sleep mode to reduce noise during the conversion.
 *        It is the caller's responsibility to select the correct sleep mode before
 *        calling this function.
 */
uint16_t adcRead(uint8_t channel);

#endif
