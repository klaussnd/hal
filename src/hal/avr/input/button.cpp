/*
From https://www.mikrocontroller.net/articles/Entprellung

Funktionsweise

Der Code basiert auf 8 parallelen vertikalen Zählern, die über die Variablen ct0 und ct1
aufgebaut werden, wobei jeweils ein Bit in ct0 mit dem gleichwertigen Bit in ct1
zusammengenommen einen 2-Bit-Zähler bildet. Der Code der sich um die 8 Zähler kümmert,
ist so geschrieben, daß er alle 8 Zähler gemeinsam parallel behandelt.

  i = button_state ^ ~BUTTON_PIN;                // key changed ?

i enthält an dieser Stelle für jede Taste, die sich im Vergleich mit dem vorhergehenden
entprellten Zustand (keystate) verändert hat, ein 1 Bit.

  ct0 = ~( ct0 & i );                             // reset or count ct0
  ct1 = ct0 ^ (ct1 & i);                          // reset or count ct1

Diese beiden Anweisungen erniedrigen den 2-Bit Zähler ct0/ct1 für jedes Bit um 1,
welches in i gesetzt ist. Liegt an der entsprechenden Stelle in i ein 0 Bit vor
(keine Änderung des Zustands), so wird der Zähler ct0/ct1 für dieses Bit auf 1 gesetzt.
Der Grundzustand des Zählers ist als ct0 == 1 und ct1 == 1 (Wert 3). Der Zähler zählt
daher mit jedem ISR Aufruf, bei dem die Taste im Vergleich zu keystate als verändert
erkannt wurde.

  ct1   ct0
    1    1   // 3
    1    0   // 2
    0    1   // 1
    0    0   // 0
    1    1   // 3

  i &= ct0 & ct1;                                 // count until roll over ?

In i bleibt nur dort ein 1-Bit erhalten, wo sowohl in ct1 als auch in ct0 ein 1 Bit
vorgefunden wird, der betreffende Zähler also bis 3 zählen konnte. Durch die zusätzliche
Verundung mit i wird der Fall abgefangen, dass ein konstanter Zählerwert von 3 in i
ein 1 Bit hinterlässt. Im Endergebnis bedeutet dass, dass nur ein Zählerwechsel
von 0 auf 3 zu einem 1 Bit an der betreffenden Stelle in i führt, aber auch nur dann,
wenn in i an dieser Bitposition ebenfalls ein 1 Bit war (welches wiederrum deswegen
auf 1 war, weil an diesem Eingabeport eine Veränderung zum letzten bekannten entprellten
Zustand festgestellt wurde). Alles zusammengenommen heißt das, dass ein Tastendruck dann
erkannt wird, wenn die Taste 4 mal hintereinander in einem anderen Zustand vorgefunden
wurde als dem zuletzt bekannten entprellten Tastenzustand.

An dieser Stelle ist i daher ein Vektor von 8 Bits, von denen jedes einzelne der Bits
darüber Auskunft gibt, ob die entsprechende Taste mehrmals hintereinander im selben
Zustand angetroffen wurde, der nicht mit dem zuletzt bekannten Tastenzustand
übereinstimmt. Ist das der Fall, dann wird eine entsprechende Veränderung des
Tastenzustands in key_state registriert

  button_state ^= i;                             // then toggle debounced state

und wenn sich in key_state das entsprechende Bit von 0 auf 1 verändert hat, wird dieses
Ereignis als 'Taste wurde niedergedrückt' gewertet.

  button_press |= button_state & i;              // 0->1: key press detect

Damit ist der Tasteneingang entprellt. Und zwar sowohl beim Drücken einer Taste
als auch beim Loslassen (damit Tastenpreller beim Loslassen nicht mit dem Niederdrücken
einer Taste verwechselt werden). Der weitere Code beschäftigt sich dann nur noch damit,
diesen entprellten Tastenzustand weiter zu verarbeiten.
 */

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
