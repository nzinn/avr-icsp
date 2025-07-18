#include "instructions.h"
#include "spi.h"

/* For delay  */
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



/* Check if the chip is ready for another instruction */
uint8_t SPI_poll_ready() {
  SPI_master_tx(0xF0);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);

  /* Only lsb matters */
  return !(SPI_master_rx() & 1);
}


/* Erases program memory and EEPROM to 0xFF */
void SPI_chip_erase() {
  SPI_master_tx(0xAC);
  SPI_master_tx(0x80);
  SPI_master_tx(0x00);
  SPI_master_tx(0x00);

  /* Wait for chip erase */
  while (!SPI_poll_ready()) {
      _delay_ms(1);
  }
}


/* Loads data into address[5:0] of page buffer*/
void SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb)  {

  /* Load low data byte */
  SPI_master_tx(0x40);
  SPI_master_tx(0x00);
  SPI_master_tx( addr_lsb);
  SPI_master_tx( data_lsb);

  /* Load high data byte */
  SPI_master_tx(0x48);
  SPI_master_tx(0x00);
  SPI_master_tx(addr_lsb);
  SPI_master_tx(data_msb);
}


uint8_t SPI_read_flash_addr_high(uint8_t addr_lsb, uint8_t addr_msb) {
  SPI_master_tx(0x28);
  SPI_master_tx(addr_msb);
  SPI_master_tx(addr_lsb);
  SPI_master_tx(0x0);

  return SPI_master_rx();
  
}

uint8_t SPI_read_flash_addr_low(uint8_t addr_lsb, uint8_t addr_msb) {
  SPI_master_tx(0x20);
  SPI_master_tx(addr_msb);
  SPI_master_tx(addr_lsb);
  SPI_master_tx(0x0);

  return SPI_master_rx();
  
}


/* write the page of address[13:6] */
void SPI_write_flash_page(uint8_t addr_lsb, uint8_t addr_msb) {
  SPI_master_tx(0x4c);
  SPI_master_tx(addr_msb);
  SPI_master_tx(addr_lsb);
  SPI_master_tx(0x00);

  /* Wait for page write */
  while(!SPI_poll_ready()) {
    _delay_ms(1);
  }
  
}



