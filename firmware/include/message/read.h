#pragma once

#include "message/header.h"
/*!
 * @brief Responds to Read Request
 * 
 * @param[in] header Message header containing request context
 */
void message_read_response(message_header_t header);