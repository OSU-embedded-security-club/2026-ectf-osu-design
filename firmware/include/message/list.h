#pragma once

#include "../sysconfig/ti_msp_dl_config.h"
#include "message/header.h"
#include "file.h"

typedef struct __attribute__((packed)) {
    //! File slot number
    uint8_t slot;

    //! Group ID owning the file
    uint16_t group_id;

    //! File name (32 bytes)
    char name[32];
} message_list_file_entry_t;

/*!
 * @brief Packed list response structure
 * 
 * @warning All items in this struct should only be assigned
 *          using byte operations.
 */
typedef struct __attribute__((packed)) {
    //! Number of files available
    uint32_t num_files;

    //! Array of file entries
    message_list_file_entry_t entries[NUM_SLOTS];
} message_list_response_t;

/*!
 * @brief Responds to a List Request
 * 
 * @param[in] header Message header containing request context
 */
void message_list_response(message_header_t header);