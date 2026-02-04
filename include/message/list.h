#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"


/**
 * @brief Responds to a List Request
 *
 * @param[in] header    Message Header
 */
void message_list_response(message_header_t header);