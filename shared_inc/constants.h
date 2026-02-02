#pragma once

#include "sysconfig/ti_msp_dl_config.h"
#include "psa/crypto.h"

//! Max File Size in Bytes
//! @warning The file size must be divisable by 128 bits for encryption purposes.
//! @note Possibly remove warning depending on implementation
#define MAX_FILE_SIZE 8192

#define MAX_GROUPS 32


/*
 * Signature Constants
 */
#define SIGNATURE_ALG       PSA_ALG_ED25519PH
#define SIGNATURE_KEY_TYPE  PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)
#define SIGNATURE_KEY_BITS  255
#define SIGNATURE_KEY_BYTES 32
#define SIGNATURE_SIZE      PSA_SIGN_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_KEY_PAIR(SIGNATURE_KEY_TYPE, SIGNATURE_KEY_SIZE, SIGNATURE_ALG)


/*
 * KD(Key Derivation) Constants
 */
#define KD_ALG              PSA_ALG_ECDH
#define KD_KEY_TYPE         PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)
#define KD_KEY_BITS         255
#define KD_KEY_BYTES        32
#define KD_OUTPUT_SIZE      PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(KD_KEY_TYPE, KD_KEY_BITS)


const uint8_t NUM_SLOTS = 8;