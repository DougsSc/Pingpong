#include <sys/usart.h>

#define PORT_1 0x3F8
#define PORT_1 0x2F8

static int SERIAL_PORT = 0x3F8;

void usart_init(int PORT)
{  
   SERIAL_PORT = PORT;
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x01);    // Set divisor to 3 (lo byte) 115000 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 1, 0x01);    // Enable data interrupt
}

void usart_write(int base_addr, unsigned char c)
{
   while (is_transmit_empty() == 0);
 
   outb(base_addr, c);
}

void usart_puts(char *str)
{
   while (*str)
      usart_write(SERIAL_PORT, *str++);   
   
   usart_write(SERIAL_PORT, '\n');
}

int is_transmit_empty() 
{
   return inb(SERIAL_PORT + 5) & 0x20;
}