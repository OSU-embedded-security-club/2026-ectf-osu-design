#include "utils.h"
#include "message/header.h"
#include "pin_utils.h"
#include "ti_msp_dl_config.h"

bool utils_verify_pin(const uint8_t *const pin, const size_t pin_length) {
  if (pin_length != 6) {
    return false;
  }
  return validate_pin((const char *)pin);
}

void utils_send_bytes(UART_Regs *const uart, const void *const buffer,
                      const size_t length) {
  const uint8_t *const temp = (const uint8_t *)buffer;

  for (size_t i = 0; i < length; ++i) {
    DL_UART_transmitDataBlocking(uart, temp[i]);
  }
}

void utils_receive_bytes(UART_Regs *const uart, void *const buffer,
                         const size_t length) {
  uint8_t *const temp = (uint8_t *)buffer;

  for (size_t i = 0; i < length; ++i) {
    temp[i] = DL_UART_receiveDataBlocking(uart);
  }
}

void utils_send_ack(UART_Regs *const uart) {
  utils_send_packet(uart, MESSAGE_ACK, "", 0);
}

bool utils_read_ack(UART_Regs *const uart) {
  const message_header_t header = message_header_request(uart);

  return (header.operation == MESSAGE_ACK) && (header.message_length == 0);
}

void utils_receive_bytes_and_ack(UART_Regs *const uart, void *const buffer,
                                 const size_t length) {
  uint8_t *const temp = (uint8_t *)buffer;

  for (size_t i = 0; i < length; ++i) {
    const uint32_t rem = (length - i) > 256 ? 256 : (length - i);

    if (((i % 256) == 0) && (i != 0)) {
      utils_send_ack(uart);
    }

    utils_receive_bytes(uart, &temp[i], rem);
  }
}

void utils_send_bytes_and_ack(UART_Regs *const uart, const void *const buffer,
                              const size_t length) {
  const uint8_t *const temp = (const uint8_t *)buffer;

  for (size_t i = 0; i < length; ++i) {
    const uint32_t rem = (length - i) > 256 ? 256 : (length - i);

    if (((i % 256) == 0) && (i != 0)) {
      utils_read_ack(uart);
    }

    utils_send_bytes(uart, &temp[i], rem);
  }

  fflush(stdout);
}

void utils_receive_packet(UART_Regs *const uart, void *const buffer,
                          size_t *const length) {
  uint8_t *const temp = (uint8_t *)buffer;

  utils_receive_bytes(uart, temp, 1);
  if (temp[0] != '%') {
    *length = 0;
    return;
  }

  const message_header_t header = message_header_request(uart);

  memcpy(temp, &header, sizeof(header));

  *length = header.message_length;

  if ((header.operation != MESSAGE_ACK) &&
      (header.operation != MESSAGE_ERROR) &&
      (header.operation != MESSAGE_DEBUG)) {
    utils_send_ack(uart);

    if (header.message_length > 0) {
      utils_receive_bytes_and_ack(uart, &temp[sizeof(header) + 1],
                                  header.message_length);
      utils_send_ack(uart);
    }
  }
}

void utils_send_packet(UART_Regs *const uart, const uint8_t msg_type,
                       const void *const buffer, const uint16_t length) {
  const char magic = '%';

  utils_send_bytes(uart, &magic, sizeof(magic));

  const message_header_t header = {.operation = msg_type,
                                   .message_length = length};

  utils_send_bytes(uart, &header, sizeof(header));

  if ((msg_type == MESSAGE_ACK) || (msg_type == MESSAGE_ERROR) ||
      (msg_type == MESSAGE_DEBUG)) {
    return;
  }

  utils_read_ack(uart);

  if (length > 0) {
    utils_send_bytes_and_ack(uart, buffer, length);
    utils_send_ack(uart);
  }
}
