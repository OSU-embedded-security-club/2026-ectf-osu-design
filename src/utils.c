#include "utils.h"
#include "random.h"
#include "pin_utils.h"

bool utils_verify_pin(uint8_t* pin, size_t pin_length) {
    if (pin_length != 6) return false;
    return validate_pin((const char*) pin);
}

void utils_send_buffer(UART_Regs * uart, const void* buffer, size_t length, size_t bytes_sent) {
    uint8_t * bytes = (uint8_t*) buffer;

    for(int i = 0; i < length; i++) {
        DL_UART_transmitDataBlocking(uart, bytes[i]);
        bytes_sent++;

        if(bytes_sent % CHUNK_SIZE == 0) {
          message_header_receive_ack(uart);
        }
    }

    if((length + bytes_sent) % CHUNK_SIZE != 0 || length == 0) {
        message_header_receive_ack(uart);
    }
}

void utils_send_buffer_no_ack(UART_Regs* uart, const void* buffer, size_t length) {
    for(size_t i = 0; i < length; i++) {
        DL_UART_transmitDataBlocking(uart, ((uint8_t*) buffer)[i]);
    }
}

void utils_receive_bytes(UART_Regs* uart, void* buffer, size_t num_bytes) {
    uint8_t* temp = buffer;
    for(int i = 0; i < num_bytes; i++)
        temp[i] = DL_UART_receiveDataBlocking(uart);
}

const group_t* utils_find_group(uint16_t group_id) {
    for(int i = 0; i < sizeof(groups) / sizeof(group_t); i++) {
        if(groups[i].group_id == group_id) {
            return &groups[i];
        }
    }
    return 0;
}

void utils_random_delay() {
    uint16_t random_num;
    random_fill_buffer((uint8_t*) &random_num, sizeof(random_num));
    
    // Num Between 0 - 4096
    random_num = random_num >> 4;
    delay_cycles(random_num);
}