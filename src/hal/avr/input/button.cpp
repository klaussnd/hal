#include <hal/avr/pin_io.h>
#include <hal/avr/timer.h>
#include <hal/input/button.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#ifndef BUTTON_PORT /* port where buttons are connected to */
#error Port for buttons undefined
#endif
#ifndef BUTTON_MASK      /* all bits with buttons */
#define BUTTON_MASK 0xff /* default: all 8 bits */
#endif
#ifndef REPEAT_MASK /* buttons where long press and/or repeat works */
#define REPEAT_MASK BUTTON_MASK
#endif

#define REPEAT_START 50  // after 500ms
#define REPEAT_NEXT 20   // every 200ms

namespace
{
volatile uint8_t button_state;  // debounced and inverted button state:
                                // bit = 1: button pressed
volatile uint8_t button_press;  // button press detect
volatile uint8_t button_rpt;    // button long press and repeat
volatile uint8_t rpt_count;     // repeat counter
}  // namespace

void buttonInit(void)
{
   DDR(BUTTON_PORT) &= ~BUTTON_MASK;  // configure button port for input
   PORT(BUTTON_PORT) |= BUTTON_MASK;  // and turn on pull up resistors
}

uint8_t buttonPress(uint8_t button_mask)
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  // read and clear atomic!
   {
      button_mask &= button_press;  // read button(s)
      button_press ^= button_mask;  // clear button(s)
   }
   return button_mask;
}

uint8_t buttonRpt(uint8_t button_mask)
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  // read and clear atomic!
   {
      button_mask &= button_rpt;  // read button(s)
      button_rpt ^= button_mask;  // clear button(s)
   }
   return button_mask;
}

uint8_t buttonShort(uint8_t button_mask)
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  // read button state and button press atomic!
   {
      button_mask = buttonPress(~button_state & button_mask);
   }
   return button_mask;
}

uint8_t buttonLong(uint8_t button_mask)
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {  // read button state and button press atomic!
      button_mask = buttonPress(buttonRpt(button_mask));
      if (button_mask)
      {
         /* Long button was pressed. Stop repeat functionality to avoid
            buttonRpt() going off without re-hitting the button. */
         rpt_count = 0;
      }
   }
   return button_mask;
}

uint8_t buttonDown(uint8_t button_mask)
{
   return (button_state & button_mask);
}

void buttonSample(void)
{
   static uint8_t ct0, ct1;

   uint8_t i = PIN(BUTTON_PORT);        // sample buttons, 0 = down
   i = button_state ^ ~i;               // button changed ?
   ct0 = ~(ct0 & i);                    // reset or count ct0
   ct1 = ct0 ^ (ct1 & i);               // reset or count ct1
   i &= ct0 & ct1;                      // count until roll over ?
   button_state ^= i;                   // then toggle debounced state
   button_press |= (button_state & i);  // 0->1: button press detect

   if (rpt_count == 0)
   {                                          // no repeat running
      if ((button_press & REPEAT_MASK) != 0)  // and repeat button press
         rpt_count = REPEAT_START + 1;        // start repeat
   }
   else
   {
      if ((button_state & REPEAT_MASK) == 0)  // no repeat button pressed any more
         rpt_count = 0;                       // stop
      else if (--rpt_count == 1)
      {                            // countdown and check repeat
         rpt_count = REPEAT_NEXT;  // repeat delay
         button_rpt |= (button_state & REPEAT_MASK);
      }
   }
}
