#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
   usartInit<19200>();
   sei();

   while(1)
   {
      usartWriteString_P(PSTR("."));
      if (usartAvailReadLine())
      {
         char buf[16];

         const auto count = usartReadLine(buf, sizeof(buf));
         fprintf_P(usart_stdout, PSTR("Received %hhu Bytes: "), count);
         usartWriteString(buf);
         usartWriteString_P(PSTR("\n"));
      }
      _delay_ms(1000);
   }

   return 0;
}
