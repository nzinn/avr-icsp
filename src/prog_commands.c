#include "prog_commands.h"
#include "iohelper.h"
#include "spi_instructions.h"

#define F_CPU 16000000L
#include <util/delay.h>



/* Tries to enable programming for a maximum of num_tries*/
PG_STATUS PROG_enable_programming(uint8_t reset_pin, uint8_t num_tries) {

  set_pin_dir(reset_pin, OUTPUT);
  dig_write(reset_pin, LOW);
  _delay_ms(20);

  int tries = 0;
  while (SPI_prog_enable() == SP_NO_ECHO && tries < num_tries - 1) {
    /* Pulse reset line */
    dig_write(reset_pin, HIGH);
    _delay_us(8);
    dig_write(reset_pin, LOW);
    _delay_ms(20);
    tries++;
  }

  if (tries >= num_tries - 1) {
    return PG_ERR;
  }

  return PG_OK;
}


/* Write the contents of buf to flash */
PG_STATUS PROG_write_flash(uint8_t *buf, uint8_t buf_size,
                         uint8_t page_num_words, uint8_t addr_lsb,
                         uint8_t addr_msb) {

  if (buf_size % 2 != 0 || buf == 0) {
    return PG_ERR;
  }


  uint16_t addr = ((uint16_t) addr_msb) << 8 | addr_lsb;
  uint8_t wrote_flash;
  
  for (int i = 0; i + 1 < buf_size; i += 2) {
    wrote_flash = 0;

    addr_lsb = (uint8_t)addr;
    addr_msb = (uint8_t) (addr >> 8);



    if (SPI_write_flash_addr(addr_lsb, buf[i], buf[i + 1]) == SP_NO_ECHO) {
      return PG_ERR;
    }

    /* Write the page if we're on the last address*/
    if ((addr_lsb & 0x3F) == page_num_words - 1) {
      if (SPI_write_flash_page(addr_lsb, addr_msb) == SP_NO_ECHO) {
        return PG_ERR;
      }

      wrote_flash = 1;
    }
    
    addr++;
  }

  
  if (!wrote_flash) {
    if (SPI_write_flash_page(addr_lsb, addr_msb) == SP_NO_ECHO) {
      return PG_ERR;
    }
  }


  return PG_OK;
}



/* Reads num_bytes starting at the address from the flash into buf */
PG_STATUS PROG_read_flash(uint8_t *buf, uint8_t num_bytes, uint8_t addr_lsb, uint8_t addr_msb) {

  uint16_t addr = ((uint16_t) addr_msb << 8) | (uint16_t) addr_lsb;

  for (int i = 0; i + 1 < num_bytes; i += 2) {

    uint8_t addr_lsb = (uint8_t)addr;
    uint8_t addr_msb = (uint8_t) (addr >> 8);

    if (SPI_read_flash_addr_low(addr_lsb, addr_msb, &buf[i]) == SP_NO_ECHO) {
      return PG_ERR;
    }


    if (SPI_read_flash_addr_high(addr_lsb, addr_msb, &buf[i + 1])) {
      return PG_ERR;
    }

    addr++;
  }

  return PG_OK;
}


/* Reads the low and high fuse bytes and puts them into buf */
PG_STATUS PROG_read_fuse_bits(uint8_t *buf) {

  if (SPI_read_fuse_low(&buf[0]) == SP_NO_ECHO) {
    return PG_ERR;
  }

  if (SPI_read_fuse_high(&buf[1]) == SP_NO_ECHO) {
    return PG_ERR;
  }

  return PG_OK;
}


/* Sets the external crystal as the clock source for the chip.
   The clock divider is not set, and a slow powerup is used.
 */
PG_STATUS PROG_clock_set_crystal() {

  if (SPI_write_fuse_low(0xF7) == SP_NO_ECHO) {
    return PG_ERR;
  }

  return PG_OK;
}


PG_STATUS PROG_erase_chip() {
  if (SPI_chip_erase() == SP_OK) {
    return PG_OK;
  }

  return PG_ERR;
}
