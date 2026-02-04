#include "message/listen.h"

void message_listen(message_header_t header) {
    // TODO: Implement listen response

    char msg[] = "Listen not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
