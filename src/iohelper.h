#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <avr/io.h>
#include <stdint.h>

enum Direction{INPUT, OUTPUT};
enum Level{LOW, HIGH};

void set_pin_dir(uint8_t pin_num, enum Direction direction);
void dig_write(uint8_t pin_num, enum Level level);

void dig_read(int pin_num);
#endif
