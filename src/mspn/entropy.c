#include "sysconfig/ti_msp_dl_config.h"
#include <mbedtls/platform.h>

int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags, size_t *estimate_bits, unsigned char *output, size_t output_size) {

    size_t i;

    // mbedtls says we must fill the entire output buffer
    while(i < output_size) {

        // Wait for TRNG to be ready
        while(DL_TRNG_isCaptureReady(TRNG)) {
            delay_cycles(1);
        }

        uint32_t rand_num = DL_TRNG_getCapture(TRNG);

        // Convert uint32_t into 4 chars
        unsigned char * bytes = (unsigned char*) &rand_num;

        for(int j = 0; j < 4 && i < output_size; j++) {
            output[i] = bytes[j];
            i++;
        }
    }

    return 0;
}