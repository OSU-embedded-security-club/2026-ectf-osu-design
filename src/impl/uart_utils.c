#include "hal.h"
#include "uart_utils.h"
#include <stdint.h>
#include <stddef.h>

int safe_uart_read(UartInterface interface, uint8_t* buffer, size_t exact_size) {
  size_t total_read = 0;
  int attempts = 0;

  while (total_read < exact_size) {
    size_t size_needed = exact_size - total_read;
    size_t bytes_got = HAL_read_uart(interface, &buffer[total_read], size_needed);

    if (bytes_got > 0) {
      total_read += bytes_got;
      attempts = 0;
    } else {
      attempts++;
      if (attempts > MAX_ATTEMPTS) return 1;

      HAL_usleep(ATTEMPT_REST_MICROSECOUNDS);
    }
  }
  return 0;
}


// NOTE: Exact same, minus names, can be a macros
int safe_uart_write(UartInterface interface, uint8_t* buffer, size_t size) {
  size_t total_written = 0;
  int attempts = 0;

  while (total_written < size) {
    size_t size_needed = size - total_written;
    size_t bytes_done = HAL_write_uart(interface, &buffer[total_written], size_needed);

    if (bytes_done > 0) {
      total_written += bytes_done;
      attempts = 0;
    } else {
      attempts++;
      if (attempts > MAX_ATTEMPTS) return 1;

      HAL_usleep(ATTEMPT_REST_MICROSECOUNDS);
    }
  }
  return 0;
}
