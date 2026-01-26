#include <stdatomic.h>

extern atomic_bool do_uart_interupts;

#define START_UART_SECTION { \
  bool _old_int_value =  atomic_exchange(&do_uart_interupts, false);

#define END_UART_SECTION  atomic_store(&do_uart_interupts, _old_int_value); }





