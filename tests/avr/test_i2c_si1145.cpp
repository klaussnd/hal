#include <hal/i2c_master.h>
#include <hal/sensor/si1145.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

int main(void)
{
   usartInit<115200>();
   sei();

   i2cMasterInit();
   if (!si1145Init())
   {
      usartWriteString_P(PSTR("Init error\n"));
      return 0;
   }

   while (1)
   {
      const auto values = si1145Measure();
      if (values)
      {
         fprintf_P(usart_stdout, PSTR("VIS %d, IR %d, UV index %d/100\n"), values->vis,
                   values->ir, values->uv_index);
      }
      else
      {
         usartWriteString_P(PSTR("Measurement failed\n"));
      }
      _delay_ms(500);
   }
   return 0;
}
