/**
 * @file interrogate.h
 * @author Andrew Langan
 * @brief eCTF Interrogate Message Handler
 * @date 2026
 *
 * This source file is part of an example system for MITRE's 2026 Embedded CTF
 * (eCTF). This code is being provided only for educational purposes for the
 * 2026 MITRE eCTF competition, and may not meet MITRE standards for quality.
 * Use this code at your own risk!
 *
 * @copyright Copyright (c) 2026 The MITRE Corporation
 */

#pragma once

#include "message/header.h"
#include "utils.h"

/**
 * @brief Responds to Interrogate Request
 *
 * @param[in] header    Message Header
 * @param[in] rx_len    Length of the received payload
 * @param[in] payload   Pointer to the received payload
 */
NOTNULL()
void message_interrogate_response(const message_header_t header,
                                  const size_t rx_len,
                                  const uint8_t *const payload);
