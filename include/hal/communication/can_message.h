#pragma once

#include <stdint.h>

struct CanMessage
{
   uint32_t id;				//!< message ID (11 or 29 bit)
   struct {
      int rtr : 1;			//!< Remote-Transmit-Request-Frame?
      int extended : 1;		//!< extended ID?
   } flags;
   uint8_t length;			//!< number of data bytes
   uint8_t data[8];			//!< the data of the message
};
