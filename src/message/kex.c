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

#include "message/kex.h"
#include "message/header.h"
#include "monocypher.h"
#include "rng.h"
#include "secrets.h"
#include "ti_msp_dl_config.h"
#include "utils.h"

/**
 * @brief Global AES cipher state.
 *
 */
aes_state_t cipher_state = {.active = false};

/**
 * @brief Generic failure response for unimplemented features or invalid
 * operations.
 *
 */
static inline void fail_generic(void) {
  char msg[] = "No.";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
  return;
}

/**
 * @brief Responds to Key Exchange Request
 *
 * @param[in] header    Message Header
 * @param[in] rx_len    Length of the received payload
 * @param[in] payload   Pointer to the received payload
 */
void message_kex_response(const message_header_t header, const size_t rx_len,
                          const uint8_t *const payload) {
  if ((header.message_length == 0) ||
      (header.message_length != sizeof(kex_payload_t)) ||
      (header.operation != MESSAGE_KEX)) {
    return fail_generic();
  }

  const kex_payload_t *const kex_payload = (const kex_payload_t *)payload;

  uint8_t temp[32 + sizeof(uint16_t) + sizeof(uint8_t)] = {0};

  /* My HSM's ephemeral key state */
  uint8_t my_priv[32] = {0};
  uint8_t my_secret[64] = {0};
  uint8_t my_pub[32] = {0};

  /* Requesting HSM's ephemeral key state */
  uint8_t other_pub[32] = {0};

  perm_t perms = PERM_NONE;
  crypto_blake2b_ctx ctx = {0};

  /* Iterates over all the permissions to find which key signs the request. */
  for (uint8_t i = 0; (i < 8) && (perms == PERM_NONE); ++i) {
    /* Derived root sub-key state based on hash(ROOT_KEY ++ GID ++ perms) */
    uint8_t derived_priv[32] = {0};
    uint8_t derived_secret[64] = {0};
    uint8_t derived_pub[32] = {0};

    memcpy(&temp[0], &ROOT_KEY[0], sizeof(ROOT_KEY));
    memcpy(&temp[32], &kex_payload->group_id, sizeof(kex_payload->group_id));
    memcpy(&temp[34], &i, sizeof(i));

    crypto_blake2b_init(&ctx, 32);
    crypto_blake2b_update(&ctx, temp, sizeof(temp));
    crypto_blake2b_final(&ctx, &derived_priv[0]);

    memcpy(&temp[0], &kex_payload->e_public_key,
           sizeof(kex_payload->e_public_key));

    /* Convert the priv key -> pub */
    crypto_eddsa_key_pair(derived_secret, derived_pub, derived_priv);
    crypto_wipe(derived_secret, sizeof(derived_secret));

    /* Check if the message is signed correctly. */
    if (crypto_eddsa_check(kex_payload->sig, &derived_pub[0], &temp[0],
                           32 + sizeof(uint16_t)) == 0) {
      perms = (perm_t)i;
    }

    crypto_wipe(derived_priv, sizeof(derived_priv));
    crypto_wipe(derived_pub, sizeof(derived_pub));
    crypto_wipe(derived_secret, sizeof(derived_secret));
  }

  crypto_wipe(temp, sizeof(temp));

  /* Invalid key given. */
  if (perms == PERM_NONE) {
    return;
  }

  if (rng_get_bytes(&my_priv[0], 32) != 0) {
    return;
  }

  crypto_eddsa_key_pair(my_secret, my_pub, my_priv);

  /* Convert from eddsa to x25519 */
  crypto_eddsa_to_x25519(other_pub, kex_payload->e_public_key);

  crypto_x25519(&temp[0], my_priv, other_pub);

  crypto_wipe(my_priv, sizeof(my_priv));
  crypto_wipe(my_secret, sizeof(my_secret));
  crypto_wipe(other_pub, sizeof(other_pub));

  /* Derive cipher state */
  crypto_blake2b_init(&ctx, 32);
  crypto_blake2b_update(&ctx, &temp[0], 32);
  crypto_blake2b_final(&ctx, &temp[0]);

  memcpy(&cipher_state.key[0], &temp[0], sizeof(cipher_state.key));
  memcpy(&cipher_state.nonce[0], &temp[24], 8);
  memcpy(&cipher_state.nonce[8], "\0x\0SU", 4);
  cipher_state.active = true;

  crypto_wipe(temp, sizeof(temp));

  memcpy(&temp[0], &my_pub[0], sizeof(my_pub));
  memcpy(&temp[32], &kex_payload->group_id, sizeof(kex_payload->group_id));

  utils_send_packet(HSM_INST, MESSAGE_KEX, &temp[0], sizeof(temp) - 1);

  crypto_wipe(temp, sizeof(temp));
  crypto_wipe(my_pub, sizeof(my_pub));
}
