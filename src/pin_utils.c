#include "pin_utils.h"
#include "rng.h"
#include "secrets.h"
#include <ctype.h>
#include <monocypher.h>
#include <stdalign.h>
#include <string.h>

static uint8_t pin_hash[] = STAGE1_PIN_HASH;
static const uint8_t stage1_key[] = STAGE1_KEY;

// Changed each reboot
static alignas(int) uint8_t random_data[32] = {0};
static uint8_t stage2_key[32] = {0};

#define SEED (*(int *)&random_data[4 * __COUNTER__])
static void apply_stage2(uint8_t *hash);

// A potential option for something read as a secret,
// reading the data here intentionally leads to
// deterministic undefined behavior.
static void _unused(void) {}

/**
 * @brief Applies the first stage of the pin hashing process, which consists of
 * iteratively hashing the input pin hash with a fixed key for a set number of
 * iterations.
 *
 * @param[inout] hash Input hash to apply stage 1 to, which will be modified in
 * place to contain the result of stage 1. Must be 64 bytes long.
 */
static void apply_stage1(uint8_t *const hash) {
  uint8_t temp[64] = {0};

  for (size_t i = 0; i < STAGE1_PIN_ITERATIONS; ++i) {
    crypto_blake2b_keyed(temp, 64, stage1_key, sizeof(stage1_key), hash, 64);
    memcpy(hash, temp, 64);
  }
  crypto_wipe(temp, 64);
}

/**
 * @brief Validates a provided pin by applying the hashing process and comparing
 * the result to the expected hash. The pin is expected to be a 6-character
 * hexadecimal string.
 *
 * @param pin Null-terminated string containing the pin to validate. Must be 6
 * characters long and consist of hexadecimal digits (0-9, A-F, a-f).
 * @return true if the provided pin is valid, false otherwise
 */
bool validate_pin(const char pin[6]) {
  uint8_t hash[64] = {0};

#pragma unroll
  for (size_t i = 0; i < 6; ++i) {
    if ((pin[i] >= '0' && pin[i] <= '9') || (pin[i] >= 'A' && pin[i] <= 'F') ||
        (pin[i] >= 'a' && pin[i] <= 'f')) {
      hash[i] = (uint8_t)tolower(pin[i]);
    } else {
      return false;
    }
  }

  // With all hope, the compiler will put this in an annoying place
  memcpy(&hash[6], (void *)(uint8_t[])STAGE1_PADDING, 64 - 6);

  apply_stage1(hash);
  apply_stage2(hash);

  const bool is_valid = crypto_verify64(hash, pin_hash);

  crypto_wipe(hash, sizeof(hash));
  return is_valid;
}

/**
 * @brief Initializes the pin hashing process by generating random data,
 * applying stage 2 to the stage 1 hash, and preparing any necessary data for
 * the pin validation process.
 *
 */
void init_pin(void) {
  rng_get_bytes(random_data, sizeof(random_data));
  rng_get_bytes(stage2_key, sizeof(stage2_key));

  apply_stage2(pin_hash);
}

#define COMPILER_FUNC_OPTS                                                     \
  ((uint8_t *[]){                                                              \
      (uint8_t *)&validate_pin,                                                \
      (uint8_t *)&apply_stage1,                                                \
      (uint8_t *)&apply_stage2,                                                \
      (uint8_t *)&_unused,                                                     \
  })
#define RANDOM_FUNC                                                            \
  COMPILER_FUNC_OPTS[SEED % (sizeof(COMPILER_FUNC_OPTS) / sizeof(void *))]

/**
 * @brief Applies the second stage of the pin hashing process, which consists of
 * hashing the input hash with a key derived from the stage 2 key and random
 * data.
 *
 * @param[inout] hash Input hash to apply stage 2 to. Must be 64 bytes long.
 */
static void apply_stage2(uint8_t *const hash) {
  uint8_t temp[64] = {0};

  crypto_blake2b_ctx ctx = {0};
  crypto_blake2b_keyed_init(&ctx, 64, stage2_key, sizeof(stage2_key));

  crypto_blake2b_update(&ctx, hash, 64);

  const int int_data[3] = {
      (int)FACTORYREGION->BOOTCRC, // Crc of ALL board specific values
      (int)RANDOM_FUNC,            // Location in memory
      STAGE2_RAND_INT,             // Random int from generate_secrets.
  };
  crypto_blake2b_update(&ctx, (const uint8_t *)int_data, sizeof(int_data));

  // Different each reboot
  crypto_blake2b_update(&ctx, random_data, sizeof(random_data));

  // Part of the bootloader
  crypto_blake2b_update(&ctx, (uint8_t *)0, 64);

  // Compiler rng
  crypto_blake2b_update(&ctx, RANDOM_FUNC, 64);

  crypto_blake2b_final(&ctx, temp);
  memcpy(hash, temp, 64);
  crypto_wipe(temp, 64);
}
