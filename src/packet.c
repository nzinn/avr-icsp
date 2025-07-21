
#include "packet.h"
#include "usart.h"


/* Values used to delimit end and start of frames */
#define FRAME_START_MAGIC 0xCEC4
#define FRAME_END_MAGIC 0x0F14



/* Recieves a UART packet. Signals 0 on error, 1 on success */
uint8_t PACK_rec_packet(uint8_t *buf, uint8_t buf_size) {
    uint8_t lsb = USART_rx();
  uint8_t msb = USART_rx();

  uint16_t magic_num = ((uint16_t)msb << 8) | lsb;

  /* Notify that the packet has an incorrect start frame and return */
  if (magic_num != FRAME_START_MAGIC) {
    return 0;
  }

  /* Get packet data */
  for (int i = 0; i < buf_size; i++) {
    buf[i] = USART_rx();
  }

  /* Verify ending packet */
  lsb = USART_rx();
  msb = USART_rx();

  magic_num = ((uint16_t)msb << 8) | lsb;

  /* Notify that the packet has an incorrect start frame and return */
  if (magic_num != FRAME_END_MAGIC) {
    return 0;
  }

  return 1;
}

void PACK_send_packet(uint8_t *buf, uint8_t buf_size) {
  /* Send frame start magic number */
  USART_tx((unsigned char)FRAME_START_MAGIC);
  USART_tx((unsigned char)(FRAME_START_MAGIC >> 8));

  for (int i = 0; i < buf_size; i++) {
    USART_tx(buf[i]);
  }

  /* Send frame end magic number */
  USART_tx((unsigned char)FRAME_END_MAGIC);
  USART_tx((unsigned char)(FRAME_END_MAGIC >> 8));
  
}

