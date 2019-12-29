#include <avr/timer.h>
#include <hal/input/button.h>
#include <hal/input/rotary_encoder.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <limits>
#include <stdlib.h>

#define BUTTON (1 << 6)

void initTimer0To1msIrq();

int main(void)
{
   usartInit<19200>();
   rotaryEncoderInit();
   buttonInit();
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
      if (buttonShort(BUTTON))
      {
         usartWriteString_P(PSTR("press\n"));
      }
   }

   return 0;
}

void initTimer0To1msIrq()
{
#define PRESCALER 64
   TMR_SET_PRESCALER(0, PRESCALER);
   TMR_SET_OCR2(0, A, TMR_MSEC2TIMER(1, PRESCALER) - 1);
   TMR_SET_COMP_IRQ(0, A);
   TMR_SET_NORMAL(0);
}

TMR_COMP_ISR(0, A)
{
   static uint8_t counter = 0;

   rotaryEncoderSample();

   ++counter;
   if (counter == 10)
   {
      buttonSample();
      counter = 0;
   }
}
