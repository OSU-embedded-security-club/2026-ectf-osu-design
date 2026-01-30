#include "hal.h"
#include "impl.h"
#include "sysconfig/ti_msp_dl_config.h"
#include "ti/driverlib/dl_uart.h"
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

void HAL_set_timer() {
  SysTick->VAL = 0;
}



// The MSPN is 32 mhz = 32 * 1000 * 1000 hz
// There are 1000 * 1000 microsecounds in a second.
// Therefore, 32 cycles is about 1 microsecound
 uint32_t HAL_get_timer() {
  return SysTick->VAL / 32;
}

void HAL_usleep(uint32_t micro) {
  delay_cycles(micro * 32);
}

static inline UART_Regs* uart_from(UartInterface interface) {
  return interface == UART_control ? CONTROL_INST : TRANSFER_INST;
}



bool HAL_write_uart_checked(UartInterface interface, uint8_t byte){
  return DL_UART_transmitDataCheck(
    uart_from(interface),
    byte
  );
}
bool HAL_read_uart_checked(UartInterface interface, uint8_t* out_byte) {
  return  DL_UART_receiveDataCheck(
    uart_from(interface),
    out_byte
  );

}

