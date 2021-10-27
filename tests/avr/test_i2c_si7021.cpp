#include <hal/i2c_master.h>
#include <hal/sensor/si7021.h>
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
   while (!si7021Init())
   {
      usartWriteString_P(PSTR("Init error\n"));
      _delay_ms(5000);
   }

   while (1)
   {
      const uint16_t temp = si7021Temperature();
      const uint8_t humidity = si7021Humidity();
      fprintf_P(usart_stdout, PSTR("T %d rH %d\n"), temp, humidity);
      _delay_ms(500);
   }

   return 0;
}
