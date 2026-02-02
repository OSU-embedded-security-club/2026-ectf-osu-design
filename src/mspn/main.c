#include "hal.h"

#include <stdint.h>
#include "sysconfig/ti_msp_dl_config.h"
#include "impl.h"
#include "ti/devices/msp/peripherals/hw_uart.h"
#include "ti/driverlib/dl_uart_main.h"
#include <string.h>



static void UART_IRQHandler(UartInterface inter, UART_Regs* inst) {
  // Read IIDX to determine the source AND clear the pending flag
  switch (DL_UART_Main_getPendingInterrupt(inst)) {
    case DL_UART_MAIN_IIDX_RX_TIMEOUT_ERROR:
    case DL_UART_MAIN_IIDX_RX:
      IMPL_on_uart(inter);
    default: break;
  }
}


// NOTE: See mspm0-sdk/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0l222x_ticlang.c
void CONTROL_INST_IRQHandler(void) {
  UART_IRQHandler(UART_control, CONTROL_INST);
}


void main_loop(void) {
  while (1) {
    // Wait for interupt
    __WFI();
  }
}


//! Program Entrypoint
int main(void) {
  // Configure System
  SYSCFG_DL_init();


  NVIC_ClearPendingIRQ(CONTROL_INST_INT_IRQN);
  NVIC_EnableIRQ(CONTROL_INST_INT_IRQN);


  // Setup LED pin
  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
  DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_14);
  
  HAL_led_on();
  main_loop();

}
