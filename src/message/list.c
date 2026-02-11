#include "message/list.h"
#include "message/constants.h"

#define PIN_LENGTH 6
// temp constants
#define MAX_FILE_COUNT 10
#define MAX_RESPONSE_FILES 10

void generate_list_files(list_response_t *file_list) {
    file_list->n_files = 0;
    file_t temp_file;

    for (uint8_t i = 0; i < MAX_FILE_COUNT; i++) {
        if (is_slot_in_use(i)) {
            if (file_list->n_files >= MAX_RESPONSE_FILES) break;

            read_file(i, &temp_file);

            file_list->metadata[file_list->n_files].slot = i;
            file_list->metadata[file_list->n_files].group_id = temp_file.group_id;
            
            // Use strncpy or memcpy to prevent buffer overflows
            // metadata[].name size is assumed to be 16 or 32
            memset(file_list->metadata[file_list->n_files].name, 0, sizeof(file_list->metadata[i].name));
            strncpy(file_list->metadata[file_list->n_files].name, (char *)&temp_file.name, sizeof(file_list->metadata[i].name) - 1);
            
            file_list->n_files++;
        }
    }
    // clear temp buffer
    memset(&temp_file, 0, sizeof(temp_file));
}

void message_list_response(message_header_t header) {
    list_command_t *command = (list_command_t*)buf;
    list_response_t file_list;
    
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
    secure_generate_list_files(&file_list);

    pkt_len_t length = LIST_PKT_LEN(file_list.n_files);
    write_packet(CONTROL_INTERFACE, LIST_MSG, &file_list, length);

    // clear file list
    memset(&file_list, 0, sizeof(file_list));
}