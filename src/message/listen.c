#include "message/listen.h"

#include "random.h"
#include "file.h"
#include "message/interrogate.h"
#include "utils.h"

#include <monocypher.h>

/**
 * @brief Handles Listen Command
 * 
 * @param header 
 */
void message_listen(message_header_t header) {
    DL_AESADV_enablePower(AESADV);

    message_header_send_ack(HOST_INST);

    uint32_t keys[3][8];
    uint32_t aes_key[16];

    random_fill_buffer((uint8_t*) aes_key, sizeof(uint8_t[32]));
    crypto_x25519_public_key((uint8_t*) keys[2], (uint8_t*) aes_key);

    message_header_t hsm_header;
    message_header_request(HSM_INST, &hsm_header);
    message_header_send_ack(HSM_INST);

    if(hsm_header.operation == MESSAGE_INTERROGATE) {
        message_interrogate_verify_t verify;
        utils_receive_bytes(HSM_INST, &verify, sizeof(verify));
        message_header_send_ack(HSM_INST);

        // Copy Public Key into array
        memcpy(keys[1], verify.public_key, sizeof(uint8_t[32]));

        crypto_x25519((uint8_t*) keys[0], (uint8_t*) aes_key, (uint8_t*) keys[1]);
        crypto_blake2b((uint8_t*) aes_key, sizeof(aes_key), (uint8_t*) keys, sizeof(keys));

        DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
        DL_AESADV_setKeyAligned(AESADV, aes_key, DL_AESADV_KEY_SIZE_256_BIT);

        crypto_wipe(aes_key, sizeof(aes_key));
        crypto_wipe(keys[0], sizeof(keys[0]));

        // We don't reuse keys so 0 iv
        uint32_t iv[4] = {};
        DL_AESADV_Config aes_config = {
            .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
            .direction = DL_AESADV_DIR_ENCRYPT,
            .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
            .iv = (uint8_t*) &iv,
            .upperCryptoLength = 0,
            .lowerCryptoLength = sizeof(message_interrogate_file_info_t),
            .aadLength = 0,
        };
        DL_AESADV_enableSavedOutputContext(AESADV);
        DL_AESADV_initGCM(AESADV, &aes_config);

        // Check Recieve Signatures
        bool hsm_verified = true;
        for(size_t i = 0; i < verify.num_groups; i++) {
            const group_t* group = utils_find_group(verify.group_ids[i]);
            int verified = crypto_eddsa_check(
                verify.signatures[i],
                groups->public.transfer_key,
                (uint8_t*) verify.public_key,
                sizeof(verify.public_key)
            );

            if(verified != 0) {
                hsm_verified = false;
            }
        }


        if(!hsm_verified) {
            char msg[] = "Signature Verification Failed";
            message_header_send_error(HSM_INST, msg, sizeof(msg));
            message_header_send_error(HOST_INST, msg, sizeof(msg));
            return;
        }

        message_header_t response_header = {
            .operation = MESSAGE_INTERROGATE,
            .message_length = 0,
        };

        message_header_response(HSM_INST, response_header);
        message_header_receive_ack(HSM_INST);

        message_interrogate_file_info_t file_info;

        // Find all matching files
        for(uint16_t i = 0; i < NUM_SLOTS; i++) {
            for(size_t j = 0; j < verify.num_groups; j++) {
                if(SLOTS[i].signed_metadata.metadata.group_id == verify.group_ids[j]) {
                    message_interrogate_file_metadata_t* metadata = &file_info.files[file_info.num_files];
                    memcpy(&(metadata->slot), &i, sizeof(metadata->slot));
                    memcpy(&(metadata->group_id), &(SLOTS[i].signed_metadata.metadata.group_id), sizeof(metadata->group_id));
                    memcpy(metadata->name, SLOTS[i].signed_metadata.metadata.name, sizeof(uint8_t[32]));
                    file_info.num_files++;
                }
            }
        }

        // Encrypt the Struct
        size_t aes_blocks = sizeof(file_info) / 16;
        for(size_t i = 0; i < aes_blocks; i++) {
            uint32_t* data = (uint32_t*) &file_info + 4 * i;
            while(!DL_AESADV_isInputReady(AESADV));
            DL_AESADV_loadInputDataAligned(AESADV, data);
            while(!DL_AESADV_isOutputReady(AESADV));
            DL_AESADV_readOutputDataAligned(AESADV, data);
        }
       
        uint32_t signature[4];
        while(!DL_AESADV_isSavedOutputContextReady(AESADV));
        DL_AESADV_readTAGAligned(AESADV, signature);

        utils_send_buffer_no_ack(HSM_INST, keys[2], sizeof(keys[2]));
        utils_send_buffer_no_ack(HSM_INST, &file_info, sizeof(file_info));
        utils_send_buffer_no_ack(HSM_INST, signature, sizeof(signature));

        message_header_receive_ack(HSM_INST);
        
        // Send Response to Host
        header.message_length = 0;
        message_header_response(HOST_INST, header);
        message_header_receive_ack(HOST_INST);

        DL_AESADV_disablePower(AESADV);
    }
}
