#include "iohelper.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>

void setPinMode(int pinNumber, enum pinMode level) {
  switch (pinNumber) {
  case 0:
    DDRD |= level<<DDD0;
    break;
  case 1:
    DDRD |= level<<DDD1;
    break;
  case 2:
    DDRD |= level<<DDD2;
    break;
  case 3:
    DDRD |= level<<DDD3;
    break;
  case 4:
    DDRD |= level<<DDD4;
    break;
  case 5:
    DDRD |= level<<DDD5;
    break;
  case 6:
    DDRD |= level<<DDD6;
    break;
  case 7:
    DDRD |= level<<DDD7;
    break;
  }
}


