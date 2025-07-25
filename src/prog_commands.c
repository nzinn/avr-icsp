#include "prog_commands.h"
#include "iohelper.h"
#include "spi_instructions.h"

#define F_CPU 16000000L
#include <util/delay.h>



/* Tries to enable programming for a maximum of num_tries, returns false on
 * error */
uint8_t PROG_enable_programming(uint8_t reset_pin, uint8_t num_tries) {

  set_pin_dir(reset_pin, OUTPUT);
  dig_write(reset_pin, LOW);
  _delay_ms(20);

  int tries = 0;
  while (!SPI_prog_enable() && tries < num_tries - 1) {
    dig_write(reset_pin, HIGH);
    _delay_us(8);
    dig_write(reset_pin, LOW);
    _delay_ms(20);
    tries++;
  }

  if (tries >= num_tries - 1) {
    return 0;
  }

  return 1;
}


/* Write the contents of buf to flash */
uint8_t PROG_write_flash(uint8_t *buf, uint8_t buf_size,
                         uint8_t page_num_words, uint8_t addr_lsb,
                         uint8_t addr_msb) {

  if (buf_size % 2 != 0 || buf == 0) {
    return 0;
  }


  uint16_t addr = ((uint16_t) addr_msb) << 8 | addr_lsb;

  for (int i = 0; i + 1 < buf_size; i += 2) {


    uint8_t addr_lsb = (uint8_t)addr;
    uint8_t addr_msb = (uint8_t) (addr >> 8);
   

    
    SPI_write_flash_addr(addr_lsb, buf[i], buf[i + 1]);

    if (addr % page_num_words == 0 && addr != 0) {
      SPI_write_flash_page(addr_lsb, addr_msb);      
    }
    
    addr++;
  }

  /* If we didn't write the page on the last word, write it now */
  addr--;
  
  if (addr % page_num_words != 0) {
    SPI_write_flash_page(addr_lsb, addr_msb);          
  }


  return 1;
}



/* Reads num_bytes starting at the address from the flash into buf */
void PROG_read_flash(uint8_t *buf, uint8_t num_bytes, uint8_t addr_lsb, uint8_t addr_msb) {

  uint16_t addr = ((uint16_t) addr_msb << 8) | (uint16_t) addr_lsb;

  for (int i = 0; i + 1 < num_bytes; i += 2) {

    uint8_t addr_lsb = (uint8_t)addr;
    uint8_t addr_msb = (uint8_t) (addr >> 8);
    
    buf[i] = SPI_read_flash_addr_low(addr_lsb, addr_msb);
    buf[i + 1] = SPI_read_flash_addr_high(addr_lsb, addr_msb);

    addr++;
  }
  
}


/* Reads the low and high fuse bytes and puts them into buf */
void PROG_read_fuse_bits(uint8_t *buf) {
  buf[0] = SPI_read_fuse_low();
  buf[1] = SPI_read_fuse_high();
}
