#include "message/list.h"

void message_list_response(const message_header_t header, const size_t rx_len,
                           const uint8_t *const payload) {
  // TODO: Implement list response

  char msg[] = "List not implemented yet";
  message_header_send_error(HOST_INST, msg, sizeof(msg));
}
