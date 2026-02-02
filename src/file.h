#include "sysconfig/ti_msp_dl_config.h"
#include "constants.h"

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
} FileFatEntry;

/**
 * @brief ECTF Host Tools Metadata
 */
typedef struct {
    uint8_t slot_number;
    uint16_t group_id;
    char name[32];
} FileECTFMetadata;


/*!
 * @brief Full File Metadata
 */
typedef struct {
    FileECTFMetadata ectf_metadata;
    size_t size;
    uint8_t aad[128];

    /*!
     * This is the public key of a keypair generated for this
     * file specifically. 
     */
    uint8_t encryption_public_key[KD_KEY_BYTES];
} FileMetadata;


/*!
 * @brief Signed File Metadata
 */
typedef struct {
    //! File Metadata
    FileMetadata metadata;

    //! Ed25519PH Signature from File Writer
    uint8_t writer_signature[SIGNATURE_KEY_BYTES];
} FileMetadataSigned;


/*!
 * @brief Structure of data in each "slot" of memory.
 */
typedef struct {
    FileMetadataSigned signed_metadata;
    uint8_t encrypted_file[MAX_FILE_SIZE];
} FileSlot;