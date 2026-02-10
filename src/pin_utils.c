#include "pin_utils.h"
#include <ctype.h>
#include <string.h>
#include <monocypher.h>
#include "secrets.h"
#include <stdalign.h>
#include "random.h"




static uint8_t pin_hash[] = STAGE1_PIN_HASH;

// Changed each reboot (to fuck with attackers)
static alignas(int) uint8_t random_data[32];
static uint8_t stage2_key[32];

static const uint8_t stage1_key[] = STAGE1_KEY;

static void apply_stage2(uint8_t *hash);

static void _unused(){}


#define SEED ( *(int*)&random_data[4*__COUNTER__] )



// Hash MUST be 64 long
static void apply_stage1(uint8_t *hash)
{
  uint8_t temp[64];

  for (int i = 0; i < STAGE1_PIN_ITERATIONS; i++) {
    crypto_blake2b_keyed(
        temp,
        64,
        stage1_key,
        sizeof(stage1_key),
        hash,
        64);
    memcpy(hash, temp, 64);
  }
  crypto_wipe(temp, 64);
}



bool validate_pin(const char pin[6]) {
  uint8_t hash[64];

#pragma unroll
  for (int i = 0; i < 6; i++) {
    if ((pin[i] >= '0' && pin[i] <= '9') || (pin[i] >= 'A' && pin[i] <= 'F') ||
        (pin[i] >= 'a' && pin[i] <= 'f')) {
      hash[i] = (char)tolower(pin[i]);
    } else {
      return false;
    }
  }

  // With all hope, the compiler will put this in an annoying place
  memcpy(&hash[6], (void *)(uint8_t[])STAGE1_PADDING, 64 - 6);

  apply_stage1(hash);
  apply_stage2(hash);

  // Constant time memcmp
  volatile bool is_valid = true;
#pragma unroll
  for (int i = 0; i < 64; i++)
    is_valid &= hash[i] == pin_hash[i];

  crypto_wipe(hash, sizeof(hash));
  return is_valid;
}

void init_pin() {
  random_fill_buffer(random_data, sizeof(random_data));
  random_fill_buffer(stage2_key, sizeof(stage2_key));

  apply_stage2(pin_hash);
}

#define COMPILER_FUNC_OPTS ((uint8_t*[]) {\
   (uint8_t*) &validate_pin,     \
   (uint8_t*) &apply_stage1,     \
   (uint8_t*) &apply_stage2,     \
   (uint8_t*) &_unused,     \
})
#define RANDOM_FUNC COMPILER_FUNC_OPTS[SEED % (sizeof(COMPILER_FUNC_OPTS) / sizeof(void*))]

// Hash MUST be 64 long
static void apply_stage2(uint8_t *hash) {
  uint8_t temp[64];

  crypto_blake2b_ctx ctx;
  crypto_blake2b_keyed_init(&ctx, 64, stage2_key, sizeof(stage2_key));

  crypto_blake2b_update(&ctx, hash, 64);

  int int_data[] = {
    FACTORYREGION->BOOTCRC,
    (int) (size_t) RANDOM_FUNC,
    STAGE2_RAND_INT,
  };

  crypto_blake2b_update(&ctx, (uint8_t*) int_data, sizeof(int_data));
  crypto_blake2b_update(&ctx, random_data, sizeof(random_data));

  // Part of the bootloader
  crypto_blake2b_update(&ctx, (uint8_t*) 0, 64);

  size_t ptr = (size_t) &apply_stage2; 

  // Compiler rng
  crypto_blake2b_update(&ctx, (uint8_t*) &ptr, sizeof(void*));
  crypto_blake2b_update(&ctx, RANDOM_FUNC, 64);




  crypto_blake2b_final(&ctx, temp);
  memcpy(hash, temp, 64);
  crypto_wipe(temp, 64);
}


