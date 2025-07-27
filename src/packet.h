#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>


typedef enum {PK_OK, PK_ERR} PK_STATUS;

void PACK_send_packet(uint8_t *buf, uint8_t buf_size);

PK_STATUS PACK_rec_packet(uint8_t *buf, uint8_t buf_size);

#endif
