#include <hal/usart.h>

#include <cerrno>   // Error number definitions
#include <cstring>  // memset()
#include <fcntl.h>  // open(), O_RDWR, O_NOCTTY etc.
#include <sys/ioctl.h>
#include <termios.h>  // Flags and Baud-settings | <asm/termbits.h>
#include <unistd.h>   // read(), write()

namespace
{
int m_fd = -1;
}

bool usartInit(const char* device, unsigned long baudrate)
{
   m_fd = open(device, O_RDWR | O_NOCTTY);
   if (m_fd == -1)
   {
      return false;
   }
   // tty settings: 8 databits, 1 stopbit, no parity, no canonical mode
   struct termios options;
   memset(&options, 0, sizeof(options));
   if (tcgetattr(m_fd, &options) != 0)
   {
      return false;
   }
   speed_t baud;
   switch (baudrate)
   {
   case 4800:
      baud = B4800;
      break;
   case 9600:
      baud = B9600;
      break;
   case 19200:
      baud = B19200;
      break;
   case 38400:
      baud = B38400;
      break;
   case 57600:
      baud = B57600;
      break;
   case 115200:
      baud = B115200;
      break;
   default:
      return false;
   }

   options.c_cflag = baud | CS8 | CLOCAL | CREAD;
   options.c_iflag = IGNPAR;
   options.c_oflag = 0;
   options.c_lflag = 0;
   options.c_cc[VMIN] = 1;  // set blocking
   cfsetospeed(&options, baud);
   cfsetispeed(&options, baud);

   tcflush(m_fd, TCIFLUSH);
   if (tcsetattr(m_fd, TCSANOW, &options) == -1)
   {
      return false;
   }

   return true;
}

void usartFinalise()
{
   if (m_fd != -1)
   {
      close(m_fd);
      m_fd = -1;
   }
}

uint8_t usartBytesAvailableToRead()
{
   int bytes_available = 0;
   if (ioctl(m_fd, FIONREAD, &bytes_available) == 0)
   {
      return static_cast<uint8_t>(bytes_available);
   }
   return 0;
}

uint8_t usartRead(char* dstbuf, uint8_t maxlength)
{
   const auto res = read(m_fd, dstbuf, maxlength);
   return res < 0 ? 0u : static_cast<uint8_t>(res);
}

bool usartReadExact(char* dstbuf, uint8_t length)
{
   int bytes_received = 0;
   do
   {
      const int bytes_missing = length - bytes_received;
      bytes_received = read(m_fd, dstbuf + bytes_received, bytes_missing);
      if (bytes_received < 0)
      {
         return false;
      }
   } while (bytes_received > length);

   return true;
}

uint8_t usartWrite(const char* buf, uint8_t length)
{
   const auto res = write(m_fd, buf, length);
   return res < 0 ? 0u : static_cast<uint8_t>(res);
}

uint8_t usartWriteString(const char* buf)
{
   return usartWrite(buf, strlen(buf));
}
