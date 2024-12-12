/*
 * LCD_Driver.c
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "Game_Driver.h"
#include "stmpe811.h"

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */

static LTDC_HandleTypeDef hltdc;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
static FONT_t *LCD_Currentfonts;
static uint16_t CurrentTextColor   = 0xFFFF;


/*
 * fb[y*W+x] OR fb[y][x]
 * Alternatively, we can modify the linker script to have an end address of 20013DFB instead of 2002FFFF, so it does not place variables in the same region as the frame buffer. In this case it is safe to just specify the raw address as frame buffer.
 */
//uint32_t frameBuffer[(LCD_PIXEL_WIDTH*LCD_PIXEL_WIDTH)/2] = {0};		//16bpp pixel format. We can size to uint32. this ensures 32 bit alignment


//Someone from STM said it was "often accessed" a 1-dim array, and not a 2d array. However you still access it like a 2dim array,  using fb[y*W+x] instead of fb[y][x].
uint16_t frameBuffer[LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT] = {0};			//16bpp pixel format.


void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the LTDC clock */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* GPIO Config
   *
    LCD pins
   LCD_TFT R2 <-> PC.10
   LCD_TFT G2 <-> PA.06
   LCD_TFT B2 <-> PD.06
   LCD_TFT R3 <-> PB.00
   LCD_TFT G3 <-> PG.10
   LCD_TFT B3 <-> PG.11
   LCD_TFT R4 <-> PA.11
   LCD_TFT G4 <-> PB.10
   LCD_TFT B4 <-> PG.12
   LCD_TFT R5 <-> PA.12
   LCD_TFT G5 <-> PB.11
   LCD_TFT B5 <-> PA.03
   LCD_TFT R6 <-> PB.01
   LCD_TFT G6 <-> PC.07
   LCD_TFT B6 <-> PB.08
   LCD_TFT R7 <-> PG.06
   LCD_TFT G7 <-> PD.03
   LCD_TFT B7 <-> PB.09
   LCD_TFT HSYNC <-> PC.06
   LCDTFT VSYNC <->  PA.04
   LCD_TFT CLK   <-> PG.07
   LCD_TFT DE   <->  PF.10
  */

  /* GPIOA configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 |
                           GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate= GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

 /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_8 | \
                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

 /* GPIOC configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

 /* GPIOD configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

 /* GPIOF configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

 /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | \
                           GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStructure.Alternate= GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void LTCD_Layer_Init(uint8_t LayerIndex)
{
	LTDC_LayerCfgTypeDef  pLayerCfg;

	pLayerCfg.WindowX0 = 0;	//Configures the Window HORZ START Position.
	pLayerCfg.WindowX1 = LCD_PIXEL_WIDTH;	//Configures the Window HORZ Stop Position.
	pLayerCfg.WindowY0 = 0;	//Configures the Window vertical START Position.
	pLayerCfg.WindowY1 = LCD_PIXEL_HEIGHT;	//Configures the Window vertical Stop Position.
	pLayerCfg.PixelFormat = LCD_PIXEL_FORMAT_1;  //INCORRECT PIXEL FORMAT WILL GIVE WEIRD RESULTS!! IT MAY STILL WORK FOR 1/2 THE DISPLAY!!! //This is our buffers pixel format. 2 bytes for each pixel
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	if (LayerIndex == 0){
		pLayerCfg.FBStartAdress = (uintptr_t)frameBuffer;
	}
	pLayerCfg.ImageWidth = LCD_PIXEL_WIDTH;
	pLayerCfg.ImageHeight = LCD_PIXEL_HEIGHT;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, LayerIndex) != HAL_OK)
	{
		LCD_Error_Handler();
	}
}

void clearScreen(void)
{
  LCD_Clear(0,LCD_COLOR_WHITE);
}

void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, const char *str) {
    while (*str) {
        LCD_DisplayChar(Xpos, Ypos, *str++);
        Xpos += LCD_Currentfonts->Width;
    }
}

void LTCD__Init(void)
{
	hltdc.Instance = LTDC;
	/* Configure horizontal synchronization width */
	hltdc.Init.HorizontalSync = ILI9341_HSYNC;
	/* Configure vertical synchronization height */
	hltdc.Init.VerticalSync = ILI9341_VSYNC;
	/* Configure accumulated horizontal back porch */
	hltdc.Init.AccumulatedHBP = ILI9341_HBP;
	/* Configure accumulated vertical back porch */
	hltdc.Init.AccumulatedVBP = ILI9341_VBP;
	/* Configure accumulated active width */
	hltdc.Init.AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	hltdc.Init.AccumulatedActiveH = 323;
	/* Configure total width */
	hltdc.Init.TotalWidth = 279;
	/* Configure total height */
	hltdc.Init.TotalHeigh = 327;
	/* Configure R,G,B component values for LCD background color */
	hltdc.Init.Backcolor.Red = 0;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;

	/* LCD clock configuration */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_8 = 48/4 = 6Mhz */

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	/* Polarity */
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

	LCD_GPIO_Init();

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	 {
	   LCD_Error_Handler();
	 }

	ili9341_Init();
}

/* START Draw functions */

/*
 * This is really the only function needed.
 * All drawing consists of is manipulating the array.
 * Adding input sanitation should probably be done.
 */
void LCD_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
	frameBuffer[y*LCD_PIXEL_WIDTH+x] = color;  //You cannot do x*y to set the pixel.
}

/*
 * These functions are simple examples. Most computer graphics like OpenGl and stm's graphics library use a state machine. Where you first call some function like SetColor(color), SetPosition(x,y), then DrawSqure(size)
 * Instead all of these are explicit where color, size, and position are passed in.
 * There is tons of ways to handle drawing. I dont think it matters too much.
 */
void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color)
{
    for(int16_t y=-radius; y<=radius; y++)
    {
        for(int16_t x=-radius; x<=radius; x++)
        {
            if(x*x+y*y <= radius*radius)
            {
            	LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
            }
        }
    }
}

void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(x, i+y, color);
  }
}

void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
    for (uint16_t i = 0; i < length; i++) {
        LCD_DrawPixel(x + i, y, color);
    }
}


void LCD_Clear(uint8_t LayerIndex, uint16_t Color)
{
	if (LayerIndex == 0){
		for (uint32_t i = 0; i < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; i++){
			frameBuffer[i] = Color;
		}
	}
  // TODO: Add more Layers if needed
}

//This was taken and adapted from stm32's mcu code
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

//This was taken and adapted from stm32's mcu code
void LCD_SetFont(FONT_t *fonts)
{
  LCD_Currentfonts = fonts;
}

//This was taken and adapted from stm32's mcu code
void LCD_Draw_Char(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0;
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) && (LCD_Currentfonts->Width <= 12)) || (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
         //Background If want to overrite text under then add a set color here
      }
      else
      {
    	  LCD_Draw_Pixel(counter + Xpos,index + Ypos,CurrentTextColor);
      }
    }
  }
}

//This was taken and adapted from stm32's mcu code
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_Draw_Char(Xpos, Ypos, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

void visualDemo(void)
{
	uint16_t x, y;

	    // Create a striped background using alternating colors
	    for (y = 0; y < LCD_PIXEL_HEIGHT; y++) {
	        for (x = 0; x < LCD_PIXEL_WIDTH; x++) {
	            if (x & 32)
	                frameBuffer[y * LCD_PIXEL_WIDTH + x] = LCD_COLOR_GREY;
	            else
	                frameBuffer[y * LCD_PIXEL_WIDTH + x] = LCD_COLOR_BLACK;
	        }
	    }

	    HAL_Delay(1500);

	    // Display the "TETRIS" logo with T-shaped Tetromino for the "T"
	    LCD_Clear(0, LCD_COLOR_DARK_GREY);
	    LCD_SetTextColor(LCD_COLOR_RED);
	    LCD_SetFont(&Font16x24);

	    // Draw T-shaped Tetromino as "T" in "TETRIS"
	    const uint8_t T_Shape[4][4] = {
	        {1, 1, 1, 0},
	        {0, 1, 0, 0},
	        {0, 1, 0, 0},
	        {0, 0, 0, 0}
	    };

	    LCD_Draw_Tetromino_Shape(
	        50, 50, 20, T_Shape,
	        LCD_COLOR_BLUE,     // Main Color
	        LCD_COLOR_BLACK,    // Outline Color
	        LCD_COLOR_LIGHT_BLUE, // Highlight Color
	        LCD_COLOR_DARK_BLUE   // Shadow Color
	    );
        HAL_Delay(500);
	    // Draw the rest of the "ETRIS" text
	    LCD_DisplayString(90, 100, "ETRIS");

	    HAL_Delay(2000);

	    // Display additional Tetromino shapes under the button
	        uint16_t startX = 40;
	        uint16_t startY = 210;
	        const uint8_t *shapes[TETROMINO_COUNT - 1] = {
	            tetrominos[0], // I-shape
	            tetrominos[1], // O-shape
	            tetrominos[3], // L-shape
	            tetrominos[4], // J-shape
	            tetrominos[5], // S-shape
	            tetrominos[6]  // Z-shape
	        };
	        TetrominoColor_t colors[TETROMINO_COUNT - 1] = {
	            TetrominoColors[0], // I-shape
	            TetrominoColors[1], // O-shape
	            TetrominoColors[3], // L-shape
	            TetrominoColors[4], // J-shape
	            TetrominoColors[5], // S-shape
	            TetrominoColors[6]  // Z-shape
	        };

	        for (int i = 0; i < TETROMINO_COUNT - 1; i++) {
	            LCD_Draw_Tetromino_Shape(
	                startX + (i * 40), startY, 10, shapes[i],
	                colors[i].mainColor, colors[i].outlineColor,
	                colors[i].highlightColor, colors[i].shadowColor
	            );
	        }

	    // Add instruction text
	    LCD_SetTextColor(LCD_COLOR_YELLOW);
	    LCD_SetFont(&Font12x12);
	    LCD_DisplayString(40, 180, "PRESS TO PLAY");

}

void visualDemo_G(void) {
    LCD_Clear(0, LCD_COLOR_BLACK); // Clear the screen with black background
    Game_Init();
    Game_Draw_Field();               // Draw the grid and playfield
    Game_Draw_Tetromino();
    DrawArrows();                  // Draw the navigation buttons (left/right arrows)
}






/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void LCD_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


// Draw a single pixel
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x < LCD_PIXEL_WIDTH && y < LCD_PIXEL_HEIGHT) {
        frameBuffer[y * LCD_PIXEL_WIDTH + x] = color;
    }
}

// Draw a filled rectangle
void LCD_DrawRectangle(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color) {
    for (uint16_t y = 0; y < Height; y++) {
        for (uint16_t x = 0; x < Width; x++) {
            LCD_DrawPixel(Xpos + x, Ypos + y, Color);
        }
    }
}

static uint16_t BackgroundColor = 0x0000; // Default to black

void LCD_SetBackgroundColor(uint16_t Color) {
    BackgroundColor = Color;
}


void LCD_Draw_Tetromino_Block(uint16_t Xpos, uint16_t Ypos, uint16_t Size, uint16_t MainColor, uint16_t OutlineColor, uint16_t HighlightColor, uint16_t ShadowColor) {
    // Draw the outline rectangle
    LCD_DrawRectangle(Xpos, Ypos, Size, Size, OutlineColor);

    // Draw the highlight (top side)
    LCD_DrawRectangle(Xpos + 1, Ypos + 1, Size - 2, Size / 3, HighlightColor);

    // Draw the shadow (bottom side)
    LCD_DrawRectangle(Xpos + 1, Ypos + Size - Size / 3 - 1, Size - 2, Size / 3, ShadowColor);

    // Draw the inner block color (center part)
    LCD_DrawRectangle(Xpos + 1, Ypos + Size / 3, Size - 2, Size - 2 * (Size / 3), MainColor);
}

void LCD_Draw_Tetromino_Shape(uint16_t Xpos, uint16_t Ypos, uint16_t Size, const uint8_t Shape[4][4], uint16_t MainColor, uint16_t OutlineColor, uint16_t HighlightColor, uint16_t ShadowColor) {
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            if (Shape[y][x]) {
                LCD_Draw_Tetromino_Block(
                    Xpos + x * Size,
                    Ypos + y * Size,
                    Size,
                    MainColor,
                    OutlineColor,
                    HighlightColor,
                    ShadowColor
                );
            }
        }
    }
}

void DrawArrows(void) {
    // Draw a simple filled rectangle for the left arrow area
    LCD_DrawRectangle(LEFT_ARROW_X_MIN, LEFT_ARROW_Y_MIN,
                      LEFT_ARROW_X_MAX - LEFT_ARROW_X_MIN,
                      LEFT_ARROW_Y_MAX - LEFT_ARROW_Y_MIN,
                      LCD_COLOR_BLUE);

    // Draw a simple filled rectangle for the right arrow area
    LCD_DrawRectangle(RIGHT_ARROW_X_MIN, RIGHT_ARROW_Y_MIN,
                      RIGHT_ARROW_X_MAX - RIGHT_ARROW_X_MIN,
                      RIGHT_ARROW_Y_MAX - RIGHT_ARROW_Y_MIN,
                      LCD_COLOR_BLUE);;

    // Print "<" symbol in left button
    LCD_DisplayChar(LEFT_ARROW_X_MIN + 10, LEFT_ARROW_Y_MIN + 10, '<');

    // Print ">" symbol in right button
    LCD_DisplayChar(RIGHT_ARROW_X_MIN + 10, RIGHT_ARROW_Y_MIN + 10, '>');
}



void DrawGameField() {
    for (int x = FIELD_START_X; x < FIELD_END_X; x += BLOCK_SIZE) {
        LCD_Draw_Vertical_Line(x, FIELD_START_Y, FIELD_END_Y, GRID_COLOR);
    }
    for (int y = FIELD_START_Y; y < FIELD_END_Y; y += BLOCK_SIZE) {
        LCD_Draw_Horizontal_Line(FIELD_START_X, y, FIELD_END_X, GRID_COLOR);
    }
}

void DrawGrid(){
	LCD_Clear(0,LCD_COLOR_BLACK);
	for(int x =0; x<=240; x+=24){
		LCD_Draw_Vertical_Line(x,0,320,LCD_COLOR_WHITE);
	}
	for(int y =0; y<=320; y+=32){
		LCD_Draw_Horizontal_Line(0,y,240,LCD_COLOR_WHITE);
	}
}

// Touch Functionality   //

#if COMPILE_TOUCH_FUNCTIONS == 1

void InitializeLCDTouch(void)
{
  if(STMPE811_Init() != STMPE811_State_Ok)
  {
	 for(;;); // Hang code due to error in initialzation
  }
}

STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct)
{
	return STMPE811_ReadTouch(touchStruct);
}

void DetermineTouchPosition(STMPE811_TouchData * touchStruct)
{
	STMPE811_DetermineTouchPosition(touchStruct);
}

uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead)
{
	return STMPE811_Read(RegToRead);
}

void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData)
{
	STMPE811_Write(RegToWrite, writeData);
}



#endif // COMPILE_TOUCH_FUNCTIONS
