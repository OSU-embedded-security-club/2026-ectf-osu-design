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



/* Defines for TRANSFER */
#define TRANSFER_INST                                                      UART0
#define TRANSFER_INST_FREQUENCY                                         32000000
#define TRANSFER_INST_IRQHandler                                UART0_IRQHandler
#define TRANSFER_INST_INT_IRQN                                    UART0_INT_IRQn
#define GPIO_TRANSFER_RX_PORT                                              GPIOA
#define GPIO_TRANSFER_TX_PORT                                              GPIOA
#define GPIO_TRANSFER_RX_PIN                                      DL_GPIO_PIN_11
#define GPIO_TRANSFER_TX_PIN                                      DL_GPIO_PIN_10
#define GPIO_TRANSFER_IOMUX_RX                                   (IOMUX_PINCM26)
#define GPIO_TRANSFER_IOMUX_TX                                   (IOMUX_PINCM25)
#define GPIO_TRANSFER_IOMUX_RX_FUNC                    IOMUX_PINCM26_PF_UART0_RX
#define GPIO_TRANSFER_IOMUX_TX_FUNC                    IOMUX_PINCM25_PF_UART0_TX
#define TRANSFER_BAUD_RATE                                              (115200)
#define TRANSFER_IBRD_32_MHZ_115200_BAUD                                    (17)
#define TRANSFER_FBRD_32_MHZ_115200_BAUD                                    (23)
/* Defines for CONTROL */
#define CONTROL_INST                                                       UART1
#define CONTROL_INST_FREQUENCY                                          32000000
#define CONTROL_INST_IRQHandler                                 UART1_IRQHandler
#define CONTROL_INST_INT_IRQN                                     UART1_INT_IRQn
#define GPIO_CONTROL_RX_PORT                                               GPIOA
#define GPIO_CONTROL_TX_PORT                                               GPIOA
#define GPIO_CONTROL_RX_PIN                                        DL_GPIO_PIN_9
#define GPIO_CONTROL_TX_PIN                                        DL_GPIO_PIN_8
#define GPIO_CONTROL_IOMUX_RX                                    (IOMUX_PINCM20)
#define GPIO_CONTROL_IOMUX_TX                                    (IOMUX_PINCM19)
#define GPIO_CONTROL_IOMUX_RX_FUNC                     IOMUX_PINCM20_PF_UART1_RX
#define GPIO_CONTROL_IOMUX_TX_FUNC                     IOMUX_PINCM19_PF_UART1_TX
#define CONTROL_BAUD_RATE                                               (115200)
#define CONTROL_IBRD_32_MHZ_115200_BAUD                                     (17)
#define CONTROL_FBRD_32_MHZ_115200_BAUD                                     (23)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_TRANSFER_init(void);
void SYSCFG_DL_CONTROL_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
