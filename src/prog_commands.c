#include "prog_commands.h"
#include "iohelper.h"
#include "spi_instructions.h"

#define F_CPU 16000000L
#include <util/delay.h>



/* Tries to enable programming for a maximum of num_tries, returns false on
 * error */
PG_STATUS PROG_enable_programming(uint8_t reset_pin, uint8_t num_tries) {

  set_pin_dir(reset_pin, OUTPUT);
  dig_write(reset_pin, LOW);
  _delay_ms(20);

  int tries = 0;
  while (SPI_prog_enable() == SP_NO_ECHO && tries < num_tries - 1) {
    dig_write(reset_pin, HIGH);
    _delay_us(8);
    dig_write(reset_pin, LOW);
    _delay_ms(20);
    tries++;
  }

  if (tries >= num_tries - 1) {
    return PG_HARDWARE_ERR;
  }

  return PG_OK;
}


/* Write the contents of buf to flash */
PG_STATUS PROG_write_flash(uint8_t *buf, uint8_t buf_size,
                         uint8_t page_num_words, uint8_t addr_lsb,
                         uint8_t addr_msb) {

  if (buf_size % 2 != 0 || buf == 0) {
    return PG_LOGIC_ERR;
  }


  uint16_t addr = ((uint16_t) addr_msb) << 8 | addr_lsb;

  for (int i = 0; i + 1 < buf_size; i += 2) {


    uint8_t addr_lsb = (uint8_t)addr;
    uint8_t addr_msb = (uint8_t) (addr >> 8);



    if (SPI_write_flash_addr(addr_lsb, buf[i], buf[i + 1]) == SP_NO_ECHO) {
      return PG_HARDWARE_ERR;
    }

    if (addr % page_num_words == 0 && addr != 0) {
      if (SPI_write_flash_page(addr_lsb, addr_msb) == SP_NO_ECHO) {
        return PG_HARDWARE_ERR;
      }
    }
    
    addr++;
  }

  /* If we didn't write the page on the last word, write it now */
  addr--;
  
  if (addr % page_num_words != 0) {
    if (SPI_write_flash_page(addr_lsb, addr_msb) == SP_NO_ECHO) {
      return PG_HARDWARE_ERR;
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
      return PG_HARDWARE_ERR;
    }


    if (SPI_read_flash_addr_high(addr_lsb, addr_msb, &buf[i + 1])) {
      return PG_HARDWARE_ERR;
    }

    addr++;
  }
  
}


/* Reads the low and high fuse bytes and puts them into buf */
PG_STATUS PROG_read_fuse_bits(uint8_t *buf) {

  if (SPI_read_fuse_low(&buf[0]) == SP_NO_ECHO) {
    return PG_HARDWARE_ERR;
  }

  if (SPI_read_fuse_high(&buf[1]) == SP_NO_ECHO) {
    return PG_HARDWARE_ERR;
  }
}


/* Sets the external crystal as the clock source for the chip.
   The clock divider is not set, and a slow powerup is used.
 */
PG_STATUS PROG_clock_set_crystal() {

  if (SPI_write_fuse_low(0xF7) == SP_NO_ECHO) {
    return PG_HARDWARE_ERR;
  }

  return PG_OK;
}
