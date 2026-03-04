#pragma once

#include "ti_msp_dl_config.h"

//! Maximum file size in bytes
#define MAX_FILE_SIZE 8192

//! Maximum number of groups supported
#define MAX_GROUPS 32

//! Number of file slots available
#define NUM_SLOTS 8

//! Size of each data chunk in bytes
#define CHUNK_SIZE 256

//! Size of write header in bytes
#define WRITE_HEADER_SIZE 59

//! Delay cycles for PIN validation (4 seconds at 32MHz)
#define PIN_DELAY 4 * 32000000