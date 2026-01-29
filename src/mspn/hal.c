#include "hal.h"
#include "sysconfig/ti_msp_dl_config.h"


noreturn void HAL_on_error() {
  while (1);
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

static inline UART_Regs* uart_from(UartInterface interface) {
  return interface == UART_control ? HOST_INST : HSM_INST;
}


size_t HAL_read_uart(UartInterface interface, uint8_t* buffer) {
  UART_Regs* uart = uart_from(interface);
  

}
void HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len);

