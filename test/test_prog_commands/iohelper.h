#ifndef IOHELPER_H
#define IOHELPER_H
#include <stdint.h>  
/* iohelper.h */
enum Direction{INPUT, OUTPUT};
enum Level{LOW, HIGH};

void set_pin_dir(uint8_t, enum Direction);
void dig_write(uint8_t, enum Level);
void dig_read(int);

#endif 
