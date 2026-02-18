#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"

/**
 * @brief Checks if a Flash slot contains a valid file.
 */
bool is_slot_in_use(uint8_t slot_num);

/**
 * @brief Copies metadata from Flash into a RAM structure.
 * @return 0 on success, -1 if slot is empty or out of bounds.
 */
int read_file_metadata(uint8_t slot_num, file_metadata_t *out_metadata);

/** @brief List out the files on the system.
 *      To be utilized by list and interrogate
 *
 *  @param file_list A pointer to the list_response_t variable in
 *      which to store the results
 */
void generate_list_files(list_response_t *file_list) {

/**
 * @brief Responds to a List Request
 *
 * @param[in] header    Message Header
 */
void message_list_response(message_header_t header);