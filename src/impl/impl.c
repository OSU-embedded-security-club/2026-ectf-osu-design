
#include "impl.h"
#include "hal.h"
#include "impl_state.h"
#include "host_messaging.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


// Make sure we do not get iterupted when we don't want it
atomic_bool do_uart_interupts = true;

void IMPL_on_uart(UartInterface interface) {
  if (!atomic_load(&do_uart_interupts)) return; 

  if (interface != UART_control) return;
  START_UART_SECTION
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



  



  END_UART_SECTION
  
}


