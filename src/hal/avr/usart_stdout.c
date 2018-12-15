// Note: This is a C module because initialising the FILE stream does not compile with C++
#include <hal/usart_stdout.h>

#include <stdint.h>
#include <stdio.h>

// duplicated from usart_priv.h to allow use in pure C module
enum MemoryLocation
{
   RAM
};
// duplicated from usart_priv.h to allow use in pure C module
uint8_t usartWriteImpl(const char* buf, uint8_t length,
                       enum MemoryLocation memoryLocation);

// for output with fprintf()
static int usartWriteChrStream(char chr, FILE* stream);

/* global variable for fprintf() on serial port */
FILE usart_stdout_var = FDEV_SETUP_STREAM(usartWriteChrStream, NULL, _FDEV_SETUP_WRITE);

FILE* usart_stdout = &usart_stdout_var;

int usartWriteChrStream(char chr, FILE* stream)
{
   usartWriteImpl(&chr, 1, RAM);
   return 0;
}
