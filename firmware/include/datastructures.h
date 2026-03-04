#pragma once

#include "ti_msp_dl_config.h"
#include "constants.h"

/*!
 * @brief Buffer node for linked list of data chunks
 */
typedef struct BufferNode {
    //! Pointer to next node in list
    struct BufferNode* next;

    //! Length of data in buffer
    uint16_t length;

    //! Data buffer (CHUNK_SIZE bytes)
    uint32_t buffer[CHUNK_SIZE / 4];
} BufferNode;