/*
 * LCD_Driver.h
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#ifndef INC_LCD_DRIVER_H_
#define INC_LCD_DRIVER_H_

#include "stm32f4xx_hal.h"
#include "ili9341.h"
#include "fonts.h"
#include "stmpe811.h"

#define COMPILE_TOUCH_FUNCTIONS COMPILE_TOUCH
#define TOUCH_INTERRUPT_ENABLED COMPILE_TOUCH_INTERRUPT_SUPPORT

/**
  * @brief  LCD color RGB565
  */

#define LCD_PIXEL_FORMAT_1     LTDC_PIXEL_FORMAT_RGB565

#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_GREY          0xF7DE
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_BLUE2         0x051F
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_CYAN          0x7FFF
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_PURPLE 0x780F
#define LCD_COLOR_ORANGE 0xFD20

// Macros to define RGB565 colors
#define RGB565(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3)))

// Highlight Colors (Lighter Shades)
#define LCD_COLOR_LIGHT_BLUE   RGB565(135, 206, 250) // Light Sky Blue
#define LCD_COLOR_LIGHT_RED    RGB565(255, 128, 128) // Light Red
#define LCD_COLOR_LIGHT_GREEN  RGB565(144, 238, 144) // Light Green
#define LCD_COLOR_LIGHT_YELLOW RGB565(255, 255, 150) // Light Yellow
#define LCD_COLOR_LIGHT_PURPLE RGB565(200, 160, 255) // Light Purple
#define LCD_COLOR_LIGHT_ORANGE RGB565(255, 200, 150) // Light Orange
#define GRID_COLOR	0xF7DE
// Shadow Colors (Darker Shades)
#define LCD_COLOR_DARK_BLUE    RGB565(0, 0, 139)     // Dark Blue
#define LCD_COLOR_DARK_RED     RGB565(139, 0, 0)     // Dark Red
#define LCD_COLOR_DARK_GREEN   RGB565(0, 100, 0)     // Dark Green
#define LCD_COLOR_DARK_YELLOW  RGB565(139, 139, 0)   // Olive
#define LCD_COLOR_DARK_PURPLE  RGB565(128, 0, 128)   // Purple
#define LCD_COLOR_DARK_ORANGE  RGB565(255, 140, 0)   // Dark Orange
#define LCD_COLOR_DARK_GREY RGB565(64, 64, 64) // Dark Grey

// Assuming you have a 240x320 screen:
// Left arrow occupies a rectangular area at (0, 200) to (40, 240)
#define LEFT_ARROW_X_MIN 0
#define LEFT_ARROW_X_MAX 40
#define LEFT_ARROW_Y_MIN 200
#define LEFT_ARROW_Y_MAX 240

// Right arrow occupies a rectangular area at (200, 200) to (240, 240)
#define RIGHT_ARROW_X_MIN 200
#define RIGHT_ARROW_X_MAX 240
#define RIGHT_ARROW_Y_MIN 200
#define RIGHT_ARROW_Y_MAX 240


// Playfield dimensions
#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20

// Block size and field offsets
#define BLOCK_SIZE 16
#define FIELD_START_X 40
#define FIELD_START_Y 0
#define WALL_THICKNESS 5
#define FIELD_END_Y (FIELD_START_Y + PLAYFIELD_HEIGHT * BLOCK_SIZE)
#define FIELD_END_X (FIELD_START_X + PLAYFIELD_WIDTH * BLOCK_SIZE)

// Speed settings
#define FALL_SPEED 500 // Adjust for gravity effect

// Declare tetromino-related symbols
#define TETROMINO_COUNT 7
extern const uint8_t tetrominos[TETROMINO_COUNT][4][4];


/* Timing configuration from datahseet
  HSYNC=10 (9+1)
  HBP=20 (29-10+1)
  ActiveW=240 (269-20-10+1)
  HFP=10 (279-240-20-10+1)

  VSYNC=2 (1+1)
  VBP=2 (3-2+1)
  ActiveH=320 (323-2-2+1)
  VFP=4 (327-320-2-2+1)
*/
#define  ILI9341_HSYNC            ((uint32_t)9)   /* Horizontal synchronization */
#define  ILI9341_HBP              ((uint32_t)29)    /* Horizontal back porch      */
#define  ILI9341_HFP              ((uint32_t)2)    /* Horizontal front porch     */
#define  ILI9341_VSYNC            ((uint32_t)1)   /* Vertical synchronization   */
#define  ILI9341_VBP              ((uint32_t)3)    /* Vertical back porch        */
#define  ILI9341_VFP              ((uint32_t)2)    /* Vertical front porch       */
#define  LCD_PIXEL_WIDTH    ((uint16_t)240)
#define  LCD_PIXEL_HEIGHT   ((uint16_t)320)
#define  LCD_PIXELS		     ((uint32_t)LCD_PIXEL_WIDTH * (uint32_t)LCD_PIXEL_HEIGHT)

void LTCD__Init(void);
void LTCD_Layer_Init(uint8_t LayerIndex);

void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetFont(FONT_t *fonts);
// Draw Circle Filled
void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color);

// Draw Vertical Line
void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void LCD_Clear(uint8_t LayerIndex, uint16_t Color);

void LCD_Error_Handler(void);

typedef struct {
    uint16_t mainColor;
    uint16_t outlineColor;
    uint16_t highlightColor;
    uint16_t shadowColor;
} TetrominoColor_t;

extern TetrominoColor_t TetrominoColors[TETROMINO_COUNT];



// Tetris game functions
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, const char *str);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawRectangle(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color);
void LCD_SetBackgroundColor(uint16_t Color);
void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void LCD_Draw_Tetromino_Block(uint16_t Xpos, uint16_t Ypos, uint16_t Size, uint16_t MainColor, uint16_t OutlineColor, uint16_t HighlightColor, uint16_t ShadowColor);
void LCD_Draw_Tetromino_Shape(uint16_t Xpos, uint16_t Ypos, uint16_t Size, const uint8_t Shape[4][4], uint16_t MainColor, uint16_t OutlineColor, uint16_t HighlightColor, uint16_t ShadowColor);
void LCD_Display_All_Tetrominos(void);
void DrawGameField(void);
void DrawArrows(void);
void DrawGrid();
// Demo using provided functions
void visualDemo(void);
void visualDemo_G(void);
void LCD_Error_Handler(void);

#if COMPILE_TOUCH_FUNCTIONS == 1
void InitializeLCDTouch(void);
STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct);
void LCD_Touch_Polling_Demo(void);
void DetermineTouchPosition(STMPE811_TouchData * touchStruct);
uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead);
void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData);

#endif // COMPILE_TOUCH_FUNCTIONS


/*        APPLICATION SPECIFIC FUNCTION DECLARATION - PUT YOUR NEWLY CREATED FUNCTIONS HERE       */


#endif /* INC_LCD_DRIVER_H_ */
