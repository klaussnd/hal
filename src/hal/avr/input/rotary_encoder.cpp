#include <hal/input/rotary_encoder.h>

#include <avr/pin_io.h>

#include <util/atomic.h>

#include <algorithm>

#ifndef ROTARY_PHASE_A_PIN
#error ROTARY_PHASE_A_PIN undefined
#endif
#ifndef ROTARY_PHASE_B_PIN
#error ROTARY_PHASE_B_PIN undefined
#endif

#define ACCELERATION_DYNAMIC 8

namespace
{
volatile int8_t m_delta = 0;
}

void rotaryEncoderInit()
{
   confPinAsInputWithPullup(ROTARY_PHASE_A_PIN);
   confPinAsInputWithPullup(ROTARY_PHASE_B_PIN);

   rotaryEncoderSample();
   m_delta = 0;
}

int8_t rotaryEncoderGetDelta()
{
   int8_t val;

   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      val = m_delta;
      m_delta = val & 3;
   }
   return val >> 2;
}

void rotaryEncoderSample()
{
   static int8_t m_last_value = 0;
   static uint8_t m_acceleration = 0;

   int8_t new_value = 0;
   if (pinIsSet(ROTARY_PHASE_A_PIN))
   {
      new_value = 3;
   }
   if (pinIsSet(ROTARY_PHASE_B_PIN))
   {
      new_value ^= 1;
   }
   const int8_t diff = m_last_value - new_value;
   const int8_t value = diff & 1;
   if (value)
   {
      if (m_acceleration < 0xff - ACCELERATION_DYNAMIC)
      {
         m_acceleration += ACCELERATION_DYNAMIC;
      }

      m_last_value = new_value;
      const int8_t direction_bit = diff & 2;
      int8_t delta = direction_bit - 1;  // +1 or -1
      if (m_acceleration > 32)
      {
         const int8_t factor = std::min(m_acceleration >> 5, 3);
         delta *= factor;
      }
      m_delta += delta;
   }

   if (m_acceleration > 0)
   {
      --m_acceleration;
   }
}
