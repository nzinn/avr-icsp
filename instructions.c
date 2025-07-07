#include "instructions.h"
#include "spi.h"
#define F_CPU 16000000L
#include <util/delay.h>


/* Enables programming mode on chip, returns true on success, false on error */
uint8_t SPI_prog_enable() {
  SPI_master_tx(0xAC);
  SPI_master_tx(0x53);
  SPI_master_tx(0x00);

  uint8_t echo = SPI_master_rx();

  SPI_master_tx(0x00);

  /* If slave echoes back signal all good */
  if (echo == 0x53)  {
    return 1;
  }

  return 0;
}


/* Erases program memory and EEPROM to 0xFF */
void SPI_chip_erase() {
  SPI_master_tx(0xAC);
  SPI_master_tx(0x80);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);
}

/* Check if the chip is ready to be programmed */
uint8_t SPI_check_ready() {
  SPI_master_tx(0xF0);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);

  return SPI_master_rx();
}

/* Loads data into address[5:0] of page buffer*/
void SPI_write_flash_addr(uint16_t address, uint16_t data)  {

  /* Load low data byte */
  SPI_master_tx(0x40);
  SPI_master_tx(0x00);
  SPI_master_tx((uint8_t) address);
  SPI_master_tx((uint8_t) data);

  /* Load high data byte */
  SPI_master_tx(0x48);
  SPI_master_tx(0x00);
  SPI_master_tx((uint8_t) address);
  SPI_master_tx((uint8_t) (data>>8));
}

/* write the page of address[13:6] */
void SPI_write_flash_page(uint16_t address) {
  SPI_master_tx(0x4c);
  SPI_master_tx((uint8_t) (address>>8));
  SPI_master_tx((uint8_t) address);
  SPI_master_tx(0x00);

  /* Wait for page write */
  _delay_ms(1);
  while(!SPI_check_ready()) {
    _delay_ms(1);
  }
  
}

char SPI_ReadLow(uint16_t address) {
  SPI_master_tx(0x20);
  SPI_master_tx(address>>8);
  SPI_master_tx((uint8_t) address);
  SPI_master_tx(0x0);

  return SPI_master_rx();
}
