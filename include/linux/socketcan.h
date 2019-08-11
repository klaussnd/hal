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
