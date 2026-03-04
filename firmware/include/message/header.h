#pragma once

#include "../sysconfig/ti_msp_dl_config.h"

/*!
 * @brief Message operation codes
 */
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

    //! Notify of an error/failure
    MESSAGE_ERROR = 'E',

    //! Provide debug information
    MESSAGE_DEBUG = 'D',
} message_operation_t;


/*!
 * @brief Message header structure
 * 
 * All messages begin with '%' followed by operation code
 * and message length.
 */
typedef struct {
    //! Message operation code
    message_operation_t operation;

    //! Length of message payload in bytes
    uint16_t message_length;
} message_header_t;

/*!
 * @brief Parses message header from UART
 * 
 * @param[in] uart UART registers for communication
 * @param[out] header Output header structure
 * 
 * @returns 0 on success, negative value on error
 */
int message_header_request(UART_Regs *uart, message_header_t* header);

/*!
 * @brief Starts a response to the host tools
 * 
 * @param[in] uart UART registers for communication
 * @param[in] ctx Message header containing response context
 */
void message_header_response(UART_Regs* uart, message_header_t ctx);

/*!
 * @brief Sends an error message to the host tools
 * 
 * @param[in] uart UART registers for communication
 * @param[in] msg Error message string
 * @param[in] msg_len Length of error message
 */
void message_header_send_error(UART_Regs* uart, const char* msg, size_t msg_len);

/*!
 * @brief Sends a debug message to the host tools
 * 
 * @param[in] uart UART registers for communication
 * @param[in] msg Debug message string
 * @param[in] msg_len Length of debug message
 */
void message_header_send_debug(UART_Regs* uart, const char* msg, size_t msg_len);

/*!
 * @brief Sends an ACK message to the host tools
 * 
 * @param[in] uart UART registers for communication
 */
void message_header_send_ack(UART_Regs* uart);

/*!
 * @brief Blocks until ACK message is received
 * 
 * @param[in] uart UART registers for communication
 */
void message_header_receive_ack(UART_Regs* uart);