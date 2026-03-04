#pragma once

#include "message/header.h"

/*!
 * @brief Responds to Receive Request
 * 
 * @param[in] header Message header containing request context
 */
void message_recieve(message_header_t header);