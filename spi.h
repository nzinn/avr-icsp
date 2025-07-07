#ifndef SPI_H
#define SPI_H
#include <stdint.h>
void SPI_master_init();

/* Transmits data over the MOSI line*/
void SPI_master_tx(char data);
void SPI_master_tx_string(char* string);

/* Returns value recieved from MISO */
uint8_t SPI_master_rx();
#endif

