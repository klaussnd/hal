#include <hal/i2c_master.h>
#include <hal/sensor/hyt939.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

int main(void)
{
   usartInit<4800>();
   sei();

   i2cMasterInit();

   while (1)
   {
      const auto meas = hyt939Measure();
      if (meas.has_value())
      {
         fprintf_P(usart_stdout, PSTR("T %d rH %d\n"), meas.value().temperature,
                   meas.value().humidity);
      }
      else
      {
         usartWriteString_P(PSTR("failed\n"));
      }
      _delay_ms(500);
   }

   return 0;
}
