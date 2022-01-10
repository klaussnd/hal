#include <hal/sensor/si1145.h>

namespace
{
bool tryHigherSignal(Si1145Range& range, Si1145Gain& gain);
bool tryLowerSignal(Si1145Range& range, Si1145Gain& gain);
}  // namespace

std::optional<Si1145VisAutoMeas> si1145MakeAutoVisMeasurement()
{
   constexpr uint16_t max_raw_val = 50000u;
   constexpr uint16_t min_raw_val = 10000u;

   const auto initial_range = si1145GetVisRange();
   const auto initial_gain = si1145GetVisGain();
   if (!initial_range.has_value() || !initial_gain.has_value())
   {
      return {};
   }

   Si1145Range range = initial_range.value();
   Si1145Gain gain = initial_gain.value();
   uint16_t vis_raw = 0;
   bool try_again = true;

   for (uint8_t try_count = 0; try_again && try_count < 16; ++try_count)
   {
      if (!si1145StartMeasurement())
      {
         return {};
      }
      auto value = si1145ReadMeasurement();
      if (!value)
      {
         return {};
      }
      vis_raw = value.value().vis;
      if ((vis_raw < min_raw_val && tryHigherSignal(range, gain))
          || (vis_raw > max_raw_val && tryLowerSignal(range, gain)))
      {
         if (!si1145SetVisMode(range, gain))
         {
            return {};
         }
      }
      else
      {
         try_again = false;
      }
   }

   return Si1145VisAutoMeas{vis_raw, range, gain};
}

namespace
{
Si1145Gain& operator++(Si1145Gain& gain);
Si1145Gain& operator--(Si1145Gain& gain);

bool tryHigherSignal(Si1145Range& range, Si1145Gain& gain)
{
   const bool can_try_higher_gain =
      (range == Si1145Range::NORMAL && gain < Si1145Gain::DIV_128)
      // If high range, go back to normal mode already at gain 32.
      // Don't use gain 128 with high range, as it sometimes leads to wrong data.
      || (range == Si1145Range::HIGH && gain < Si1145Gain::DIV_32);
   if (can_try_higher_gain)
   {
      ++gain;
      return true;
   }
   else if (range == Si1145Range::HIGH)
   {
      range = Si1145Range::NORMAL;  // changes the gain by a factor of 14.5
      gain = Si1145Gain::DIV_2;
      return true;
   }

   return false;
}

bool tryLowerSignal(Si1145Range& range, Si1145Gain& gain)
{
   if (gain > Si1145Gain::DIV_1)
   {
      --gain;
      return true;
   }
   else if (range == Si1145Range::NORMAL)
   {
      range = Si1145Range::HIGH;  // changes the gain by a factor of 14.5
      gain = Si1145Gain::DIV_8;
      return true;
   }

   return false;
}

inline Si1145Gain& operator++(Si1145Gain& gain)
{
   using T = std::underlying_type<Si1145Gain>::type;
   gain = static_cast<Si1145Gain>(static_cast<T>(gain) + 1);
   return gain;
}

inline Si1145Gain& operator--(Si1145Gain& gain)
{
   using T = std::underlying_type<Si1145Gain>::type;
   gain = static_cast<Si1145Gain>(static_cast<T>(gain) - 1);
   return gain;
}
}  // namespace
