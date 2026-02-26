#include "message/read.h"

void message_read_response(const message_header_t header, const size_t rx_len,
                           const uint8_t *const payload) {
  // TODO: Implement read response

  char msg[] = "Read not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
