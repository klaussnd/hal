/*  Hardware abstraction layer for embedded systems
 *  AVR timer abstraction
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

#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>
#include <avr/io.h>


#define TMR0_PRE_0    0
#define TMR0_PRE_1    (1<<CS00)
#define TMR0_PRE_8    (1<<CS01)
#define TMR0_PRE_64   ((1<<CS01) | (1<<CS00))
#define TMR0_PRE_256  (1<<CS02)
#define TMR0_PRE_1024 ((1<<CS02) | (1<<CS00))
#define TMR0_PRE_MASK ((1<<CS02) | (1<<CS01) | (1<<CS00))
#define TMR0_EXT_CLOCK(edge) ((1<<CS02) | (1<<CS01) | (edge << CS00))

#define TMR1_PRE_0    0
#define TMR1_PRE_1    (1<<CS10)
#define TMR1_PRE_8    (1<<CS11)
#define TMR1_PRE_64   ((1<<CS11) | (1<<CS10))
#define TMR1_PRE_256  (1<<CS12)
#define TMR1_PRE_1024 ((1<<CS12) | (1<<CS10))
#define TMR1_PRE_MASK ((1<<CS12) | (1<<CS11) | (1<<CS10))
#define TMR1_EXT_CLOCK(edge) ((1<<CS12) | (1<<CS11) | (edge << CS10))

#define TMR2_PRE_0    0
#define TMR2_PRE_1    (1<<CS20)
#define TMR2_PRE_8    (1<<CS21)
#define TMR2_PRE_32   ((1<<CS21) | (1<<CS20))
#define TMR2_PRE_64   (1<<CS22)
#define TMR2_PRE_128  ((1<<CS22) | (1<<CS20))
#define TMR2_PRE_256  ((1<<CS22) | (1<<CS21))
#define TMR2_PRE_1024 ((1<<CS22) | (1<<CS21) | (1<<CS20))
#define TMR2_PRE_MASK ((1<<CS22) | (1<<CS21) | (1<<CS20))

#define RAISING_EDGE 1
#define FALLING_EDGE 0

#if AVR_ARCH == 2 || AVR_ARCH == 25
 #define TMR_VECT_BASENAME TIM
#else
 #define TMR_VECT_BASENAME TIMER
#endif



/** Set value of timer @e tmr_no to @e val */
#define TMR_SET_VAL(tmr_no, val) TMR_SET_VAL_X(tmr_no, val)

/** Get value of timer @e tmr_no */
#define TMR_GET_VAL(tmr_no) TMR_GET_VAL_X(tmr_no)

/** convert time in milliseconds to number of timer tics */
#define TMR_MSEC2TIMER(ms, pre) TMR_MSEC2TIMER_X(ms, pre)

/** set timer to normal mode */
#define TMR_SET_NORMAL(tmr_no) TMR_SET_NORMAL_X(tmr_no)

/** Set timer @e tmr_no prescaler to value @e pre
 *  and thereby also starts the timer
 *  @e pre must be a valid prescaler factor
 */
#define TMR_SET_PRESCALER(tmr_no, pre) TMR_SET_PRE(tmr_no, pre)

/** Sets timer @e tmr_no to operate on external clock on Tn pin
 *  (with n = tmr_no) on @e edge
 *  @param edge RAISING_EDGE or FALLING_EDGE
 */
#define TMR_SET_EXT_CLOCK(tmr_no, edge) TMR_SET_EXT_CLOCK_X(tmr_no, edge)

/** Stop timer */
#define TMR_STOP(tmr_no) TMR_SET_PRESCALER(tmr_no, 0)

/** Set compare match value for timer/channel @e tmr_no_ch to @e val
 *
 *  @param tmr_no_ch timer/channel in comma-separated notation as 1,A
 *  @param val    value
 *  @note Some AVR chips only support one channel (e.g. atmega16), with these
 *        chips the channel is ignored.
 */
#define TMR_SET_OCR(tmr_no_ch, val) TMR_SET_OCR2(tmr_no_ch, val)


/** Set compare match value for timer @e tmr_no and channel @e ch to e val
 *  In contrast to TMR_SET_OCR, this macro needs two separate arguments
 *  for number and channel.
 *  @param tmr_no timer number (0, 1, 2)
 *  @param ch     channel (A, B)
 *  @param val    value
 *  @see TMR_SET_OCR
 */
#define TMR_SET_OCR2(tmr_no, ch, val) TMR_SET_OCR_X(tmr_no, ch, val)

/** Get compare match value for timer/channel @e tmr_no_ch
 *  @see TMR_SET_OCR
 */
#define TMR_GET_OCR(tmr_no_ch) TMR_GET_OCR2(tmr_no_ch)

/** Get compare match value for timer @e tmr_no and channel @e ch
 *  @see TMR_SET_OCR2
 */
#define TMR_GET_OCR2(tmr_no, ch) TMR_GET_OCR_X(tmr_no, ch)

/** Set timer overflow interrupt
 *  @param tmr_no timer number (0, 1, 2)
 */
#define TMR_SET_OVF_IRQ(tmr_no) TMR_SET_OVF_IRQ_X(tmr_no)

/** Set compare match interrupt
 *  @param tmr_no timer number (0, 1, 2)
 *  @param ch     channel (A, B)
 */
#define TMR_SET_COMP_IRQ(tmr_no, ch) TMR_SET_COMP_IRQ_X(tmr_no, ch)

/** Unset compare match interrupt */
#define TMR_UNSET_COMP_IRQ(tmr_no, ch) TMR_UNSET_COMP_IRQ_X(tmr_no, ch)

/** timer overflow irq handler for timer no @e tmr_no */
#define TMR_OVF_ISR(tmr_no) TMR_OVF_ISR_X(TMR_VECT_BASENAME, tmr_no)

/** timer compare match irq handler for timer no @e tmr_no
    and channel @e ch */
#define TMR_COMP_ISR(tmr_no, ch) TMR_COMP_ISR_X(TMR_VECT_BASENAME, tmr_no, ch)


/* with all those macros, an additional macro is required to allow using
   constants in them, otherwise the ## joining will fail */

#define TMR_GET_VAL_X(tmr_no) TCNT ## tmr_no
#define TMR_SET_VAL_X(tmr_no, val) TMR_GET_VAL_X(tmr_no) = val

#define TMR_OVF_ISR_X(tmr_basename, tmr_no) TMR_OVF_ISR_XX(tmr_basename, tmr_no)
#define TMR_OVF_ISR_XX(tmr_basename, tmr_no) ISR(tmr_basename ## tmr_no ## _OVF_vect)

#define TMR_MSEC2TIMER_X(ms, pre) TMR_MSEC2TIMER_XX(ms ## UL, pre ## UL)
#define TMR_MSEC2TIMER_XX(ms, pre) (ms * (F_CPU / pre) / 1000UL)

#if defined __AVR_ATmega88__ || defined __AVR_ATmega128__ || \
    defined __AVR_ATmega168__ || defined __AVR_ATmega168P__ || \
    defined __AVR_ATmega168A__ || defined __AVR_ATmega328P__ || \
    defined __AVR_ATmega644__ || \
    defined __AVR_ATtiny84__ || defined __AVR_ATtiny84A__ || \
    defined __AVR_ATtiny44__ || defined __AVR_ATtiny44A__ || \
    defined __AVR_ATtiny24__ || defined __AVR_ATtiny24A__

#define TMR_SET_PRE(tmr_no, pre) \
   TCCR ## tmr_no ## B = ((TCCR ## tmr_no ## B) & ~(TMR ## tmr_no ## _PRE_MASK)) | (TMR ## tmr_no ## _PRE_ ## pre)

#define TMR_SET_EXT_CLOCK_X(tmr_no, edge) \
   TCCR ## tmr_no ## B = ((TCCR ## tmr_no ## B) & ~(TMR ## tmr_no ## _PRE_MASK)) | (TMR ## tmr_no ## _EXT_CLOCK(edge))

#define TMR_SET_NORMAL_X(tmr_no) \
   TCCR ## tmr_no ## A = 0; \
   TCCR ## tmr_no ## B &= ~(1<<WGM ## tmr_no ## 2)

#define TMR_SET_OCR_X(tmr_no, ch, val) \
   OCR ## tmr_no ## ch = val

#define TMR_GET_OCR_X(tmr_no, ch) \
   OCR ## tmr_no ## ch

#define TMR_SET_OVF_IRQ_X(tmr_no) \
   TIMSK ## tmr_no |= (1<<TOIE ## tmr_no)

#define TMR_SET_COMP_IRQ_X(tmr_no, ch) \
   TIMSK ## tmr_no |= (1<<OCIE ## tmr_no ## ch)

#define TMR_UNSET_COMP_IRQ_X(tmr_no, ch) \
   TIMSK ## tmr_no &= ~(1<<OCIE ## tmr_no ## ch)

#define TMR_COMP_ISR_X(tmr_basename, tmr_no, ch) TMR_COMP_ISR_XX(tmr_basename, tmr_no, ch)
#define TMR_COMP_ISR_XX(tmr_basename, tmr_no, ch) ISR(tmr_basename ## tmr_no ## _COMP ## ch ## _vect)

#elif defined __AVR_ATmega32__ || defined __AVR_ATmega16__ || \
      defined __AVR_ATmega8__  || \
      defined __AVR_ATtiny45__ || defined __AVR_ATtiny85__

#define TMR_SET_NORMAL_X(tmr_no) TMR ## tmr_no ## _SET_NORMAL_X /* special for each timer */

#define TMR0_SET_NORMAL_X
#define TMR1_SET_NORMAL_X \
   TCCR1A = 0; \
   TCCR1B &= ~((1<<ICES1) | (1<<WGM13) | (1<<WGM12))
#define TMR2_SET_NORMAL_X TCCR2 &= ~((1<<WGM20) | (1<<WGM21) | (1<<COM20) | (1<<COM21))

#define TMR_TCCR0 TCCR0
#define TMR_TCCR1 TCCR1B
#define TMR_TCCR2 TCCR2
#define TMR_SET_PRE(tmr_no, pre) \
   TMR_TCCR ## tmr_no = ((TMR_TCCR ## tmr_no) & ~(TMR ## tmr_no ## _PRE_MASK)) | (TMR ## tmr_no ## _PRE_ ## pre)

#if defined __AVR_ATmega32__ || defined __AVR_ATmega16__
 #define TMR0_OCR(ch) OCR0
#elif defined __AVR_ATmega8__
 #define TMR0_OCR(ch) NON_EXISTENT
#endif
#define TMR1_OCR(ch) OCR1 ## ch
#define TMR2_OCR(ch) OCR2
#define TMR_SET_OCR_X(tmr_no, ch, val) TMR ## tmr_no ## _OCR(ch) = val
#define TMR_GET_OCR_X(tmr_no, ch) TMR ## tmr_no ## _OCR(ch)

#define TMR_SET_OVF_IRQ_X(tmr_no) \
   TIMSK |= (1<<TOIE ## tmr_no)

#define TMR0_OCIE(ch) OCIE0
#define TMR1_OCIE(ch) OCIE1 ## ch
#define TMR2_OCIE(ch) OCIE2
#define TMR_SET_COMP_IRQ_X(tmr_no, ch) \
   TIMSK |= (1<<TMR ## tmr_no ## _OCIE(ch))
#define TMR_UNSET_COMP_IRQ_X(tmr_no, ch) \
   TIMSK &= ~(1<<TMR ## tmr_no ## _OCIE(ch))

#define TMR0_COMP_ISR(tmr_basename, ch) ISR(tmr_basename ## 0_COMP_vect)
#define TMR1_COMP_ISR(tmr_basename, ch) ISR(tmr_basename ## 1_COMP ## ch ## _vect)
#define TMR2_COMP_ISR(tmr_basename, ch) ISR(tmr_basename ## 2_COMP_vect)
#define TMR_COMP_ISR_X(tmr_basename, tmr_no, ch) TMR ## tmr_no ## _COMP_ISR(tmr_basename, ch)

#else
 #error Unknown AVR type
#endif

/** convert number in macro to UL */
#define MAKE_UL(num) MAKE_UL_X(num)
#define MAKE_UL_X(num) num ## UL

#endif /* TIMER_H */
