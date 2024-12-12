/*
 * Game_driver.h
 *
 *  Created on: Dec 9, 2024
 *      Author: bilim
 */

#ifndef INC_GAME_DRIVER_H_
#define INC_GAME_DRIVER_H_

#include "ApplicationCode.h"
#include "LCD_Driver.h"
#include "stmpe811.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

// Game states
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_RESULTS
} GameState_t;

// Tetromino definitions
#define TETROMINO_COUNT 7
extern const uint8_t tetrominos[TETROMINO_COUNT][4][4];


// Function prototypes
void Game_Init(void);
void Game_Update(void);
void Game_Render(void);
void DisplayTetrisGridAndBlocks(void);
extern GameState_t currentGameState;
void RotateTetromino(void);
void Game_Draw_Field(void);
void Game_Draw_Tetromino(void);
void ClearActiveTetromino(void);
void Game_Handle_Input(void);
void SpawnTetromino(void);
void CopyTetrominoToPlayfield(void);
void EndGame(void);
void ClearFullRows(void);
bool CheckCollision(int8_t newX, int8_t newY);
uint8_t MoveTetromino(int8_t dx, int8_t dy);
bool Game_IsOver(void);
#endif /* INC_GAME_DRIVER_H_ */
