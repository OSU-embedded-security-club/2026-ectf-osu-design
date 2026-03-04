#pragma once

#include "message/header.h"


/*!
 * @brief Write file command header structure
 */
typedef struct {
    //! Host PIN for authentication (6 bytes)
    uint8_t pin[6];

    //! Target file slot number
    uint8_t slot;

    //! Group ID for file access permissions
    uint16_t group_id;

    //! File name (32 bytes)
    uint8_t name[32];

    //! File UUID for identification (16 bytes)
    uint8_t file_uuid[16];

    //! File length in bytes
    uint16_t file_length;
} message_write_header_t;

/*!
 * @brief Parses the header of the Write File command
 * 
 * @param[in] uart UART registers for communication
 * 
 * @returns Write header structure containing parsed information
 */
message_write_header_t message_write_parse_header(UART_Regs* uart);

/*!
 * @brief Responds to Write Request
 * 
 * @param[in] header Message header containing request context
 */
void message_write_response(message_header_t header);