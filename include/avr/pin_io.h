/* Pin I/O helper
 *
 * Originally developed (c) 2007 Roboterclub Aachen e.V.
 * Modified by Klaus Schneider-Zapp klaus underscore snd at web dot de
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef PIN_IO_H
#define PIN_IO_H

/* Macros for setting or reading a certain I/O pin. Those macros help to make the code portable.
 * Changing the pin only requires changing one define.
 *
 * Example:
 * @code
 * #define LED   D,5
 * SET_OUTPUT(LED);
 * SET(LED);
 * @endcode
 */
#if defined(__DOXYGEN__)

#define RESET(x)   /**< Set a pin to zero */
#define SET(x)     /**< Set a pin to one */
#define	TOGGLE(x)  /**< Toggle a pin */

#define	SET_OUTPUT(x)            /**< Set a pin as output */
#define	SET_INPUT(x)             /**< Set a pin as input */
#define	SET_PULLUP(x)            /**< Activates pullup resistor of a pin (if it is set as input) */
#define	SET_INPUT_WITH_PULLUP(x) /**< Set a pin as input with activated pullup resistor */
#define	IS_SET(x)                /**< Check state of a pin */

#else /* !DOXYGEN */

/* Warum hier zum Teil so seltsame Konstrukte notwendig sind wird zum Beispiel
 * in http://www.mikrocontroller.net/forum/read-1-324854.html#324980 erklärt. */
#define	PORT(x)			_port2(x)
#define	DDR(x)			_ddr2(x)
#define	PIN(x)			_pin2(x)
#define	REG(x)			_reg(x)
#define	PIN_NUM(x)		_pin_num(x)

#define	RESET(x)		RESET2(x)
#define	SET(x)			SET2(x)
#define	TOGGLE(x)		TOGGLE2(x)
#define	SET_OUTPUT(x)	SET_OUTPUT2(x)
#define	SET_INPUT(x)	SET_INPUT2(x)
#define	SET_PULLUP(x)	SET2(x)
#define	IS_SET(x)		IS_SET2(x)

#define	SET_INPUT_WITH_PULLUP(x)	SET_INPUT_WITH_PULLUP2(x)

#define	_port2(x)	PORT ## x
#define	_ddr2(x)	DDR ## x
#define	_pin2(x)	PIN ## x

#define	_reg(x,y)		x
#define	_pin_num(x,y)	y

#define	RESET2(x,y)		PORT(x) &= ~(1<<y)
#define	SET2(x,y)		PORT(x) |= (1<<y)
#define	TOGGLE2(x,y)	PORT(x) ^= (1<<y)

#define	SET_OUTPUT2(x,y)	DDR(x) |= (1<<y)
#define	SET_INPUT2(x,y)		DDR(x) &= ~(1<<y)
#define	SET_INPUT_WITH_PULLUP2(x,y)	SET_INPUT2(x,y);SET2(x,y)

#define	IS_SET2(x,y)	((PIN(x) & (1<<y)) != 0)

#endif /* !DOXYGEN */

#endif /* PIN_IO_H */
