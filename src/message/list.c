#include "message/list.h"

#define MAX_RETRY_DELAY_MS 5000

void message_list_response(message_header_t header) {
    // TODO: Implement list response

    print_debug("Test");

    /*
    List details
    Description:
    List the files stored on the current HSM

    usage:
    uvx ectf tools <PORT> list [OPTIONS] <PIN: 6 digits>
    example:
    uvx ectf tools COM10 list 123456
    Options:
    --help
    */

    /*
    Vulnerabilities to keep in mind:
    1. Timing attacks when inserting pins
    2. Brute force (only 1,000,000 combinations)
    3. Buffer overflows - deals with file names
    4. 

    Solutions for vulnerabilities
    1. Retry counter
    2. Brute force delay (5 seconds)
    3. Constant time check for pin
    4. Make the pin verification inline
    */

    /*
    psuedocode:
    get the pin from the user
    hash and salt the pin
    inline check the pin in constant time
    If correct
        list all files on the system
    else
        deny and wait 5 seconds
    clear sensitive data
    */

    char msg[] = "List not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
