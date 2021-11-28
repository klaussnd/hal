/*  Hardware abstraction layer for embedded systems
 *  AVR special pin definitions for various chips
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

#ifndef PIN_NAMES_H
#define PIN_NAMES_H

/* external interrupts */
#if AVR_ARCH == 2 || AVR_ARCH == 25
 #define PIN_INT0 B,2
#else
 #define PIN_INT0 D,2
 #define PIN_INT1 D,3
#endif

/* OC pins */
#if defined __AVR_ATmega644__
 #define PIN_OC0A B,3
 #define PIN_OC0B B,4
 #define PIN_OC2A D,7
 #define PIN_OC2B D,6
#elif defined __AVR_ATmega168__ || defined __AVR_ATmega168P__ || \
      defined __AVR_ATmega168A__ || \
      defined __AVR_ATmega328P__
 #define PIN_OC0A D,6
 #define PIN_OC0B D,5
 #define PIN_OC1A B,1
 #define PIN_OC1B B,2
 #define PIN_OC2A B,3
 #define PIN_OC2B D,3
#elif defined __AVR_ATmega8__
 #define PIN_OC1A B,1
 #define PIN_OC1B B,2
 #define PIN_OC2  B,3
#elif defined __AVR_ATmega16__ || defined __AVR_ATmega32__
 #define PIN_OC0A B,3 /* OC0 */
 #define PIN_OC2A D,7 /* OC2 */
#elif defined __AVR_ATtiny84__ || defined __AVR_ATtiny84A__ || \
      defined __AVR_ATtiny44__ || defined __AVR_ATtiny44A__ || \
      defined __AVR_ATtiny24__ || defined __AVR_ATtiny24A__
 #define PIN_OC0A B,2
 #define PIN_OC0B A,7
 #define PIN_OC1A A,6
 #define PIN_OC1B A,5
#elif defined __AVR_ATtiny25__ || defined __AVR_ATtiny45__ || \
      defined __AVR_ATtiny85__
 #define PIN_OC0A B,0
 #define PIN_OC0B B,1
 #define PIN_OC1A B,1 /* same as OC0B */
 #define PIN_OC1B B,4
#else
 #error "Unknown MCU"
#endif

/* USART */
#define PIN_USART_RX D,0
#define PIN_USART_TX D,1

/* SPI */
#if defined __AVR_ATmega168__ || defined __AVR_ATmega168P__ || \
    defined __AVR_ATmega168A__ || \
    defined __AVR_ATmega328P__
#define PIN_SPI_MOSI B,3
#define PIN_SPI_MISO B,4
#define PIN_SPI_SCK B,5
#define PIN_SPI_SS B, 2
#endif

#endif /* PIN_NAMES_H */
