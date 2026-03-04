#pragma once

#include "message/header.h"
#include "constants.h"
#include "secrets.h"

/*!
 * @brief Interrogated group list structure (aligned to AES block size)
 */
typedef struct __attribute__((aligned(16))) {
    //! Number of groups available
    uint16_t num_groups;

    //! Array of group IDs
    uint16_t group_ids[8];
} message_interrogate_group_list_t;

/*!
 * @brief Encrypted interrogated group list structure
 */
typedef struct {
    //! IV for AES-GCM encryption (128 bits)
    uint32_t iv[4];

    //! Group list data
    message_interrogate_group_list_t group_list;

    //! Authentication tag (128 bits)
    uint32_t tag[4];
} message_interrogate_group_list_encrypted_t;

/*!
 * @brief Interrogated file metadata structure (packed)
 */
typedef struct __attribute__((packed)) {
    //! File slot number
    uint8_t slot;

    //! Group ID owning the file
    uint16_t group_id;

    //! File name (32 bytes)
    char name[32];
} message_interrogate_file_metadata_t;

/*!
 * @brief Interrogated file info structure (aligned to AES block size)
 */
typedef struct __attribute__((aligned(16))) {
    //! Number of files available
    uint32_t num_files;

    //! Array of file metadata entries
    message_interrogate_file_metadata_t files[8];
} message_interrogate_file_info_t;

/*!
 * @brief Encrypted interrogation file info structure
 */
typedef struct {
    //! IV for AES-GCM encryption (128 bits)
    uint32_t iv[4];

    //! File info data
    message_interrogate_file_info_t file_info;

    //! Authentication tag (128 bits)
    uint32_t tag[4];
} message_interrogate_file_info_encrypted_t;

/*!
 * @brief Responds to Interrogate Request
 * 
 * @param[in] header Message header containing request context
 */
void message_interrogate(message_header_t header);