#ifndef PROG_H
#define PROG_H
#include <stdint.h>


uint8_t PROG_write_flash(uint8_t *buf, uint8_t buf_size,
                         uint8_t page_num_words, uint8_t addr_lsb,
                         uint8_t addr_msb);

void PROG_read_flash(uint8_t *buf, uint8_t num_bytes, uint8_t addr_lsb, uint8_t addr_msb);

uint8_t PROG_enable_programming(uint8_t reset_pin, uint8_t num_tries);

void PROG_read_fuse_bits(uint8_t *buf);

#endif

