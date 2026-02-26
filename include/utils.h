#pragma once

#include "message/header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ti_msp_dl_config.h"

/**
 * @brief Macro to indicate that a function parameter must not be NULL. If a
 * NULL pointer is passed to a parameter marked with this attribute, gcc will
 * emit a warning at compile time.
 *
 */
#define NOTNULL() __attribute__((nonnull))

/**
 * @brief Parses and Verifies Host Pin
 *
 * @return true     Host provided a valid pin
 * @return false    Host provided an invalid pin
 */
NOTNULL()
bool utils_verify_pin(const uint8_t *const pin, const size_t pin_length);

/**
 * @brief Sends a Buffer over UART
 *
 * @param[in] uart      UART to send Buffer Over
 * @param[in] buffer    Buffer Pointer
 * @param[in] length    Buffer Length
 */
NOTNULL()
void utils_send_bytes(UART_Regs *const uart, const void *const buffer,
                      const size_t length);

/**
 * @brief Receives a specified number of bytes from the UART
 *
 * Blocks until `length` have been read from the provided `uart`
 * and stored into `buffer`.
 *
 * @param[in] uart      UART peripheral to read from
 * @param[out] buffer    Destination buffer where received bytes will be written
 * @param[in] length Number of bytes to receive
 */
NOTNULL()
void utils_receive_bytes(UART_Regs *const uart, void *const buffer,
                         const size_t length);

/**
 * @brief Sends a packet over UART, including the message header and payload.
 *
 * @param[in] uart UART peripheral to send buffer through
 * @param[in] msg_type Message type (operation) to include in the header
 * @param[in] buffer Pointer to the buffer payload to send
 * @param[in] length Length of the buffer payload
 */
NOTNULL()
void utils_send_packet(UART_Regs *const uart, const uint8_t msg_type,
                       const void *const buffer, const uint16_t length);

/**
 * @brief Receives a packet from the UART, including the message header and
 * payload.
 *
 * @param[in] uart UART registers to receive data from
 * @param[out] buffer Buffer to store the received packet (must be large enough
 * to hold header + payload)
 * @param[out] length Pointer to a size_t variable where the total size of
 * the received packet will be stored
 */
NOTNULL()
void utils_receive_packet(UART_Regs *const uart, void *const buffer,
                          size_t *const length);

/**
 * @brief Sends an ACK packet over UART
 *
 * @param[in] uart UART peripheral to send ACK through
 */
NOTNULL() void utils_send_ack(UART_Regs *const uart);
/**
 * @brief Reads an ACK packet from UART and verifies it is a valid ACK response
 * @param[in] uart UART peripheral to read ACK from
 * @return true if a valid ACK packet is received, false otherwise
 *
 */
NOTNULL() bool utils_read_ack(UART_Regs *const uart);

/**
 * @brief Receives a specified number of bytes from the UART, sending an ACK
 * after every 256 bytes to ensure flow control.
 *
 * @param[in] uart UART peripheral to receive data from
 * @param[out] buffer Buffer to store received data (must be large enough to
 * hold length)
 * @param[in] length Number of bytes to receive
 */
NOTNULL()
void utils_receive_bytes_and_ack(UART_Regs *const uart, void *const buffer,
                                 const size_t length);

#ifdef DEBUG
#define _WRITE_TO(FUNC, ...)                                                   \
  do {                                                                         \
    char *_buffer;                                                             \
    if (-1 != asprintf(&_buffer, __VA_ARGS__)) {                               \
      FUNC(HOST_INST, _buffer, strlen(_buffer));                               \
      free(_buffer);                                                           \
    }                                                                          \
  } while (0);

#define print_debug(...) _WRITE_TO(message_header_send_debug, __VA_ARGS__)
#define print_error(...) _WRITE_TO(message_header_send_error, __VA_ARGS__)

#else

#define print_debug(...)                                                       \
  {                                                                            \
  }
#define print_error(...) message_header_send_error(HOST_INST, 0, 0)

#endif
