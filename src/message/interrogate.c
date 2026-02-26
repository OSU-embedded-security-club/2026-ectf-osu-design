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

/**
 * @brief Flag to indicate whether the communication channel is open and ready
 * for secure communication.
 *
 */
static bool channel_is_open = false;

/**
 * @brief Checks if the communication channel is open and ready for secure
 * communication.
 *
 * @return true if the channel is open and ready, false otherwise
 */
bool channel_open(void) { return channel_is_open; }

/**
 * @brief Sends data securely over the communication channel, ensuring
 * confidentiality and integrity.
 *
 * @param data Pointer to the data to be sent
 * @param length Length of the data in bytes
 */
void secure_send(const uint8_t *const data, const size_t length) {}

/**
 * @brief Receives data securely from the communication channel, ensuring
 * confidentiality and integrity.
 *
 * @param buffer Pointer to the buffer where received data will be stored
 * @param length Length of the buffer in bytes
 */
void secure_receive(uint8_t *const buffer, const size_t length) {}

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

  // TODO: KEX

  char msg[] = "Interrogate not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
