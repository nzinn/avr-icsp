#include "iohelper.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

/* Sets an input to be low or high, using arduino pin number conventions */
void set_pin_dir(uint8_t pin_num, enum Direction direction) {
  uint8_t value = 1;
  if (pin_num < 7) {
    value <<= pin_num;

    if (direction == OUTPUT) {
      DDRD |= value;
    } else {
      DDRD &= ~value;
    }
  } else if (pin_num < 14) {
    if (direction == OUTPUT) {
      DDRB |= value;
    } else {
      DDRB &= ~value;
    }
  }
  
}


/* Writes level to the pin number */
void dig_write(uint8_t pin_num, enum Level level) {
  uint8_t value = 1;
  if (pin_num < 7) {
    value <<= pin_num;

    if (level == HIGH) {
      PORTD |= value;
    } else {
      PORTD &= ~value;
    }
  } else if (pin_num < 14){
    value <<= pin_num - 8;
    if (level == HIGH) {
      PORTB |= value;
    } else {
      PORTB &= ~value;
    }
  }
}



