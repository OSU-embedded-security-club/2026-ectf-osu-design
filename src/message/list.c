/*
Next steps:
1. Look into building the code to check for compiliation errors
2. Look into the testing service

*/

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ti_msp_dl_config.h"
#include "message/list.h"
#include "file.h"
#include "message/constants.h"

#define PIN_LENGTH 6

#define FLASH_STORAGE_START    0x00020000 
#define MAX_FILE_COUNT         20
#define EMPTY_FLASH_WORD       0xFFFFFFFF

#define MAX_PACKET_SIZE 256
#define UART_INST       UART_0_INST

// External functions ===================================================================

extern void wait_for_ack(void);
extern void utils_verify_pins(uint8_t* pin, size_t len);

// Helper functions ===================================================================

bool is_slot_in_use(uint8_t slot_num) {
    if (slot_num >= MAX_FILE_COUNT) return false;
    uint32_t address = FLASH_STORAGE_START + (slot_num * SLOT_SIZE);
    uint32_t *flash_ptr = (uint32_t *)address;
    return (*flash_ptr != EMPTY_FLASH_WORD);
}

int read_file_metadata(uint8_t slot_num, file_metadata_t *out_metadata) {
    if (slot_num >= MAX_FILE_COUNT || out_metadata == NULL) {
        return -1;
    }
    if (!is_slot_in_use(slot_num)) {
        return -1;
    }

    uint32_t address = FLASH_STORAGE_START + (slot_num * SLOT_SIZE);

    // read flash
    memcpy(out_metadata, (void *)address, sizeof(file_metadata_t));

    return 0;
}

void generate_list_files(list_response_t file_list) {
    file_list->n_files = 0;
    file_metadata_t temp_meta; // Temporary storage on the stack

    for (uint8_t i = 0; i < MAX_FILE_COUNT; i++) {
        if (is_slot_in_use(i)) {
            if (read_file_metadata(i, &temp_meta) == 0) {
                file_list->metadata[file_list->n_files].slot = i;
                file_list->metadata[file_list->n_files].group_id = temp_meta.group_id;
                
                // Safe string copy to prevent buffer overflows
                memset(file_list->metadata[file_list->n_files].name, 0, 32);
                strncpy(file_list->metadata[file_list->n_files].name, temp_meta.name, 31);
                
                file_list->n_files++;
            }

            // clear temp memory
            memset(&temp_meta, 0, sizeof(file_metadata_t));

            // stop reading at max
            if (file_list->n_files >= MAX_RESPONSE_FILES) break;
        }
    }
}

// Communication functions ===================================================================

void send_byte_with_flow_control(uint8_t byte, uint32_t *total_bytes_sent) {
    DL_UART_transmitDataBlocking(UART_INST, byte);
    
    (*total_bytes_sent)++;
    if ((*total_bytes_sent) % MAX_PACKET_SIZE == 0) {
        // handle ACK feature
        wait_for_ack();
    }
}

void write_packet(uint8_t msg_type, void* data, uint16_t len) {
    uint8_t *payload = (unint8_t *)data;
    uint8_t checksum = 0;
    uint32_t bytes_sent_counter = 0;

    // create packet header
    uint8_t len_low = (uint8_t)(len & 0xFF);
    uint8_t len_high = (uint8_t)((len >> 8) & 0xFF);

    // send header
    send_byte_with_flow_control(len_low, &bytes_sent_counter);
    send_byte_with_flow_control(len_high, &bytes_sent_counter);
    send_byte_with_flow_control(msg_type, &bytes_sent_counter);

    // calculate header checksum
    checksum ^= len_low;
    checksum ^= len_high;
    checksum ^= msg_type;

    // send payload
    for (uint16_t i = 0; i < len; i++) {
        send_byte_with_flow_control(payload[i], &bytes_sent_counter);
        checksum ^= payload[i];
    }

    // send checksum
    send_byte_with_flow_control(checksum, &bytes_sent_counter);
}

// Main function ===================================================================

void message_list_response(message_header_t header) {
    file_metadata_t file_list;
    
    // clear stale memory
    memset(&file_list, 0, sizeof(file_list));

    // check pin
    if (!utils_verify_pin(command->pin, 6)) {
        // time penalty delay: 5 seconds
        for(int i = 0; i < 5; i++) {
            delay_cycles(32000000);
        }
        
        print_error("Error.");
        return;
    }

    // generate file list
    generate_list_files(file_list);

    // print metadata of all files
    pkt_len_t length = LIST_PKT_LEN(file_list.n_files);
    write_packet(LIST_MSG, &file_list, length);

    // clear file list
    memset(&file_list, 0, sizeof(file_list));
}
