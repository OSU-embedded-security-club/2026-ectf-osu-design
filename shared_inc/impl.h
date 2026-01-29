#pragma once
#include <stddef.h>
#include "hal.h"



// Gets called when a uart has data
// Note: This can come from an interupt, so be wise
void IMPL_on_uart(UartInterface interface);
void IMPL_write_debug(char* message);

// Can take null as no messsage
void IMPL_write_error(char* message);

#ifdef DEBUG


#ifdef SIMU
#include <stdio.h>
#define _to_str(X) #X
#define _double_debug_print(FUNC, str) printf("SIMU " _to_str(FUNC) ": %s\n", str)
#else
#define _double_debug_print(FUNC, str) {}
#endif

#include <stdlib.h>

#define _WRITE_TO(FUNC, ...) do{                            \
  char* _buffer;                                            \
  if (-1 != asprintf(&_buffer, __VA_ARGS__)){ \
    _double_debug_print(FUNC, _buffer); \
    FUNC(_buffer); \
    free(_buffer); \
  } \
} while(0);

#define print_debug(...) _WRITE_TO(IMPL_write_debug, __VA_ARGS__)
#define print_error(...) _WRITE_TO(IMPL_write_error, __VA_ARGS__)

#else

// More secure for production

#define print_debug(...) {}
#define print_error(...) IMPL_write_error(NULL)
#endif
