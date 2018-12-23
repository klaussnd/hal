#include "pin_io.h"
#include "pin_names.h"

#include <type_traits>

constexpr HwPwmOutput operator|(HwPwmOutput lhs, HwPwmOutput rhs)
{
   using UnderlyingIntegralType = std::underlying_type<HwPwmOutput>::type;
   return static_cast<HwPwmOutput>(static_cast<UnderlyingIntegralType>(lhs)
                                 | static_cast<UnderlyingIntegralType>(rhs));
}

constexpr bool shouldHaveOutput(HwPwmOutput bit_field, HwPwmOutput check_for)
{
   using UnderlyingIntegralType = std::underlying_type<HwPwmOutput>::type;
   return static_cast<UnderlyingIntegralType>(bit_field)
          & static_cast<UnderlyingIntegralType>(check_for)
               == static_cast<UnderlyingIntegralType>(check_for);
}

struct PwmImpl
{
   template <HwPwmOutput output, HwPwmDirection direction>
   static void init()
   {
      initOC0A<shouldHaveOutput(output, HwPwmOutput::OC0A), direction>();
      initOC0B<shouldHaveOutput(output, HwPwmOutput::OC0B), direction>();
      initOC1A<shouldHaveOutput(output, HwPwmOutput::OC1A), direction>();
      initOC1B<shouldHaveOutput(output, HwPwmOutput::OC1B), direction>();
      initOC2A<shouldHaveOutput(output, HwPwmOutput::OC2A), direction>();
      initOC2B<shouldHaveOutput(output, HwPwmOutput::OC2B), direction>();
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
      // phase correct PWM
      TCCR0A |= (1 << WGM00);
      TCCR0A &= ~(1 << WGM01);
      TCCR0B &= ~(1 << WGM02);
      // activate OC0A
      TCCR0A |= (1 << COM0A1);
      if (direction == HwPwmDirection::Inverted)
         TCCR0A |= (1 << COM0A0);
      else
         TCCR0A &= ~(1 << COM0A0);
#else
 #error "Hardware PWM on timer 0 is not supported by this chip."
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
      // phase correct PWM
      TCCR0A |= (1 << WGM00);
      TCCR0A &= ~(1 << WGM01);
      TCCR0B &= ~(1 << WGM02);
      // activate OC0B
      TCCR0A |= (1 << COM0B1);
      if (direction == HwPwmDirection::Inverted)
         TCCR0A |= (1 << COM0B0);
      else
         TCCR0A &= ~(1 << COM0B0);
#else
 #error "Hardware PWM on timer 0 is not supported by this chip."
#endif
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC1A()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC1A()
   {
      SET_OUTPUT(PIN_OC1A);
      TMR_SET_OCR2(1, A, 0);

      // phase correct PWM 8bit
      TCCR1A |= (1 << WGM10);
      TCCR1A &= ~(1 << WGM11);
      TCCR1B &= ~((1 << WGM12) | (1<< WGM13));
      // activate OC1A
      TCCR1A |= (1 << COM1A1);
      if (direction == HwPwmDirection::Inverted)
         TCCR1A |= (1 << COM1A0);
      else
         TCCR1A &= ~(1 << COM1A0);
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC1B()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC1B()
   {
      SET_OUTPUT(PIN_OC1B);
      TMR_SET_OCR2(1, B, 0);

      // phase correct PWM 8bit
      TCCR1A |= (1 << WGM10);
      TCCR1A &= ~(1 << WGM11);
      TCCR1B &= ~((1 << WGM12) | (1<< WGM13));
      // activate OC1B
      TCCR1A |= (1 << COM1B1);
      if (direction == HwPwmDirection::Inverted)
         TCCR1A |= (1 << COM1B0);
      else
         TCCR1A &= ~(1 << COM1B0);
   }

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<!enabled>::type initOC2A()
   {}

   template <bool enabled, HwPwmDirection direction>
   static typename std::enable_if<enabled>::type initOC2A()
   {
      SET_OUTPUT(PIN_OC2A);
      TMR_SET_OCR2(2, A, 0);

#ifdef TCCR2A
      // phase correct PWM
      TCCR2A |= (1 << WGM20);
      TCCR2A &= ~(1 << WGM21);
      TCCR2B &= ~(1 << WGM22);
      // activate OC0A
      TCCR2A |= (1 << COM2A1);
      if (direction == HwPwmDirection::Inverted)
         TCCR2A |= (1 << COM2A0);
      else
         TCCR2A &= ~(1 << COM2A0);
#elif defined TCCR2
      TCCR2 &= ~((1 << COM21) | (1 << WGM21) | (1 << WGM22));
      TCCR2 |=
         (1 << COM21)
         | (direction == HwPwmDirection::Inverted ? (1 << COM20) : 0)
         | (1 << WGM20);  // phase correct PWM mode
#else
 #error "Hardware PWM on timer 2 is not supported by this chip."
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

#ifdef TCCR2A
      // phase correct PWM
      TCCR2A |= (1 << WGM20);
      TCCR2A &= ~(1 << WGM21);
      TCCR2B &= ~(1 << WGM22);
      // activate OC0B
      TCCR2A |= (1 << COM2B1);
      if (direction == HwPwmDirection::Inverted)
         TCCR2A |= (1 << COM2B0);
      else
         TCCR2A &= ~(1 << COM2B0);
#else
 #error "Hardware PWM on timer 2 only supports one channel for this chip."
#endif
   }
};

template <HwPwmOutput output, HwPwmDirection direction>
void hwPwmInit()
{
   PwmImpl::init<output, direction>();
}
