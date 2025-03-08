#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <avr/io.h>

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#define pinMode(pinNumber, direction)                                          \
  do {                                                                         \
    if (pinNumber < 8) {                                                       \
      DDRD |= direction << pinNumber;                                          \
    } else if (pinNumber < 13) {                                               \
      DDRB |= direction << (pinNumber - 8);                                    \
    }                                                                          \
  } while (0)								\

#define digWrite(pinNumber, level)                                             \
  do {                                                                         \
    if (pinNumber < 8) {                                                       \
      if (DDRD >> pinNumber & 1) {                                             \
        PORTD |= level << pinNumber;                                           \
      }                                                                        \
    } else if (pinNumber < 13) {                                               \
      if (DDRB >> (pinNumber - 8) & 1) {					\
        PORTB |= level << (pinNumber - 8);                                     \
      }                                                                        \
    }                                                                          \
  } while (0)								\

#endif
