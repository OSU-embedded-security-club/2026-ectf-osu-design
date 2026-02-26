#pragma once

#include "message/header.h"
#include "constants.h"
#include "secrets.h"

typedef struct {
    uint8_t public_key[32];
    uint16_t num_groups;
    uint16_t group_ids[8];
    uint8_t signatures[8][32];
} message_interrogate_verify_t;

typedef struct __attribute__((packed)) {
    uint8_t slot;
    uint16_t group_id;
    char name[32];
} message_interrogate_file_metadata_t;

typedef struct __attribute__((aligned(16))) {
    uint32_t num_files;
    message_interrogate_file_metadata_t files[8];
} message_interrogate_file_info_t;

/**
 * @brief Responds to Interrogate Request
 *
 * @param[in] header    Message Header
 */
void message_interrogate(message_header_t header);