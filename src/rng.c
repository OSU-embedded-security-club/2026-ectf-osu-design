/**
 * @file rng.c
 * @author Andrew Langan
 * @brief eCTF Random Number Generator (RNG) Interface
 * @date 2026
 *
 * This source file is part of an example system for MITRE's 2026 Embedded CTF
 * (eCTF). This code is being provided only for educational purposes for the
 * 2026 MITRE eCTF competition, and may not meet MITRE standards for quality.
 * Use this code at your own risk!
 *
 * @copyright Copyright (c) 2026 The MITRE Corporation
 */

#include "rng.h"

#include <stdint.h>
#include <string.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_trng.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

/**
 * @brief Initialize the RNG hardware module.
 *
 * @return int 0 on success, negative value on failure.
 */
int rng_init(void) {
  DL_TRNG_reset(TRNG);

  DL_TRNG_enablePower(TRNG);

  while (!DL_TRNG_isCaptureReady(TRNG)) {
    delay_cycles(1);
  }

  return 0;
}

/**
 * @brief Get random bytes from the RNG.
 *
 * @param[out] buf Pointer to the buffer where random bytes will be stored.
 * @param[in] len Number of random bytes to generate.
 * @return int 0 on success, negative value on failure.
 */
int rng_get_bytes(uint8_t *const buf, const uint32_t len) {
  if (len == 0) {
    return -1;
  }

  for (uint32_t i = 0; i < (len / sizeof(uint32_t)); ++i) {
    while (!DL_TRNG_isCaptureReady(TRNG)) {
      delay_cycles(1);
    }

    const uint32_t random_value = DL_TRNG_getCapture(TRNG);
    memcpy(&buf[i * sizeof(uint32_t)], &random_value, sizeof(uint32_t));
  }

  // Account for any remaining bytes if len is not a multiple of 4
  const uint32_t rem = len % sizeof(uint32_t);
  if (rem > 0) {
    while (!DL_TRNG_isCaptureReady(TRNG)) {
      delay_cycles(1);
    }

    const uint32_t random_value = DL_TRNG_getCapture(TRNG);
    memcpy(&buf[len - rem], &random_value, rem);
  }

  return 0;
}
