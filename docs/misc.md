# Miscellaneous Details

This page details some of the implementation specifics that didn't really fit into any other section.

## Stalls

In our design we have configured the WWDT for 4 seconds. The timer is only reset in the main event loop so if a function gets stuck at any point the watchdog will expire and trigger a reset. This makes any issues with incomplete messages or other errors recoverable.

## UART Errors

The UART peripheral has the ability to detect frame errors, overrrun errors, and noise errors. We have enabled the interrupts for all of these exceptions as any of these errors would be breaking for our implementation. This allows errors to be caught early and helps diagnose issues that would simply arise later in the testing of our design.

## Random Data

All of the random data generated in our design comes from the [random_fill_buffer function](@ref random_fill_buffer) which uses the true random number generator on the MSPM0l2228 chip.

## File System Layout

The FAT and file system are represented using structs that are placed in a section that is in the flash memory. These structs cannot be written to in a traditional sense but can be written using the FlashCTL. I like this design because it allows us to reference files using a simple C array and the write address can be obtained using the & operation. For more information on the filesystem itself you can look at the [FILE_ADDRESS_TABLE](@ref FILE_ADDRESS_TABLE) variable and the [SLOTS](@ref SLOTS) variable. For information of the layout of each entry in the FAT and file system you can take a look at the [file_address_table_entry](@ref file_address_table_t) and the [file_slot_entry_t](@ref file_slot_entry_t) structs respectively.