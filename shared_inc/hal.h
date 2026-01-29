#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdnoreturn.h>


typedef enum{
  UART_control,
  UART_transfer
} UartInterface;




noreturn void HAL_on_error();

void HAL_led_on();
void HAL_led_off();
void HAL_led_toggle();


// Use this for your buffer size
#define MAX_READ_SIZE 512 

// Note: Only reads avalible data, might be truncated. 
size_t HAL_read_uart(UartInterface interface, uint8_t* buffer, size_t max_count);
void HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len);
