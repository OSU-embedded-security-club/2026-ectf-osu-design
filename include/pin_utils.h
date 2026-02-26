#pragma once
#include <stdbool.h>

/**
 * @brief Validates a provided pin by applying the hashing process and comparing
 * the result to the expected hash. The pin is expected to be a 6-character
 * hexadecimal string.
 *
 * @param pin Null-terminated string containing the pin to validate. Must be 6
 * characters long and consist of hexadecimal digits (0-9, A-F, a-f).
 * @return true if the provided pin is valid, false otherwise
 */
bool validate_pin(const char pin[6]);

/**
 * @brief Initializes the pin hashing process by generating random data,
 * applying stage 2 to the stage 1 hash, and preparing any necessary data for
 * the pin validation process.
 *
 */
void init_pin(void);
