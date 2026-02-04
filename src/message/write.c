#include "message/write.h"

void message_write_response(message_header_t header) {
    // TODO: Implement write response

    char msg[] = "Write not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
