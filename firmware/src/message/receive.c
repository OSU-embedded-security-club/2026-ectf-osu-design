#include "message/receive.h"

#include "message/header.h"
#include "file.h"
#include "utils.h"

#include <string.h>
#include <monocypher.h>

/**
 * Our recieve design uses asymetric encryption to ensure secrecy.
 * Instead of trying to verify that an HSM can is allowed to decrypt
 * a file we designed our system where we encrypt the file so it can only
 * be decrypted by an HSM that has the private transfer key for the file group.
 * We accomplish this by generating the AES secret key from a randomly generated
 * ED25519 keypair and the public transfer key that every HSM is provisioned with.
 * We then send the public key from the generated keypair to the recieving HSM so
 * that the secret can be regenerated with the private transfer key. We transfer the
 * entire file_slot_entry_t as it is one less point of failure in our design.
 *
 * @plantumlfile recieve.pu
*/
void message_recieve(message_header_t header) {
    DL_AESADV_enablePower(AESADV);

    message_header_send_ack(HOST_INST);

    uint8_t pin[6];
    utils_receive_bytes(HOST_INST, pin, sizeof(pin));

    uint8_t read_slot = DL_UART_receiveDataBlocking(HOST_INST);
    uint8_t write_slot = DL_UART_receiveDataBlocking(HOST_INST);

    message_header_send_ack(HOST_INST);

    bool pin_valid = utils_verify_pin(pin, sizeof(pin));

    utils_random_delay();

    if(!pin_valid) {
        const char msg[] = "Invalid PIN";

        delay_cycles(PIN_DELAY);
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    if(read_slot > NUM_SLOTS || write_slot > NUM_SLOTS) {
        char msg[] = "Invalid Slot Number";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    message_header_t hsm_header = {
        .operation = MESSAGE_RECEIVE,
        .message_length = 0
    };

    message_header_response(HSM_INST, hsm_header);
    message_header_receive_ack(HSM_INST);

    DL_UART_transmitDataBlocking(HSM_INST, read_slot);
    message_header_receive_ack(HSM_INST);

    message_header_request(HSM_INST, &hsm_header);
    message_header_send_ack(HSM_INST);

    if(hsm_header.operation != MESSAGE_RECEIVE) {
        char msg[] = "File not Found";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    uint16_t group_id;
    uint32_t keys[3][8];
    uint32_t aes_key[8];

    utils_receive_bytes(HSM_INST, &group_id, sizeof(group_id));
    utils_receive_bytes(HSM_INST, keys[2], sizeof(keys[2]));

    const group_t* group = utils_find_group(group_id);

    utils_random_delay();

    if(group == 0) {
        const char msg[] = "Group not Found";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    if(!group->private.permissions.transfer) {
        const char msg[] = "HSM doens't have transfer permission";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }

    memcpy(keys[1], group->public.transfer_key, sizeof(uint8_t[32]));

    crypto_x25519((uint8_t*) keys[0], group->private.transfer_key, (uint8_t*) keys[2]);
    crypto_blake2b((uint8_t*) aes_key, sizeof(aes_key), (uint8_t*) keys, sizeof(keys));

    DL_AESADV_setKeySize(AESADV, DL_AESADV_KEY_SIZE_256_BIT);
    DL_AESADV_setKeyAligned(AESADV, aes_key, DL_AESADV_KEY_SIZE_256_BIT);

    crypto_wipe(aes_key, sizeof(aes_key));
    crypto_wipe(keys[0], sizeof(keys[0]));

    uint32_t iv[4];
    file_slot_entry_t file_entry;
    uint32_t tag[4];

    message_header_send_ack(HSM_INST);

    utils_receive_bytes(HSM_INST, iv, sizeof(iv));
    utils_receive_bytes(HSM_INST, &file_entry, sizeof(file_entry));
    utils_receive_bytes(HSM_INST, tag, sizeof(tag));

    message_header_send_ack(HSM_INST);

    DL_AESADV_Config aes_config = {
        .mode = DL_AESADV_MODE_GCM_AUTONOMOUS,
        .direction = DL_AESADV_DIR_DECRYPT,
        .ctr_ctrWidth = DL_AESADV_CTR_WIDTH_96_BIT,
        .upperCryptoLength = 0,
        .lowerCryptoLength = sizeof(file_slot_entry_t),
        .aadLength = 0,
    };
    DL_AESADV_loadInitializationVectorAligned(AESADV, iv);
    DL_AESADV_enableSavedOutputContext(AESADV);
    DL_AESADV_initGCM(AESADV, &aes_config); 

    size_t aes_blocks = sizeof(file_entry) / 16;
    for(size_t i = 0; i < aes_blocks; i++) {
        uint32_t* data = (uint32_t*) &file_entry + 4 * i;
        while(!DL_AESADV_isInputReady(AESADV));
        DL_AESADV_loadInputDataAligned(AESADV, data);
        while(!DL_AESADV_isOutputReady(AESADV));
        DL_AESADV_readOutputDataAligned(AESADV, data);
    }

    uint32_t local_tag[4];
    while(!DL_AESADV_isSavedOutputContextReady(AESADV));
    DL_AESADV_readTAGAligned(AESADV, local_tag);

    utils_random_delay();

    int verified = crypto_verify16((uint8_t*) local_tag, (uint8_t*) tag);

    utils_random_delay();

    if(verified != 0) {
        char msg[] = "File Info Corrupted";
        message_header_send_error(HOST_INST, msg, sizeof(msg));
        return;
    }
    
    file_address_table_t fat;
    file_read_fat(&fat);
    fat.entries[write_slot].address = (uint32_t) &SLOTS[write_slot];
    fat.entries[write_slot].length = sizeof(file_slot_entry_t);
    memcpy(fat.entries[write_slot].uuid, file_entry.signed_metadata.metadata.uuid, sizeof(uint8_t[16]));
    file_write_fat(&fat);

    for(size_t i = 0; i < sizeof(file_slot_entry_t) / FLASH_SECTOR_SIZE; i++) {
        uint32_t address = (uint32_t) &SLOTS[write_slot] + FLASH_SECTOR_SIZE * i;
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_eraseMemory(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }

    for(size_t i = 0; 8 * i < sizeof(file_slot_entry_t); i++) {
        uint32_t address = (uint32_t) &SLOTS[write_slot] + 8 * i;
        uint32_t* data = (uint32_t*) ((uint8_t*) &file_entry + 8 * i);

        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, address, data);
        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }

    header.message_length = 0;
    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

    DL_AESADV_disablePower(AESADV);
}
