#include "usart.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <stddef.h>

void usartInit(uint16_t ubrr) {

#define USART_INIT
  /* Set baud rate */

  UBRR0H = (uint8_t)(ubrr >> 8);
  UBRR0L = (uint8_t)ubrr;

  /* Enable transmit and recieve */

  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
}

/* Transmits a character over usart */
void putChar(char c) {

#ifndef USART_INIT
#error Usart not initialized! Use usartInit()!
#endif

  /* Wait for data register to be empty */
  while (!(UCSR0A >> UDRE0 & 1)) {
  }

  UDR0 = c;
}


/* Print a string over usart */
void print(char *s) {

  for (int i = 0; s[i] != '\0'; i++) {
    putChar(s[i]);
  }
  
}

void println(char *s) {
  print(s);
  print("\r\n");
}
