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

/**
 * @brief ECTF Host Tools Metadata
 */
typedef struct {
    uint8_t slot_number;
    uint16_t group_id;
    char name[32];
} file_ectf_metadata_t;


/*!
 * @brief Full File Metadata
 */
typedef struct {
    file_ectf_metadata_t ectf_metadata;
    size_t file_size;
    uint8_t aad[128];

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
    //! File Metadata
    file_metadata_t metadata;

    //! Ed25519PH Signature from File Writer
    uint8_t writer_signature[64];
} file_metadata_signed_t;


/*!
 * @brief Structure of data in each "slot" of memory.
 */
typedef struct {
    file_metadata_signed_t signed_metadata;
    uint8_t encrypted_file[MAX_FILE_SIZE];
    
    //! Pad Slot so a whole number of sectors are used
    uint8_t padding[760];
} file_slot_entry_t;

__attribute__((location(0x3A000))) file_fat_entry_t file_address_table[NUM_SLOTS];

// Using const will initalize this on flash
// This will probably be changed later
const file_slot_entry_t slots[NUM_SLOTS];