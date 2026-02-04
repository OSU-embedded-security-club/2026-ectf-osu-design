#include "message/interrogate.h"

void message_interrogate(message_header_t header) {
    // TODO: Implement interrogate response

    char msg[] = "Interrogate not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
