#ifndef SPI_INSTRUCTIONS_H
#define SPI_INSTRUCTIONS_H
#include <stdint.h>
uint8_t SPI_prog_enable();
void SPI_chip_erase(void);
void SPI_write_flash_addr(uint8_t, uint8_t, uint8_t);

uint8_t SPI_read_flash_addr_high(uint8_t, uint8_t);
uint8_t SPI_read_flash_addr_low(uint8_t, uint8_t);


void SPI_write_flash_page(uint8_t, uint8_t);

uint8_t SPI_read_fuse_low();
uint8_t SPI_read_fuse_high();


#endif
