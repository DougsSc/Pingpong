#ifndef USART_H
#define USART_H

extern void usart_init(int base_addr);
extern void usart_write(int base_addr, unsigned char c);

#endif /* USART_H */