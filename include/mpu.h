/**
 * @file mpu.h
 * @author Andrew Langan
 * @brief eCTF MPU Interface
 * @date 2026
 *
 * This source file is part of an example system for MITRE's 2026 Embedded CTF
 * (eCTF). This code is being provided only for educational purposes for the
 * 2026 MITRE eCTF competition, and may not meet MITRE standards for quality.
 * Use this code at your own risk!
 *
 * @copyright Copyright (c) 2026 The MITRE Corporation
 */

#pragma once

#include <stdint.h>

#define BOOTLOADER_BASE 0x00000000
#define FLASH_BASE 0x00006000
#define FILE_STORE_BASE 0x00020000
#define FAT_BASE 0x0003A000
#define APP2_BASE 0x0003A400
#define SRAM_BASE 0x20200000
#define BCR_CONFIG_BASE 0x41C00000
#define BSL_CONFIG_BASE 0x41C00100

#define BOOTLOADER_SIZE 0x00006000
#define FLASH_SIZE 0x00019FFF
#define FILE_STORE_SIZE 0x0001A000
#define FAT_SIZE 0x00000400
#define APP2_SIZE 0x00005c00
#define SRAM_SIZE 0x00008000
#define BCR_CONFIG_SIZE 0x000000FF
#define BSL_CONFIG_SIZE 0x00000080

static const uint32_t __bootloader_start = BOOTLOADER_BASE;
static const uint32_t __bootloader_size = BOOTLOADER_SIZE;

static const uint32_t __flash_start = FLASH_BASE;
static const uint32_t __flash_size = FLASH_SIZE;

static const uint32_t __file_store_start = FILE_STORE_BASE;
static const uint32_t __file_store_size = FILE_STORE_SIZE;

static const uint32_t __fat_start = FAT_BASE;
static const uint32_t __fat_size = FAT_SIZE;

static const uint32_t __app2_start = APP2_BASE;
static const uint32_t __app2_size = APP2_SIZE;

static const uint32_t __sram_start = SRAM_BASE;
static const uint32_t __sram_size = SRAM_SIZE;

static const uint32_t __bcr_config_start = BCR_CONFIG_BASE;
static const uint32_t __bcr_config_size = BCR_CONFIG_SIZE;

static const uint32_t __bsl_config_start = BSL_CONFIG_BASE;
static const uint32_t __bsl_config_size = BSL_CONFIG_SIZE;

/**
 * @brief Initializes the Memory Protection Unit (MPU) with predefined regions
 * and access permissions to enhance the security of the system.
 *
 */
void mpu_init(void);
