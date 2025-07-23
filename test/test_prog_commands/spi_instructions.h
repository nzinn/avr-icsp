#ifndef SPI_INSTRUCTIONS_H
#define SPI_INSTRUCTIONS_H
#include "../fff.h"
#include <stdint.h>
DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(uint8_t, SPI_prog_enable);

FAKE_VOID_FUNC(SPI_chip_erase);

FAKE_VOID_FUNC(SPI_write_flash_addr, uint8_t, uint8_t, uint8_t);

FAKE_VALUE_FUNC(uint8_t, SPI_read_flash_addr_high, uint8_t, uint8_t);
FAKE_VALUE_FUNC(uint8_t, SPI_read_flash_addr_low, uint8_t, uint8_t);


FAKE_VOID_FUNC(SPI_write_flash_page, uint8_t, uint8_t);


#endif
