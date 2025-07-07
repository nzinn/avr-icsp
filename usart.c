#include "usart.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <stddef.h>

void USART_init(uint16_t ubrr) {

  /* Set baud rate */

  UBRR0H = (uint8_t)(ubrr >> 8);
  UBRR0L = (uint8_t)ubrr;

  /* Enable transmit and recieve */

  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
}

/* Transmits a character over usart */
void USART_tx(char c) {

  /* Wait for data register to be empty */
  while (!(UCSR0A >> UDRE0 & 1)) {
  }

  UDR0 = c;
}

/* Recieve a character over usart */
unsigned char USART_rx() {
  /* Wait for data */
  while (!(UCSR0A & (1 << RXC0)))
    ;

  return UDR0;
} 

/* Print a string over usart */
void USART_tx_string(char *s) {

  for (int i = 0; s[i] != '\0'; i++) {
    USART_tx(s[i]);
  }
  
}

