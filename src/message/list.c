#include "message/list.h"

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
    2. Brute force
    3. Buffer overflows - deals with file names
    4. 

    Solutions for vulnerabilities
    1. Retry counter
    2. 
    3. 
    4. 
    */

    /*
    psuedocode
    get the PIN from the user
    hash the entered pin and check it with the stored pin hash
    If the hash works, then get the file names stored on the board
    List all the files on the system
    if the hash does not work, then return an error
    */

    char msg[] = "List not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
