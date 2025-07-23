#ifndef IOHELPER_H
#define IOHELPER_H
#include "../fff.h"
#include <stdint.h>
enum Direction{INPUT, OUTPUT};
enum Level{LOW, HIGH};
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(set_pin_dir, uint8_t, enum Direction);

FAKE_VOID_FUNC(dig_write, uint8_t, enum Level);

FAKE_VOID_FUNC(dig_read, int);


#endif
