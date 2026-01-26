#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdnoreturn.h>

// Use this for your buffer size
#define MAX_READ_SIZE 512 

typedef enum{
  UART_control,
  UART_transfer
} UartInterface;




noreturn void HAL_on_error();

void HAL_led_on();
void HAL_led_off();

size_t HAL_read_uart(UartInterface interface, uint8_t* buffer);
void HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len);
