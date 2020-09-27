#include <hal/i2c_master.h>
#include <hal/sensor/si1145.h>

#include <iostream>
#include <thread>

#include <cstdint>
#include <ctime>
#include <unistd.h>

constexpr int INTERVAL = 5;

int main()
{
   const char* device = "/dev/i2c-1";
   if (!i2cMasterInit(device))
   {
      std::cerr << "Unable to initialise i2c " << device << "\n";
      return EXIT_FAILURE;
   }
   if (!si1145Init())
   {
      std::cerr << "Unable to initialise SI1145\n";
      return EXIT_FAILURE;
   }

   while (1)
   {
      si1145StartMeasurement();
      const auto data = si1145ReadMeasurement();
      if (data)
      {
         std::cout << std::time(nullptr) << " UV index " << data->uv_index << " VIS "
                   << data->vis << " IR " << data->ir << std::endl;
      }
      else
      {
         std::cout << "measurement error" << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
   }

   return EXIT_SUCCESS;
}
