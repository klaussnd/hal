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
   struct termios newtio;
   memset(&newtio, 0, sizeof(newtio));
   newtio.c_cflag = CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNBRK;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;
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
   cfsetospeed(&newtio, baud);
   cfsetispeed(&newtio, baud);
   newtio.c_cc[VMIN] = 1;
   newtio.c_cc[VTIME] = 10;  // max wait 1s
   tcflush(m_fd, TCIOFLUSH);
   if (tcsetattr(m_fd, TCSANOW, &newtio) == -1)
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
   ioctl(m_fd, FIONREAD, &bytes_available);
   return static_cast<uint8_t>(bytes_available);
}

uint8_t usartRead(char* dstbuf, uint8_t maxlength)
{
   const auto res = read(m_fd, dstbuf, maxlength);
   return res < 0 ? 0 : static_cast<uint8_t>(res);
}

uint8_t usartWrite(const char* buf, uint8_t length)
{
   const auto res = write(m_fd, buf, length);
   return res < 0 ? 0 : static_cast<uint8_t>(res);
}

uint8_t usartWriteString(const char* buf)
{
   return usartWrite(buf, strlen(buf));
}
