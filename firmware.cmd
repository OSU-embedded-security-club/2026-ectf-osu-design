-uinterruptVectors
--stack_size=256

#define FLASH_BASE 0x6000
#define FAT_BASE 0x3A000

MEMORY
{
    /* Divide this in half and allocate the first half to user space and the second reserved */
    BOOTLOADER      (RX)  : origin = 0x00000000, length = 0x00006000 /* Bootloader flash */
    FLASH           (RX)  : origin = 0x00006000, length = 0x00019FFF /* Location of team firmware */
    FILE_STORE      (RW)  : origin = 0x00020000, length = 0x0001A000 /* File Store */
    FAT             (RW)  : origin = 0x0003A000, length = 0x00000400 /* Used to store tag, length, pointer values for files */
    APP2            (RX)  : origin = 0x0003A400, length = 0x00005c00 /* Location of team firmware */
    SRAM            (RWX) : origin = 0x20200000, length = 0x00008000
    BCR_CONFIG      (R)   : origin = 0x41C00000, length = 0x000000FF
    BSL_CONFIG      (R)   : origin = 0x41C00100, length = 0x00000080
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