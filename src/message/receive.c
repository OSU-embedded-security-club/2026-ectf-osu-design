#include "message/receive.h"

void message_recieve(message_header_t header) {
    // TODO: Implement receive response

    char msg[] = "Receive not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
