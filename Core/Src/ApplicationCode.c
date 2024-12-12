/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"
#include "Game_driver.h"
#include "LCD_Driver.h"
#include "Button_driver.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#if TOUCH_INTERRUPT_ENABLED == 1
static EXTI_HandleTypeDef LCDTouchIRQ;
void LCDTouchScreenInterruptGPIOInit(void);
#endif // TOUCH_INTERRUPT_ENABLED
#endif // COMPILE_TOUCH_FUNCTIONS

volatile uint32_t elapsed_time; // Incremented in the interrupt handler

void TIM7_Init(void) {
    // Activate TIM7 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Set prescaler: with 16 MHz clock, dividing by 16000 yields 1kHz (1ms)
    TIM7->PSC = 16000 - 1; // PSC counts from 0, so subtract 1

    // Auto-reload for 1ms: 1000 ticks at 1kHz = 1ms
    TIM7->ARR = 1000 - 1;

    // Enable update event interrupts
    TIM7->DIER |= TIM_DIER_UIE;

    // Start counting
    TIM7->CR1 |= TIM_CR1_CEN;

    // Enable TIM7 IRQ in NVIC
    NVIC_EnableIRQ(TIM7_IRQn);

}

// Starts the TIM7 counter if it was stopped
void TimerStart(void) {
    TIM7->CR1 |= TIM_CR1_CEN;
}

// Stops the TIM7 counter
void TimerStop(void) {
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

uint32_t Timer_GetValue(void) {
    return TIM7->CNT; // Return the current count value of TIM7
}

uint32_t Timer_GetElapsedTime(void) {
    return elapsed_time + Timer_GetValue(); // Add hardware counter to elapsed time
}


// The TIM7 interrupt handler runs every 1ms
void TIM7_IRQHandler(void) {
    // Check if update interrupt flag is set
    if (TIM7->SR & TIM_SR_UIF) {
        TIM7->SR &= ~TIM_SR_UIF; // Clear the flag

        // Increment elapsed time counter by 1 millisecond
        elapsed_time++;
    }
}

void Button_Interrupt(){
	Button_Init();
}

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
	Button_Init();
    LTCD__Init();
    TIM7_Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);
    visualDemo();
    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();
	while(returnTouchStateAndLocation(&StaticTouchData) != STMPE811_State_Pressed){
	    	HAL_Delay(100);
	    }
    LCD_Clear(0, LCD_COLOR_DARK_GREY);
	Game_Init();

	 while (1) {
	      if (!Game_IsOver()) {
	    	TimerStart();
	        Game_Update();
	      }
	      else {
	    	  EndGame();
	      }

	  }
	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;
	#if TOUCH_INTERRUPT_ENABLED == 1
	LCDTouchScreenInterruptGPIOInit();


	#endif // TOUCH_INTERRUPT_ENABLED

	#endif // COMPILE_TOUCH_FUNCTIONS
}


void LCD_Visual_Demo(void)
{
	 visualDemo_G();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}


// TouchScreen Interrupt
#if TOUCH_INTERRUPT_ENABLED == 1

void LCDTouchScreenInterruptGPIOInit(void)
{
	GPIO_InitTypeDef LCDConfig = {0};
    LCDConfig.Pin = GPIO_PIN_15;
    LCDConfig.Mode = GPIO_MODE_IT_RISING_FALLING;
    LCDConfig.Pull = GPIO_NOPULL;
    LCDConfig.Speed = GPIO_SPEED_FREQ_HIGH;
    
    // Clock enable
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // GPIO Init      
    HAL_GPIO_Init(GPIOA, &LCDConfig);

    // Interrupt Configuration
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	LCDTouchIRQ.Line = EXTI_LINE_15;

}

#define TOUCH_DETECTED_IRQ_STATUS_BIT   (1 << 0)  // Touchscreen detected bitmask

static uint8_t statusFlag;

void EXTI15_10_IRQHandler()
{
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); // May consider making this a universial interrupt guard
	bool isTouchDetected = false;

	static uint32_t count;
	count = 0;
	while(count == 0){
		count = STMPE811_Read(STMPE811_FIFO_SIZE);
	}

	// Disable touch interrupt bit on the STMPE811
	uint8_t currentIRQEnables = ReadRegisterFromTouchModule(STMPE811_INT_EN);
	WriteDataToTouchModule(STMPE811_INT_EN, 0x00);

	// Clear the interrupt bit in the STMPE811
	statusFlag = ReadRegisterFromTouchModule(STMPE811_INT_STA);
	uint8_t clearIRQData = (statusFlag | TOUCH_DETECTED_IRQ_STATUS_BIT); // Write one to clear bit
	WriteDataToTouchModule(STMPE811_INT_STA, clearIRQData);
	
	uint8_t ctrlReg = ReadRegisterFromTouchModule(STMPE811_TSC_CTRL);
	if (ctrlReg & 0x80)
	{
		isTouchDetected = true;
	}

	// Determine if it is pressed or unpressed
	if(isTouchDetected) // Touch has been detected
	{
		printf("\nPressed");
		// May need to do numerous retries? 
		DetermineTouchPosition(&StaticTouchData);
		/* Touch valid */
		printf("\nX: %03d\nY: %03d \n", StaticTouchData.x, StaticTouchData.y);

	}else{

		/* Touch not pressed */
		printf("\nNot pressed \n");
	}

	STMPE811_Write(STMPE811_FIFO_STA, 0x01);
	STMPE811_Write(STMPE811_FIFO_STA, 0x00);

	// Re-enable IRQs
    WriteDataToTouchModule(STMPE811_INT_EN, currentIRQEnables);
	HAL_EXTI_ClearPending(&LCDTouchIRQ, EXTI_TRIGGER_RISING_FALLING);

	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	//Potential ERRATA? Clearing IRQ bit again due to an IRQ being triggered DURING the handling of this IRQ..
	WriteDataToTouchModule(STMPE811_INT_STA, clearIRQData);

}
#endif // TOUCH_INTERRUPT_ENABLED

void EXTI0_IRQHandler() {
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    RotateTetromino();
    EXTI_ClearPending();
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


#endif // COMPILE_TOUCH_FUNCTIONS


