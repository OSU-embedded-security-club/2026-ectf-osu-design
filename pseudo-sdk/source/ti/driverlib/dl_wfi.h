#pragma once
#include "dl_uart.h"

__STATIC_INLINE void __WFI(void) { __dl_uart_poll(); }
