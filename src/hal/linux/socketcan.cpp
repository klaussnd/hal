/* Based on code from the modm project
 *
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2019, Klaus Schneider-Zapp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <hal/linux/socketcan.h>

#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>

#ifndef CAN_INTERFACE_NAME
#define CAN_INTERFACE_NAME "can0"
#endif

namespace
{
int m_can_socket{-1};
}

bool can_init()
{
   m_can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);

   /* Locate the interface you wish to use */
   struct ifreq ifr;
   strcpy(ifr.ifr_name, CAN_INTERFACE_NAME);
   ioctl(m_can_socket, SIOCGIFINDEX,
         &ifr); /* ifr.ifr_ifindex gets filled with that device's index */

   /* Select that CAN interface, and bind the socket to it. */
   struct sockaddr_can addr;
   addr.can_family = AF_CAN;
   addr.can_ifindex = ifr.ifr_ifindex;
   if (bind(m_can_socket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
   {
      return false;
   };

   return true;
}

bool can_close()
{
   const bool res = close(m_can_socket) == 0;
   m_can_socket = -1;
   return res;
}

bool can_check_message()
{
   struct can_frame frame;
   const int nbytes =
      recv(m_can_socket, &frame, sizeof(struct can_frame), MSG_DONTWAIT | MSG_PEEK);

   // recv returns 'Resource temporary not available' which is weird but ignored here.
   return (nbytes > 0);
}

bool can_get_message(CanMessage* message, Blocking isBlocking)
{
   struct can_frame frame;
   const auto nbytes = recv(m_can_socket, &frame, sizeof(struct can_frame),
                            isBlocking == Blocking::NON ? MSG_DONTWAIT : 0);

   if (nbytes > 0)
   {
      message->id = frame.can_id & CAN_EFF_MASK;
      message->length = frame.can_dlc;
      message->flags.extended = frame.can_id & CAN_EFF_FLAG;
      message->flags.rtr = frame.can_id & CAN_RTR_FLAG;
      for (uint8_t i = 0; i < frame.can_dlc; ++i)
      {
         message->data[i] = frame.data[i];
      }
      return true;
   }
   return false;
}

bool can_send_message(const CanMessage* message)
{
   struct can_frame frame;

   frame.can_id = message->id;
   if (message->flags.extended)
   {
      frame.can_id |= CAN_EFF_FLAG;
   }
   if (message->flags.rtr)
   {
      frame.can_id |= CAN_RTR_FLAG;
   }
   frame.can_dlc = message->length;
   for (uint8_t i = 0; i < message->length; ++i)
   {
      frame.data[i] = message->data[i];
   }

   const int bytes_sent = write(m_can_socket, &frame, sizeof(frame));
   return (bytes_sent > 0);
}
