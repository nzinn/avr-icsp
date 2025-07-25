#include "spi_instructions.h"
#include "spi_base.h"
#include "iohelper.h"


/* For delay  */
#define F_CPU 16000000L

#include <util/delay.h>





/* Send a 4-byte instruction, putting the response  */
static uint8_t SPI_send_rec(const uint8_t instruction[], uint8_t *response) {
  SPI_master_tx(instruction[0]);
  SPI_master_tx(instruction[1]);
  SPI_master_tx(instruction[2]);

  uint8_t echo = SPI_master_rx();
  SPI_master_tx(instruction[3]);

  if (response) {
    *response = SPI_master_rx();
  }

  return echo == instruction[1];
} 



/* Enables programming mode on chip, signals if synchronization was achieved. */
uint8_t SPI_prog_enable() {

  const uint8_t inst[] = {0xAC, 0x53, 0x00, 0x00};
  
  return SPI_send_rec(inst, 0);
}




/* Check if the chip is ready for another instruction */
static uint8_t SPI_poll_ready() {

  const uint8_t inst[] = {0xF0, 0x00, 0x00, 0x00};

  uint8_t rec;

  SPI_send_rec(inst, &rec);


  /* Only lsb matters */
  return !(rec & 1);
}


/* Erases program memory and EEPROM to 0xFF */
void SPI_chip_erase() {

  const uint8_t inst[] = {0xAC, 0x80, 0x00, 0x00};

  SPI_send_rec(inst, 0);
  /* Wait for chip erase */
  while (!SPI_poll_ready()) {
      _delay_ms(1);
  }
}


/* Loads data into address[5:0] of page buffer*/
void SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb)  {


  const uint8_t low_inst[] = {0x40, 0x00, addr_lsb, data_lsb};
  const uint8_t high_inst[] = {0x48, 0x00, addr_lsb, data_msb};
  /* Load low byte */
  SPI_send_rec(low_inst, 0);
  /* Load high byte */
  SPI_send_rec(high_inst, 0);
}


uint8_t SPI_read_flash_addr_high(uint8_t addr_lsb, uint8_t addr_msb) {

  const uint8_t inst[] = {0x28, addr_msb, addr_lsb, 0x00};
  uint8_t rec;

  SPI_send_rec(inst, &rec);
  return rec;
  
}

uint8_t SPI_read_flash_addr_low(uint8_t addr_lsb, uint8_t addr_msb) {

  const uint8_t inst[] = {0x20, addr_msb, addr_lsb, 0x00};
  

  uint8_t rec;
  SPI_send_rec(inst, &rec);

  return rec;
}


/* write the page of address[13:6] */
void SPI_write_flash_page(uint8_t addr_lsb, uint8_t addr_msb) {

  const uint8_t inst[] = {0x4c, addr_msb, addr_lsb, 0x00};

  SPI_send_rec(inst, 0);
  
  /* Wait for page write */
  while(!SPI_poll_ready()) {
    _delay_ms(1);
  }
  
}



