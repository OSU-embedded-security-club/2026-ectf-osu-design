#include "async_uart.h"

#include "message/header.h"
#include "constants.h"

async_uart_ctx* transfers[16];

int async_uart_receive(async_uart_ctx* ctx) {
    transfers[ctx->dma_channel] = ctx;
    ctx->transfer_complete = false;
    ctx->stop = false;
    ctx->stopped = false;
    
    DL_DMA_Config config = {
        .triggerType = DL_DMA_TRIGGER_TYPE_EXTERNAL,
        .transferMode = DL_DMA_SINGLE_TRANSFER_MODE,
        .extendedMode = DL_DMA_NORMAL_MODE,
        .srcWidth = DL_DMA_WIDTH_BYTE,
        .destWidth = DL_DMA_WIDTH_BYTE,
        .srcIncrement = DL_DMA_ADDR_UNCHANGED,
        .destIncrement = DL_DMA_ADDR_INCREMENT,
    };

    switch((uint32_t) ctx->uart) {
        case (uint32_t) UART0:
            config.trigger = 15;
            break;
        case (uint32_t) UART1:
            config.trigger = 17;
            break;
        default:
            return -1;
    }

    DL_DMA_initChannel(DMA, ctx->dma_channel, &config);
    DL_DMA_setSrcAddr(DMA, ctx->dma_channel, (uint32_t) &(ctx->uart->RXDATA));
    DL_DMA_setDestAddr(DMA, ctx->dma_channel, (uint32_t) (ctx->buffer));

    DL_UART_enableDMAReceiveEvent(ctx->uart, DL_UART_DMA_INTERRUPT_RX);

    size_t chunk_bytes_remaining = CHUNK_SIZE - ctx->first_chunk_offset;
    if(ctx->total_bytes < chunk_bytes_remaining) {
        DL_DMA_setTransferSize(DMA, ctx->dma_channel, ctx->total_bytes);
        ctx->bytes_transfered = ctx->total_bytes;
    } else {
        DL_DMA_setTransferSize(DMA, ctx->dma_channel, chunk_bytes_remaining);
        ctx->bytes_transfered = chunk_bytes_remaining;
    }

    DL_DMA_enableInterrupt(DMA, (1 << (ctx->dma_channel)));

    delay_cycles(DMA_DELAY);
    DL_DMA_enableChannel(DMA, ctx->dma_channel);

    return 0;
}

__attribute__((interrupt))
void DMA_IRQHandler(void) {
    DL_DMA_EVENT_IIDX interrupt = DL_DMA_getPendingInterrupt(DMA);
    uint32_t interrupt_mask = (1 << (interrupt - 1));

    // The enumeration lines up with the DMA channel #
    async_uart_ctx* ctx = transfers[interrupt - 1];

    if(ctx->stop) {
        DL_DMA_disableInterrupt(DMA, interrupt_mask);
        DL_UART_disableDMAReceiveEvent(ctx->uart, DL_UART_DMA_INTERRUPT_RX);
        DL_DMA_clearInterruptStatus(DMA, interrupt_mask);
        ctx->stopped = true;
        return;
    }

    size_t bytes_remaining = ctx->total_bytes - ctx->bytes_transfered;
    if(bytes_remaining == 0) {
        ctx->transfer_complete = true;
        message_header_send_ack(ctx->uart);
        DL_DMA_disableInterrupt(DMA, interrupt_mask);
        DL_UART_disableDMAReceiveEvent(ctx->uart, DL_UART_DMA_INTERRUPT_RX);
        DL_DMA_clearInterruptStatus(DMA, interrupt_mask);
        return;
    } else if (bytes_remaining < CHUNK_SIZE) {
        DL_DMA_setTransferSize(DMA, ctx->dma_channel, bytes_remaining);
        ctx->bytes_transfered += bytes_remaining;
    } else {
        DL_DMA_setTransferSize(DMA, ctx->dma_channel, CHUNK_SIZE);
        ctx->bytes_transfered += CHUNK_SIZE;
    }

    DL_DMA_clearInterruptStatus(DMA, interrupt_mask);

    // Enable Next Transfer
    DL_DMA_enableChannel(DMA, ctx->dma_channel);
    message_header_send_ack(ctx->uart);

}