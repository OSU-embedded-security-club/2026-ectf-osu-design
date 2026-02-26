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

#include "message/interrogate.h"
#include "ti_msp_dl_config.h"
#include <limits.h>

#define MAX_PACKET_SIZE UINT16_MAX

extern void wait_for_ack(void);

/**
 * @brief Generic failure response for unimplemented features or invalid
 * operations.
 *
 */
static void fail_generic(void) {
  char msg[] = "No.";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
  return;
}

/**
 * @brief Responds to Interrogate Request
 *
 * @param[in] header    Message Header
 * @param[in] rx_len    Length of the received payload
 * @param[in] payload   Pointer to the received payload
 */
void message_interrogate_response(const message_header_t header,
                                  const size_t rx_len,
                                  const uint8_t *const payload) {
  if (header.message_length == 0) {
    return fail_generic();
  } else if (header.operation != MESSAGE_INTERROGATE) {
    return fail_generic();
  }

  char msg[] = "Interrogate not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
