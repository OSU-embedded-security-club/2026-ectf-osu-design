#pragma once

#include "ti_msp_dl_config.h"
#include "constants.h"

#define FLASH_SECTOR_SIZE 1024
#define FLASH_WORD_SIZE 8

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

//! Ensure FAT Table takes whole number of sectors
typedef struct __attribute__((aligned(FLASH_SECTOR_SIZE))){
    file_fat_entry_t entries[NUM_SLOTS];
} file_address_table_t;


/*!
 * @brief Full File Metadata
 * 
 * To generate the file's encryption key you must
 * permform an X25519 Key Exchange using the public key
 * from the metadata and the private read key from the file's
 * group.
 */
typedef struct {
    //! File UUID
    uint8_t uuid[16];

    //! File Group ID
    uint16_t group_id;

    //! File Name
    char name[32];

    //! File Size
    uint16_t file_size;

    //! File AES-GCM Tag
    uint32_t file_signature[4];

    /*!
     * This is the public key of a keypair generated for this
     * file specifically. 
     */
    uint8_t encryption_public_key[32];
} file_metadata_t;


/*!
 * @brief Signed File Metadata
 *
 * Signing the file metadata with the group writer key
 * allows us to validate that the file was written by an HSM with
 * the write permission for that group. Only HSMs with the write
 * permission for a group have the private key.
 *
 * @if cypto_ed25519_check(&this->writer_signature, WRITER_PUBKEY, &this->metadata, sizeof(this->metadata)) == 0
 *      This metadata / file was written by a valid writer.
 * @endif
 */
typedef struct __attribute__((aligned(FLASH_WORD_SIZE))) {
    //! File Metadata
    file_metadata_t metadata;

    //! Ed25519 Signature from File Writer
    uint8_t writer_signature[64];
} file_metadata_signed_t;

//! Maximum padding required at back of file to align with AES block boundary
#define MAX_PADDING_BACK (16 - MAX_FILE_SIZE % 16)

/*!
 * @brief Structure of data in each "slot" of memory.
 *
 * This struct is designed to be written to flash so we have
 * to change how the memory alignment works. Each member of this
 * struct is aligned to the flash word size (8 bytes / 64 bits).
 * This allows us to write each structure seperatly without having
 * to worry about ECC which is automatically generated for flash word writes.
 * The entire structure is aligned to the flash sector size (1024 bytes) so
 * each entry can be cleanly erased without having to write back any of the other
 * members in an array.
 * 
 */
typedef struct __attribute__((aligned(FLASH_SECTOR_SIZE))) {
    //! Signed File Metadata
    file_metadata_signed_t signed_metadata __attribute__((aligned(FLASH_WORD_SIZE)));

    //! Encrypted File with Padding using AES-GCM
    uint8_t encrypted_file[MAX_FILE_SIZE + MAX_PADDING_BACK] __attribute__((aligned(FLASH_WORD_SIZE)));
} file_slot_entry_t;

extern file_address_table_t FILE_ADDRESS_TABLE;
extern file_slot_entry_t SLOTS[NUM_SLOTS];

/*!
 * @brief Reads the FAT table from flash
 * 
 * @param[out] fat Pointer to FAT table structure to populate
 * 
 * @returns 0 on success, negative value on error
 */
int file_read_fat(file_address_table_t* fat);

/*!
 * @brief Writes the FAT table to flash
 * 
 * @param[in] fat Pointer to FAT table structure to write
 * 
 * @returns 0 on success, negative value on error
 */
int file_write_fat(file_address_table_t* fat);