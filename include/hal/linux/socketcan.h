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

#include <hal/communication/can_message.h>

/* Interface for socketcan (Linux) which is API-compatible with avr-canlib */

bool can_init();
bool can_close();

/** Check if a new CAN message is available */
bool can_check_message();

/** Receive a CAN message
 *  @return false if no message was available
 */
bool can_get_message(CanMessage* message);

/** Send a CAN message */
bool can_send_message(const CanMessage* message);
