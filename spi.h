#ifndef SPI_H
#define SPI_H
void SPI_MasterInit();

/* Transmits data over the MOSI line*/
void SPI_MasterTX(char data);
void SPI_MasterTXString(char* string);

/* Returns value recieved from MISO */
uint8_t SPI_MasterRX();
#endif

