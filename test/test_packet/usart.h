#ifndef USART_H
#define USART_H

#include <stdint.h>
void USART_init(uint16_t baudRate);
void USART_tx(char c);
uint8_t USART_rx();
void USART_tx_string(char *s);
#endif 
