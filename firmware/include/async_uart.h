#pragma once

#include "ti_msp_dl_config.h"

/*!
 * @brief Async UART context structure
 */
typedef struct {
    //! UART peripheral registers
    UART_Regs* uart;

    //! DMA channel used for transfer
    uint8_t dma_channel;

    //! Flag indicating transfer completion
    bool transfer_complete;

    //! Stop transfer request flag
    bool stop;

    //! Flag indicating transfer has stopped
    bool stopped;

    //! Offset of first chunk in buffer (for ACKing)
    size_t first_chunk_offset;

    //! Number of bytes transferred so far
    size_t bytes_transfered;

    //! Total number of bytes to transfer
    size_t total_bytes;

    //! Buffer for data transfer
    uint8_t* buffer;
} async_uart_ctx;

/*!
 * @brief Starts asynchronous UART receive operation
 * 
 * @param[in,out] ctx Pointer to async UART context
 *
 * @if **ctx->transfer_complete** == true
 *   **ctx->buffer** is filled with **ctx->total_bytes** bytes from **ctx->uart**
 * @endif
 *
 * @if **ctx->stop** == true
 *   The UART DMA operation will be halted at the next ACK
 * @endif
 *
 * @if **ctx->stopped** == true
 *   The UART DMA operation is halted
 * @endif
 *
 * @returns 0 on success, negative value on error
 */
int async_uart_receive(async_uart_ctx* ctx);