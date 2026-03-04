#include "message/list.h"

#include "file.h"
#include "utils.h"

#include <string.h>

/**
 * List is fairly simple in function. We simply iterrate over
 * the file system checking if the length of each structure is in
 * less than the max file size and if it is less than the max file
 * size we know a file exists in that slot.
 */
void message_list_response(message_header_t header) {

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

    message_list_response_t response;
    response.num_files = 0;

    for(uint16_t i = 0; i < NUM_SLOTS; i++) {

        if(SLOTS[i].signed_metadata.metadata.file_size > MAX_FILE_SIZE) {
            continue;
        }

        response.entries[response.num_files].slot = i; 
        
        // Copy GroupID using bytes
        memcpy(
            &response.entries[response.num_files].group_id, 
            &SLOTS[i].signed_metadata.metadata.group_id, 
            sizeof(uint16_t)
        );

        memcpy(
            &response.entries[response.num_files].name,
            &SLOTS[i].signed_metadata.metadata.name,
            sizeof(char[32])
        );

        response.num_files++;
    }

    size_t response_size = sizeof(response.num_files) + response.num_files * sizeof(message_list_file_entry_t);

    header.message_length = response_size;

    message_header_response(HOST_INST, header);
    message_header_receive_ack(HOST_INST);

    utils_send_buffer(HOST_INST, &response, response_size, 0);

}
