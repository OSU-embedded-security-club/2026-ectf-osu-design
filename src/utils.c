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

<<<<<<< HEAD
void utils_receive_bytes(UART_Regs* uart, uint8_t* buffer, size_t num_bytes) {
    for(int i = 0; i < num_bytes; i++)
        buffer[i] = DL_UART_receiveDataBlocking(HOST_INST);
}
=======
void utils_receive_bytes(UART_Regs* uart, void* buffer, size_t num_bytes) {
    uint8_t* temp = buffer;
    for(int i = 0; i < num_bytes; i++)
        temp[i] = DL_UART_receiveDataBlocking(HOST_INST);
}
>>>>>>> origin
