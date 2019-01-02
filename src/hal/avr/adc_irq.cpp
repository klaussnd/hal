#include <hal/adc.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#if AVR_ARCH == 2
 #if defined ADC_INTERNAL_REF
  #define ADC_REF_MASK (1 << REFS1) /* internal band gap 1.1V */
 #else
  #define ADC_REF_MASK 0
 #endif
 #define ADC_MUX_MASK 0x3f
#elif defined __AVR_ATmega8__ || defined __AVR_ATmega16__ || defined __AVR_ATmega32__    \
   || defined __AVR_ATmega168__ || defined __AVR_ATmega168P__                            \
   || defined __AVR_ATmega328P__
 #if defined ADC_REF_AVCC
  #define ADC_REF_MASK (1 << REFS0) /* AVcc with external capacitor on AREF pin */
 #elif defined ADC_INTERNAL_REF
  #define ADC_REF_MASK (1 << REFS1) | (1 << REFS0) /* internal band gap 1.1V or 2.56V */
 #else
  #define ADC_REF_MASK 0
 #endif
 #define ADC_MUX_MASK 0x0f
#else
 #error Unknown MCU
#endif

#define MEASUREMENT_COUNT 5

namespace
{
   struct AdcData
   {
      uint16_t value;
      uint8_t busy; /* ADC is busy */
   };

   volatile AdcData m_adc_data;

   void adcActivate(uint8_t channel);
   uint16_t adcTriggerConversionAndWaitForResult();
   void adcDeactivate();
}

uint16_t adcRead(const uint8_t channel)
{
   adcActivate(channel);

   adcTriggerConversionAndWaitForResult(); /* dummy conversion */
   uint16_t value = 0;
   for (uint8_t count = 0; count < MEASUREMENT_COUNT; ++count)
   {
      value += adcTriggerConversionAndWaitForResult();
   }

   adcDeactivate();
   return value / MEASUREMENT_COUNT;
}

namespace
{
   inline void adcActivate(const uint8_t channel)
   {
      /* select channel and reference voltage */
      ADMUX = ADC_REF_MASK | (channel & ADC_MUX_MASK);
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

#if defined ADC_INTERNAL_REF && defined __AVR_ATtiny44__
      _delay_ms(1); /* wait 1ms for the reference voltage to settle */
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
}

/* Interrupt handler for ADC ready */
ISR(ADC_vect)
{
   m_adc_data.value = ADCW;
   m_adc_data.busy = 0;
}
