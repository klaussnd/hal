#include <hal/sensor/si1145.h>

namespace
{
enum class ChangeDirection : uint8_t
{
   NO_CHANGE,
   HIGHER_SIGNAL,
   LOWER_SIGNAL,
};

bool tryHigherSignal(Si1145Range& range, Si1145Gain& gain);
bool tryLowerSignal(Si1145Range& range, Si1145Gain& gain);
}  // namespace

Si1145AutoExposureResult si1145VisAutoExposure()
{
   constexpr uint16_t max_raw_val = 50000u;
   constexpr uint16_t min_raw_val = 7500u;

   Si1145AutoExposureResult res = Si1145AutoExposureResult::KEPT;

   const auto initial_range = si1145GetVisRange();
   const auto initial_gain = si1145GetVisGain();
   if (!initial_range.has_value() || !initial_gain.has_value())
   {
      return Si1145AutoExposureResult::ERROR;
   }

   Si1145Range range = initial_range.value();
   Si1145Gain gain = initial_gain.value();

   auto maybe_meas = si1145MeasureVis();
   if (!maybe_meas)
   {
      return Si1145AutoExposureResult::ERROR;
   }
   uint16_t vis_raw = maybe_meas.value();
   ChangeDirection direction = ChangeDirection::NO_CHANGE;

   for (uint8_t try_count = 0; try_count < 16; ++try_count)
   {
      bool make_change = false;
      if (vis_raw < min_raw_val && direction != ChangeDirection::LOWER_SIGNAL)
      {
         direction = ChangeDirection::HIGHER_SIGNAL;
         make_change = tryHigherSignal(range, gain);
      }
      else if (vis_raw > max_raw_val && direction != ChangeDirection::HIGHER_SIGNAL)
      {
         direction = ChangeDirection::LOWER_SIGNAL;
         make_change = tryLowerSignal(range, gain);
      }

      if (!make_change)
      {
         break;
      }

      res = Si1145AutoExposureResult::CHANGED;
      if (!si1145SetVisMode(range, gain))
      {
         return Si1145AutoExposureResult::ERROR;
      }
      auto maybe_meas = si1145MeasureVis();
      if (!maybe_meas)
      {
         return Si1145AutoExposureResult::ERROR;
      }
      vis_raw = maybe_meas.value();

      const bool is_overexposed = vis_raw == 0xffff;
      if (direction == ChangeDirection::HIGHER_SIGNAL && is_overexposed)
      {
         // it was a bit too much, reject the last change
         tryLowerSignal(range, gain);
         if (!si1145SetVisMode(range, gain))
         {
            return Si1145AutoExposureResult::ERROR;
         }
         break;
      }
   }

   return res;
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
