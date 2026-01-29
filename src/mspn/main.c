#include "hal.h"
#include "sysconfig/ti_msp_dl_config.h"
#include "impl.h"

void UART_0_INST_IRQHandler(void) {
  switch (DL_UART_Main_getPendingInterrupt(HOST_INST)) {
  case DL_UART_MAIN_IIDX_RX:
      IMPL_on_uart(UART_control);

    break;
  default:
    break;
  }
}
void main_loop(void) {
  while (1) {
    __WFE();
    if (DL_UART_isRXFIFOEmpty(HOST_INST)) IMPL_on_uart(UART_control);
    if (DL_UART_isRXFIFOEmpty(HSM_INST)) IMPL_on_uart(UART_transfer);
  }
}


//! Program Entrypoint
int main(void) {
  // Configure System
  SYSCFG_DL_init();

  // Setup LED pin
  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
  DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_14);

  main_loop();
}
