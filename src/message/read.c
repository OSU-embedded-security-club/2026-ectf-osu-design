#include "message/read.h"
#include <monocypher.h>

void message_read_response(message_header_t header) {
    // TODO: Implement read response
    //pull entire file into memory
    //check to see if the group has the private read key
    //if it has the private key key exchange with the metadata write pub key
    //use crypto_x25519() 
    //else return an erorr
    //decrypt with the key and delete it from memory using crypto_wipe()
    //check the last block with the aad and if not the same return error, else return data

    char msg[] = "Read not implemented yet";
    message_header_send_error(HOST_INST, msg, sizeof(msg));
}
