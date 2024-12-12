/*
 * Button_driver.c
 *
 *  Created on: Dec 8, 2024
 *      Author: bilim
 */


#include "Button_Driver.h"


// Button Initialization Function
void Button_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Button;
    GPIO_Button.Pin = GPIO_PIN_0;                    // Button is connected to Pin 0 of GPIO Port F
    GPIO_Button.Mode = GPIO_MODE_IT_FALLING;         // Set mode to trigger an interrupt on falling edge (button press)
    GPIO_Button.Pull = GPIO_NOPULL;                  // No internal pull-up or pull-down resistor
    GPIO_Button.Speed = GPIO_SPEED_FREQ_LOW;         // Low speed is sufficient for button inputs

    HAL_GPIO_Init(GPIOA, &GPIO_Button);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


// Interrupt Handler for EXTI Line (Button Press)
void EXTI_ClearPending(void){
	EXTI_HandleTypeDef clearP;
	clearP.Line = EXTI_LINE_0;
	HAL_EXTI_ClearPending(&clearP, EXTI_TRIGGER_RISING_FALLING);
}

void Button_EnableInterrupt() {

    Enable_IRQ(EXTI0_IRQn);
}

