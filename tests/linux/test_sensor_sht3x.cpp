#include <hal/i2c_master.h>
#include <hal/sensor/sht3x.h>

#include <iostream>
#include <thread>

std::ostream& operator<<(std::ostream& os, const SHT3xError& err);

int main(int argc, char** argv)
{
   const char* device = argc > 1 ? argv[1] : "/dev/i2c-2";
   if (!i2cMasterInit(device))
   {
      std::cerr << "Unable to initialise i2c " << device << "\n";
      return EXIT_FAILURE;
   }

   const auto serial_or_err = sht3xReadSerialNumber();
   if (std::holds_alternative<SHT3xError>(serial_or_err))
   {
      std::cerr << "Unable to read serial number: " << std::get<SHT3xError>(serial_or_err)
                << std::endl;
      return EXIT_FAILURE;
   }
   std::cout << "Found SHT3x serial number " << std::get<uint32_t>(serial_or_err)
             << std::endl;

   while (true)
   {
      const auto meas_or_err = sht3xMeasure();
      if (std::holds_alternative<SHT3xError>(meas_or_err))
      {
         std::cerr << "Error reading sensor: " << std::get<SHT3xError>(meas_or_err)
                   << std::endl;
      }
      else
      {
         const auto& raw = std::get<SHT3xRawData>(meas_or_err);
         const float temp =
            175.0f * static_cast<float>(raw.temperature) / 65535.0f - 45.0f;
         const float humi = 100.0f * static_cast<float>(raw.humidity) / 65535.0f;
         const auto data = sht3xCompute(raw);
         std::cout << "Temperature " << temp << u8"°C humidity " << humi << "% - "
                   << data.temperature / 10.f << u8"°C "
                   << static_cast<int>(data.humidity) << "%" << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
   }

   return EXIT_SUCCESS;
}

std::ostream& operator<<(std::ostream& os, const SHT3xError& err)
{
   switch (err)
   {
   case SHT3xError::I2C:
      os << "i2c error";
      break;
   case SHT3xError::CRC:
      os << "CRC error";
      break;
   }

   return os;
}
