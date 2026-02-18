#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "message/header.h"

#include "ti_msp_dl_config.h"

/**
 * @brief Parses and Verifies Host Pin
 * 
 * @return true     Host provided a valid pin
 * @return false    Host provided an invalid pin
 */
bool utils_verify_pin(uint8_t* pin, size_t pin_length);


/**
 * @brief Sends a Buffer over UART
 * 
 * @param uart      UART to send Buffer Over
 * @param buffer    Buffer Pointer
 * @param length    Buffer Length
 */
void utils_send_buffer(UART_Regs * uart, const void* buffer, size_t length);

/**
 * @brief Receives a specified number of bytes from the UART
 *
 * Blocks until `num_bytes` have been read from the provided `uart`
 * and stored into `buffer`.
 *
 * @param uart      UART peripheral to read from
 * @param buffer    Destination buffer where received bytes will be written
 * @param num_bytes Number of bytes to receive
 */
void utils_receive_bytes(UART_Regs* uart, void* buffer, size_t num_bytes);

#ifdef DEBUG
#define _WRITE_TO(FUNC, ...) do{                            \
  char* _buffer;                                            \
  if (-1 != asprintf(&_buffer, __VA_ARGS__)){ \
    FUNC(HOST_INST, _buffer, strlen(_buffer)); \
    free(_buffer); \
  } \
} while(0);

#define print_debug(...) _WRITE_TO(message_header_send_debug, __VA_ARGS__)
#define print_error(...) _WRITE_TO(message_header_send_error, __VA_ARGS__)


#else

#define print_debug(...) {}
#define print_error(...)  message_header_send_error(HOST_INST, 0, 0)

#endif
