#include "message/write.h"

#include "file.h"
#include "async_uart.h"
#include "datastructures.h"
#include "random.h"
#include "utils.h"

#include <monocypher.h>

#include <string.h>

BufferNode buffers[2];

#define DMA_CHANNEL 1

message_write_header_t message_write_parse_header(UART_Regs* uart) {
    message_write_header_t write_header;

    // Parse UART message into struct
    utils_receive_bytes(uart, &write_header.pin, sizeof(write_header.pin));
    utils_receive_bytes(uart, &write_header.slot, sizeof(write_header.slot));
    utils_receive_bytes(uart, &write_header.group_id, sizeof(write_header.group_id));
    utils_receive_bytes(uart, &write_header.name, sizeof(write_header.name));
    utils_receive_bytes(uart, &write_header.file_uuid, sizeof(write_header.file_uuid));
    utils_receive_bytes(uart, &write_header.file_length, sizeof(write_header.file_length));

    return write_header;
}

void message_write_response(message_header_t header) {

    // // Reset AES
    // DL_AESADV_enablePower(AESADV);
    // while(!DL_AESADV_isInputContextWriteable(AESADV));

    // Start recieving write command
    message_header_send_ack(HOST_INST);

    // Parse Write Header
    message_write_header_t write_header = message_write_parse_header(HOST_INST);




    // 32bit alignment
    __attribute__((aligned(4)))
    uint8_t file_buffer[MAX_FILE_SIZE];

    async_uart_ctx uart_ctx = {
        .uart = HOST_INST,
        .dma_channel = DMA_CHANNEL,
        .first_chunk_offset = WRITE_HEADER_SIZE,
        .total_bytes = write_header.file_length,
        .buffer = file_buffer,
    };
    int async_status = async_uart_receive(&uart_ctx);

    if(async_status != 0) {
        const char msg[] = "DMA Error";
        uart_ctx.stop = true;
        while(uart_ctx.stopped != true);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    if(write_header.file_length == 0 || write_header.file_length > MAX_FILE_SIZE) {
        const char msg[] = "Invalid File Size";
        uart_ctx.stop = true;
        while(uart_ctx.stopped != true);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    bool pin_valid = utils_verify_pin(write_header.pin, sizeof(write_header.pin));

    utils_random_delay();

    if(!pin_valid) {
        const char msg[] = "Invalid PIN";
    
        delay_cycles(PIN_DELAY);
        uart_ctx.stop = true;
        while(uart_ctx.stopped != true);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    // Find Group
    const group_t* group = utils_find_group(write_header.group_id);

    if(group == 0 || !group->private.permissions.write) {
        char msg[] = "HSM not provisioned for group";
        uart_ctx.stop = true;
        while(uart_ctx.stopped != true);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    if(write_header.slot > NUM_SLOTS) {
        char msg[] = "Invalid Slot Number";
        uart_ctx.stop = true;
        while(uart_ctx.stopped != true);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    // Erase Flash
    for(size_t i = 0; i < sizeof(file_slot_entry_t); i += FLASH_SECTOR_SIZE) {
        uint32_t address = ((uint32_t) &(SLOTS[write_header.slot])) + i;

        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_eraseMemory(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }

    file_address_table_t fat;
    file_read_fat(&fat);

    fat.entries[write_header.slot].address = (uint32_t) &SLOTS[write_header.slot];
    fat.entries[write_header.slot].length = sizeof(file_slot_entry_t);
    memcpy(
        fat.entries[write_header.slot].uuid,
        write_header.file_uuid,
        sizeof(uint8_t[16])
    );

    file_write_fat(&fat);

    // Construct Metadata
    file_metadata_signed_t signed_metadata;
    signed_metadata.metadata.group_id = write_header.group_id;
    signed_metadata.metadata.file_size = write_header.file_length;
    memcpy(
        signed_metadata.metadata.name,
        write_header.name,
        sizeof(char[32])
    );
    memcpy(
        signed_metadata.metadata.uuid,
        write_header.file_uuid,
        sizeof(uint8_t[16])
    );

    // Calculate Padding
    uint8_t padding = 16 - write_header.file_length % 16;
    if(padding > 15) {
        padding = 0;
    }

    // 32bit for alignment purposes
    uint32_t keys[3][8];

    uint8_t* raw_secret = (uint8_t*) keys[0];
    uint8_t* rand_pub_key = (uint8_t*) keys[1];
    uint8_t* read_pub_key = (uint8_t*) keys[2];

    memcpy(read_pub_key, group->public.read_key, sizeof(uint8_t[32]));

    // 32bit for alignment purposes
    uint32_t secret[8];
    random_fill_buffer((uint8_t*) secret, sizeof(uint8_t[32]));
    
    // Generate x25519 vals
    crypto_x25519_public_key(rand_pub_key, (uint8_t*) secret);
    crypto_x25519(raw_secret, (uint8_t*) secret, read_pub_key);

    crypto_blake2b((uint8_t*) secret, sizeof(secret), (uint8_t*) keys, sizeof(keys));

    // Move key into AES
    DL_AESADV_setKeyAligned(AESADV, secret, DL_AESADV_KEY_SIZE_256_BIT);

    // Wipe Secrets
    crypto_wipe(secret, sizeof(secret));
    crypto_wipe(keys[0], sizeof(keys[0]));

    // Copy Rand public key into metadata
    memcpy(signed_metadata.metadata.encryption_public_key, rand_pub_key, sizeof(uint8_t[32]));

    // We don't reuse keys so 0 iv
    uint32_t iv[4] = {};
    DL_AESADV_Config aes_config = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_ENCRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .iv = (uint8_t*) &iv,
        .upperCryptoLength = 0,
        .lowerCryptoLength = write_header.file_length + padding,
        .aadLength = 0,
    };
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config);

    for(size_t i = 0; i < padding; i++) {
        file_buffer[write_header.file_length + i] = 0;
    }

    while(!uart_ctx.transfer_complete);

    // Encrypt and write file
    uint32_t aes_blocks = (write_header.file_length + padding) / 16;
    for(size_t i = 0; i < aes_blocks; i++) {
        while(!DL_AESADV_isInputReady(AESADV)) {}
        DL_AESADV_loadInputDataAligned(AESADV, (uint32_t*) &file_buffer[16 * i]);

        uint32_t aes_output[4];

        while(!DL_AESADV_isOutputReady(AESADV)) {}
        DL_AESADV_readOutputDataAligned(AESADV, aes_output);

        uint32_t address = (uint32_t) &(SLOTS[write_header.slot].encrypted_file[16 * i]);
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, address, aes_output);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);

        address += 8;
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, address, aes_output + 2);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }

    // Wait for TAG
    while(!DL_AESADV_isSavedOutputContextReady(AESADV));
    DL_AESADV_readTAGAligned(AESADV, signed_metadata.metadata.file_signature);

    // Sign File
    crypto_eddsa_sign(
        signed_metadata.writer_signature,
        group->private.write_key,
        (uint8_t*) &signed_metadata.metadata,
        sizeof(signed_metadata.metadata)
    );

    // Write File Metadata
    for(int i = 0; i < sizeof(signed_metadata); i += 8) {
        uint32_t address = (uint32_t) &(SLOTS[write_header.slot].signed_metadata) + i;
        uint32_t* data = (uint32_t*) ((uint32_t) &signed_metadata + i);
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, address, data);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }


    // DL_AESADV_disablePower(AESADV);

    header.message_length = 0;
    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

}
