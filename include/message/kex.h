/**
 * @file kex.h
 * @author Andrew Langan
 * @brief eCTF Key Exchange Message Handler
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
#include <stdint.h>

/**
 * @brief Structure for the Key Exchange command payload.
 *
 */
typedef struct {
  uint8_t e_public_key[32];
  uint16_t group_id;
  uint8_t sig[64];
} kex_payload_t;

/**
 * @brief Structure to hold an AES cipher state.
 *
 */
typedef struct {
  uint8_t key[16];
  uint8_t nonce[12];
  bool active;
} aes_state_t;

/**w
 * @brief Enumeration of permissions requested
 *
 */
typedef enum : uint8_t {
  PERM_NONE = 0b000,
  PERM_R = 0b001,
  PERM_W = 0b010,
  PERM_C = 0b100,
  PERM_RW = 0b011,
  PERM_RC = 0b101,
  PERM_WC = 0b110,
  PERM_RWC = 0b111,
} perm_t;

/**
 * @brief Global AES cipher state.
 *
 */
extern aes_state_t cipher_state;

/**
 * @brief Responds to Key Exchange Request
 *
 * @param[in] header    Message Header
 * @param[in] rx_len    Length of the received payload
 * @param[in] payload   Pointer to the received payload
 */
NOTNULL()
void message_kex_response(const message_header_t header, const size_t rx_len,
                          const uint8_t *const payload);
