#include "iohelper.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>


void setPinDirection(uint8_t pinNumber, enum Direction direction) {
  uint8_t value = 1;
  if (pinNumber < 7) {
    value <<= pinNumber;

    if (direction == OUTPUT) {
      DDRD |= value;
    } else {
      DDRD &= ~value;
    }
  } else if (pinNumber < 14) {
    if (direction == OUTPUT) {
      DDRB |= value;
    } else {
      DDRB &= ~value;
    }
  }
  
}

void digWrite(uint8_t pinNumber, enum Level level) {
  uint8_t value = 1;
  if (pinNumber < 7) {
    value <<= pinNumber;

    if (level == HIGH) {
      PORTD |= value;
    } else {
      PORTD &= ~value;
    }
  } else if (pinNumber < 14){
    value <<= pinNumber - 8;
    if (level == HIGH) {
      PORTB |= value;
    } else {
      PORTB &= ~value;
    }
  }
}



