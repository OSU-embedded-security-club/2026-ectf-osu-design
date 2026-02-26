#include "message/interrogate.h"

#include "secrets.h"
#include "group.h"
#include "random.h"
#include "utils.h"

#include <monocypher.h>

void message_interrogate(message_header_t header) {
    DL_AESADV_enablePower(AESADV);

    message_header_send_ack(HOST_INST);
    uint8_t pin[6];
    utils_receive_bytes(HOST_INST, pin, sizeof(pin));
    message_header_send_ack(HOST_INST);

    // TODO: Check Pin

    uint32_t secret_key[16];
    uint32_t keys[3][8];

    random_fill_buffer((uint8_t*) secret_key, sizeof(uint8_t[32]));
    crypto_x25519_public_key((uint8_t*) keys[1], (uint8_t*) secret_key);

    message_interrogate_verify_t verify;
    verify.num_groups = 0;
    memcpy(verify.public_key, keys[1], sizeof(uint8_t[32]));

    // Send group_id and signature for every transfer group
    for(size_t i = 0; i < sizeof(groups) / sizeof(group_t); i++) {
        if(groups[i].private.permissions.transfer) {
            verify.group_ids[verify.num_groups] = groups[i].group_id;
            crypto_eddsa_sign(verify.signatures[i], groups[i].private.transfer_key, (uint8_t*) keys[1], sizeof(keys[1]));
            verify.num_groups++;
        }
    }

    message_header_t hsm_header = {
        .operation = MESSAGE_INTERROGATE,
        .message_length = sizeof(verify),
    };
    message_header_response(HSM_INST, hsm_header);
    message_header_receive_ack(HSM_INST);

    // Send Verify Struct
    utils_send_buffer_no_ack(HSM_INST, &verify, sizeof(verify));
    message_header_receive_ack(HSM_INST);

    message_header_t incoming_header;
    message_header_request(HSM_INST, &incoming_header);
    
    if(incoming_header.operation == MESSAGE_ERROR) {
        char msg[] = "Signature Check Failed";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }
    message_header_send_ack(HSM_INST);

    message_interrogate_file_info_t file_info;
    uint32_t signature[4];

    // Receive Public Key
    utils_receive_bytes(HSM_INST, keys[2], sizeof(keys[2]));
    utils_receive_bytes(HSM_INST, &file_info, sizeof(file_info));
    utils_receive_bytes(HSM_INST, signature, sizeof(signature));

    message_header_send_ack(HSM_INST);

    // Setup Encryption
    crypto_x25519((uint8_t*) keys[0], (uint8_t*) secret_key, (uint8_t*) keys[2]);
    crypto_blake2b((uint8_t*) secret_key, sizeof(secret_key), (uint8_t*) keys, sizeof(keys));

    DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
    DL_AESADV_setKeyAligned(AESADV, secret_key, DL_AESADV_KEY_SIZE_256_BIT);

    crypto_wipe(secret_key, sizeof(secret_key));
    crypto_wipe(keys[0], sizeof(keys[0]));

    // We don't reuse keys so 0 iv
    uint32_t iv[4] = {};
    DL_AESADV_Config aes_config = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_DECRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .iv = (uint8_t*) &iv,
        .upperCryptoLength = 0,
        .lowerCryptoLength = sizeof(message_interrogate_file_info_t),
        .aadLength = 0,
    };
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config);

    // Decrypt the Struct
    size_t aes_blocks = sizeof(file_info) / 16;
    for(size_t i = 0; i < aes_blocks; i++) {
        uint32_t* data = (uint32_t*) &file_info + 4 * i;
        while(!DL_AESADV_isInputReady(AESADV));
        DL_AESADV_loadInputDataAligned(AESADV, data);
        while(!DL_AESADV_isOutputReady(AESADV));
        DL_AESADV_readOutputDataAligned(AESADV, data);
    }

    uint32_t local_signature[4];
    while(!DL_AESADV_isSavedOutputContextReady(AESADV));
    DL_AESADV_readTAGAligned(AESADV, local_signature);
    int verified = crypto_verify16((uint8_t*) local_signature, (uint8_t*) signature);

    // TODO: Insert Random Wait
    if(verified != 0) {
        char msg[] = "File Info Corrupted";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    header.message_length = sizeof(uint32_t) + 35 * file_info.num_files;
    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

    utils_send_buffer_no_ack(HOST_INST, &file_info.num_files, sizeof(file_info.num_files));
    utils_send_buffer(HOST_INST, file_info.files, file_info.num_files * sizeof(message_interrogate_file_metadata_t), sizeof(file_info.num_files));

    DL_AESADV_disablePower(AESADV);
}
