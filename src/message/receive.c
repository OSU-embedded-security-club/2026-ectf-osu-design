#include "message/receive.h"

void message_reeieve(const message_header_t header, const size_t rx_len,
                     const uint8_t *const payload) {
  // TODO: Implement receive response

  char msg[] = "Receive not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
