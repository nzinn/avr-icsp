#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <avr/io.h>
#include <stdint.h>

enum Direction{INPUT, OUTPUT};
enum Level{LOW, HIGH};

void setPinDirection(uint8_t pinNumber, enum Direction direction);
void digWrite(uint8_t pinNumber, enum Level level);

void digRead(int pinNumber);
#endif
