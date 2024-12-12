/*
 * Button_driver.h
 *
 *  Created on: Dec 8, 2024
 *      Author: bilim
 */

#ifndef INC_BUTTON_DRIVER_H_
#define INC_BUTTON_DRIVER_H_

#include <stdio.h>
#include "stm32f4xx_hal.h"

// Function Prototypes for Button Driver
void Button_Init(void);                   // Initializes the button GPIO and configures interrupts
void Button_EnableInterrupt(void);        // Enables the button interrupt (NVIC)
void EXTI_ClearPending(void);// Clears the pending button interrupt flag


#endif /* INC_BUTTON_DRIVER_H_ */
