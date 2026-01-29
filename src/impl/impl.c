#include "impl.h"
#include "hal.h"
#include "host_messaging.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>


void IMPL_on_uart(UartInterface interface) {

  if (interface != UART_control) return;
  HostOp hp;
  uint16_t len;

  read_packet_header(&hp, &len);
  STREAM_PACKET_BODY(len, buff, new_len) {
    buff[new_len] = 0 ;
  }
  print_debug("Pin: %s", buff);


  uint8_t files_d[4 + 1 + 2 + 32] = {
    1, 0, 0, 0,
    '1',
    'a', 'a',
    't', 'e', 's', 't',' ', 'f', 'i', 'l', 'e', '!'
  };
  packet_send(hp, files_d, sizeof(files_d));
}

// See host_messaging.c for:
// void IMPL_write_debug(char* message)
// void IMPL_write_error(char* message)
