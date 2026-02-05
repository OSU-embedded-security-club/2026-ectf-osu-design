/**
 * @file aes.c
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

#include "aes.h"

#include <stdint.h>
#include <string.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_aesadv.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

/**
 * @brief Initialize the AES hardware module.
 *
 * @return int 0 on success, negative value on failure.
 */
int aes_init(void) {
  DL_AESADV_reset(AESADV);
  DL_AESADV_enablePower(AESADV);

  while (!DL_AESADV_isInputContextWriteable(AESADV)) {
    delay_cycles(1);
  }

  return 0;
}

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
                uint32_t *const out, const uint32_t *const key, uint8_t *iv) {
  if (len % 16 != 0) {
    return -1; // Length must be a multiple of AES block size (16 bytes)
  }

  // TODO: Make this a different mode (GCM?)
  const DL_AESADV_Config encrypt_config = {
      .mode = DL_AESADV_MODE_CTR,
      .direction = DL_AESADV_DIR_ENCRYPT,
      .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_32_BIT,
      .cfb_fbWidth = DL_AESADV_FB_WIDTH_128,
      .ccm_ctrWidth = DL_AESADV_CCM_CTR_WIDTH_2_BYTES,
      .ccm_tagWidth = DL_AESADV_CCM_TAG_WIDTH_1_BYTE,
      .iv = iv,
      .nonce = NULL,
      .lowerCryptoLength = 32,
      .upperCryptoLength = 0,
      .aadLength = 0,
  };

  DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
  DL_AESADV_setKeyAligned(AESADV, &key[0], DL_AESADV_KEY_SIZE_256_BIT);
  DL_AESADV_initCTR(AESADV, &encrypt_config);

  for (uint32_t i = 0; i < len; i += 4) {
    while (!DL_AESADV_isInputReady(AESADV)) {
      delay_cycles(1);
    }
    DL_AESADV_loadInputDataAligned(AESADV, &in[i]);

    while (!DL_AESADV_isOutputReady(AESADV)) {
      delay_cycles(1);
    }
    DL_AESADV_readOutputDataAligned(AESADV, &out[i]);
  }

  return DL_AESADV_readInitializationVector(AESADV, iv);
}

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
                uint32_t *const out, const uint32_t *const key, uint8_t *iv) {
  if (len % 16 != 0) {
    return -1; // Length must be a multiple of AES block size (16 bytes)
  }

  // TODO: Make this a different mode (GCM?)
  const DL_AESADV_Config decrypt_config = {
      .mode = DL_AESADV_MODE_CTR,
      .direction = DL_AESADV_DIR_DECRYPT,
      .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_32_BIT,
      .cfb_fbWidth = DL_AESADV_FB_WIDTH_128,
      .ccm_ctrWidth = DL_AESADV_CCM_CTR_WIDTH_2_BYTES,
      .ccm_tagWidth = DL_AESADV_CCM_TAG_WIDTH_1_BYTE,
      .iv = iv,
      .nonce = NULL,
      .lowerCryptoLength = 32,
      .upperCryptoLength = 0,
      .aadLength = 0,
  };

  DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
  DL_AESADV_setKeyAligned(AESADV, &key[0], DL_AESADV_KEY_SIZE_256_BIT);
  DL_AESADV_initCTR(AESADV, &decrypt_config);

  for (uint32_t i = 0; i < len; i += 4) {
    while (!DL_AESADV_isInputReady(AESADV)) {
      delay_cycles(1);
    }
    DL_AESADV_loadInputDataAligned(AESADV, &in[i]);

    while (!DL_AESADV_isOutputReady(AESADV)) {
      delay_cycles(1);
    }
    DL_AESADV_readOutputDataAligned(AESADV, &out[i]);
  }

  return DL_AESADV_readInitializationVector(AESADV, iv);
}
