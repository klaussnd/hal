/*  Hardware abstraction layer for embedded systems
 *  CAN message helper
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

/** A CAN ID decribes a particular topic or message type.
 *  In order for the CAN bus arbitration to work, the same ID must not be sent from
 *  different bus participants. Therefore, if a certain topic can be sent from different
 *  senders, the ID must be made unique by changing it for each sender. An example is
 *  a lamp which can be controlled from several switches. Every sender is assigned a
 *  device ID, and the device ID is part of the CAN ID. However, for the receiver,
 *  it may be irrelevant who exactly sent the message.
 *  If a particular topic applies to several different devices, the particular device
 *  that should be addressed must be known from the ID. Therefore, the receiver device ID
 *  is part of the CAN ID in that case. An example is if there are several lamps which
 *  should be controlled separately.
 */
struct CanId
{
   uint32_t type;
   bool has_sender;          ///< The message requires to supply the sender device ID.
   bool has_receiver;        ///< The message requires to supply the receiver device ID.
   uint8_t length;           ///< Data length in byte.
   uint8_t optional_length;  ///< Optional extra data length in byte. Zero if all fields
                             ///< are mandantory.
};

constexpr uint8_t ANY_DEVICE_ID = 0;

constexpr uint32_t makeCanId(const CanId can_id, const uint8_t sender,
                             const uint8_t receiver)
{
   uint32_t id = can_id.type;
   if (can_id.has_sender)
   {
      id |= static_cast<uint32_t>(sender);
   }
   if (can_id.has_receiver)
   {
      id |= (static_cast<uint32_t>(receiver) << 8);
   }
   return id;
}

constexpr bool isMatchingCanId(const CanId can_id, const uint8_t my_device_id,
                               const uint32_t id_to_test)
{
   const uint32_t mask = can_id.has_sender ? 0xffff00 : 0xffffff;
   const uint32_t expected_id = makeCanId(can_id, ANY_DEVICE_ID, my_device_id);
   return (id_to_test & mask) == expected_id;
}

constexpr bool hasMatchingLength(const CanId can_id, const uint8_t length)
{
   return can_id.length == length || (can_id.length + can_id.optional_length) == length;
}

inline constexpr uint8_t getSender(const CanId can_id, const uint32_t id)
{
   return can_id.has_sender ? static_cast<uint8_t>(id & 0x0000ff) : ANY_DEVICE_ID;
}

namespace CanMsg
{
constexpr CanMessage makeBasicCanMessage(const CanId canId,
                                         const uint8_t sender_device_id,
                                         const uint8_t receiver_device_id)
{
   CanMessage msg{
      makeCanId(canId, sender_device_id, receiver_device_id), {0, 1}, canId.length, {}};
   return msg;
}

template <typename T>
constexpr void serialise(T value, uint8_t* buffer)
{
   static_assert(sizeof(T) == 2, "Invalid type");
   buffer[0] = static_cast<uint8_t>(value);
   buffer[1] = static_cast<uint8_t>(value >> 8);
}

template <typename T>
constexpr T deserialise(const uint8_t* buffer)
{
   static_assert(sizeof(T) == 2, "Invalid type");
   return static_cast<T>(buffer[0] | buffer[1] << 8);
}
}  // namespace CanMsg
