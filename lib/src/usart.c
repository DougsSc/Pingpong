#include <sys/usart.h>

static int SERIAL_PORT = 0x3f8;

void usart_init(int base_addr)
{  
   outb(base_addr + 1, 0x00);    // Disable all interrupts
   outb(base_addr + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(base_addr + 0, 0x01);    // Set divisor to 3 (lo byte) 115000 baud
   outb(base_addr + 1, 0x00);    //                  (hi byte)
   outb(base_addr + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(base_addr + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(base_addr + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void usart_write(int base_addr, unsigned char c)
{
   while (is_transmit_empty(base_addr) == 0);
 
   outb(base_addr,c);
}

void usart_puts(char *str)
{
   while (*str)
      usart_write(SERIAL_PORT, *str++);   
   
   usart_write(SERIAL_PORT, '\n');
}

int is_transmit_empty(int base_addr) {
   return inb(base_addr + 5) & 0x20;
}