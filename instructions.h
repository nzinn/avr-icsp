#ifndef IST_H
#define IST_H

#include <stdint.h>

uint8_t SPI_prog_enable();

void SPI_chip_erase();

void SPI_load_prog(uint16_t addr, uint16_t data);

void SPI_write_prog(uint16_t addr);

char SPI_read_low(uint16_t addr);

#endif 
