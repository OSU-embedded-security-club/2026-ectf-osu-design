#pragma once

#include "../sysconfig/ti_msp_dl_config.h"

typedef enum {
    //! List files command/response
    MESSAGE_LIST = 'L',
    //! Read file command/response
    MESSAGE_READ = 'R',
    //! Write file command/response
    MESSAGE_WRITE = 'W',
    //! Receive file command/response
    MESSAGE_RECEIVE = 'C',
    //! Interrogate files command/response
    MESSAGE_INTERROGATE = 'I',
    //! Listen command/response
    MESSAGE_LISTEN = 'N',
    //! Acknowledge the receipt of data
    MESSAGE_ACK = 'A',
    //! Notify of an error/failure. Exits Host Tool
    MESSAGE_ERROR = 'E',
    //! Provide debug information (ignored by testing framework)
    MESSAGE_DEBUG = 'D',
} message_operation_t;


/**
 * @brief Message Initalization Packet
 * 
 * @note All messages should begin with '%'.
 */
typedef struct {
    message_operation_t operation;
    uint16_t message_length;
} message_header_t;

/**
 * @brief Parses Message Header from uart
 *
 * @param[in] Uart Registers to parse header from
 * 
 * @returns Request Headers
 */
message_header_t message_header_request(UART_Regs *uart);

/**
 * @brief Starts a Response to the Host Tools
 * 
 * @param[in] ctx   Message Header
 */
void message_header_response(UART_Regs* uart, message_header_t ctx);

/**
 * @brief Sends an error message to the Host Tools
 *
 * @param[in] uart      UART registers for communication
 * @param[in] msg       Error message string
 * @param[in] msg_len   Length of the error message
 */
void message_header_send_error(UART_Regs* uart, const char* msg, size_t msg_len);

/**
 * @brief Sends a debug message to the Host Tools
 *
 * @param[in] uart      UART registers for communication
 * @param[in] msg       Debug message string
 * @param[in] msg_len   Length of the debug message
 */
void message_header_send_debug(UART_Regs* uart, const char* msg, size_t msg_len);

/**
 * @brief Sends an ACK message to the Host Tools
 *
 * @param[in] uart      UART registers for communication
 */
void message_header_send_ack(UART_Regs* uart);
