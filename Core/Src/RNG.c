/*
 * RNG.c
 *
 *  Created on: Dec 11, 2024
 *      Author: bilim
 */


#include "RNG.h"

// Handle for RNG peripheral
static RNG_HandleTypeDef hrng;

// Function to initialize the RNG peripheral
void RNG_Init(void) {
    __HAL_RCC_RNG_CLK_ENABLE(); // Enable RNG clock
    hrng.Instance = RNG;
    HAL_RNG_Init(&hrng);

    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        // Initialization error
        Error_Handler();
    }
}

// Function to generate a random number
uint32_t RNG_GetRandomNumber(void) {
    uint32_t randomValue;
    HAL_RNG_GenerateRandomNumber(&hrng, &randomValue);
    if (HAL_RNG_GenerateRandomNumber(&hrng, &randomValue) != HAL_OK) {
        // Error handling
        Error_Handler();
    }

    return randomValue;
}
