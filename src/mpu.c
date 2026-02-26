/**
 * @file mpu.c
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

#include "ti_msp_dl_config.h"

#include "mpu.h"
#include "third_party/CMSIS/Core/Include/mpu_armv7.h"

/**
 * @brief Initializes the Memory Protection Unit (MPU) with predefined regions
 * and access permissions to enhance the security of the system.
 *
 */
void mpu_init(void) {
  for (uint32_t i = 0; i < 8; ++i) {
    ARM_MPU_ClrRegion(i);
  }

  // TODO: align bases or this will fail

  // Region 0: Entire address space, read-write access.
  ARM_MPU_SetRegionEx(
      0, ARM_MPU_RBAR(0, __bootloader_start),
      ARM_MPU_RASR(1, ARM_MPU_AP_FULL, 0, 1, 0, 1, 0, ARM_MPU_REGION_SIZE_4GB));

  // Region 1: Flash memory, read-execute access.
  ARM_MPU_SetRegionEx(
      1, ARM_MPU_RBAR(1, __flash_start),
      ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 0, 0, ARM_MPU_REGION_SIZE_128KB));

  // Region 2: File store + FAT region, read-only access.
  ARM_MPU_SetRegionEx(
      2, ARM_MPU_RBAR(2, __file_store_start),
      ARM_MPU_RASR(1, ARM_MPU_AP_RO, 0, 0, 1, 0, 0, ARM_MPU_REGION_SIZE_128KB));

  // Region 3: App2 region, no access.
  ARM_MPU_SetRegionEx(3, ARM_MPU_RBAR(3, __app2_start),
                      ARM_MPU_RASR(1, ARM_MPU_AP_NONE, 0, 0, 1, 0, 0,
                                   ARM_MPU_REGION_SIZE_32KB));

  // Region 4: SRAM region, read-write access.
  ARM_MPU_SetRegionEx(4, ARM_MPU_RBAR(4, __sram_start),
                      ARM_MPU_RASR(1, ARM_MPU_AP_FULL, 0, 1, 1, 0, 0,
                                   ARM_MPU_REGION_SIZE_32KB));

  ARM_MPU_Enable(0);
}
