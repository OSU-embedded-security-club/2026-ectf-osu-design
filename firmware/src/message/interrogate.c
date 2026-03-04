#include "message/interrogate.h"

#include "message/header.h"
#include "secrets.h"
#include "group.h"
#include "random.h"
#include "secrets.h"
#include "utils.h"

#include <monocypher.h>

void message_interrogate(message_header_t header) {
    DL_AESADV_enablePower(AESADV);

    message_header_send_ack(HOST_INST);
    uint8_t pin[6];
    utils_receive_bytes(HOST_INST, pin, sizeof(pin));
    message_header_send_ack(HOST_INST);

    bool pin_valid = utils_verify_pin(pin, sizeof(pin));

    utils_random_delay();

    if(!pin_valid) {
        const char msg[] = "Invalid PIN";
    
        delay_cycles(PIN_DELAY);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    message_interrogate_group_list_encrypted_t encrypted_group_list;
    message_interrogate_group_list_t* group_list = &encrypted_group_list.group_list;
    group_list->num_groups = 0;

    // Send group_id and signature for every transfer group
    for(size_t i = 0; i < sizeof(groups) / sizeof(group_t); i++) {
        if(groups[i].private.permissions.transfer) {
            group_list->group_ids[group_list->num_groups] = groups[i].group_id;
            group_list->num_groups++;
        }
    }

    message_header_t hsm_header = {
        .operation = MESSAGE_INTERROGATE,
        .message_length = sizeof(encrypted_group_list),
    };
    message_header_response(HSM_INST, hsm_header);
    message_header_receive_ack(HSM_INST);


    // Random IV
    random_fill_buffer((uint8_t*) encrypted_group_list.iv, sizeof(encrypted_group_list.iv));

    DL_AESADV_Config aes_config_encrypt = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_ENCRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .upperCryptoLength = 0,
        .lowerCryptoLength = sizeof(message_interrogate_group_list_t),
        .aadLength = 0,
    };
    DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
    DL_AESADV_setKeyAligned(AESADV, (uint32_t*) TRANSFER_KEY, DL_AESADV_KEY_SIZE_256_BIT);
    DL_AESADV_loadInitializationVectorAligned(AESADV, encrypted_group_list.iv);
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config_encrypt);

    // Encrypt the Struct
    size_t aes_blocks = sizeof(*group_list) / 16;
    for(size_t i = 0; i < aes_blocks; i++) {
        uint32_t* data = (uint32_t*) group_list + 4 * i;
        while(!DL_AESADV_isInputReady(AESADV));
        DL_AESADV_loadInputDataAligned(AESADV, data);
        while(!DL_AESADV_isOutputReady(AESADV));
        DL_AESADV_readOutputDataAligned(AESADV, data);
    }

    while(!DL_AESADV_isSavedOutputContextReady(AESADV));
    DL_AESADV_readTAGAligned(AESADV, encrypted_group_list.tag);

    // Send group list
    utils_send_buffer_no_ack(HSM_INST, &encrypted_group_list, sizeof(encrypted_group_list));
    message_header_receive_ack(HSM_INST);

    message_header_t incoming_header;
    message_header_request(HSM_INST, &incoming_header);
    message_header_send_ack(HSM_INST);

    if(incoming_header.operation != MESSAGE_INTERROGATE) {
        char msg[] = "Response Header Error";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    message_interrogate_file_info_encrypted_t file_info_encrypted;
    message_interrogate_file_info_t* file_info = &file_info_encrypted.file_info;

    utils_receive_bytes(HSM_INST, &file_info_encrypted, sizeof(file_info_encrypted));
    message_header_send_ack(HSM_INST);


    DL_AESADV_Config aes_config_decrypt = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_DECRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .upperCryptoLength = 0,
        .lowerCryptoLength = sizeof(message_interrogate_file_info_t),
        .aadLength = 0,
    };
    DL_AESADV_loadInitializationVectorAligned(AESADV, file_info_encrypted.iv);
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config_decrypt);

    // Decrypt the Struct
    aes_blocks = sizeof(*file_info) / 16;
    for(size_t i = 0; i < aes_blocks; i++) {
        uint32_t* data = (uint32_t*) file_info + 4 * i;
        while(!DL_AESADV_isInputReady(AESADV));
        DL_AESADV_loadInputDataAligned(AESADV, data);
        while(!DL_AESADV_isOutputReady(AESADV));
        DL_AESADV_readOutputDataAligned(AESADV, data);
    }

    uint32_t local_signature[4];
    while(!DL_AESADV_isSavedOutputContextReady(AESADV));
    DL_AESADV_readTAGAligned(AESADV, local_signature);


    int verified = crypto_verify16((uint8_t*) local_signature, (uint8_t*) file_info_encrypted.tag);

    utils_random_delay();

    // Check GCM tag
    if(verified != 0) {
        char msg[] = "File Info Corrupted";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    // Check for overflow
    if(file_info->num_files > NUM_SLOTS) {
        const char msg[] = "Overflow";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    header.message_length = sizeof(uint32_t) + 35 * file_info->num_files;
    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

    utils_send_buffer_no_ack(HOST_INST, &file_info->num_files, sizeof(file_info->num_files));
    utils_send_buffer(HOST_INST, file_info->files, file_info->num_files * sizeof(message_interrogate_file_metadata_t), sizeof(file_info->num_files));

    DL_AESADV_disablePower(AESADV);
}
