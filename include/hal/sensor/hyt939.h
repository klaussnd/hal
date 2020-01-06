// Hygrosens Hyt939 temperature and humidity sensor
#pragma once

#if defined __GNUC__ && __GNUC__ < 7
 #include <experimental/optional>
 namespace std
 {
   template <typename T>
   using optional = experimental::optional<T>;
 }
#else
 #include <optional>
#endif

#include <stdint.h>

struct Hyt939Data
{
   uint8_t humidity; ///< percent
   int16_t temperature; ///< decicelsius
};

std::optional<Hyt939Data> hyt939Measure();
