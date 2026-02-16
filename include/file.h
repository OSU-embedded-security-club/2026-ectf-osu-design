#pragma once

#include "constants.h"
#include "ti_msp_dl_config.h"

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
 *
 * To generate the file's encryption key you must
 * permform an X25519 Key Exchange using the public key
 * from the metadata and the private read key from the file's
 * group.
 */
typedef struct {
  //! File Group ID
  uint16_t group_id;

  //! File Name
  char name[32];

  //! File Size
  uint16_t file_size;

  //! File AES-GCM Tag
  uint8_t file_signature[128];

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
 * @if cypto_ed25519_check(&this->writer_signature, WRITER_PUBKEY,
 * &this->metadata, sizeof(this->metadata)) == 0 This metadata / file was
 * written by a valid writer.
 * @endif
 */
typedef struct {
  //! File Metadata
  file_metadata_t metadata;

  //! Ed25519 Signature from File Writer
  uint8_t writer_signature[64];
} file_metadata_signed_t;

/*!
 * @brief Structure of data in each "slot" of memory.
 *
 */
typedef struct {
  //! Signed File Metadata
  file_metadata_signed_t signed_metadata;

  //! Encrypted File with Padding using AES-GCM
  uint8_t
      encrypted_file[FILE_PADDING_FRONT + MAX_FILE_SIZE + FILE_PADDING_BACK];

  //! Pad Slot so a whole number of sectors are used
  uint8_t padding[1024 - (sizeof(file_metadata_signed_t) +
                          sizeof(uint8_t[FILE_PADDING_FRONT + MAX_FILE_SIZE +
                                         FILE_PADDING_BACK])) %
                             1024];
} file_slot_entry_t;

#ifdef __MSPM0L2228__
__attribute__((
    location(0x3A000))) static file_fat_entry_t file_address_table[NUM_SLOTS];

__attribute__((
    section(".file_store"))) static file_slot_entry_t slots[NUM_SLOTS];
#else
static file_fat_entry_t file_address_table[NUM_SLOTS];
static file_slot_entry_t slots[NUM_SLOTS];
#endif