#pragma once

#include "ti_msp_dl_config.h"

/*!
 * @brief Fills buffer with random data using hardware TRNG
 * 
 * @param[out] buffer Buffer to fill with random data
 * @param[in] output_size Number of random bytes to generate
 */
void random_fill_buffer(unsigned char *buffer, size_t output_size);