#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>


#include "iohelper.h"
#include "spi.h"
#include "instructions.h"
#include "usart.h"

/* For delay  */
#define F_CPU 16000000L
#include <util/delay.h>


#define RESET 2

#define USART_UBRR_VAL 103

#define MAX_PROG_ENABLE_TRIES 5



#define PACKET_SIZE 256
#define PACKET_STATUS_POS 0
#define PACKET_INST_POS 0
#define PACKET_DATA_START 1



#define FRAME_START_MAGIC 0xCEC4
#define FRAME_END_MAGIC 0x0F14
#define FRAME_SIZE PACKET_SIZE + 4




enum PACKET_INSTRUCTION_TYPE {
  PROG_ENABLE,
  CHIP_ERASE,
  WRITE_FLASH,
  READ_FLASH,
  WRITE_EEPROM,
  READ_EEPROM,
  WRITE_FUSE,
  READ_FUSE
};

enum PACKET_ERROR_STATUS {OK, MALFORMED_PACKET, EXEC_ERR};




typedef void (*state_func_t)();



/* Init functino */
void state_init();


/* Waits for a packet, writes it to memory and decodes it */
void state_recieve_packet();

/* Sends the contents of the packet buffer through UART */
void state_send_packet();


/* Writes and reads the packet to program memory */
void state_write_flash();
void state_read_flash();

/* Writes and reads packet to EEPROM */
void state_write_EEPROM();
void state_read_EEPROM();

/* Sets and reads FUSE bits */
void state_write_FUSE();
void state_read_FUSE();

/* Erases program and EEPROM memory */
void state_erase_chip();

/* Enables the programming of the MCU */
void state_enable_programming();


state_func_t current_state = state_init;
unsigned char packet_buffer[PACKET_SIZE];


int main() {
  while (1) {
    current_state();
  }
}

void state_init() {
  /* Initialize usart and SPI and await for packets */
  SPI_master_init();
  USART_init(USART_UBRR_VAL);

 
  current_state = state_recieve_packet;
}


/* Waits for a packet and decodes its instruction*/
void state_recieve_packet() {



  uint8_t lsb = USART_rx();
  uint8_t msb = USART_rx();
  
  uint16_t magic_num = ((uint16_t) msb << 8) | lsb;

  


  /* Notify that the packet has an incorrect start frame and return */
  if (magic_num != FRAME_START_MAGIC) {
    packet_buffer[PACKET_STATUS_POS] = MALFORMED_PACKET;

    current_state = state_send_packet;
    return;
  }


  /* Get packet data */
  for (int i = 0; i < PACKET_SIZE; i++) {
    packet_buffer[i] = USART_rx();
  }

  /* Verify ending packet */
  lsb = USART_rx();
  msb = USART_rx();
  
  magic_num = ((uint16_t) msb << 8) | lsb;


  /* Notify that the packet has an incorrect start frame and return */
  if (magic_num != FRAME_END_MAGIC) {
    packet_buffer[PACKET_STATUS_POS] = MALFORMED_PACKET;

    current_state = state_send_packet;
    return;
  }

  packet_buffer[PACKET_STATUS_POS] = OK;

  /* decode packet instruction */
  switch (packet_buffer[PACKET_INST_POS]) {
  case PROG_ENABLE:
    /* current_state = state_enable_programming; */
    break;
  case CHIP_ERASE:
    /* current_state = state_erase_chip; */
    break;
  case WRITE_FLASH:
    /* current_state = state_write_flash; */
    break;
  case READ_FLASH:
    /* current_state = state_read_flash; */
    break;
  }
  current_state = state_send_packet;
}


/* Sends the contenst of the packet buffer */
void state_send_packet() {
  /* Send frame start magic number */
  USART_tx((unsigned char)FRAME_START_MAGIC);
  USART_tx((unsigned char)(FRAME_START_MAGIC >> 8));

  for (int i = 0; i < PACKET_SIZE; i++) {
    USART_tx(packet_buffer[i]);
  }

  /* Send frame end magic number */

  USART_tx((unsigned char)FRAME_END_MAGIC);
  USART_tx((unsigned char)(FRAME_END_MAGIC >> 8));
  current_state = state_recieve_packet;
}


void state_enable_programming() {

  /* Pull reset pin low */
  set_pin_dir(RESET, OUTPUT);
  dig_write(RESET, LOW);
  /* Wait before sending prog enable command */
  _delay_ms(20);


  int tries = 0;
  /* Send prog enable command and, if failed, send a positive pulse on the reset line*/
  while(!SPI_prog_enable() && tries < MAX_PROG_ENABLE_TRIES) {
    dig_write(RESET, HIGH);
    _delay_us(8);
    dig_write(RESET, LOW);
    _delay_ms(20);
    tries++;
  }


  /* Signal if prog enable was successful */
  if (tries < MAX_PROG_ENABLE_TRIES) {
    packet_buffer[PACKET_STATUS_POS] = OK;
  } else {
    packet_buffer[PACKET_STATUS_POS] = EXEC_ERR;
  }
  
  current_state = state_send_packet;
}

