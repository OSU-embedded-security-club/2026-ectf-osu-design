#pragma once

#include "ti_msp_dl_config.h"

//! Max File Size in Bytes
//! @warning The file size must be divisable by 128 bits for encryption purposes.
//! @note Possibly remove warning depending on implementation
#define MAX_FILE_SIZE 8192

#define MAX_GROUPS 32
#define NUM_SLOTS 8

//! This padding is used to align the ACK for write requests
//! with the AES block size
#define FILE_PADDING_FRONT 5
#define FILE_PADDING_BACK 3