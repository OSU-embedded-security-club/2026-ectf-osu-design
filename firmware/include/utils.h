#pragma once
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
#include "message/header.h"
#include "group.h"

#include "ti_msp_dl_config.h"

/*!
 * @brief Delay between 0 - 4096 cycles to mitigate timing attacks
 */
void utils_random_delay();

/*!
 * @brief Verifies host PIN
 * 
 * @param[in] pin PIN buffer
 * @param[in] pin_length Length of PIN
 * 
 * @returns true if PIN is valid, false otherwise
 */
bool utils_verify_pin(uint8_t* pin, size_t pin_length);

/*!
 * @brief Sends buffer over UART with ACKing
 * 
 * @param[in] uart UART peripheral
 * @param[in] buffer Buffer to send
 * @param[in] length Buffer length
 * @param[in] bytes_sent Number of bytes already sent (for resumption)
 */
void utils_send_buffer(UART_Regs * uart, const void* buffer, size_t length, size_t bytes_sent);

/*!
 * @brief Sends buffer over UART without ACKing
 * 
 * @param[in] uart UART peripheral
 * @param[in] buffer Buffer to send
 * @param[in] length Buffer length
 */
void utils_send_buffer_no_ack(UART_Regs* uart, const void* buffer, size_t length);

/*!
 * @brief Receives specified number of bytes from UART
 * 
 * Blocks until num_bytes have been read from the provided uart
 * and stored into buffer.
 * 
 * @param[in] uart UART peripheral
 * @param[out] buffer Destination buffer
 * @param[in] num_bytes Number of bytes to receive
 */
void utils_receive_bytes(UART_Regs* uart, void* buffer, size_t num_bytes);

/*!
 * @brief Finds group by ID
 * 
 * @param[in] group_id Group identifier to search for
 * 
 * @returns Pointer to group if found, NULL otherwise
 */
const group_t* utils_find_group(uint16_t group_id);


// #ifdef DEBUG
// #define _WRITE_TO(FUNC, ...) do{                            \
//   char* _buffer;                                            \
//   if (-1 != asprintf(&_buffer, __VA_ARGS__)){ \
//     FUNC(HOST_INST, _buffer, strlen(_buffer)); \
//     free(_buffer); \
//   } \
// } while(0);

// #define print_debug(...) _WRITE_TO(message_header_send_debug, __VA_ARGS__)
// #define print_error(...) _WRITE_TO(message_header_send_error, __VA_ARGS__)


// #else

// #define print_debug(...) {}
// #define print_error(...)  message_header_send_error(HOST_INST, 0, 0)

// #endif
