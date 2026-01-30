#include "hal.h"
#include "sysconfig/ti_msp_dl_config.h"
#include <stdint.h>


noreturn void HAL_on_error() {
  // TODO: is there a need for a recovery mechinism?
  while (1) {
    HAL_usleep(250 * 1000);
    HAL_led_on();
    HAL_usleep(500 * 1000);
    HAL_led_off();
  }
}

void HAL_led_on() {
  DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_14);
}
void HAL_led_off() {
  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_14);
}
void HAL_led_toggle(){
  DL_GPIO_togglePins(GPIOB, DL_GPIO_PIN_14);
}




void HAL_usleep(uint32_t micro) {
  // The MSPN is 32 mhz = 32 * 1000 * 1000 hz
  // There are 1000 * 1000 microsecounds in a second.
  // Therefore, 32 cycles is about 1 microsecound
  delay_cycles(micro * 32);
}

static inline UART_Regs* uart_from(UartInterface interface) {
  return interface == UART_control ? HOST_INST : HSM_INST;
}


size_t HAL_read_uart(UartInterface interface, uint8_t* buffer, size_t max_count) {
  UART_Regs* uart = uart_from(interface);
  return (size_t) DL_UART_drainRXFIFO(uart, buffer, max_count);
}
size_t HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len) {
  UART_Regs* uart = uart_from(interface);
  return (size_t) DL_UART_fillTXFIFO(uart, buffer, len);
}


