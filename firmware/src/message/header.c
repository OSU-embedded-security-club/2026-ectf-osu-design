#include "message/header.h"
#include "utils.h"

int message_header_request(UART_Regs *uart, message_header_t* header) {

    char MAGIC = DL_UART_receiveDataBlocking(uart);

    if(MAGIC != '%') {
        return -1;
    }

    header->operation = DL_UART_receiveDataBlocking(uart);

    uint8_t length[2] = {
        DL_UART_receiveDataBlocking(uart),
        DL_UART_receiveDataBlocking(uart),
    };
    
    header->message_length = *((uint16_t*) length);

    return 0;
}

void message_header_response(UART_Regs *uart, message_header_t header) {
    // Message MAGIC Byte
    DL_UART_transmitDataBlocking(uart, '%');

    DL_UART_transmitDataBlocking(uart, header.operation);


    uint8_t * length = (uint8_t*) (&header.message_length);

    DL_UART_transmitDataBlocking(uart, length[0]);
    DL_UART_transmitDataBlocking(uart, length[1]);

}

void message_header_send_error(UART_Regs* uart, const char* msg, size_t msg_len) {
    delay_cycles(32 * 10^6);
    message_header_t header = {
        .operation = MESSAGE_ERROR,
        .message_length = msg_len,
    };
    message_header_response(uart, header);
    message_header_receive_ack(uart);

    utils_send_buffer_no_ack(uart, msg, msg_len);
}

void message_header_send_debug(UART_Regs* uart, const char* msg, size_t msg_len) {
    message_header_t header = {
        .operation = MESSAGE_DEBUG,
        .message_length = msg_len,
    };
    message_header_response(uart, header);

    utils_send_buffer_no_ack(uart, msg, msg_len);
}

void message_header_send_ack(UART_Regs* uart) {
    message_header_t header = {
        .operation = MESSAGE_ACK,
        .message_length = 0,
    };

    message_header_response(uart, header);
}

void message_header_receive_ack(UART_Regs* uart) {
    message_header_t req;
    int result = message_header_request(uart, &req);

    if(result != 0 && req.operation != MESSAGE_ACK) {
        const char msg[] = "Malformed ACK";
        message_header_send_debug(uart, msg, sizeof(msg));
    }

    // if(req.message_length != 0) {
    //     const char msg[] = "ACK Length Error";
    //     message_header_send_debug(uart, msg, sizeof(msg));
    // }
}