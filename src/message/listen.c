#include "message/listen.h"
#include "utils.h"
#include "random.h"
#include "file.h"
#include <monocypher.h>



// TODO: find secrets.h
void message_listen(message_header_t header) {
    message_operation_t cmd;

    const file_metadata_signed_t *metadata;  // Still need to implement

    uint8_t pin[6];
    uint8_t signatures[8][64];
    uint8_t secret_key[64];
    uint8_t public_key[64];
    uint8_t seed[32];
    uint8_t raw_shared_secret[32];
    uint8_t symmetric_key[64];

    random_fill_buffer(seed, 32);  

    crypto_ed25519_key_pair(secret_key, public_key, seed); 
    // TODO: crypto wipe keys after done using

    cmd = DL_UART_receiveDataBlocking(HOST_INST);
    switch (cmd) {
        case MESSAGE_INTERROGATE:
            // Verify PIN
            message_header_send_debug(HOST_INST, "Enter PIN: ", 12);
            
            utils_receive_bytes(HOST_INST, pin, 6);
            if (!message_utils_verify_pin(pin)) {
                DL_SYSCTL_resetDevice();
            }

            // Send public key
            utils_send_buffer(HSM_INST, public_key, sizeof(public_key));


            // Receives number detailing how many signatures to expect
            uint8_t signature_count;
            utils_receive_bytes(HSM_INST, &signature_count, 1);
            if (signature_count > 8 || signature_count < 0) {
                DL_SYSCTL_resetDevice();
            }


            uint8_t signature[64];
            uint8_t hsm2_public_key;
            uint8_t message[8][64]; // TODO: this is an array of hsm2's public key + file permission string- i'm assuming the later is 32 bytes
            // eddsa_check is not constant time, supposedly doesn't have to be 
            for (int i = 0; i < signature_count; i++) {
                utils_receive_bytes(HSM_INST, signature, 64);
                utils_receive_bytes(HSM_INST, message[i], 64);
                // TODO: replace x with file public keys
                if (!crypto_eddsa_check(signature, x, message, sizeof(message[i]))) {
                    DL_SYSCTL_resetDevice();
                }
            }


            crypto_x25519(raw_shared_secret, secret_key, hsm2_public_key);

            // TODO: not sure what to do with the context
            crypto_blake2b_ctx ctx;
            crypto_blake2b_init(&ctx, 64);
        
            crypto_blake2b(symmetric_key, 64, &raw_shared_secret, sizeof(raw_shared_secret));


            // TODO: read and validate permission strings, then get oadded file metadata list and send


        case MESSAGE_RECEIVE:
            // TODO: implement
    }
}
