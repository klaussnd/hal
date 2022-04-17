#include <hal/i2c_master.h>
#include <hal/sensor/hyt939.h>
#include <hal/sensor/si1145.h>
#include <hal/sensor/si7021.h>
#include <hal/sensor/veml6075.h>

#include <iostream>
#include <thread>

#include <cstdint>
#include <ctime>
#include <unistd.h>

constexpr int INTERVAL = 5;

int main(int argc, char** argv)
{
   const char* device = argc > 1 ? argv[1] : "/dev/i2c-1";
   if (!i2cMasterInit(device))
   {
      std::cerr << "Unable to initialise i2c " << device << "\n";
      return EXIT_FAILURE;
   }
   const bool has_si7021 = si7021Init();
   const bool has_veml6075 = veml6075Init();
   const bool has_si1145 = si1145Init();

   if (has_veml6075)
   {
      std::cout << "UV sensor found" << std::endl;
   }
   if (has_si1145)
   {
      std::cout << "Sunshine sensor found" << std::endl;
   }

   while (1)
   {
      std::cout << std::time(nullptr) << ',';

      const auto hyt939_data = hyt939Measure();
      if (hyt939_data)
      {
         std::cout << hyt939_data->temperature / 10.0 << ','
                   << static_cast<int>(hyt939_data->humidity) << ',';
      }
      else
      {
         std::cout << ",,";
      }
      if (has_si7021)
      {
         const float si7021_temp = si7021Temperature() / 10.0f;
         const int si7021_hum = si7021Humidity();
         std::cout << si7021_temp << ',' << si7021_hum << ",";
      }
      else
      {
         std::cout << ",,";
      }
      if (has_veml6075)
      {
         const auto data = veml6075Measure();
         if (data.has_value())
         {
            constexpr double UVA_A_COEF = 2.22;
            constexpr double UVA_B_COEF = 1.33;
            constexpr double UVA_C_COEF = 2.95;
            constexpr double UVA_D_COEF = 1.74;

            constexpr double UVA_RESPONSIVITY_100MS_UNCOVERED = 0.001111;
            constexpr double UVB_RESPONSIVITY_100MS_UNCOVERED = 0.00125;

            const double corr_uva = std::max(
               data->uva - UVA_A_COEF * data->comp1 - UVA_B_COEF * data->comp2, 0.0);
            const double corr_uvb = std::max(
               data->uvb - UVA_C_COEF * data->comp1 - UVA_D_COEF * data->comp2, 0.0);
            const double uva = corr_uva * UVA_RESPONSIVITY_100MS_UNCOVERED;
            const double uvb = corr_uvb * UVB_RESPONSIVITY_100MS_UNCOVERED;
            const double index = (uva + uvb) / 2.0;

            std::cout << data->uva << ',' << data->uvb << ',' << data->comp1 << ','
                      << data->comp2 << ',' << uva << ',' << uvb << ',' << index << ',';
         }
         else
         {
            std::cout << "--,--,--,--,--,--,--,";
         }
      }
      else
      {
         std::cout << ",,,,,,,";
      }
      if (has_si1145)
      {
         const auto data = si1145MakeAutoVisMeasurement();
         if (data)
         {
            constexpr unsigned int dark_current = 260u;
            constexpr double highrange_divisor = 14.5;
            const double extra_factor =
               data->range == Si1145Range::HIGH ? highrange_divisor : 1.0;
            const double gain = logicalValue(data->gain);
            const double adc_counts_per_lux = 0.282;
            const double lux = static_cast<double>(data->raw - dark_current) / gain
                               * extra_factor / adc_counts_per_lux;

            std::cout << lux;
         }
      }

      std::cout << std::endl;

      std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
   }

   return EXIT_SUCCESS;
}
