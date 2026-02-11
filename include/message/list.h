#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"

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