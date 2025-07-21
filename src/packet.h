#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

void PACK_send_packet(uint8_t *buf, uint8_t buf_size);

uint8_t PACK_rec_packet(uint8_t *buf, uint8_t buf_size);

#endif
