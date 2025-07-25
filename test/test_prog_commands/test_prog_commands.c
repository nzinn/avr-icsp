#include "../unity.h"

#define BUFFER_SIZE 128

#define FFF_ARG_HISTORY_LEN BUFFER_SIZE

#include "../fff.h"
#include "../../src/prog_commands.h"
#include <stdlib.h>
#include <math.h>

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
FAKE_VALUE_FUNC(uint8_t, SPI_read_fuse_low);
FAKE_VALUE_FUNC(uint8_t, SPI_read_fuse_high);

/* util/delay.h */

FAKE_VOID_FUNC(_delay_us, int);
FAKE_VOID_FUNC(_delay_ms, int);




uint8_t test_buffer[BUFFER_SIZE];

uint8_t test_buffer_low[BUFFER_SIZE / 2];
uint8_t test_buffer_high[BUFFER_SIZE / 2];

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

  RESET_FAKE(SPI_write_flash_page);


  int low_index = 0;
  int high_index = 0;
  
  for (int i = 0; i < BUFFER_SIZE; i++) {
    uint8_t rand_num = (uint8_t) rand();
    test_buffer[i] = rand_num;

    if (i % 2 == 0) {
      test_buffer_low[low_index++] = rand_num;
    } else {
      test_buffer_high[high_index++] = rand_num;
    }
  }
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




void print_arrays(uint8_t *a1, uint8_t *a2, int size) {
  for (int i = 0; i < size; i++) {
    printf("a1[%d] = %d, a2[%d] = %d ", i, a1[i], i, a2[i]);


    if (a1[i] != a2[i]) {
      printf("Not equal!");
    }

    printf("\n");
  }
}

int verify_read_addresses(uint8_t start_lsb, uint8_t start_msb) {


  uint16_t start_addr = (((uint16_t)start_msb) << 8) | (uint16_t) start_lsb;
  
  for (int i = 0; i < BUFFER_SIZE / 2; i++) {
    uint8_t low_lsb = SPI_read_flash_addr_low_fake.arg0_history[i];
    uint8_t low_msb = SPI_read_flash_addr_low_fake.arg1_history[i];
    
    uint8_t high_lsb = SPI_read_flash_addr_high_fake.arg0_history[i];
    uint8_t high_msb = SPI_read_flash_addr_high_fake.arg1_history[i];

    if (low_lsb != high_lsb || low_msb != high_msb) {
      return 0;
    }


    uint16_t addr_to_check = (((uint16_t) low_msb) << 8) | (uint16_t) low_lsb;

    if (addr_to_check != start_addr + i) {
      return 0;
    }
    
  }

  return 1;
}




void test_read_flash_base() {

  SET_RETURN_SEQ(SPI_read_flash_addr_low, test_buffer_low, BUFFER_SIZE / 2);
  SET_RETURN_SEQ(SPI_read_flash_addr_high, test_buffer_high, BUFFER_SIZE / 2);


  uint8_t check_buffer[BUFFER_SIZE];
  
  PROG_read_flash(check_buffer, BUFFER_SIZE, 0, 0);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(check_buffer, test_buffer, BUFFER_SIZE);
  TEST_ASSERT(verify_read_addresses(0, 0));

  
}

void test_read_flash_overflow() {

  SET_RETURN_SEQ(SPI_read_flash_addr_low, test_buffer_low, BUFFER_SIZE / 2);
  SET_RETURN_SEQ(SPI_read_flash_addr_high, test_buffer_high, BUFFER_SIZE / 2);


  uint8_t check_buffer[BUFFER_SIZE];


  uint8_t start_addr = 255;
  PROG_read_flash(check_buffer, BUFFER_SIZE, start_addr, 0);

  
  TEST_ASSERT_EQUAL_UINT8_ARRAY(check_buffer, test_buffer, BUFFER_SIZE);
  TEST_ASSERT(verify_read_addresses(start_addr, 0));
}


void test_write_flash_base() {


  uint8_t page_size = 64;
  
  PROG_write_flash(test_buffer, BUFFER_SIZE, page_size, 0, 0);

  uint8_t addr_msb_buf[BUFFER_SIZE / 2];

  uint8_t addr_msb = 0;
  for (int i = 0; i < BUFFER_SIZE / 2; i++) {
    addr_msb_buf[i] = addr_msb++;
  }

  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_buffer_low, SPI_write_flash_addr_fake.arg1_history, BUFFER_SIZE / 2);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_buffer_high,
                                SPI_write_flash_addr_fake.arg2_history,
                                BUFFER_SIZE / 2);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
      addr_msb_buf, SPI_write_flash_addr_fake.arg0_history, BUFFER_SIZE / 2);

  TEST_ASSERT_EQUAL(ceil((float) BUFFER_SIZE / 2 / page_size), SPI_write_flash_page_fake.call_count);
}


void test_write_flash_overflow() {


  uint8_t page_size = 64;
  uint8_t start_addr = 255;
  PROG_write_flash(test_buffer, BUFFER_SIZE, page_size, start_addr, 0);

  uint8_t addr_msb_buf[BUFFER_SIZE / 2];

  uint8_t addr_msb = start_addr;
  
  for (int i = 0; i < BUFFER_SIZE / 2; i++) {
    addr_msb_buf[i] = addr_msb++;
  }

  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_buffer_low, SPI_write_flash_addr_fake.arg1_history, BUFFER_SIZE / 2);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_buffer_high,
                                SPI_write_flash_addr_fake.arg2_history,
                                BUFFER_SIZE / 2);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
      addr_msb_buf, SPI_write_flash_addr_fake.arg0_history, BUFFER_SIZE / 2);
}


void test_read_fuse_low(void) {



  uint8_t buf[BUFFER_SIZE];


  uint8_t fuse_low = 0xAC;
  uint8_t fuse_high = 0x50;
  
  SPI_read_fuse_low_fake.return_val = fuse_low;
  SPI_read_fuse_high_fake.return_val = fuse_high;
  PROG_read_fuse_bits(buf);

  
  TEST_ASSERT_EQUAL_UINT8(buf[0], fuse_low);
  TEST_ASSERT_EQUAL_UINT8(buf[1], fuse_high);
}



int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_enable_prog_success);
  RUN_TEST(test_enable_prog_fail);
  RUN_TEST(test_read_flash_base);
  RUN_TEST(test_read_flash_overflow);
  RUN_TEST(test_write_flash_base);
  RUN_TEST(test_write_flash_overflow);
  RUN_TEST(test_read_fuse_low);
  return UNITY_END();
}


