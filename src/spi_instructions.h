#ifndef IST_H
#define IST_H

#include <stdint.h>



typedef enum {SP_OK, SP_NO_ECHO} SP_STATUS;


SP_STATUS SPI_prog_enable();

SP_STATUS SPI_chip_erase();

SP_STATUS SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb);

SP_STATUS SPI_read_flash_addr_high(uint8_t addr_lsb, uint8_t addr_msb, uint8_t *high_byte);
SP_STATUS SPI_read_flash_addr_low(uint8_t addr_lsb, uint8_t addr_msb, uint8_t *low_byte);
  
SP_STATUS SPI_write_flash_page(uint8_t addr_lsb, uint8_t addr_msb);

SP_STATUS SPI_read_fuse_low(uint8_t *low_bits);
SP_STATUS SPI_read_fuse_high(uint8_t *high_bits);


SP_STATUS SPI_write_fuse_low(uint8_t low_bits);
SP_STATUS SPI_write_fuse_high(uint8_t high_bits);

#endif 
