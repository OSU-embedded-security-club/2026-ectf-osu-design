#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"

typedef struct __attribute__((packed)) {
    uint8_t slot;
    uint16_t group_id;
    char name[32];
} message_list_file_entry_t;

/**
 * @brief Packed List Response
 * 
 * @warning All items in this struct should only be assigned
 *          using byte operations.
 */
typedef struct __attribute__((packed)) {
    uint32_t num_files;
    message_list_file_entry_t entries[NUM_SLOTS];
} message_list_response_t;

/**
 * @brief Responds to a List Request
 *
 * @param[in] header    Message Header
 */
void message_list_response(message_header_t header);