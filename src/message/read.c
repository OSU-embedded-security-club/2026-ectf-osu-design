#include "message/read.h"

#include "file.h"
#include "datastructures.h"
#include "utils.h"

#include <monocypher.h>

#define FLASH_DMA 0
#define AES_DMA 1
#define UART_DMA 2

BufferNode buffers[2];

#define READ_HEADER_LENGTH 32

void message_read_response(message_header_t header) {
    DL_AESADV_enablePower(AESADV);
    // DL_AESADV_reset(AESADV);

    // Init Buffers
    BufferNode* aes_buffer = &buffers[0];
    BufferNode* uart_buffer = &buffers[1];

    aes_buffer->next = uart_buffer;
    uart_buffer->next = aes_buffer;

    DL_DMA_Config uart_dma = {
        .trigger = 16,
        .triggerType = DL_DMA_TRIGGER_TYPE_EXTERNAL,
        .transferMode = DL_DMA_SINGLE_TRANSFER_MODE,
        .extendedMode = DL_DMA_NORMAL_MODE,
        .srcWidth = DL_DMA_WIDTH_BYTE,
        .destWidth = DL_DMA_WIDTH_BYTE,
        .srcIncrement = DL_DMA_ADDR_INCREMENT,
        .destIncrement = DL_DMA_ADDR_UNCHANGED,
    };
    DL_DMA_initChannel(DMA, UART_DMA, &uart_dma);
    DL_DMA_setDestAddr(DMA, UART_DMA, (uint32_t) &(HOST_INST->TXDATA));
    DL_UART_enableDMATransmitEvent(HOST_INST);

    // Read Request from Host
    message_header_send_ack(HOST_INST);

    uint8_t pin[6];
    utils_receive_bytes(HOST_INST, pin, sizeof(pin));

    uint8_t slot_number = DL_UART_receiveDataBlocking(HOST_INST);

    // ACK Host Command
    message_header_send_ack(HOST_INST);

    // TODO: Check Pin

    const file_slot_entry_t* file = &SLOTS[slot_number];
    const group_t* group = utils_find_group(file->signed_metadata.metadata.group_id);

    uint16_t file_length = file->signed_metadata.metadata.file_size;

    uint16_t padded_length = file_length;
    if(padded_length % 16 != 0) {
        padded_length += 16 - padded_length % 16;
    }

    // Check if File Exists
    if(file_length == 0 || file_length > MAX_FILE_SIZE) {
        // TODO: Error
        while(1) {}
    }

    // Check if HSM can read
    if(!group->private.permissions.read){
        while(1);
    }

    // Check File Signature
    int valid = crypto_eddsa_check(
        file->signed_metadata.writer_signature,
        group->public.write_key,
        (uint8_t*) &(file->signed_metadata.metadata),
        sizeof(file->signed_metadata.metadata)
    );

    // TODO: Random Pause

    if(valid == -1) {
        // TODO: Error
    }


    uint32_t keys[3][8];

    uint8_t* secret_key = (uint8_t*) keys[0];
    uint8_t* file_pub_key = (uint8_t*) keys[1];
    uint8_t* read_pub_key = (uint8_t*) keys[2];

    memcpy(file_pub_key, file->signed_metadata.metadata.encryption_public_key, sizeof(uint8_t[32]));
    memcpy(read_pub_key, group->public.read_key, sizeof(uint8_t[32]));

    uint32_t aes_key[16];
    crypto_x25519(secret_key, group->private.read_key, file_pub_key);
    crypto_blake2b((uint8_t*) aes_key, sizeof(aes_key), (uint8_t*) keys, sizeof(keys));

    DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
    DL_AESADV_setKeyAligned(AESADV, aes_key, DL_AESADV_KEY_SIZE_256_BIT);

    // Wipe Secrets
    crypto_wipe(aes_key, sizeof(aes_key));
    crypto_wipe(secret_key, sizeof(uint8_t[32]));


    // Initalize AES
    // We don't reuse keys so 0 iv
    uint32_t iv[4] = {};
    DL_AESADV_Config aes_config = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_DECRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .iv = (uint8_t*) &iv,
        .upperCryptoLength = 0,
        .lowerCryptoLength = padded_length,
        .aadLength = 0,
    };
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config);

    // Send Resposne Header
    header.message_length = sizeof(file->signed_metadata.metadata.name) + file_length;
    message_header_response(HOST_INST, header);

    // Wait for host command ACK
    message_header_receive_ack(HOST_INST);

    // Send File Name
    for(int i = 0; i < sizeof(file->signed_metadata.metadata.name); i++) {
        DL_UART_transmitDataBlocking(HOST_INST, file->signed_metadata.metadata.name[i]);
    }

    size_t bytes_read = 0;

    // Calculate First transfer size
    uint16_t first_chunk_size = CHUNK_SIZE - READ_HEADER_LENGTH;
    if(padded_length < first_chunk_size) {
        aes_buffer->length = padded_length;
    } else {
        aes_buffer->length = first_chunk_size;
    }

    bytes_read += aes_buffer->length;

    uint32_t* encrypted_data = (uint32_t*) file->encrypted_file;

    uint32_t* buffer_ptr = aes_buffer->buffer;
    for(int i = 0; i < aes_buffer->length; i += 16) {
        while(!DL_AESADV_isInputReady(AESADV));
        DL_AESADV_loadInputDataAligned(AESADV, encrypted_data);
        while(!DL_AESADV_isOutputReady(AESADV));
        DL_AESADV_readOutputDataAligned(AESADV, buffer_ptr);

        buffer_ptr += 4;
        encrypted_data += 4;
    }

    aes_buffer = aes_buffer->next;
    uart_buffer = uart_buffer->next;

    while(bytes_read < padded_length) {
        uint16_t bytes_remaining = padded_length - bytes_read;
        if(bytes_remaining < CHUNK_SIZE) {
            aes_buffer->length = bytes_remaining;
        } else {
            aes_buffer->length = CHUNK_SIZE;
        }
        bytes_read += aes_buffer->length;

        DL_DMA_setSrcAddr(DMA, UART_DMA, (uint32_t) uart_buffer->buffer);
        DL_DMA_setTransferSize(DMA, UART_DMA, uart_buffer->length);

        DL_DMA_enableChannel(DMA, UART_DMA);

        buffer_ptr = aes_buffer->buffer;
        for(int i = 0; i < aes_buffer->length; i += 16) {
            while(!DL_AESADV_isInputReady(AESADV));
            DL_AESADV_loadInputDataAligned(AESADV, encrypted_data);
            while(!DL_AESADV_isOutputReady(AESADV));
            DL_AESADV_readOutputDataAligned(AESADV, buffer_ptr);

            buffer_ptr += 4;
            encrypted_data += 4;
        }

        // Wait for transfer
        while(DL_DMA_isChannelEnabled(DMA, UART_DMA));

        message_header_receive_ack(HOST_INST);

        aes_buffer = aes_buffer->next;
        uart_buffer = uart_buffer->next;
    }

        // Check GCM Tag
        uint32_t gcm_tag[4];
        DL_AESADV_readTAGAligned(AESADV, gcm_tag);
        int verify = crypto_verify16((uint8_t*) gcm_tag, (uint8_t*) file->signed_metadata.metadata.file_signature);

        if(verify == -1) {
            while(1) {}
        }
        

        uint16_t padding = padded_length - file_length;
        DL_DMA_setSrcAddr(DMA, UART_DMA, (uint32_t) uart_buffer->buffer);
        DL_DMA_setTransferSize(DMA, UART_DMA, ((uart_buffer->length) - padding));

        DL_DMA_enableChannel(DMA, UART_DMA);
        while(DL_DMA_isChannelEnabled(DMA, UART_DMA));

        message_header_receive_ack(HOST_INST);

        DL_UART_disableDMATransmitEvent(HOST_INST);
        DL_AESADV_disablePower(AESADV);
}
