#include "pin_io.h"
#include "pin_names.h"

#include <type_traits>

template <PwmOutput output, PwmDirection direction>
struct PwmImpl
{
   static void init();
};

template <PwmDirection direction>
struct PwmImpl<PwmOutput::OC0A, direction>
{
   static void init()
   {
      SET_OUTPUT(PIN_OC0A);
      TMR_SET_OCR2(0, A, 0);

#ifdef TCCR0A
      TCCR0A &= ~((1 << COM0A0) | (1 << WGM01) | (1 << WGM02));
      TCCR0A |= (1 << COM0A1)  // enable OC0A
                | (direction == PwmDirection::Inverted ? (1 << COM0A0) : 0)
                | (1 << WGM00);  // phase correct PWM mode
#elif TCCR0
      TCCR0 &= ~((1 << COM01) | (1 << WGM01) | (1 << WGM02));
      TCCR0 |= (1 << COM01) | (direction == PwmDirection::Inverted ? (1 << COM00) : 0)
               | (1 << WGM00);  // phase correct PWM mode
#endif
   }
};

template <PwmDirection direction>
struct PwmImpl<PwmOutput::OC0B, direction>
{
   static void init()
   {
      SET_OUTPUT(PIN_OC0B);
      TMR_SET_OCR2(0, B, 0);

#ifdef TCCR0A
      TCCR0A &= ~((1 << COM0B0) | (1 << WGM01) | (1 << WGM02));
      TCCR0A |=
         (1 << COM0B1)  // activate OC0B
         | (direction == PwmDirection::Inverted ? (1 << COM0B0) : 0)
         | (1 << WGM00);  // phase correct PWM
#else
#error This MCU only has one PWM channel on timer 0
#endif
   }
};

template <PwmDirection direction>
struct PwmImpl<PwmOutput::OC2A, direction>
{
   static void init()
   {
      SET_OUTPUT(PIN_OC2A);
      TMR_SET_OCR2(2, A, 0);

#ifdef TCCR0A
      TCCR2A &= ~((1 << COM2B0) | (1 << WGM21) | (1 << WGM22));
      TCCR2A |=
         (1 << COM2B1)  // activate OC2B
         | (direction == PwmDirection::Inverted ? (1 << COM2B0) : 0)
         | (1 << WGM20);  // phase correct PWM
#elif defined TCCR0
      TCCR0 &= ~((1 << COM21) | (1 << WGM21) | (1 << WGM22));
      TCCR0 |=
         (1 << COM21)
         | (direction == PwmDirection::Inverted ? (1 << COM20) : 0)
         | (1 << WGM20);  // phase correct PWM mode
#endif
   }
};

template <PwmDirection direction>
struct PwmImpl<PwmOutput::OC2B, direction>
{
   static void init()
   {
      SET_OUTPUT(PIN_OC2B);
      TMR_SET_OCR2(2, B, 0);

#ifdef TCCR0A
      TCCR2A &= ~((1 << COM2B0) | (1 << WGM21) | (1 << WGM22));
      TCCR2A |=
         (1 << COM2B1)  // activate OC2B
         | (direction == PwmDirection::Inverted ? (1 << COM2B0) : 0)
         | (1 << WGM20);  // phase correct PWM
#else
#error This MCU only has one PWM channel on timer 0
#endif
   }
};

template <PwmOutput output, PwmDirection direction>
void pwmInit()
{
   PwmImpl<output, direction>::init();
}
