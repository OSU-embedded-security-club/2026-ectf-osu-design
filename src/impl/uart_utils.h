#include "hal.h"
#define MAX_ATTEMPTS 64

// 50 ms
#define ATTEMPT_REST_MICROSECOUNDS (5 * 100)


int safe_uart_read(UartInterface interface, uint8_t* buffer, size_t expected_size);



