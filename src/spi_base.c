#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#include "spi_base.h"

/* For delay  */
#define F_CPU 16000000L
#include <util/delay.h>


#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_SS DDB2

void SPI_master_init() {

  /* Set output and clock pin to output */
DDRB |= (1 << DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);

  /* Enable SPI, set mode to master and clock to fck/64 */
 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
}

void SPI_master_tx(char data) {
  SPDR = data;
  /* Wait to transmit */
  while (!(SPSR & (1<<SPIF)))
    ;

  _delay_us(8);
}

/* Transmit a string over MOSI */
void SPI_master_tx_string(char* string) {
  for (int i = 0; string[i] != '\0'; i++) {
    SPI_master_tx(string[i]);
  }
}


/* Returns recieved value from master */
uint8_t SPI_master_rx() {
  while(!(SPSR & (1<<SPIF)))
    ;
  
  return SPDR;
}



