#include <hal/usart.h>

#include <hal/avr/pin_io.h>
#include <hal/avr/pin_names.h>
#include <utils/fifo.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifndef USART_RX_BUFLEN
#define USART_RX_BUFLEN 64
#endif
#ifndef USART_TX_BUFLEN
#define USART_TX_BUFLEN 64
#endif

namespace
{
void setWriteIrq();
void deactivateWriteIrq();

// Temporarily deactivate interrupts for receive buffer for atomic access
class AtomicBlockReceive
{
public:
   AtomicBlockReceive();
   ~AtomicBlockReceive();

private:
   uint8_t m_originalState;
};

// send and receive buffers (global state)
struct UsartReceiveBuffer
{
   Fifo<char, USART_RX_BUFLEN> fifo;
   uint8_t newline_count{0};
};

struct
{
   volatile UsartReceiveBuffer recv;
   volatile Fifo<char, USART_TX_BUFLEN> send;
} m_usart_data;
}  // namespace

void usartInitImpl()
{
   confPinAsOutput(PIN_USART_TX);
   confPinAsInput(PIN_USART_RX);

#if defined UCSR0B
   UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    // enable UART TX / RX
   UCSR0B |= (1 << RXCIE0);                  // interrupt on receive
   UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);  // asynchronous 8N1
#elif UCSRB
   UCSRB |= (1 << TXEN) | (1 << RXEN);
   UCSRB |= (1 << RXCIE);
   UCSRC = (1 << UCSZ1) | (1 << UCSZ0)
#ifdef URSEL
           | (1 << URSEL)  // chip-specific to set this register
#endif
      ;
#endif
}

bool usartAvailReadLine()
{
   /* atomic read so no need to de-activate interrupts */
   return (m_usart_data.recv.newline_count > 0);
}

uint8_t usartReadUntil(char* dstbuf, uint8_t maxlength, const char endchar)
{
   if (endchar != 0)
   {
      --maxlength; /* string mode: require additional '\0' to terminate string */
   }

   uint8_t ind = 0;

   /* disable USART receive interrupt to avoid the irq handler to
      write the global memory while we are accessing it */
   {
      AtomicBlockReceive lock;
      /* copy data */
      for (ind = 0; !m_usart_data.recv.fifo.isEmpty() && ind < maxlength; ++ind, ++dstbuf)
      {
         *dstbuf = m_usart_data.recv.fifo.read();
         if (*dstbuf == '\n') /* newline counter */
         {
            --m_usart_data.recv.newline_count;
         }
         if (endchar != 0 && *dstbuf == endchar) /* encountered endchar */
            break;
      }
   }
   /* Terminate string if string mode */
   if (endchar != 0)
   {
      *dstbuf = '\0';
   }

   return ind;
}

uint8_t usartWriteImpl(const char* buf, const uint8_t length, const MemoryLocation memory)
{
   uint8_t ind = 0;

   /* de-activate interrupts to avoid writing the global buffer
      during an interrupt */
   deactivateWriteIrq();
   /* Copy data into buffer */
   char chr;
   for (ind = 0;
        ((chr = (memory == MemoryLocation::PROGRAM_SPACE) ? pgm_read_byte(buf) : *buf)
            != '\0'
         && length == 0)
        || (ind < length);
        ++ind, ++buf)
   {
      /* If the transmit buffer is full, wait until we have space again.
         We need to temorarily re-enable interrupts for the transmit
         to be conducted by the ISR
         If data can be written just now, no interrupt is generated
         automatically. Thus manually trigger it in this case. */
      while (m_usart_data.send.isFull())
      {
         setWriteIrq();
         _delay_ms(50);
         deactivateWriteIrq();
      }
      m_usart_data.send.write(chr);
   }
   /* Enable write interrupt. If data can be written just now, interrupt is not generated
      automatically. */
   setWriteIrq();

   return ind;
}

/* Interrupt handler for interrupt-driven data receiption */
#if defined USART0_RX_vect
ISR(USART0_RX_vect)
#elif defined USART_RX_vect
ISR(USART_RX_vect)
#else
ISR(USART_RXC_vect)
#endif
{
   const uint8_t chr =
#ifdef UDR0
      UDR0;
#else
      UDR;
#endif
   if (!m_usart_data.recv.fifo.isFull())
   {
      m_usart_data.recv.fifo.write(chr);
      if (chr == '\n')
      {
         ++m_usart_data.recv.newline_count;
      }
   }
}

/* Interrupt handler for interrupt-driven character sending */
#if defined USART0_UDRE_vect
ISR(USART0_UDRE_vect)
#else
ISR(USART_UDRE_vect)
#endif
{
   if (!m_usart_data.send.isEmpty())
   {
      /* We have data to send */
      const uint8_t chr = m_usart_data.send.read();
#ifdef UDR0
      UDR0 = chr;
#else
      UDR = chr;
#endif
   }
   else
   {
      /* no more data, disable interrupt */
      deactivateWriteIrq();
   }
}

namespace
{
inline void setWriteIrq()
{
#ifdef UCSR0A
   UCSR0B |= (1 << UDRIE0);
#else
   UCSRB |= (1 << UDRIE);
#endif
}

inline void deactivateWriteIrq()
{
#ifdef UCSR0B
   UCSR0B &= ~(1 << UDRIE0);
#else
   UCSRB &= ~(1 << UDRIE);
#endif
}

AtomicBlockReceive::AtomicBlockReceive()
      : m_originalState(
#ifdef UCSR0B
           UCSR0B & (1 << RXCIE0)
#else
           UCSRB & (1 << RXCIE);
#endif
        )
{
#ifdef UCSR0B
   UCSR0B &= ~(1 << RXCIE0);
#else
   UCSRB &= ~(1 << RXCIE);
#endif
}

AtomicBlockReceive::~AtomicBlockReceive()
{
#ifdef UCSR0B
   UCSR0B |= m_originalState;
#else
   UCSRB |= m_originalState;
#endif
}
}  // namespace
