#pragma once

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
void utils_send_buffer(UART_Regs * uart, void* buffer, size_t length);

<<<<<<< HEAD
void utils_receive_bytes(UART_Regs* uart, uint8_t* buffer, size_t num_bytes);
=======
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
>>>>>>> origin
