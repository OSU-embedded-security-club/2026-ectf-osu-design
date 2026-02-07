#include "message/header.h"
#include "utils.h"

message_header_t message_header_request(UART_Regs *uart) {

    uint8_t opcode = DL_UART_receiveDataBlocking(uart);

    uint8_t length[2] = {
        DL_UART_receiveDataBlocking(uart),
        DL_UART_receiveDataBlocking(uart),
    };
    
    message_header_t header = {
        .operation = opcode,
        .message_length = *((uint16_t*) length)
    };

    return header;
}

void message_header_response(UART_Regs *uart, message_header_t header) {
    // Message MAGIC Byte
    DL_UART_transmitDataBlocking(uart, '%');

    DL_UART_transmitDataBlocking(uart, header.operation);


    uint8_t * length = (uint8_t*) (&header.message_length);

    DL_UART_transmitDataBlocking(uart, length[0]);
    DL_UART_transmitDataBlocking(uart, length[1]);

}

void message_header_send_error(UART_Regs* uart, char* msg, size_t msg_len) {
    message_header_t header = {
        .operation = MESSAGE_ERROR,
        .message_length = msg_len,
    };
    message_header_response(uart, header);

    utils_send_buffer(uart, msg, msg_len);
}

void message_header_send_debug(UART_Regs* uart, char* msg, size_t msg_len) {
    message_header_t header = {
        .operation = MESSAGE_DEBUG,
        .message_length = msg_len,
    };
    message_header_response(uart, header);

    utils_send_buffer(uart, msg, msg_len);
}

void message_header_send_ack(UART_Regs * uart) {
    message_header_t ack_header = {
        .operation = MESSAGE_ACK,
        .message_length = 0,
    };

    message_header_response(uart, ack_header);
}