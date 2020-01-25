#include <hal/i2c_master.h>
#include <hal/sensor/hyt939.h>
#include <hal/sensor/si7021.h>

#include <iostream>
#include <thread>

#include <cstdint>
#include <ctime>
#include <unistd.h>

constexpr int INTERVAL = 5;

int main()
{
   if (!i2cMasterInit())
   {
      std::cerr << "Unable to initialise i2c\n";
      return EXIT_FAILURE;
   }
   if (!si7021Init())
   {
      std::cerr << "Unable to initialise SI7021\n";
      return EXIT_FAILURE;
   }

   std::cout << "time,hyt939_temp,hyt939_humi,si7021_temp,si7021_humi" << std::endl;

   while (1)
   {
      const auto hyt939_data = hyt939Measure();
      const float si7021_temp = si7021Temperature() / 10.0f;
      const int si7021_hum = si7021Humidity();

      std::cout << std::time(nullptr) << ',';
      if (hyt939_data)
      {
         std::cout << hyt939_data->temperature / 10.0 << ','
                   << static_cast<int>(hyt939_data->humidity) << ',';
      }
      else
      {
         std::cout << ",,";
      }
      std::cout << si7021_temp << ',' << si7021_hum << std::endl;

      std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
   }

   return EXIT_SUCCESS;
}
