#ifndef IST_H
#define IST_H

#include <stdint.h>

uint8_t SPI_prog_enable();

void SPI_chip_erase();

void SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb);
uint8_t SPI_read_flash_addr_high(uint8_t addr_lsb, uint8_t addr_msb);
uint8_t SPI_read_flash_addr_low(uint8_t addr_lsb, uint8_t addr_msb);
  
void SPI_write_flash_page(uint8_t addr_lsb, uint8_t addr_msb);

#endif 
