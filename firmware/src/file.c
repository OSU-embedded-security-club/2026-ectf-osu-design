#include "file.h"

#include <monocypher.h>
#include <string.h>

// __attribute__((persistent, location(0x3A000))) file_address_table_t FILE_ADDRESS_TABLE = {};
// __attribute__((persistent, location(0x20000))) file_slot_entry_t SLOTS[NUM_SLOTS] = {};

__attribute__((section(".fat"))) file_address_table_t FILE_ADDRESS_TABLE;
__attribute__((section(".file_store"))) file_slot_entry_t SLOTS[NUM_SLOTS];



int file_read_fat(file_address_table_t* fat) {
        for(uint16_t i = 0; i < NUM_SLOTS; i++) {
            memcpy(&fat->entries[i], &FILE_ADDRESS_TABLE.entries[i], sizeof(file_fat_entry_t));
        }
        return 0;
}
int file_write_fat(file_address_table_t* fat) {
    // Erase FAT in Flash
    for(int i = 0; i < sizeof(FILE_ADDRESS_TABLE) / FLASH_SECTOR_SIZE; i++) {
        uint32_t address = (uint32_t) &FILE_ADDRESS_TABLE + FLASH_SECTOR_SIZE * i;
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_eraseMemory(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);

        DL_FlashCTL_waitForCmdDone(FLASHCTL);
    }

    for(size_t i = 0; i < sizeof(FILE_ADDRESS_TABLE.entries); i += 8) {
        uint32_t address = (uint32_t) &FILE_ADDRESS_TABLE + i;
        uint32_t* data = (uint32_t*) fat + i / 4;

        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, address, data);

        DL_FlashCTL_waitForCmdDone(FLASHCTL);

        // TODO: Check for Flash Errors
    }

    return 0;
}