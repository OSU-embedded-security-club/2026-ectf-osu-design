#include "utils.h"

bool message_utils_verify_pin(uint8_t pin[6]) {
    // TODO: Implement pin verification logic
    return true;
}

void utils_send_buffer(UART_Regs * uart, void* buffer, size_t length) {
    uint8_t * bytes = (uint8_t*) buffer;

    for(int i = 0; i < length; i++) {
        DL_UART_transmitDataBlocking(uart, bytes[i]);
    }
}
