#pragma once

#include "message/header.h"
#include "utils.h"

/**
 * @brief Responds to Recieve Request
 *
 * @param[in] header    Message Header
 * @param[in] rx_len    Length of the received payload
 * @param[in] payload   Pointer to the received payload
 */
NOTNULL()
void message_receive(const message_header_t header, const size_t rx_len,
                     const uint8_t *const payload);
