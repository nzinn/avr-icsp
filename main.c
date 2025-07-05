#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#include "iohelper.h"
#include "spi.h"

#define RESET 2

/* enables programming mode, returns 1 on success and zero on failure */
uint8_t SPI_ProgEnable();

void SPI_ChipErase();

void SPI_LoadProg(uint16_t addr, uint16_t data);

void SPI_WriteProg(uint16_t addr);

char SPI_ReadLow(uint16_t addr);



#define BAUD_RATE 103

int main() {

  SPI_MasterInit();

  /* Pull reset pin low */
  setPinDirection(RESET, OUTPUT);
  digWrite(RESET, LOW);
  /* Wait before prog enable */
  _delay_ms(20);


  while(!SPI_ProgEnable()) {
    digWrite(RESET, HIGH);
    _delay_us(8);
    digWrite(RESET, LOW);
    _delay_ms(20);
  }

  
}

/* Enables programming mode on chip, returns true on success, false on error */
uint8_t SPI_ProgEnable() {
  SPI_MasterTX(0xAC);
  SPI_MasterTX(0x53);
  SPI_MasterTX(0x00);

  uint8_t echo = SPI_MasterRX();

  SPI_MasterTX(0x00);

  /* If slave echoes back signal all good */
  if (echo == 0x53)  {
    return 1;
  }

  return 0;
}


/* Writes 0xFF to program memory and EEPROM  */
void SPI_ChipErase() {

  SPI_MasterTX(0xAC);
  SPI_MasterTX(0x80);
  SPI_MasterTX(0x00);
  SPI_MasterTX(0x00);

  
}

/* Check if the chip is ready to be programmed */
uint8_t SPI_CheckReady() {
  SPI_MasterTX(0xF0);
  SPI_MasterTX(0x00);
  SPI_MasterTX(0x00);
  SPI_MasterTX(0x00);

  return SPI_MasterRX();
}

/* Loads data into address[5:0] of page buffer*/
void SPI_LoadProg(uint16_t address, uint16_t data)  {

  /* Load low data byte */
  SPI_MasterTX(0x40);
  SPI_MasterTX(0x00);
  SPI_MasterTX((uint8_t) address);
  SPI_MasterTX((uint8_t) data);

  /* Load high data byte */
  SPI_MasterTX(0x48);
  SPI_MasterTX(0x00);
  SPI_MasterTX((uint8_t) address);
  SPI_MasterTX((uint8_t) (data>>8));
}

/* write the page of address[13:6] */
void SPI_WritePage(uint16_t address) {
  SPI_MasterTX(0x4c);
  SPI_MasterTX((uint8_t) (address>>8));
  SPI_MasterTX((uint8_t) address);
  SPI_MasterTX(0x00);

  /* Wait for page write */
  _delay_ms(1);
  while(!SPI_CheckReady()) {
    _delay_ms(1);
  }
  
}

char SPI_ReadLow(uint16_t address) {
  SPI_MasterTX(0x20);
  SPI_MasterTX(address>>8);
  SPI_MasterTX((uint8_t) address);
  SPI_MasterTX(0x0);

  return SPI_MasterRX();
}
