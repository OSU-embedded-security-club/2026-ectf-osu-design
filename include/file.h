#pragma once

#include "ti_msp_dl_config.h"
#include "constants.h"

#define FLASH_SECTOR_SIZE 1024

//! Required FAT Structure
typedef struct {
    //! UUID of the file in that slot
    uint8_t uuid[16];

    //! Length of the file
    uint16_t length;
    
    //! Unused to pad to 32-bit alignment
    uint16_t padding;

    //! Starting flash address of the file
    uint32_t address;
} file_fat_entry_t;


/*!
 * @brief Full File Metadata
 */
typedef struct {
    uint16_t group_id;
    char name[32];
    uint16_t file_size;
    uint8_t file_signature[128];

    /*!
     * This is the public key of a keypair generated for this
     * file specifically. 
     */
    uint8_t encryption_public_key[32];
} file_metadata_t;


/*!
 * @brief Signed File Metadata
 */
typedef struct {
    // Unsigned Slot Number
    uint8_t slot_number;

    //! File Metadata
    file_metadata_t metadata;

    //! Ed25519PH Signature from File Writer
    uint8_t writer_signature[32];
} file_metadata_signed_t;


/*!
 * @brief Structure of data in each "slot" of memory.
 */
typedef struct {
    file_metadata_signed_t signed_metadata;
    uint8_t encrypted_file[FILE_PADDING_FRONT + MAX_FILE_SIZE + FILE_PADDING_BACK];
    
    //! Pad Slot so a whole number of sectors are used
    uint8_t padding[1024 - (sizeof(file_metadata_signed_t) + sizeof(uint8_t[FILE_PADDING_FRONT + MAX_FILE_SIZE + FILE_PADDING_BACK])) % 1024];
} file_slot_entry_t;


__attribute__((location(0x3A000))) static file_fat_entry_t file_address_table[NUM_SLOTS];

__attribute__((section(".file_store"))) static file_slot_entry_t slots[NUM_SLOTS];