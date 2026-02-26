#include "message/listen.h"

void message_listen(const message_header_t header, const size_t rx_len,
                    const uint8_t *const payload) {
  // TODO: Implement listen response

  char msg[] = "Listen not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
