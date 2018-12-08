#include "pin_io.h"
#include "pin_names.h"

#include <type_traits>

constexpr HwPwmOutput operator|(HwPwmOutput lhs, HwPwmOutput rhs)
{
   using UnderlyingIntegralType = std::underlying_type<HwPwmOutput>::type;
   return static_cast<HwPwmOutput>(static_cast<UnderlyingIntegralType>(lhs)
                                 | static_cast<UnderlyingIntegralType>(rhs));
}

constexpr bool shouldHaveOutput(HwPwmOutput requested, HwPwmOutput checkFor)
{
   using UnderlyingIntegralType = std::underlying_type<HwPwmOutput>::type;
   return static_cast<UnderlyingIntegralType>(requested)
          & static_cast<UnderlyingIntegralType>(checkFor)
               == static_cast<UnderlyingIntegralType>(checkFor);
}

struct PwmImpl
{
   template <HwPwmOutput output, HwPwmDirection direction>
   static void init()
   {
      initOC0A<shouldHaveOutput(HwPwmOutput::OC0A, output), direction>();
      initOC0B<shouldHaveOutput(HwPwmOutput::OC0B, output), direction>();
      initOC2A<shouldHaveOutput(HwPwmOutput::OC2A, output), direction>();
      initOC2B<shouldHaveOutput(HwPwmOutput::OC2B, output), direction>();
   }

private:
   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC0A()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC0A()
   {
      SET_OUTPUT(PIN_OC0A);
      TMR_SET_OCR2(0, A, 0);

#ifdef TCCR0A
      TCCR0A &= ~((1 << COM0A0) | (1 << WGM01) | (1 << WGM02));
      TCCR0A |= (1 << COM0A1)  // enable OC0A
                | (direction == HwPwmDirection::Inverted ? (1 << COM0A0) : 0)
                | (1 << WGM00);  // phase correct PWM mode
#elif TCCR0
      TCCR0 &= ~((1 << COM01) | (1 << WGM01) | (1 << WGM02));
      TCCR0 |= (1 << COM01) | (direction == HwPwmDirection::Inverted ? (1 << COM00) : 0)
               | (1 << WGM00);  // phase correct PWM mode
#endif
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC0B()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC0B()
   {
      SET_OUTPUT(PIN_OC0B);
      TMR_SET_OCR2(0, B, 0);

#ifdef TCCR0A
      TCCR0A &= ~((1 << COM0B0) | (1 << WGM01) | (1 << WGM02));
      TCCR0A |=
         (1 << COM0B1)  // activate OC0B
         | (direction == HwPwmDirection::Inverted ? (1 << COM0B0) : 0)
         | (1 << WGM00);  // phase correct PWM
#else
#error This MCU only has one PWM channel on timer 0
#endif
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC2A()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC2A()
   {
      SET_OUTPUT(PIN_OC2A);
      TMR_SET_OCR2(2, A, 0);

#ifdef TCCR0A
      TCCR2A &= ~((1 << COM2B0) | (1 << WGM21) | (1 << WGM22));
      TCCR2A |=
         (1 << COM2B1)  // activate OC2B
         | (direction == HwPwmDirection::Inverted ? (1 << COM2B0) : 0)
         | (1 << WGM20);  // phase correct PWM
#elif defined TCCR0
      TCCR0 &= ~((1 << COM21) | (1 << WGM21) | (1 << WGM22));
      TCCR0 |=
         (1 << COM21)
         | (direction == HwPwmDirection::Inverted ? (1 << COM20) : 0)
         | (1 << WGM20);  // phase correct PWM mode
#endif
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC2B()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC2B()
   {
      SET_OUTPUT(PIN_OC2B);
      TMR_SET_OCR2(2, B, 0);

#ifdef TCCR0A
      TCCR2A &= ~((1 << COM2B0) | (1 << WGM21) | (1 << WGM22));
      TCCR2A |=
         (1 << COM2B1)  // activate OC2B
         | (direction == HwPwmDirection::Inverted ? (1 << COM2B0) : 0)
         | (1 << WGM20);  // phase correct PWM
#else
#error This MCU only has one PWM channel on timer 0
#endif
   }
};

template <HwPwmOutput output, HwPwmDirection direction>
void hwPwmInit()
{
   PwmImpl::init<output, direction>();
}
