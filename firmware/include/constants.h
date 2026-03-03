#pragma once

#include "ti_msp_dl_config.h"

//! Max File Size in Bytes
//! @warning The file size must be divisable by 128 bits for encryption purposes.
//! @note Possibly remove warning depending on implementation
#define MAX_FILE_SIZE 8192

#define MAX_GROUPS 32
#define NUM_SLOTS 8

#define CHUNK_SIZE 256
#define WRITE_HEADER_SIZE 59

#define PIN_DELAY 4 * 32000000