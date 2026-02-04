#include "message/list.h"

void message_list_response(message_header_t header) {
    // TODO: Implement list response

    char msg[] = "List not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
