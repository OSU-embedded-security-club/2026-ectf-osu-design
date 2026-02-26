/**
 * @file aes.h
 * @author Andrew Langan
 * @brief eCTF AES Interface
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
 * @brief Initialize the AES hardware module.
 *
 * @return int 0 on success, negative value on failure.
 */
int aes_init(void);

/**
 * @brief Encrypt data using AES-CTR-256 with hardware acceleration.
 *
 * @param[in] in Input data to be encrypted
 * @param[in] len Length of input data in bytes
 * @param[out] out Output buffer for encrypted data and authentication tag
 * @param[in] key AES encryption key
 * @param[in] iv Initialization vector
 * @return int 0 on success, negative value on failure.
 */
int aes_encrypt(const uint32_t *const in, const uint32_t len,
                uint32_t *const out, const uint32_t *const key, uint8_t *iv);

/**
 * @brief Decrypt data using AES-CTR-256 with hardware acceleration.
 *
 * @param[in] in Input data to be decrypted
 * @param[in] len Length of input data in bytes
 * @param[out] out Output buffer for decrypted data
 * @param[in] key AES decryption key
 * @param[in] iv Initialization vector
 * @param[in] tag Authentication tag to verify integrity of the ciphertext
 * @return int 0 on success, negative value on failure.
 */
NOTNULL()
int aes_decrypt(const uint32_t *const in, const uint32_t len,
                uint32_t *const out, const uint32_t *const key, uint8_t *iv,
                const uint32_t *const tag);
