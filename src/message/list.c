#include "message/list.h"

#define PIN_LENGTH 6
#define MAX_FILES 10

/**
 * @brief Helper to send data over UART
 */
void uart_write_string(char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        DL_UART_transmitDataBlocking(UART_0_INST, str[i]);
    }
}

void get_filename_from_storage(i) {
    // TODO: implement this function, or find an already implemented version
}

/**
 * @brief Implementation of the 'list' command
 */
void message_list_response(message_header_t header) {
    uint8_t input_pin[PIN_LENGTH];

    // get the 6-digit pin
    for (int i = 0; i < PIN_LENGTH; i++) {
        input_pin[i] = (uint8_t)DL_UART_receiveDataBlocking(UART_0_INST);
    }

    // verify pin
    bool is_valid = utils_verify_pin(input_pin, PIN_LENGTH);

    // wipe the pin from the stack
    memset(input_pin, 0, sizeof(input_pin));

    if (is_valid) {         // success
        // list the filenames
        uart_write_string("Listing files:\r\n");
        
        for (int i = 0; i < MAX_FILES; i++) {
            char* name = get_filename_from_storage(i);
            if (name != NULL) {
                uart_write_string(" - ");
                uart_write_string(name);
                uart_write_string("\r\n");
            }
        }
    } else {                // failure
        // wait around 5 seconds
        // the MSPM0L board runs at 32MHz
        // 5 * 32000000 = 5 seconds
        for(int i = 0; i < 5; i++) {
            delay_cycles(32000000); 
        }
        uart_write_string("Error: Invalid PIN provided.\r\n");
    }
}