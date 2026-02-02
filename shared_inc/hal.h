#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
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


// Note: This is intentionally made to be like DL_UART_{receive,transmit}DataCheck
//       See uart_utils.c:safe_uart_{read, write} for what you should actually use
//
// Returns true if successful.
bool HAL_write_uart_checked(UartInterface interface, uint8_t byte);
bool HAL_read_uart_checked(UartInterface interface, uint8_t* out_byte);


void HAL_set_timer();
// Return timer info is in micro secounds
uint32_t HAL_get_timer();

