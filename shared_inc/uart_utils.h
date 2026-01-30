#include "hal.h"
#include <stddef.h>

// 500 ms
#define UART_TIMEOUT 500 * 1000 * 1000


/*
* Attempts to read from uart, doing untill exactly `expected_size` is found.
* Returns: 0 on ok, 1 on timeout
*/
int safe_uart_read(UartInterface interface, uint8_t* buffer, size_t expected_size);
int safe_uart_write(UartInterface interface, uint8_t* buffer, size_t size);


