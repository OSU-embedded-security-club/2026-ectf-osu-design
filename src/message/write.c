#include "message/write.h"

void message_write_response(const message_header_t header, const size_t rx_len,
                            const uint8_t *const payload) {
  // TODO: Implement write response

  char msg[] = "Write not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
