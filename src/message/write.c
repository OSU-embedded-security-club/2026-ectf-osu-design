#include "message/write.h"

#include "file.h"
#include "utils.h"

#define HEADER_SIZE 59

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1

void message_write_response(message_header_t header) {
    // TODO: Implement write response

    // Setup DMA before ACKing

    // Trigger 16 is UART0 Publisher 1
    // Check Datasheet for more info
    const DL_DMA_Config dma_config = {
        .trigger = 16,
        .triggerType = DL_DMA_TRIGGER_TYPE_EXTERNAL,
        .transferMode = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
        .extendedMode = DL_DMA_NORMAL_MODE,
        .srcWidth = DL_DMA_WIDTH_BYTE,
        .destWidth = DL_DMA_WIDTH_BYTE,
        .srcIncrement = DL_DMA_ADDR_UNCHANGED,
        .destIncrement = DL_DMA_ADDR_INCREMENT
    };
    DL_DMA_initChannel(DMA, 0, &dma_config);

    uint8_t buffer[256];
    uint8_t buffer_length = 256 - HEADER_SIZE;

    DL_DMA_setSrcAddr(DMA, DMA_CHANNEL_0, HOST_INST->RXDATA);
    DL_DMA_setDestAddr(DMA, DMA_CHANNEL_0, *buffer);
    DL_DMA_setTransferSize(DMA, DMA_CHANNEL_0, buffer_length);


    // message_header_response(HOST_INST, MESSAGE_HEADER_ACK);

    uint8_t pin[PIN_LENGTH];

    for(int i = 0; i < PIN_LENGTH; i++) {
        pin[i] = DL_UART_receiveDataBlocking(HOST_INST);
    }


    file_slot_entry_t slot_entry;

    file_metadata_t * metadata = &slot_entry.signed_metadata.metadata;

    metadata->ectf_metadata.slot_number = DL_UART_receiveDataBlocking(HOST_INST);

    uint8_t group_id[2] = {
        DL_UART_receiveDataBlocking(HOST_INST),
        DL_UART_receiveDataBlocking(HOST_INST),
    };
    metadata->ectf_metadata.group_id = *((uint16_t*) group_id);

    for(int i = 0; i < sizeof(metadata->ectf_metadata.name); i++) {
        metadata->ectf_metadata.name[i] = DL_UART_receiveDataBlocking(HOST_INST);

        // If last character is null-terminator break
        if(metadata->ectf_metadata.name[i] == '\0')
            break;
    }

    uint8_t uuid[2] = {
        DL_UART_receiveDataBlocking(HOST_INST),
        DL_UART_receiveDataBlocking(HOST_INST),
    };

    uint8_t file_size[2] = {
        DL_UART_receiveDataBlocking(HOST_INST),
        DL_UART_receiveDataBlocking(HOST_INST),
    };
    metadata->file_size = *((uint16_t*)file_size);

    // Check if file size if less than 256 - HEADER_SIZE
    if(metadata->file_size < buffer_length) {
        buffer_length = metadata->file_size;
        DL_DMA_setTransferSize(DMA, DMA_CHANNEL_0, buffer_length);
    }

    // Begin using DMA for Transfer
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
