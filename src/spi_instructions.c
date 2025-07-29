#include "spi_instructions.h"
#include "spi_base.h"


/* For delay  */
#define F_CPU 16000000L

#include <util/delay.h>




#define ERASE_WAIT_DELAY 9
#define FLASH_WRITE_DELAY 5


/* Send a 4-byte instruction, putting the response in rec and returning if there
 * was a successful echo*/
static SP_STATUS SPI_send_rec(const uint8_t instruction[], uint8_t *response) {
  SPI_master_tx(instruction[0]);
  SPI_master_tx(instruction[1]);
  SPI_master_tx(instruction[2]);

  uint8_t echo = SPI_master_rx();
  SPI_master_tx(instruction[3]);

  if (response) {
    *response = SPI_master_rx();
  }

  if (echo != instruction[1]) {
    return SP_NO_ECHO;
  }

  return SP_OK;
} 





/* Wait until the chip is ready for another instruction */
static SP_STATUS SPI_chip_wait_ready(uint8_t max_ms) {

  const uint8_t inst[] = {0xF0, 0x00, 0x00, 0x00};

  uint8_t pending;
<<<<<<< Updated upstream
  uint8_t ms_waited = 0;
=======
  uint8_t ms_waited;
>>>>>>> Stashed changes
  do {
    _delay_ms(1);
    ms_waited++;
    
    if (SPI_send_rec(inst, &pending) == SP_NO_ECHO) {
      return SP_NO_ECHO;
    }

    pending &= 1;
  } while (pending && ms_waited < max_ms);

  return SP_OK;
}



/* Enables programming mode on chip, signals if synchronization was achieved. */
SP_STATUS SPI_prog_enable() {

  const uint8_t inst[] = {0xAC, 0x53, 0x00, 0x00};
  
  return SPI_send_rec(inst, 0);
}




/* Erases program memory and EEPROM to 0xFF */
SP_STATUS SPI_chip_erase() {

  const uint8_t inst[] = {0xAC, 0x80, 0x00, 0x00};

  if (SPI_send_rec(inst, 0) == SP_NO_ECHO) {
    return SP_NO_ECHO;
  }

  return SPI_chip_wait_ready(ERASE_WAIT_DELAY);
}


/* Loads data into address[5:0] of page buffer*/
SP_STATUS SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb)  {


  const uint8_t low_inst[] = {0x40, 0x00, addr_lsb, data_lsb};
  const uint8_t high_inst[] = {0x48, 0x00, addr_lsb, data_msb};


  /* Load the high and low address, ORing the status. (1 is failure) */
  return SPI_send_rec(low_inst, 0) | SPI_send_rec(high_inst, 0);
}

/* Reads the high byte of the given flash address */
SP_STATUS SPI_read_flash_addr_high(uint8_t addr_lsb, uint8_t addr_msb, uint8_t *high_byte) {

  const uint8_t inst[] = {0x28, addr_msb, addr_lsb, 0x00};

  return SPI_send_rec(inst, high_byte);
}

/* Reads the low byte of the given flash address */
SP_STATUS SPI_read_flash_addr_low(uint8_t addr_lsb, uint8_t addr_msb, uint8_t *low_byte) {

  const uint8_t inst[] = {0x20, addr_msb, addr_lsb, 0x00};

  return SPI_send_rec(inst, low_byte);
}


/* write the page of address[13:6] */
SP_STATUS SPI_write_flash_page(uint8_t addr_lsb, uint8_t addr_msb) {

  const uint8_t inst[] = {0x4c, addr_msb, addr_lsb, 0x00};

  if (SPI_send_rec(inst, 0) == SP_NO_ECHO) {
    return SP_NO_ECHO;
  }

  return SPI_chip_wait_ready(FLASH_WRITE_DELAY);
}

/* Read the fuse low bits */
SP_STATUS SPI_read_fuse_low(uint8_t *low_bits) {
  const uint8_t inst[] = {0x50, 0x00, 0x00, 0x00};

  return SPI_send_rec(inst, low_bits);
}

/* Read the fuse high bits */
SP_STATUS SPI_read_fuse_high(uint8_t *high_bits) {
  const uint8_t inst[] = {0x58, 0x00, 0x00, 0x00};

  return SPI_send_rec(inst, high_bits);
}


SP_STATUS SPI_write_fuse_high(uint8_t high_bits) {
  const uint8_t inst[] = {0xAC, 0xA8, 0x00, high_bits};

  return SPI_send_rec(inst, 0);
}


SP_STATUS SPI_write_fuse_low(uint8_t low_bits) {
  const uint8_t inst[] = {0xAC, 0xA0, 0x00, low_bits};

  return SPI_send_rec(inst, 0);
}

