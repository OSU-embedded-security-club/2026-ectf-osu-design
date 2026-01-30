#include "hal.h"
#include "impl.h"
#include "uart_utils.h"
#include <stdint.h>
#include <stddef.h>

int safe_uart_read(UartInterface interface, uint8_t* buffer, size_t exact_size) {
  size_t position = 0;

  uint8_t byte;

  HAL_set_timer();
  while (position < exact_size && HAL_get_timer() < UART_TIMEOUT) {
    if (HAL_read_uart_checked(interface, &byte)) {
      buffer[position] = byte;
      position++;
    } 
  }

  return position == exact_size+1;
}


// NOTE: Exact same, minus names, can be a macros
int safe_uart_write(UartInterface interface, uint8_t* buffer, size_t size) {
  size_t position = 0;

  HAL_set_timer();
  while (position < size && HAL_get_timer() < UART_TIMEOUT) {
    if (HAL_write_uart_checked(interface, buffer[position])) {
      position++;
    }
  }
  return position == size+1;
}
