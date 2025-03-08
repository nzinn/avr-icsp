#ifndef USART_H
#define USART_H

#include <stdint.h>
void usartInit(uint16_t baudRate);
void putChar(char c);
void print(char *s);
void println(char *s);


#endif 
