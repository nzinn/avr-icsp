#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>



#include "instructions.h"
#include "iohelper.h"
#include "spi.h"
#include "usart.h"


/* For delay  */
#define F_CPU 16000000L

#include <util/delay.h>

#define RESET 2

/* Value for 9600 baud rate */
#define USART_UBRR_VAL 103

/* Number of attempts to sync with MCU */
#define MAX_PROG_ENABLE_TRIES 5

#define PAGE_NUM_BYTES 128

#define PACKET_SIZE PAGE_NUM_BYTES + 4
#define PACKET_STATUS_POS 0

#define PACKET_INST_POS 0
#define PACKET_ADDR_LSB 1
#define PACKET_ADDR_MSB 2
#define PACKET_DATA_POS 4

/* Values used to delimit end and start of frames */
#define FRAME_START_MAGIC 0xCEC4
#define FRAME_END_MAGIC 0x0F14


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

enum PACKET_ERROR_STATUS { OK, MALFORMED_PACKET, EXEC_ERR };

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

  uint16_t magic_num = ((uint16_t)msb << 8) | lsb;

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

  magic_num = ((uint16_t)msb << 8) | lsb;

  /* Notify that the packet has an incorrect start frame and return */
  if (magic_num != FRAME_END_MAGIC) {
    packet_buffer[PACKET_STATUS_POS] = MALFORMED_PACKET;

    current_state = state_send_packet;
    return;
  }

  /* decode packet instruction */
  switch (packet_buffer[PACKET_INST_POS]) {
  case PROG_ENABLE:
    current_state = state_enable_programming;
    break;
  case CHIP_ERASE:
    current_state = state_erase_chip;
    break;
  case WRITE_FLASH:
    current_state = state_write_flash;
    break;
  case READ_FLASH:
    current_state = state_read_flash;
    break;
  }
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

  set_pin_dir(RESET, OUTPUT);
  dig_write(RESET, LOW);
  _delay_ms(20);

  int tries = 0;
  while (!SPI_prog_enable() && tries < MAX_PROG_ENABLE_TRIES) {
    dig_write(RESET, HIGH);
    _delay_us(8);
    dig_write(RESET, LOW);
    _delay_ms(20);
    tries++;
  }

  if (tries < MAX_PROG_ENABLE_TRIES) {
    packet_buffer[PACKET_STATUS_POS] = OK;
  } else {
    packet_buffer[PACKET_STATUS_POS] = EXEC_ERR;
  }

  current_state = state_send_packet;
}

void state_erase_chip() {
  SPI_chip_erase();

  packet_buffer[PACKET_STATUS_POS] = OK;
  current_state = state_send_packet;
}

void state_write_flash() {

  uint8_t addr_msb = packet_buffer[PACKET_ADDR_MSB];
  uint8_t addr_lsb = packet_buffer[PACKET_ADDR_LSB];

  for (int i = 0; i < (PAGE_NUM_BYTES / 2); i++) {
    SPI_write_flash_addr(i, packet_buffer[PACKET_DATA_POS + (i * 2)],
                         packet_buffer[PACKET_DATA_POS + (i * 2) + 1]);
  }

  SPI_write_flash_page(addr_lsb, addr_msb);

  current_state = state_send_packet;
}

void state_read_flash() {

  uint8_t addr_lsb = packet_buffer[PACKET_ADDR_LSB];
  uint8_t addr_msb = packet_buffer[PACKET_ADDR_MSB];


  uint16_t addr = ((uint16_t) addr_msb) << 8 | addr_lsb;
  
  for (int i = 0; i < (PAGE_NUM_BYTES / 2); i++) {


    addr_lsb = (uint8_t) addr;
    addr_msb = (uint8_t) (addr >> 8);
    
    /* Divide i by two as there are two bytes in a word */
    packet_buffer[PACKET_DATA_POS + i * 2] = SPI_read_flash_addr_low(addr_lsb, addr_msb);
    packet_buffer[PACKET_DATA_POS + (i * 2) + 1] = SPI_read_flash_addr_high(addr_lsb, addr_msb);

    addr++;

  }

  packet_buffer[PACKET_STATUS_POS] = OK;
  current_state = state_send_packet;
  
}
