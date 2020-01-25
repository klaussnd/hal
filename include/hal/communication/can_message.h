/*  Hardware abstraction layer for embedded systems
 *
 *  (c) Klaus Schneider-Zapp klaus underscore snd at web dot de
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 */

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
