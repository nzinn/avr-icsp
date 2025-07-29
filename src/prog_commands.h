#ifndef PROG_H
#define PROG_H
#include <stdint.h>


typedef enum {PG_OK, PG_ERR} PG_STATUS;

PG_STATUS PROG_write_flash(uint8_t *buf, uint8_t buf_size,
                         uint8_t page_num_words, uint8_t addr_lsb,
                         uint8_t addr_msb);

PG_STATUS PROG_read_flash(uint8_t *buf, uint8_t num_bytes, uint8_t addr_lsb, uint8_t addr_msb);

PG_STATUS PROG_enable_programming(uint8_t reset_pin, uint8_t num_tries);

PG_STATUS PROG_read_fuse_bits(uint8_t *buf);

PG_STATUS PROG_clock_set_crystal(void);

PG_STATUS PROG_erase_chip();

#endif

