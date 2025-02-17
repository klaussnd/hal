#include <hal/i2c_master.h>
#include <hal/sensor/abp2.h>
#include <hal/sensor/adt7410.h>
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
   const bool has_adt7410 = adt7410Init();

   if (has_adt7410)
   {
      std::cout << "ADT7410 temperature sensor found" << std::endl;
   }
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
      if (const auto result = abp2StartMeasurement(); result == I2cStatus::SUCCESS)
      {
         std::cout << "ABP2 found, started measurement\n";
         std::this_thread::sleep_for(std::chrono::milliseconds(10));
         bool busy = true;
         while (busy)
         {
            const auto result = abp2GetMeasurement();
            if (std::holds_alternative<Abp2RawData>(result))
            {
               const auto& raw = std::get<Abp2RawData>(result);
               constexpr double outputmax =
                  15099494;  // output at maximum pressure [counts]
               constexpr double outputmin =
                  1677722;  // output at minimum pressure [counts]
               constexpr double one_psi_in_bar = 0.068947573;
               // pressure range
               constexpr double pmax = 60 * one_psi_in_bar;
               constexpr double pmin = 0;
               const double pressure =
                  ((raw.pressure - outputmin) * (pmax - pmin)) / (outputmax - outputmin)
                  + pmin;
               const double temperature = (raw.temperature * 200. / 16777215.) - 50.;
               std::cout << "ABP2 raw pressure " << raw.pressure << ", raw temp "
                         << raw.temperature << '\n'
                         << "pressure " << pressure << " bar, temp " << temperature
                         << " Celsius" << std::endl;
               busy = false;
            }
            else
            {
               const auto& status = std::get<Abp2Status>(result);
               if (status == Abp2Status::I2C_FAILURE)
               {
                  std::cerr << "I2C failure reading from ABP2" << std::endl;
                  busy = false;
               }
            }
         }
      }
      if (has_adt7410)
      {
         if (const auto temp = adt7410ReadTemp(); temp.has_value())
         {
            std::cout << "ADT7410 " << temp.value() / 10.0 << std::endl;
         }
      }
      const auto hyt939_data = hyt939Measure();
      if (hyt939_data)
      {
         std::cout << "HYT939 " << hyt939_data->temperature / 10.0 << ','
                   << static_cast<int>(hyt939_data->humidity) << std::endl;
      }
      if (has_si7021)
      {
         const float si7021_temp = si7021Temperature() / 10.0f;
         const int si7021_hum = si7021Humidity();
         std::cout << "SI7021 " << si7021_temp << ',' << si7021_hum << std::endl;
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

            std::cout << "VEML6075 " << data->uva << ',' << data->uvb << ','
                      << data->comp1 << ',' << data->comp2 << ',' << uva << ',' << uvb
                      << ',' << index << std::endl;
         }
         else
         {
            std::cout << "VEML6075 --,--,--,--,--,--,--" << std::endl;
            ;
         }
      }
      if (has_si1145)
      {
         const auto autoexp_res = si1145VisAutoExposure();
         const auto raw = si1145MeasureVis();
         const auto range = si1145GetVisRange();
         const auto gain = si1145GetVisGain();
         if (autoexp_res != Si1145AutoExposureResult::ERROR && raw && range && gain)
         {
            constexpr unsigned int dark_current = 260u;
            constexpr double highrange_divisor = 14.5;
            const double extra_factor =
               range.value() == Si1145Range::HIGH ? highrange_divisor : 1.0;
            const double gain_value = logicalValue(gain.value());
            const double adc_counts_per_lux = 0.282;
            const double lux = static_cast<double>(raw.value() - dark_current)
                               / gain_value * extra_factor / adc_counts_per_lux;

            std::cout << "SI1145 " << lux << std::endl;
         }
      }

      std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
   }

   return EXIT_SUCCESS;
}
