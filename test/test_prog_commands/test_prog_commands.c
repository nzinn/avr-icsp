#include "unity.h"

void setUp(void) {
  
}

void tearDown(void) {
  
}


void test_write_flash(void) {
  TEST_ASSERT(1);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_write_flash);
  return UNITY_END();
}


