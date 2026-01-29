#include "hal.h"
#define MAX_ATTEMPTS 64

// 10 ms
#define ATTEMPT_REST_MICROSECOUNDS (10 * 1000)


/*
* Attempts to read from uart, doing untill exactly `expected_size` is found.
* Returns: 0 on ok, 1 on timeout
*/
int safe_uart_read(UartInterface interface, uint8_t* buffer, size_t expected_size);



