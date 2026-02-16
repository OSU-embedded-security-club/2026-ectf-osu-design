#include "ti_msp_dl_config.h"

SYSCONFIG_WEAK void SYSCFG_DL_init(void) {
  SYSCFG_DL_initPower();
  SYSCFG_DL_GPIO_init();
  SYSCFG_DL_SYSCTL_init();
  SYSCFG_DL_HOST_init();
  SYSCFG_DL_HSM_init();
  SYSCFG_DL_TRNG_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void) {}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void) {}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void) {}

SYSCONFIG_WEAK void SYSCFG_DL_HOST_init(void) {
  DL_UART_enablePower(HOST_INST);
}

SYSCONFIG_WEAK void SYSCFG_DL_HSM_init(void) { DL_UART_enablePower(HSM_INST); }

SYSCONFIG_WEAK void SYSCFG_DL_TRNG_init(void) {}
