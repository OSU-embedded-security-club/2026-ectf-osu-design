#pragma once

#include "ti_msp_dl_config.h"
#include "constants.h"

typedef struct BufferNode {
    struct BufferNode* next;
    uint16_t length;
    uint32_t buffer[CHUNK_SIZE / 4];
} BufferNode;