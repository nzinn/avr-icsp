#ifndef DELAY_H
#define DELAY_H
#include "../fff.h"
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(_delay_us, int);
FAKE_VOID_FUNC(_delay_ms, int);

#endif
