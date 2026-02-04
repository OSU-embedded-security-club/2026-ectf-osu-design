#include "message/list.h"
#include "utils.h"

void message_list_response(message_header_t header) {
    // TODO: Implement list response

    print_debug("Test");
    char msg[] = "List not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
