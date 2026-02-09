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
void utils_send_buffer(UART_Regs * uart, const void* buffer, size_t length);