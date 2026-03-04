#pragma once
#include <stdbool.h>




/*!
 * @brief Validates PIN with 2-stage BLAKE2b hashing
 * 
 * @param[in] pin PIN string to validate (6 characters)
 * 
 * @returns true if PIN is valid, false otherwise
 */
bool validate_pin(const char pin[6]);

/*!
 * @brief Initializes PIN validation system
 */
void init_pin();







