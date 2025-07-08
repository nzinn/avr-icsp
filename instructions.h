#ifndef IST_H
#define IST_H

#include <stdint.h>

uint8_t SPI_prog_enable();

void SPI_chip_erase();

void SPI_write_flash_addr(uint8_t addr_lsb, uint8_t data_lsb, uint8_t data_msb);
void SPI_write_flash_page(uint8_t addr_msb);
char SPI_read_low(uint16_t addr);

#endif 
