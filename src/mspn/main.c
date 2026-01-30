#include "hal.h"
#include <stdint.h>
#include "sysconfig/ti_msp_dl_config.h"
#include "impl.h"
#include "ti/devices/msp/peripherals/hw_uart.h"
#include "ti/driverlib/dl_uart_main.h"
#include <string.h>



static void UART_IRQHandler(UartInterface inter, UART_Regs* inst) {
  static bool handle_uart = true;

  // Critical section
  __disable_irq();
  if (!handle_uart) return;
  handle_uart = false;
  __enable_irq();

  if (DL_UART_MAIN_IIDX_RX == DL_UART_Main_getPendingInterrupt(inst)) {
    uint8_t byte;
    while (DL_UART_receiveDataCheck(inst, &byte)) {
      DL_UART_transmitDataBlocking(inst, byte);
    }


  }

  handle_uart = true;

  
}

// void UART0_IRQHandler(void) {
//   UART_IRQHandler(UART_control, CONTROL_INST);
// }
// void UART_1_INST_IRQHandler(void) {
//   UART_IRQHandler(UART_transfer, TRANSFER_INST);
// }

// NOTE: See mspm0-sdk/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0l222x_ticlang.c

void main_loop(void) {
  while (1) {
    HAL_usleep(500 * 1000);
    HAL_led_toggle();

    DL_UART_transmitDataBlocking(CONTROL_INST, 'h');

  }
}


//! Program Entrypoint
int main(void) {
  NVIC_EnableIRQ(UART0_INT_IRQn);
  NVIC_EnableIRQ(UART1_INT_IRQn); 
  // Configure System
  SYSCFG_DL_init();

  // Setup LED pin
  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
  DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_14);
  
  HAL_led_on();
  main_loop();

}
