#pragma once

#include "ti_msp_dl_config.h"

typedef struct {
    UART_Regs* uart;
    uint8_t dma_channel;
    bool transfer_complete;

    bool stop;
    bool stopped;

    size_t first_chunk_offset;
    size_t bytes_transfered;
    size_t total_bytes;
    uint8_t* buffer;
} async_uart_ctx;

int async_uart_receive(async_uart_ctx* ctx);