#include <hal/adc.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

namespace
{
constexpr uint8_t ADC_REF_MASK = ((1 << REFS1) | (1 << REFS0));

struct AdcData
{
   uint16_t value;
   uint8_t busy; /* ADC is busy */
};

volatile AdcData m_adc_data;

void adcActivate(uint8_t channel);
uint16_t adcTriggerConversionAndWaitForResult();
void adcDeactivate();
}  // namespace

void adcSetReference(AdcReference ref)
{
   ADMUX = (ADMUX & ~ADC_REF_MASK) | static_cast<uint8_t>(ref);
}

uint16_t adcRead(const uint8_t channel, const uint8_t meas_count)
{
   adcActivate(channel);

   adcTriggerConversionAndWaitForResult(); /* dummy conversion */
   uint16_t value = 0;
   for (uint8_t idx = 0; idx < meas_count; ++idx)
   {
      value += adcTriggerConversionAndWaitForResult();
   }

   adcDeactivate();
   return value / meas_count;
}

namespace
{
inline void adcActivate(const uint8_t channel)
{
   /* select channel, keep reference voltage selection */
   ADMUX = (ADMUX & ADC_REF_MASK) | (channel & ADC_MUX_MASK);
   /* enable ADC, set interrupt,
      set prescaler for lowest possible clock (min. 50kHz) */
   ADCSRA = (1 << ADEN) | (1 << ADIE) |
#if (F_CPU > 6400000UL) /* f > 6.4MHz: 128 */
            (1 << ADPS0) |
#endif
#if (F_CPU > 3200000UL)
            (1 << ADPS1) | /* 3.2MHz < f <= 6.4MHz: 64 */
#endif
            (1 << ADPS2); /* f <= 3.2 MHz: 32 */

#if defined __AVR_ATtiny44__
   if ((ADMUX & ADC_REF_MASK) == ADC_REF_INTERNAL)
   {
      _delay_ms(1); /* wait 1ms for the reference voltage to settle */
   }
#endif

   /* Note: If we use the TWI in slave mode, we need to temporarily disable it,
      since the TWI may lock up if the ADC interrupt occurs during a
      TWI address match in sleep mode. */
}

inline uint16_t adcTriggerConversionAndWaitForResult()
{
   ADCSRA |= (1 << ADSC); /* start single conversion */
   m_adc_data.busy = 1;
   while (m_adc_data.busy)
   {
      sleep_mode();
   }
   return m_adc_data.value;
}

inline void adcDeactivate()
{
   ADCSRA &= ~(1 << ADEN);
}
}  // namespace

/* Interrupt handler for ADC ready */
ISR(ADC_vect)
{
   m_adc_data.value = ADCW;
   m_adc_data.busy = 0;
}
