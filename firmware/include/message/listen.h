#pragma once

#include "message/header.h"

/*!
 * @brief Responds to Listen Request
 * 
 * @param[in] header Message header containing request context
 */
void message_listen(message_header_t header);