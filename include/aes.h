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

#ifndef __AES__
#define __AES__

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
 * @param in Input data to be encrypted
 * @param len Length of input data in bytes
 * @param out Output buffer for encrypted data
 * @param key AES encryption key
 * @param iv Initialization vector
 * @return int 0 on success, negative value on failure.
 */
int aes_encrypt(const uint32_t *const in, const uint32_t len,
                uint32_t *const out, const uint32_t *const key, uint8_t *iv);

/**
 * @brief Decrypt data using AES-CTR-256 with hardware acceleration.
 *
 * @param in Input data to be decrypted
 * @param len Length of input data in bytes
 * @param out Output buffer for decrypted data
 * @param key AES decryption key
 * @param iv Initialization vector
 * @return int 0 on success, negative value on failure.
 */
int aes_decrypt(const uint32_t *const in, const uint32_t len,
                uint32_t *const out, const uint32_t *const key, uint8_t *iv);

#endif