#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#include "spi.h"

#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_SS DDB2

void SPI_MasterInit() {

  /* Set output and clock pin to output */
DDRB = (1 << DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);

  /* Enable SPI, set mode to master and clock to fck/16 */
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_MasterTX(char data) {
  SPDR = data;
  /* Wait to transmit */
  while (!(SPSR & (1<<SPIF)))
    ;
}

/* Transmit a string over MOSI */
void SPI_MasterTXString(char* string) {
  for (int i = 0; string[i] != '\0'; i++) {
    SPI_MasterTX(string[i]);
  }
}


/* Returns recieved value from master */
uint8_t SPI_MasterRX() {
  return SPDR;
}



