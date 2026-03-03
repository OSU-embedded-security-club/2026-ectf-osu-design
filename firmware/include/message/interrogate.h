#pragma once

#include "message/header.h"
#include "constants.h"
#include "secrets.h"

// Aligned to AES Block Size
typedef struct __attribute__((aligned(16))) {
    uint16_t num_groups;
    uint16_t group_ids[8];
} message_interrogate_group_list_t;

typedef struct {
    uint32_t iv[4];
    message_interrogate_group_list_t group_list;
    uint32_t tag[4];
} message_interrogate_group_list_encrypted_t;

typedef struct __attribute__((packed)) {
    uint8_t slot;
    uint16_t group_id;
    char name[32];
} message_interrogate_file_metadata_t;

// Aligned to AES Block Size
typedef struct __attribute__((aligned(16))) {
    uint32_t num_files;
    message_interrogate_file_metadata_t files[8];
} message_interrogate_file_info_t;

typedef struct {
    uint32_t iv[4];
    message_interrogate_file_info_t file_info;
    uint32_t tag[4];
} message_interrogate_file_info_encrypted_t;

/**
 * @brief Responds to Interrogate Request
 *
 * @param[in] header    Message Header
 */
void message_interrogate(message_header_t header);