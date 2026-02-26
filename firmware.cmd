-uinterruptVectors
--stack_size=256

#define BOOTLOADER_BASE 0x00000000
#define FLASH_BASE      0x00006000
#define FILE_STORE_BASE 0x00020000
#define FAT_BASE        0x0003A000
#define APP2_BASE       0x0003A400
#define SRAM_BASE       0x20200000
#define BCR_CONFIG_BASE 0x41C00000
#define BSL_CONFIG_BASE 0x41C00100

#define BOOTLOADER_SIZE 0x00006000
#define FLASH_SIZE      0x00019FFF
#define FILE_STORE_SIZE 0x0001A000
#define FAT_SIZE        0x00000400
#define APP2_SIZE       0x00005c00
#define SRAM_SIZE       0x00008000
#define BCR_CONFIG_SIZE 0x000000FF
#define BSL_CONFIG_SIZE 0x00000080

MEMORY
{
    /* Divide this in half and allocate the first half to user space and the second reserved */
    BOOTLOADER      (R)  : origin = BOOTLOADER_BASE, length = BOOTLOADER_SIZE /* Bootloader flash */
    FLASH           (R)  : origin = FLASH_BASE, length = FLASH_SIZE /* Location of team firmware */
    FILE_STORE      (R)  : origin = FILE_STORE_BASE, length = FILE_STORE_SIZE /* File Store */
    FAT             (R)  : origin = FAT_BASE, length = FAT_SIZE /* Used to store tag, length, pointer values for files */
    APP2            (R)  : origin = APP2_BASE, length = APP2_SIZE /* Location of team firmware */
    SRAM            (RW) : origin = SRAM_BASE, length = SRAM_SIZE
    BCR_CONFIG      (R)  : origin = BCR_CONFIG_BASE, length = BCR_CONFIG_SIZE
    BSL_CONFIG      (R)  : origin = BSL_CONFIG_BASE, length = BSL_CONFIG_SIZE
}

SECTIONS
{
    .fat:   > FAT_BASE
    .file_store : palign(1024) {} > FILE_STORE

    .intvecs:   > FLASH_BASE
    .text   : palign(8) {} > FLASH
    .const  : palign(8) {} > FLASH
    .cinit  : palign(8) {} > FLASH
    .pinit  : palign(8) {} > FLASH
    .rodata : palign(8) {} > FLASH
    .ARM.exidx    : palign(8) {} > FLASH
    .init_array   : palign(8) {} > FLASH
    .binit        : palign(8) {} > FLASH
    .files        : palign(8)
    .TI.ramfunc   : load = FLASH, palign(8), run=SRAM, table(BINIT)

    .vtable :   > SRAM
    .args   :   > SRAM
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM (HIGH)

    .BCRConfig  : {} > BCR_CONFIG
    .BSLConfig  : {} > BSL_CONFIG
}