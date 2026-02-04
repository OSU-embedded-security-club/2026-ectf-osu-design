#include "message/read.h"

void message_read_response(message_header_t header) {
    // TODO: Implement read response

    char msg[] = "Read not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
