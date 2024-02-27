#include <hal/sensor/mhz19.h>
#include <hal/usart.h>

#include <iostream>
#include <unistd.h>

std::string toString(Mhz19Status status);

int main(int argc, char* argv[])
{
   const char* device = (argc >= 2) ? argv[1] : "/dev/ttyUSB0";
   usartInit(device, 9600);

   std::cout << "CO2_ppm,temp_Celsius" << std::endl;

   while (1)
   {
      mhz19StartMeasurement();
      sleep(1);
      const Mhz19Data result = mhz19GetMeasurement();
      if (result.status == Mhz19Status::SUCCESS)
      {
         std::cout << result.co2Ppm << ',' << result.temperature << std::endl;
      }
      else
      {
         std::cerr << "Error: " << toString(result.status) << std::endl;
      }
   }

   return 0;
}

std::string toString(Mhz19Status status)
{
   switch (status)
   {
   case Mhz19Status::SUCCESS:
      return "success";
   case Mhz19Status::USART_ERROR:
      return "USART error";
   case Mhz19Status::CHECKSUM_ERROR:
      return "Checksum error";
   case Mhz19Status::BAD_RESPONSE_ERROR:
      return "Bad response";
   }
   return "";
}
