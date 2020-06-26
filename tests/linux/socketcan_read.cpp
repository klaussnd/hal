#include <iostream>

#include <hal/linux/socketcan.h>

int main(void)
{
   if (!can_init())
   {
      std::cerr << "Unable to open CAN interface\n";
      return EXIT_FAILURE;
   }
   std::cout << "CAN interface opened successfully\n";

   while (1)
   {
      CanMessage msg;
      if (can_check_message() && can_get_message(&msg))
      {
         std::cout << "Received message:\n"
                   << "ID " << msg.id << (msg.flags.extended ? " extended " : "") << '\n'
                   << "Length " << static_cast<int>(msg.length) << '\n'
                   << "Data ";
         for (int i = 0; i < msg.length; ++i)
         {
            std::cout << static_cast<int>(msg.data[i]) << ' ';
         }
         std::cout << '\n';
      }
   }

   return EXIT_SUCCESS;
}
