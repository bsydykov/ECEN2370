/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "stm32f4xx_hal.h"
#include "stmpe811.h"
#include <stdio.h>
#include "main.h"



#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

void ApplicationInit(void);
void LCD_Visual_Demo(void);

/* Function Prototypes */
void TIM7_Init(void);           // Initialize TIM7 for periodic interrupts
void TimerStart(void);          // Start TIM7 counter
void TimerStop(void);           // Stop TIM7 counter
uint32_t Timer_GetValue(void);
uint32_t Timer_GetElapsedTime(void);
void Button_Interrupt(void);    // Initialize button interrupts

#if (COMPILE_TOUCH_FUNCTIONS == 1) && (COMPILE_TOUCH_INTERRUPT_SUPPORT == 0)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1) && (COMPILE_TOUCH_INTERRUPT_SUPPORT == 0)

#endif /* INC_APPLICATIONCODE_H_ */
