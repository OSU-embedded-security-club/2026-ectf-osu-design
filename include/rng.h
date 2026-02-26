/**
 * @file rng.h
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

#pragma once

#include "utils.h"
#include <stdint.h>

/**
 * @brief Initialize the RNG hardware module.
 *
 * @return int 0 on success, negative value on failure.
 */
int rng_init(void);

/**
 * @brief Get random bytes from the RNG.
 *
 * @param[out] buf Pointer to the buffer where random bytes will be stored.
 * @param[in] len Number of random bytes to generate.
 * @return int 0 on success, negative value on failure.
 */
NOTNULL() int rng_get_bytes(uint8_t *const buf, const uint32_t len);
