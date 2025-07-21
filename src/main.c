#include "spi_base.h"
#include "spi_instructions.h"
#include "prog_commands.h"
#include "usart.h"

#include "packet.h"



#define PAGE_NUM_WORDS 64

#define HEADER_SIZE 4

#define PACKET_SIZE (PAGE_NUM_WORDS * 2 + HEADER_SIZE)

#define PACKET_STATUS_POS 0

#define PACKET_INST_POS 0
#define PACKET_ADDR_LSB 1
#define PACKET_ADDR_MSB 2
#define PACKET_DATA_POS 4


/* For a baudrate of 9600 */
#define USART_UBRR_VAL 103


/* Which pin to use for reset */
#define RESET 2


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

enum PACKET_STATUS { OK, MALFORMED_PACKET, EXEC_ERR };

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


/* Initialize usart and SPI and await for packets */
void state_init() {
  SPI_master_init();
  USART_init(USART_UBRR_VAL);

  current_state = state_recieve_packet;
}

/* Waits for a packet and decodes its instruction*/
void state_recieve_packet() {

  if (PACK_rec_packet(packet_buffer, PACKET_SIZE)) {

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
}

/* Sends the contenst of the packet buffer */
void state_send_packet() {

  PACK_send_packet(packet_buffer, PACKET_SIZE);
  current_state = state_recieve_packet;
}

void state_enable_programming() {


  if (PROG_enable_programming(RESET, 3)) {
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

  PROG_write_flash(packet_buffer, PACKET_SIZE, PAGE_NUM_WORDS, packet_buffer[PACKET_ADDR_LSB], packet_buffer[PACKET_ADDR_MSB]);

  packet_buffer[PACKET_STATUS_POS] = OK;
  current_state = state_send_packet;
}

void state_read_flash() {

  PROG_read_flash(packet_buffer, PACKET_SIZE, packet_buffer[PACKET_ADDR_LSB], packet_buffer[PACKET_ADDR_MSB]);

  packet_buffer[PACKET_STATUS_POS] = OK;
  current_state = state_send_packet;
  
}
