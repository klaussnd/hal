#include <hal/adc.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include <stdlib.h>

int main(void)
{
   adcSetReference(AdcReference::AVCC);
   set_sleep_mode(SLEEP_MODE_ADC);
   usartInit<19200>();
   sei();

   while (1)
   {
      usartWriteString_P(PSTR("."));
      if (usartIsLineAvailableToRead())
      {
         char buf[16];

         usartReadLine(buf, sizeof(buf));
         const uint8_t channel = atoi(buf);
         fprintf_P(usart_stdout, PSTR("\nAcquiring from ADC %u ..."), channel);
         const auto value = adcRead(channel);
         fprintf_P(usart_stdout, PSTR(" value %u\n"), value);
      }
      _delay_ms(100);
   }

   return 0;
}
