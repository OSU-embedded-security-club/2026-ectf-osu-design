#pragma once

#include "message/header.h"


typedef struct {
    uint8_t pin[6];
    uint8_t slot;
    uint16_t group_id;
    uint8_t name[32];
    uint8_t file_uuid[16];
    uint16_t file_length;
} message_write_header_t;

/**
 * @brief Parses the header of the Write File command
 * 
 * @param uart[in] UART Registers for Communication
 * @return message_write_header_t containing header information
 */
message_write_header_t message_write_parse_header(UART_Regs* uart);

/**
 * @brief Responds to Write Request
 *
 * @param[in] header    Message Header
 */
void message_write_response(message_header_t header);