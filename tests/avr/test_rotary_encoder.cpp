#include <avr/timer.h>
#include <hal/input/rotary_encoder.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <limits>
#include <stdlib.h>

void initTimer0To1msIrq();

int main(void)
{
   usartInit<19200>();
   rotaryEncoderInit();
   sei();
   initTimer0To1msIrq();

   uint8_t value = 0;

   while (1)
   {
      const int8_t delta = rotaryEncoderGetDelta();
      if ((delta > 0 && value <= std::numeric_limits<uint8_t>::max() - delta)
          || (delta < 0 && value >= -delta))
      {
         value += delta;
         fprintf_P(usart_stdout, PSTR("%d\n"), value);
      }
   }

   return 0;
}

void initTimer0To1msIrq()
{
#define PRESCALER 64
   TMR_SET_PRESCALER(0, PRESCALER);
   TMR_SET_OCR2(0, A, TMR_MSEC2TIMER(1, PRESCALER));
   TMR_SET_COMP_IRQ(0, A);
   TMR_SET_NORMAL(0);
}

TMR_COMP_ISR(0, A)
{
   rotaryEncoderSample();
}
