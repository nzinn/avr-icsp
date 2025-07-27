#include "../../src/packet.h"


#include "../unity.h"
#include <stdlib.h>

#define DATA_SIZE 128
#define HEADER_SIZE 4
#define PACKET_SIZE HEADER_SIZE + DATA_SIZE
#define MAGIC_SIZE 4

#define FFF_ARG_HISTORY_LEN PACKET_SIZE + MAGIC_SIZE
#include "../fff.h"


DEFINE_FFF_GLOBALS;


FAKE_VALUE_FUNC(uint8_t, USART_rx);
FAKE_VOID_FUNC(USART_tx, char);

uint8_t test_usart[PACKET_SIZE + MAGIC_SIZE];

void setUp(void) {
  for (int i = 0; i < PACKET_SIZE; i++) {
    test_usart[i + MAGIC_SIZE / 2] = rand();
  }

  RESET_FAKE(USART_rx);
}

void tearDown(void) {

}

void test_recieve_ok(void) {
  test_usart[0] = 0xC4;
  test_usart[1] = 0xCE;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2] = 0x14;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2 + 1] = 0x0F;


  SET_RETURN_SEQ(USART_rx, test_usart, PACKET_SIZE + MAGIC_SIZE);
  
  uint8_t test_recieve_buffer[PACKET_SIZE];

  PK_STATUS stat = PACK_rec_packet(test_recieve_buffer, PACKET_SIZE);

  TEST_ASSERT_EQUAL(PK_OK, stat);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_usart + 2, test_recieve_buffer, PACKET_SIZE);
}

void test_recieve_error_start(void) {
  test_usart[0] = 0;
  test_usart[1] = 0;

  SET_RETURN_SEQ(USART_rx, test_usart, PACKET_SIZE + MAGIC_SIZE);
  
  uint8_t test_recieve_buffer[PACKET_SIZE];

  PK_STATUS stat = PACK_rec_packet(test_recieve_buffer, PACKET_SIZE);

  TEST_ASSERT_EQUAL(PK_ERR, stat);
  
}

void test_recieve_error_end(void) {
  test_usart[0] = 0xC4;
  test_usart[1] = 0xCE;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2] = 0;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2 + 1] = 0;


  SET_RETURN_SEQ(USART_rx, test_usart, PACKET_SIZE + MAGIC_SIZE);
  
  uint8_t test_recieve_buffer[PACKET_SIZE];

  PK_STATUS stat = PACK_rec_packet(test_recieve_buffer, PACKET_SIZE);

  TEST_ASSERT_EQUAL(PK_ERR, stat);
}

void test_send_packet(void) {

  test_usart[0] = 0xC4;
  test_usart[1] = 0xCE;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2] = 0x14;
  test_usart[PACKET_SIZE + MAGIC_SIZE / 2 + 1] = 0x0F;

  PACK_send_packet(test_usart + 2, PACKET_SIZE);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_usart, USART_tx_fake.arg0_history, PACKET_SIZE + MAGIC_SIZE);
  
}



int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_recieve_ok);
  RUN_TEST(test_recieve_error_start);
  RUN_TEST(test_recieve_error_end);
  RUN_TEST(test_send_packet);
  return UNITY_END();
}


