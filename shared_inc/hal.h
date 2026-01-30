#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdnoreturn.h>


typedef enum{
  UART_control,
  UART_transfer
} UartInterface;




noreturn void HAL_on_error();

void HAL_led_on();
void HAL_led_off();
void HAL_led_toggle();


void HAL_usleep(uint32_t micro);


// Use this for your buffer size
#define MAX_READ_SIZE 512 


// Note: Only write as much as is possible. See uart_utils.c:safe_uart_write for what you should actually use
size_t HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len);

// Note: Only reads avalible data, might be truncated. See uart_utils.c:safe_uart_read for what you should actually use
size_t HAL_read_uart(UartInterface interface, uint8_t* buffer, size_t max_count);
