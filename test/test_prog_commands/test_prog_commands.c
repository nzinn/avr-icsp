#include "../unity.h"
#include "../fff.h"
#include "prog_commands.h"

DEFINE_FFF_GLOBALS;



/* iohelper.h */
enum Direction{INPUT, OUTPUT};
enum Level{LOW, HIGH};

FAKE_VOID_FUNC(set_pin_dir, uint8_t, enum Direction);
FAKE_VOID_FUNC(dig_write, uint8_t, enum Level);
FAKE_VOID_FUNC(dig_read, int);



/* spi_instructions.h */
FAKE_VALUE_FUNC(uint8_t, SPI_prog_enable);
FAKE_VOID_FUNC(SPI_chip_erase);
FAKE_VOID_FUNC(SPI_write_flash_addr, uint8_t, uint8_t, uint8_t);
FAKE_VALUE_FUNC(uint8_t, SPI_read_flash_addr_high, uint8_t, uint8_t);
FAKE_VALUE_FUNC(uint8_t, SPI_read_flash_addr_low, uint8_t, uint8_t);
FAKE_VOID_FUNC(SPI_write_flash_page, uint8_t, uint8_t);



/* util/delay.h */

FAKE_VOID_FUNC(_delay_us, int);
FAKE_VOID_FUNC(_delay_ms, int);




void setUp(void) {
  RESET_FAKE(set_pin_dir);
  RESET_FAKE(dig_write);
  RESET_FAKE(dig_read);

  RESET_FAKE(_delay_us);
  RESET_FAKE(_delay_ms);

  RESET_FAKE(SPI_prog_enable);
  RESET_FAKE(SPI_chip_erase);
  RESET_FAKE(SPI_write_flash_addr);
  RESET_FAKE(SPI_read_flash_addr_high);
  RESET_FAKE(SPI_read_flash_addr_low);

  RESET_FAKE(SPI_write_flash_page)
}

void tearDown(void) {
  
}


void test_write_flash(void) { TEST_ASSERT(1); }


void test_enable_prog_success(void) {
 
  SPI_prog_enable_fake.return_val = 1;

  TEST_ASSERT(PROG_enable_programming(2, 3));
  TEST_ASSERT_EQUAL_INT(SPI_prog_enable_fake.call_count, 1);
}


void test_enable_prog_fail(void) {
 
  SPI_prog_enable_fake.return_val = 0;

  TEST_ASSERT_FALSE(PROG_enable_programming(2, 3));
  TEST_ASSERT_EQUAL_INT(SPI_prog_enable_fake.call_count, 3);
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_enable_prog_success);
  RUN_TEST(test_enable_prog_fail);
  return UNITY_END();
}


