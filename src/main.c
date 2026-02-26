// #include "secrets.h"
#include "aes.h"
#include "message/header.h"
#include "message/interrogate.h"
#include "message/list.h"
#include "message/listen.h"
#include "message/read.h"
#include "message/receive.h"
#include "message/write.h"
#include "mpu.h"
#include "pin_utils.h"
#include "rng.h"
#include "utils.h"
#include <limits.h>

/**
 * @brief Buffer to store incoming messages. Size is 1 byte for magic + header +
 * max payload size. Can also be used for outgoing messages.
 *
 */
static uint8_t message_buffer[1 + sizeof(message_header_t) + 2048] = {0};

int main(void) {
  const char err_msg[] = "No.";

  aes_init();
  rng_init();
  mpu_init();

  // TODO: Init Cortex MPU

  // Configure System
  SYSCFG_DL_init();

  // Get the insecure stage1 hash rehashed as quickly as possible
  init_pin();

  // Disable LED
  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
  DL_GPIO_disableOutput(GPIOB, DL_GPIO_PIN_14);

  // Length of packet payload (not including header)
  size_t rx_len = 0;

  while (1) {
    utils_receive_packet(HOST_INST, message_buffer, &rx_len);

    const char magic = (char)message_buffer[0];
    message_header_t header = {0};

    memcpy(&header, &message_buffer[1], sizeof(header));

    // TODO: Set watchdog to ensure reset if header gets hungup

    switch (header.operation) {
    case MESSAGE_LIST: {
      message_list_response(header, rx_len,
                            &message_buffer[sizeof(header) + 1]);
      break;
    }
    case MESSAGE_READ: {
      message_read_response(header, rx_len,
                            &message_buffer[sizeof(header) + 1]);
      break;
    }
    case MESSAGE_WRITE: {
      message_write_response(header, rx_len,
                             &message_buffer[sizeof(header) + 1]);
      break;
    }
    case MESSAGE_RECEIVE: {
      message_receive(header, rx_len, &message_buffer[sizeof(header) + 1]);
      break;
    }
    case MESSAGE_INTERROGATE: {
      message_interrogate_response(header, rx_len,
                                   &message_buffer[sizeof(header) + 1]);
      break;
    }
    case MESSAGE_LISTEN: {
      message_listen_response(header, rx_len,
                              &message_buffer[sizeof(header) + 1]);
      break;
    }
    default: {
      message_header_send_error(HOST_INST, err_msg, sizeof(err_msg));
      break;
    }
    }
  }
}
