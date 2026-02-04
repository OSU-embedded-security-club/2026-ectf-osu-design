#pragma once

#include "message/header.h"

/**
 * @brief Responds to Write Request
 *
 * @param[in] header    Message Header
 */
void message_write_response(message_header_t header);