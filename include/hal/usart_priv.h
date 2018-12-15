// private implementation-specific

#ifdef __AVR
#include <avr/io.h>

void usartInitImpl();

constexpr uint16_t computeUbrrValue(unsigned long baudRate)
{
   return static_cast<uint16_t>((F_CPU + baudRate * 8UL) / (baudRate * 16UL) -1UL);
}

constexpr unsigned long computeRealBaudRate(uint16_t ubrr)
{
   return F_CPU / (16L * (static_cast<unsigned long>(ubrr) + 1L));
}

template <unsigned long baudrate>
void usartInit()
{
   const auto ubrr = computeUbrrValue(baudrate);
   const auto real_baudrate = computeRealBaudRate(ubrr);
   constexpr const unsigned long baudrate_error_promille = real_baudrate * 1000UL / baudrate;
   static_assert(baudrate_error_promille >= 990 && baudrate_error_promille <= 1010,
                 "Systematic baud rate error is too large. It must be lower than 1%.");
#ifdef UBRR0H
   UBRR0H = static_cast<uint8_t>(ubrr >> 8);
   UBRR0L = static_cast<uint8_t>(ubrr & 0xFF);
#else
   UBRRH = static_cast<uint8_t>(ubrr >> 8);
   UBRRL = static_cast<uint8_t>(ubrr & 0xFF);
#endif

   usartInitImpl();
}
#endif

/*  Read received characters from serial port
 *
 *  @internal
 *  @param dstbuf Buffer to fill in data
 *  @param maxlength Buffer length
 *  @param endchar Set to 0 for binary mode
 *                 Set to any other character for string mode where
 *                 reading stops if \e endchr is encountered
 *  @return Number of characters read (may be 0 if none are available to be read)
 *  @note Can also return 0 if usartAvailRead() returns true,
 *        in case there is just one character left which is \e endchar
 */
uint8_t usartReadUntil(char* dstbuf, uint8_t maxlength, char endchar);

inline uint8_t usartRead(char* dstbuf, uint8_t maxlength)
{
   return usartReadUntil(dstbuf, maxlength, '\0');
}

inline uint8_t usartReadLine(char* dstbuf, uint8_t maxlength)
{
   return usartReadUntil(dstbuf, maxlength, '\n');
}

#ifdef __AVR
enum class MemoryLocation
{
   RAM,
   PROGRAM_SPACE
};

/** Write out data to serial port
 *
 *  @param buf     Buffer which contains the data
 *  @param length  Number of bytes to be written, or the special value 0
 *                 which means write until end of string ('\0')
 *  @param memory  Specifies whether @e buf is located in RAM or in program memory
 *  @return Number of bytes actually written
 *  @note Uses extern "C" linkage to allow linking from a C module. This is required
 *        for the FILE stream use, see usart_stdout.c.
 *  @note Even if interrupt-driven operation is enabled, this function may block for
 *        a while if the write buffer does not have sufficiently space to take all data.
 *        However it will not discard any data, thus on return all data will have been
 *        written.
 */
extern "C" uint8_t usartWriteImpl(const char* buf, uint8_t length,
                                  MemoryLocation memoryLocation);

inline uint8_t usartWrite(const char* buf, uint8_t length)
{
   return usartWriteImpl(buf, length, MemoryLocation::RAM);
}

inline uint8_t usartWrite_P(const char* buf, uint8_t length)
{
   return usartWriteImpl(buf, length, MemoryLocation::PROGRAM_SPACE);
}

inline uint8_t usartWriteString(const char* buf)
{
   return usartWriteImpl(buf, 0, MemoryLocation::RAM);
}

inline uint8_t usartWriteString_P(const char* buf)
{
   return usartWriteImpl(buf, 0, MemoryLocation::PROGRAM_SPACE);
}
#endif
