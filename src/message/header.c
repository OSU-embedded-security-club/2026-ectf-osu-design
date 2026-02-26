#include "message/header.h"
#include "utils.h"

message_header_t message_header_request(UART_Regs *const uart) {

  const uint8_t opcode = DL_UART_receiveDataBlocking(uart);

  const uint8_t length[2] = {
      DL_UART_receiveDataBlocking(uart),
      DL_UART_receiveDataBlocking(uart),
  };

  const message_header_t header = {
      .operation = (char)opcode,
      .message_length = *((const uint16_t *)length),
  };

  return header;
}

void message_header_response(UART_Regs *const uart,
                             const message_header_t header) {
  // Message MAGIC Byte
  DL_UART_transmitDataBlocking(uart, '%');

  DL_UART_transmitDataBlocking(uart, (uint8_t)header.operation);

  const uint8_t *const length = (const uint8_t *)(&header.message_length);

  DL_UART_transmitDataBlocking(uart, length[0]);
  DL_UART_transmitDataBlocking(uart, length[1]);
}

void message_header_send_error(UART_Regs *const uart, const char *const msg,
                               const uint16_t msg_len) {
  const message_header_t header = {
      .operation = MESSAGE_ERROR,
      .message_length = msg_len,
  };
  message_header_response(uart, header);

  utils_send_bytes(uart, msg, msg_len);
}

void message_header_send_debug(UART_Regs *const uart, const char *const msg,
                               const uint16_t msg_len) {
  const message_header_t header = {
      .operation = MESSAGE_DEBUG,
      .message_length = msg_len,
  };
  message_header_response(uart, header);

  utils_send_bytes(uart, msg, msg_len);
}
