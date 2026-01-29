#include "hal.h"
#include "uart_utils.h"
#include <stdint.h>

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
