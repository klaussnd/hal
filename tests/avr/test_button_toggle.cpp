#include <hal/avr/timer.h>
#include <hal/input/button.h>
#include <hal/usart.h>
#include <hal/usart_stdout.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BUTTON (1 << 6)

void initTimer0To10msIrq();

int main(void)
{
   usartInit<19200>();
   buttonInit();
   sei();
   initTimer0To10msIrq();

   while (1)
   {
      if (buttonToggle(BUTTON))
      {
         usartWriteString_P(PSTR("switch\n"));
      }
   }

   return 0;
}

void initTimer0To10msIrq()
{
#define PRESCALER 1024
   TMR_SET_PRESCALER(0, PRESCALER);
   TMR_SET_OCR2(0, A, TMR_MSEC2TIMER(10, PRESCALER) - 1UL);
   TMR_SET_COMP_IRQ(0, A);
   TMR_SET_NORMAL(0);
}

TMR_COMP_ISR(0, A)
{
   buttonSample();
}
