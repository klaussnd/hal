#include <hal/usart.h>

#include <iostream>

int main(int argc, char* argv[])
{
   const char* device = (argc >= 2) ? argv[1] : "/dev/ttyUSB0";
   usartInit(device, 57600);

   while (1)
   {
      char buf[32];
      const int res = usartRead(buf, sizeof(buf));
      if (res > 0)
      {
         std::cout << "Received " << res << " bytes: ";
         for (int i = 0; i < res; ++i)
         {
            std::cout << ' ' << "0x" << std::hex << static_cast<int>(buf[i]);
         }
         std::cout << std::endl;
      }
   }

   return EXIT_SUCCESS;
}
