#include <hal/avr/pin_io.h>
#include <hal/avr/timer.h>

#include <avr/interrupt.h>

#define PIN_OUT D, 7

void initTimerTo100msIntervalWithIrq();

int main()
{
   confPinAsOutput(PIN_OUT);

   initTimerTo100msIntervalWithIrq();
   sei();

   while (true)
      ;

   return 0;
}

void initTimerTo100msIntervalWithIrq()
{
#define TIMER_NO 1
#if F_CPU > 4000000UL
#define PRESCALER 256
#else
#define PRESCALER 64
#endif

   // set CTC mode with top = OCR1A
   TCCR1A = 0;
   TCCR1B = (1 << WGM12);

   TMR_SET_OCR2(TIMER_NO, A, TMR_MSEC2TIMER(100, PRESCALER) - 1UL);
   TMR_SET_COMP_IRQ(TIMER_NO, A);
   TMR_SET_PRESCALER(TIMER_NO, PRESCALER);
}

TMR_COMP_ISR(TIMER_NO, A)
{
   pinToggle(PIN_OUT);
}
