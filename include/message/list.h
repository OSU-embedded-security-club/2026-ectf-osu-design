#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"

/**
 * @brief Checks if a flash slot contains a valid file.
 */
bool is_slot_in_use(uint8_t slot_num);

/**
 * @brief Copies metadata from Flash into a RAM structure.
 * 
 * @return 0 on success, -1 if slot is empty or out of bounds.
 */
int read_file_metadata(uint8_t slot_num, file_metadata_t *out_metadata);

/** @brief Create the list of files to display
 *
 *  @param file_list    The variable that holds the list of files to be displayed
 */
void generate_list_files(list_response_t file_list) {

/**
 * @brief Sends one byte to the console.
 *
 * @param byte              The current byte being sent
 * @param total_bytes_sent  The total number of bytes sent
 */
void send_byte_with_flow_control(unit8_t byte, unit8_t *total_bytes_sent);

/**
 * @brief Writes bytes from listing the files to the console.
 *
 * @param msg_type      Tells whether the message is a header or the payload
 * @param data          The data to be transmitted
 * @param len           The length of the number of files to be transmitted
 */
void write_packet(uint8_t msg_type, void* data, uint16_t len);

/**
 * @brief Responds to a List Request
 *
 * @param[in] header    Message Header
 */
void message_list_response(message_header_t header);
