#include <hal/i2c_master.h>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#if __has_include(<i2c/smbus.h>)
#include <i2c/smbus.h>
#endif

namespace
{
int m_fd;
}

bool i2cMasterInit(const char* device)
{
   m_fd = open(device, O_RDWR);
   return (m_fd >= 0);
}

I2cStatus i2cMasterWrite(uint8_t dev_addr, const uint8_t* data, uint8_t length)
{
   if (ioctl(m_fd, I2C_SLAVE, dev_addr) < 0)
   {
      return I2cStatus::START_ERROR;
   }

   const bool success = write(m_fd, data, length) == length;
   return success ? I2cStatus::SUCCESS : I2cStatus::TRANSMISSION_ERROR;
}

I2cStatus i2cMasterRead(uint8_t dev_addr, uint8_t* data, uint8_t length)
{
   if (ioctl(m_fd, I2C_SLAVE, dev_addr) < 0)
   {
      return I2cStatus::START_ERROR;
   }

   const bool success = read(m_fd, data, length) == length;
   return success ? I2cStatus::SUCCESS : I2cStatus::TRANSMISSION_ERROR;
}

I2cStatus i2cMasterWriteThenRead(uint8_t dev_addr, const uint8_t* write_data,
                                 uint8_t write_length, uint8_t* read_data,
                                 uint8_t read_length)
{
   using TBuf = __typeof__(i2c_msg::buf);
   struct i2c_msg msg_write_read[] = {
      {.addr = dev_addr,
       .flags = 0, /* write */
       .len = write_length,
       .buf = reinterpret_cast<TBuf>(const_cast<uint8_t*>(write_data))},
      {.addr = dev_addr,
       .flags = I2C_M_RD,
       .len = read_length,
       .buf = reinterpret_cast<TBuf>(read_data)}};

   struct i2c_rdwr_ioctl_data msgset = {.msgs = msg_write_read, .nmsgs = 2};
   const int res = ioctl(m_fd, I2C_RDWR, &msgset);
   return res == -1 ? I2cStatus::TRANSMISSION_ERROR : I2cStatus::SUCCESS;
}
