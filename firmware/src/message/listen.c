#include "message/listen.h"

#include "random.h"
#include "file.h"
#include "message/interrogate.h"
#include "utils.h"

#include <string.h>
#include <monocypher.h>

#define DMA_CHANNEL 0

/**
 * The listen is other end of the [recieve command](@ref message_recieve)
 * and the [interrogate command](@ref message_interrogate). The listen command
 * starts by recieving a header from the remote HSM and then based on the header
 * opcode the listen command provides a response in accordance to the
 * [recieve command definition](@ref message_recieve) or the
 * [interrogate command definition](@ref message_interrogate).
 */
void message_listen(message_header_t header) {
    DL_AESADV_enablePower(AESADV);

    message_header_send_ack(HOST_INST);

    uint32_t keys[3][8];
    uint32_t aes_key[8];

    random_fill_buffer((uint8_t*) aes_key, sizeof(uint8_t[32]));
    crypto_x25519_public_key((uint8_t*) keys[2], (uint8_t*) aes_key);

    message_header_t hsm_header;
    message_header_request(HSM_INST, &hsm_header);

    if(hsm_header.operation == MESSAGE_INTERROGATE) {
        message_interrogate_group_list_encrypted_t group_list_encrypted;
        message_header_send_ack(HSM_INST);
        utils_receive_bytes(HSM_INST, &group_list_encrypted, sizeof(group_list_encrypted));
        message_header_send_ack(HSM_INST);

        DL_AESADV_Config aes_config_decrypt = {
            .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
            .direction = DL_AESADV_DIR_DECRYPT,
            .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
            .upperCryptoLength = 0,
            .lowerCryptoLength = sizeof(message_interrogate_group_list_t),
            .aadLength = 0,
        };
        DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
        DL_AESADV_setKeyAligned(AESADV, (uint32_t*) TRANSFER_KEY, DL_AESADV_KEY_SIZE_256_BIT);
        DL_AESADV_loadInitializationVectorAligned(AESADV, group_list_encrypted.iv);
        DL_AESADV_enableSavedOutputContext(AESADV);
        DL_AESADV_initGCM(AESADV, &aes_config_decrypt);
       
        message_interrogate_group_list_t* group_list = &group_list_encrypted.group_list;

        // Decrypt the Struct
        size_t aes_blocks = sizeof(*group_list) / 16;
        for(size_t i = 0; i < aes_blocks; i++) {
            uint32_t* data = (uint32_t*) group_list + 4 * i;
            while(!DL_AESADV_isInputReady(AESADV));
            DL_AESADV_loadInputDataAligned(AESADV, data);
            while(!DL_AESADV_isOutputReady(AESADV));
            DL_AESADV_readOutputDataAligned(AESADV, data);
        }

        uint32_t local_signature[4];
        while(!DL_AESADV_isSavedOutputContextReady(AESADV));
        DL_AESADV_readTAGAligned(AESADV, local_signature);

        int verified = crypto_verify16((uint8_t*) local_signature, (uint8_t*) group_list_encrypted.tag);

        utils_random_delay();

        if(verified != 0) {
            char msg[] = "File Info Corrupted";
            message_header_send_error(HOST_INST, msg, sizeof(msg));
            return;
        }

       
        message_interrogate_file_info_encrypted_t file_info_encrypted;
        message_interrogate_file_info_t* file_info = &file_info_encrypted.file_info;
        file_info->num_files = 0;

         // Find all matching files
        for(uint16_t i = 0; i < NUM_SLOTS; i++) {
            for(size_t j = 0; j < group_list->num_groups; j++) {
                if(SLOTS[i].signed_metadata.metadata.group_id == group_list->group_ids[j]) {
                    message_interrogate_file_metadata_t* metadata = &file_info->files[file_info->num_files];
                    memcpy(&(metadata->slot), &i, sizeof(metadata->slot));
                    memcpy(&(metadata->group_id), &(SLOTS[i].signed_metadata.metadata.group_id), sizeof(metadata->group_id));
                    memcpy(metadata->name, SLOTS[i].signed_metadata.metadata.name, sizeof(uint8_t[32]));
                    file_info->num_files++;
                }
            }
        }       

        // Random Fill IV
        random_fill_buffer((uint8_t*) file_info_encrypted.iv, sizeof(file_info_encrypted.iv));

        DL_AESADV_Config aes_config_encrypt = {
            .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
            .direction = DL_AESADV_DIR_ENCRYPT,
            .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
            .upperCryptoLength = 0,
            .lowerCryptoLength = sizeof(message_interrogate_file_info_t),
            .aadLength = 0,
        };
        DL_AESADV_loadInitializationVectorAligned(AESADV, file_info_encrypted.iv);
        DL_AESADV_enableSavedOutputContext(AESADV);
        DL_AESADV_initGCM(AESADV, &aes_config_encrypt);



        // Encrypt the Struct
        aes_blocks = sizeof(*file_info) / 16;
        for(size_t i = 0; i < aes_blocks; i++) {
            uint32_t* data = (uint32_t*) file_info + 4 * i;
            while(!DL_AESADV_isInputReady(AESADV));
            DL_AESADV_loadInputDataAligned(AESADV, data);
            while(!DL_AESADV_isOutputReady(AESADV));
            DL_AESADV_readOutputDataAligned(AESADV, data);
        }
       
        while(!DL_AESADV_isSavedOutputContextReady(AESADV));
        DL_AESADV_readTAGAligned(AESADV, file_info_encrypted.tag);


        message_header_t response_header = {
            .operation = MESSAGE_INTERROGATE,
            .message_length = 0,
        };

        message_header_response(HSM_INST, response_header);
        message_header_receive_ack(HSM_INST);

        utils_send_buffer_no_ack(HSM_INST, &file_info_encrypted, sizeof(file_info_encrypted));

        message_header_receive_ack(HSM_INST);
    } else if (hsm_header.operation == MESSAGE_RECEIVE) {
        message_header_send_ack(HSM_INST);
        uint8_t slot = DL_UART_receiveDataBlocking(HSM_INST);
        message_header_send_ack(HSM_INST);

        utils_random_delay();

        if(slot > NUM_SLOTS) {
            hsm_header.operation = MESSAGE_ERROR;
        } else if (SLOTS[slot].signed_metadata.metadata.file_size > MAX_FILE_SIZE) {
            hsm_header.operation = MESSAGE_ERROR;
        }

        // Setup DMA Transfer into RAM
        file_slot_entry_t file_entry = {};
        DL_DMA_Config dma_config = {
            .trigger = 0,
            .triggerType = DL_DMA_TRIGGER_TYPE_EXTERNAL,
            .transferMode = DL_DMA_SINGLE_BLOCK_TRANSFER_MODE,
            .extendedMode = DL_DMA_NORMAL_MODE,
            .srcWidth = DL_DMA_WIDTH_BYTE,
            .destWidth = DL_DMA_WIDTH_BYTE,
            .srcIncrement = DL_DMA_ADDR_INCREMENT,
            .destIncrement = DL_DMA_ADDR_INCREMENT,
        };
        DL_DMA_initChannel(DMA, DMA_CHANNEL, &dma_config);
        DL_DMA_setTransferSize(DMA, DMA_CHANNEL, sizeof(file_entry));
        DL_DMA_setSrcAddr(DMA, DMA_CHANNEL, (uint32_t) &SLOTS[slot]);
        DL_DMA_setDestAddr(DMA, DMA_CHANNEL, (uint32_t) &file_entry);

        DL_DMA_enableChannel(DMA, DMA_CHANNEL);
        DL_DMA_startTransfer(DMA, DMA_CHANNEL);

        hsm_header.message_length = 0;
        message_header_response(HSM_INST, hsm_header);
        message_header_receive_ack(HSM_INST);

        utils_random_delay();

        if(hsm_header.operation != MESSAGE_RECEIVE) {
            char msg[] = "File not Found";
            message_header_send_error(HOST_INST, msg, sizeof(msg));
            return;
        }
        
        const group_t* group = utils_find_group(SLOTS[slot].signed_metadata.metadata.group_id);

        if(group == 0) {
            const char msg[] = "Group not Found";
            message_header_send_error(HOST_INST, msg, sizeof(msg));
            return;
        }

        // Send Public Key
        utils_send_buffer_no_ack(HSM_INST, &(group->group_id), sizeof(group->group_id));
        utils_send_buffer_no_ack(HSM_INST, keys[2], sizeof(keys[2]));

        // Copy transfer public key to buffer
        memcpy(keys[1], group->public.transfer_key, sizeof(uint8_t[32]));

        crypto_x25519((uint8_t*) keys[0], (uint8_t*) aes_key, (uint8_t*) keys[1]);
        crypto_blake2b((uint8_t*) aes_key, sizeof(aes_key), (uint8_t*) keys, sizeof(keys));

        DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
        DL_AESADV_setKeyAligned(AESADV, aes_key, DL_AESADV_KEY_SIZE_256_BIT);

        crypto_wipe(aes_key, sizeof(aes_key));
        crypto_wipe(keys[0], sizeof(keys[0]));

        uint32_t iv[4];
        random_fill_buffer((uint8_t*) iv, sizeof(iv));

        DL_AESADV_Config aes_config = {
            .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
            .direction = DL_AESADV_DIR_ENCRYPT,
            .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
            .upperCryptoLength = 0,
            .lowerCryptoLength = sizeof(file_slot_entry_t),
            .aadLength = 0,
        };
        DL_AESADV_loadInitializationVectorAligned(AESADV, iv);
        DL_AESADV_enableSavedOutputContext(AESADV);
        DL_AESADV_initGCM(AESADV, &aes_config); 

        while(DL_DMA_isChannelEnabled(DMA, DMA_CHANNEL));

        size_t aes_blocks = sizeof(file_entry) / 16;
        for(size_t i = 0; i < aes_blocks; i++) {
            uint32_t* data = (uint32_t*) &file_entry + 4 * i;
            while(!DL_AESADV_isInputReady(AESADV));
            DL_AESADV_loadInputDataAligned(AESADV, data);
            while(!DL_AESADV_isOutputReady(AESADV));
            DL_AESADV_readOutputDataAligned(AESADV, data);
        }

        uint32_t tag[4];
        while(!DL_AESADV_isSavedOutputContextReady(AESADV));
        DL_AESADV_readTAGAligned(AESADV, tag);

        message_header_receive_ack(HSM_INST);

        utils_send_buffer_no_ack(HSM_INST, iv, sizeof(iv));
        utils_send_buffer_no_ack(HSM_INST, &file_entry, sizeof(file_entry));
        utils_send_buffer_no_ack(HSM_INST, &tag, sizeof(tag));

        message_header_receive_ack(HSM_INST);
    }

    DL_AESADV_disablePower(AESADV);

    // Send Response to Host
    header.message_length = 0;
    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

}
