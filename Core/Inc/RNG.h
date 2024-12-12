/*
 * RNG.h
 *
 *  Created on: Dec 11, 2024
 *      Author: bilim
 */

#ifndef INC_RNG_H_
#define INC_RNG_H_

#include "stm32f4xx_hal.h"

// Function to initialize the RNG peripheral
void RNG_Init(void);

// Function to generate a random number
uint32_t RNG_GetRandomNumber(void);

// Function to generate a random number within a range [min, max]
uint32_t RNG_GetRandomNumberInRange(uint32_t min, uint32_t max);

#endif // RNG_DRIVER_H
