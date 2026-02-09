#include "utils.h"

bool utils_verify_pin(uint8_t* pin, size_t pin_length) {
    // TODO: Implement pin verification logic
    return true;
}

void utils_send_buffer(UART_Regs * uart, const void* buffer, size_t length) {
    uint8_t * bytes = (uint8_t*) buffer;

    for(int i = 0; i < length; i++) {
        DL_UART_transmitDataBlocking(uart, bytes[i]);
    }
}
