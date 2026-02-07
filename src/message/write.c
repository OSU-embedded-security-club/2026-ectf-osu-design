#include "message/write.h"

#include "file.h"
#include "utils.h"

#define HEADER_SIZE 59

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1

void message_write_response(message_header_t header) {
    //
    // Setup DMA before ACKing
    //

    // Trigger 16 is UART0 Publisher 1
    // Check Datasheet for more info
    const DL_DMA_Config dma_config = {
        .trigger = 15,
        .triggerType = DL_DMA_TRIGGER_TYPE_EXTERNAL,
        .transferMode = DL_DMA_SINGLE_TRANSFER_MODE,
        .extendedMode = DL_DMA_NORMAL_MODE,
        .srcWidth = DL_DMA_WIDTH_BYTE,
        .destWidth = DL_DMA_WIDTH_BYTE,
        .srcIncrement = DL_DMA_ADDR_UNCHANGED,
        .destIncrement = DL_DMA_ADDR_INCREMENT
    };
    DL_DMA_initChannel(DMA, 0, &dma_config);

    // Buffer to hold parital messages
    uint8_t buffer[256];

    // Configure transfer from HOST UART into buffer
    DL_DMA_setSrcAddr(DMA, DMA_CHANNEL_0, (uint32_t) &(HOST_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CHANNEL_0, (uint32_t) buffer);

    // Send ACK to begin message transfer
    message_header_send_ack(HOST_INST);

    // Bytes until next ACK
    uint16_t bytes_to_ack = 256;

    // Parse PIN from HOST UART
    uint8_t pin[PIN_LENGTH];
    for(int i = 0; i < PIN_LENGTH; i++) {
        pin[i] = DL_UART_receiveDataBlocking(HOST_INST);
    }
    bytes_to_ack -= 6;

    // Begin Constructing Metadata Entry
    file_metadata_t metadata;

    // Parse Slot from HOST UART
    utils_receive_bytes(HOST_INST, &metadata.ectf_metadata.slot_number, sizeof(metadata.ectf_metadata.slot_number));
    bytes_to_ack--;

    // Parse Group ID from HOST UART
    uint8_t group_id[2] = {
        DL_UART_receiveDataBlocking(HOST_INST),
        DL_UART_receiveDataBlocking(HOST_INST),
    };
    metadata.ectf_metadata.group_id = *((uint16_t*) group_id);
    bytes_to_ack -= 2;

    // Parse file name from HOST UART
    for(int i = 0; i < sizeof(metadata.ectf_metadata.name); i++) {
        metadata.ectf_metadata.name[i] = DL_UART_receiveDataBlocking(HOST_INST);
    }
    bytes_to_ack -= sizeof(metadata.ectf_metadata.name);

    // Parse UUID from HOST UART
    uint8_t uuid[16];
    for(int i = 0; i < 16; i++) {
        uuid[i] = DL_UART_receiveDataBlocking(HOST_INST);
    }

    // Parse file size from HOST UART
    uint8_t file_size[2] = {
        DL_UART_receiveDataBlocking(HOST_INST),
        DL_UART_receiveDataBlocking(HOST_INST),
    };
    metadata.file_size = *((uint16_t*)file_size);

    // Check if file size if less than 256 - HEADER_SIZE
    if(metadata.file_size < buffer_length) {
        buffer_length = metadata->file_size;
        DL_DMA_setTransferSize(DMA, DMA_CHANNEL_0, buffer_length);
    }

    // Begin using DMA for Transfer
    DL_DMA_enableChannel(DMA, DMA_CHANNEL_0);
    DL_UART_enableDMAReceiveEvent(HOST_INST, DL_UART_DMA_INTERRUPT_RX);

    // bool authenticated = utils_verify_pin(pin, sizeof(pin));

    // if(!authenticated) {
    //     char msg[] = "Invalid PIN";
    //     message_header_send_error(HOST_INST, msg, sizeof(msg));

    //     // Wait 5 Seconds
    //     delay_cycles(5 * 32000000);

    //     return;
    // }

    while(true) {
        delay_cycles(1000);
    }

    char msg[] = "Write not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
