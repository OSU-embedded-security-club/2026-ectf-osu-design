#include <string.h>
#include "ti_msp_dl_config.h"
#include "message/list.h"
#include "file.h"
#include "message/constants.h"

#define PIN_LENGTH 6

#define FLASH_STORAGE_START    0x00020000 
#define MAX_FILE_COUNT         20
#define EMPTY_FLASH_WORD       0xFFFFFFFF

bool is_slot_in_use(uint8_t slot_num) {
    if (slot_num >= MAX_FILE_COUNT) return false;
    uint32_t address = FLASH_STORAGE_START + (slot_num * FLASH_SECTOR_SIZE);
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

void generate_list_files(list_response_t *file_list) {
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

void message_list_response(message_header_t header) {
    list_command_t *command = (list_command_t*)buf;
    file_metadata_t file_list;
    
    // clear stale memory
    memset(&file_list, 0, sizeof(file_list));

    // check pin
    if (!utils_verify_pin(command->pin, 6)) {
        // time penalty delay
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
    write_packet(CONTROL_INTERFACE, LIST_MSG, &file_list, length);

    // clear file list
    memset(&file_list, 0, sizeof(file_list));
}

// mitchel will implement the write_packet
// 256 bytes there is an awk packet
// write out 8 groups - 256 bytes
//  could do something different for the eight group