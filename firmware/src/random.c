#include "random.h"

/**
 * This function generates random bytes by repeatedly sampling the True Random Number
 * Generator (TRNG) hardware module and converting the 32-bit outputs into individual
 * bytes. The function blocks until the entire output buffer is filled.
 */
void random_fill_buffer(uint8_t *buffer, size_t length) {

    size_t i = 0;

    // mbedtls says we must fill the entire output buffer
    while(i < length) {

        // Wait for TRNG to be ready
        while(!DL_TRNG_isCaptureReady(TRNG)) {
            delay_cycles(100);
        }

        uint32_t rand_num = DL_TRNG_getCapture(TRNG);

        // Convert uint32_t into 4 chars array
        uint8_t * bytes = (uint8_t*) &rand_num;

        for(int j = 0; j < 4 && i < length; j++) {
            buffer[i] = bytes[j];
            i++;
        }
    }
}