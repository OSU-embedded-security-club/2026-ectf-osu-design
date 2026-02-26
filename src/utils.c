#include "utils.h"
#include "message/header.h"
#include "pin_utils.h"
#include "rng.h"
#include <monocypher.h>

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
      (header.operation != MESSAGE_DEBUG) && (*length <= 2048)) {
    utils_send_ack(uart);

    if ((header.message_length > 0) && (header.message_length <= 2048)) {
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

  const message_header_t header = {
      .operation = (char)msg_type,
      .message_length = length,
  };

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

/**
 * @brief Shuffles the elements of the provided array in place using the
 * Fisher-Yates algorithm, with randomness sourced from the RNG.
 *
 * @param[in] array A pointer to the array to be shuffled. The array is modified
 * in place.
 */
static void shuffle_array(uint8_t *const array) {
  uint8_t offset[4] = {0};

  for (size_t i = 3; i > 0; --i) {
    rng_get_bytes(offset, sizeof(offset));

    const uint32_t rand_index = *((uint32_t *)offset) % (i + 1);

    const uint8_t temp = array[i];
    array[i] = array[rand_index];
    array[rand_index] = temp;
  }

  crypto_wipe(offset, sizeof(offset));
}

void random_memcpy(void *dest, const void *src, const size_t n) {
  const uint8_t *const temp_src = (const uint8_t *)src;
  uint8_t *const temp_dest = (uint8_t *)dest;

  uint8_t unpacked[4] = {0};

  uint8_t map[4] = {0, 1, 2, 3};

  /* Copies src to dest in 4-byte chunks, randomizing the order of bytes within
   each chunk based on random values from the RNG. If n is not a multiple of
   4, the remaining bytes are copied without randomization at the end. */
  for (size_t i = 0; i < (n / sizeof(uint32_t)); ++i) {
    shuffle_array(map);

#pragma unroll
    for (size_t j = 0; j < 4; ++j) {
      unpacked[j] = temp_src[(i * sizeof(uint32_t)) + map[j]];
    }

#pragma unroll
    for (size_t j = 4; j > 0; --j) {
      temp_dest[(i * sizeof(uint32_t)) + map[j - 1]] = unpacked[j - 1];
    }
  }

  const uint32_t rem = n % sizeof(uint32_t);
  memcpy(&temp_dest[n - rem], &temp_src[n - rem], rem);

  crypto_wipe(unpacked, sizeof(unpacked));
  crypto_wipe(map, sizeof(map));
}
