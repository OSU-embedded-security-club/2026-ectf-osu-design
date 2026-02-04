/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0L2228
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0L2228
#define CONFIG_MSPM0L2228

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for HOST */
#define HOST_INST                                                          UART0
#define HOST_INST_FREQUENCY                                             32000000
#define HOST_INST_IRQHandler                                    UART0_IRQHandler
#define HOST_INST_INT_IRQN                                        UART0_INT_IRQn
#define GPIO_HOST_RX_PORT                                                  GPIOA
#define GPIO_HOST_TX_PORT                                                  GPIOA
#define GPIO_HOST_RX_PIN                                          DL_GPIO_PIN_11
#define GPIO_HOST_TX_PIN                                          DL_GPIO_PIN_10
#define GPIO_HOST_IOMUX_RX                                       (IOMUX_PINCM26)
#define GPIO_HOST_IOMUX_TX                                       (IOMUX_PINCM25)
#define GPIO_HOST_IOMUX_RX_FUNC                        IOMUX_PINCM26_PF_UART0_RX
#define GPIO_HOST_IOMUX_TX_FUNC                        IOMUX_PINCM25_PF_UART0_TX
#define HOST_BAUD_RATE                                                  (115200)
#define HOST_IBRD_32_MHZ_115200_BAUD                                        (17)
#define HOST_FBRD_32_MHZ_115200_BAUD                                        (23)
/* Defines for HSM */
#define HSM_INST                                                           UART1
#define HSM_INST_FREQUENCY                                              32000000
#define HSM_INST_IRQHandler                                     UART1_IRQHandler
#define HSM_INST_INT_IRQN                                         UART1_INT_IRQn
#define GPIO_HSM_RX_PORT                                                   GPIOA
#define GPIO_HSM_TX_PORT                                                   GPIOA
#define GPIO_HSM_RX_PIN                                            DL_GPIO_PIN_9
#define GPIO_HSM_TX_PIN                                            DL_GPIO_PIN_8
#define GPIO_HSM_IOMUX_RX                                        (IOMUX_PINCM20)
#define GPIO_HSM_IOMUX_TX                                        (IOMUX_PINCM19)
#define GPIO_HSM_IOMUX_RX_FUNC                         IOMUX_PINCM20_PF_UART1_RX
#define GPIO_HSM_IOMUX_TX_FUNC                         IOMUX_PINCM19_PF_UART1_TX
#define HSM_BAUD_RATE                                                   (115200)
#define HSM_IBRD_32_MHZ_115200_BAUD                                         (17)
#define HSM_FBRD_32_MHZ_115200_BAUD                                         (23)




/* Defines for TRNG */
/*
 * The TRNG interrupt is part of INT_GROUP1. Refer to the TRM for more details
 * on interrupt grouping
 */
#define TRNG_INT_IRQN                                            (TRNG_INT_IRQn)
#define TRNG_INT_IIDX                            (DL_INTERRUPT_GROUP1_IIDX_TRNG)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_HOST_init(void);
void SYSCFG_DL_HSM_init(void);
void SYSCFG_DL_TRNG_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
