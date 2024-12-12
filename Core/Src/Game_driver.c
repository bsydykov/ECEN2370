/*
 * Game_Driver.c
 *
 *  Created on: Dec 9, 2024
 *      Author: bilim
 */

#include "Game_Driver.h"
#include "ApplicationCode.h"
#include "LCD_Driver.h"
#include "RNG.h"
#include "Button_driver.h"
#include <string.h> // For memcpy



// Tetromino definitions
const uint8_t tetrominos[TETROMINO_COUNT][4][4] = {
    // I-Tetromino
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // O-Tetromino
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // T-Tetromino
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // L-Tetromino
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // J-Tetromino
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // S-Tetromino
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // Z-Tetromino
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};



#define FIELD_WIDTH PLAYFIELD_WIDTH
#define FIELD_HEIGHT PLAYFIELD_HEIGHT

// Global variables for the game
uint32_t playfield[FIELD_HEIGHT][FIELD_WIDTH] = {0}; // Main playfield grid
uint8_t currentTetromino[4][4] = {0};             // Current active tetromino
uint8_t currentX = FIELD_WIDTH / 2 - 2;
uint8_t currentY = 0;
uint8_t currentRotation = 0;
uint8_t currentTetrominoType = 0;
uint16_t tetrominoColor;
uint16_t tetrominoOutlineColor = LCD_COLOR_BLACK;
uint16_t tetrominoHighlightColor = LCD_COLOR_GREY;
uint16_t tetrominoShadowColor = LCD_COLOR_DARK_GREY;
static bool gameOver = false;
static uint32_t lastFallTime = 0;

static TetrominoColor_t currentTetrominoColor; // Store the current tetromino's color

TetrominoColor_t TetrominoColors[TETROMINO_COUNT] = {
    {LCD_COLOR_CYAN, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_BLUE, LCD_COLOR_DARK_BLUE},    // I
    {LCD_COLOR_YELLOW, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_YELLOW, LCD_COLOR_DARK_YELLOW}, // O
    {LCD_COLOR_PURPLE, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_PURPLE, LCD_COLOR_DARK_PURPLE}, // T
    {LCD_COLOR_ORANGE, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_ORANGE, LCD_COLOR_DARK_ORANGE}, // L
    {LCD_COLOR_BLUE, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_BLUE, LCD_COLOR_DARK_BLUE},    // J
    {LCD_COLOR_GREEN, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_GREEN, LCD_COLOR_DARK_GREEN}, // S
    {LCD_COLOR_RED, LCD_COLOR_BLACK, LCD_COLOR_LIGHT_RED, LCD_COLOR_DARK_RED}        // Z
};

bool Game_IsOver(void) {
    return gameOver; // gameOver is a global variable in Game_Driver.c
}

void Game_Init(void) {
    memset(playfield, 0, sizeof(playfield));
    currentX = FIELD_WIDTH / 2 - 2;
    currentY = 0;
    SpawnTetromino();
    Game_Draw_Field();

}

void Game_Draw_Field(void) {
    // Draw background and grid if needed
    for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
        for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
            uint16_t xPos = FIELD_START_X + x * BLOCK_SIZE;
            uint16_t yPos = FIELD_START_Y + y * BLOCK_SIZE;
            if (playfield[y][x]) {
                // Draw block using a function similar to LCD_Draw_Tetromino_Block()
            	LCD_Draw_Tetromino_Block(xPos, yPos, BLOCK_SIZE,
            	                         playfield[y][x],          // MainColor
            	                         LCD_COLOR_BLACK,          // OutlineColor
            	                         LCD_COLOR_WHITE,          // HighlightColor
            	                         LCD_COLOR_DARK_GREY);     // ShadowColor

            } else {
                // Draw empty cell
                LCD_DrawRectangle(xPos, yPos, BLOCK_SIZE, BLOCK_SIZE, LCD_COLOR_BLACK);
            }
        }
    }
}

void Game_Draw_Tetromino(void) {
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                uint16_t xPos = FIELD_START_X + (currentX + x) * BLOCK_SIZE;
                uint16_t yPos = FIELD_START_Y + (currentY + y) * BLOCK_SIZE;
                LCD_Draw_Tetromino_Block(
                    xPos, yPos, BLOCK_SIZE,
                    currentTetrominoColor.mainColor,
                    currentTetrominoColor.outlineColor,
                    currentTetrominoColor.highlightColor,
                    currentTetrominoColor.shadowColor
                );
            }
        }
    }
}



void CopyTetrominoToPlayfield(void) {
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                uint8_t gridX = currentX + x;
                uint8_t gridY = currentY + y;
                if (gridX < FIELD_WIDTH && gridY < FIELD_HEIGHT) {
                    playfield[gridY][gridX] = tetrominoColor;
                }
            }
        }
    }
}


void SpawnTetromino(void) {
    // Choose a random tetromino type
    uint32_t randomValue = RNG_GetRandomNumber() % TETROMINO_COUNT;
    currentTetrominoType = (uint8_t)randomValue;

    // Copy the chosen tetromino shape into currentTetromino
    memcpy(currentTetromino, tetrominos[currentTetrominoType], sizeof(currentTetromino));

    // Set the tetromino color from the array
    currentTetrominoColor = TetrominoColors[currentTetrominoType];
    tetrominoColor = currentTetrominoColor.mainColor; // Important fix

    // Initial position
    currentX = FIELD_WIDTH / 2 - 2; // Center horizontally
    currentY = 0;

    // If spawning a piece immediately collides, game over
    if (CheckCollision(0, 0)) {
        EndGame();
    }
}



void ClearFullRows(void) {
    for (int8_t row = FIELD_HEIGHT - 1; row >= 0; row--) {
        uint8_t fullRow = 1;
        for (uint8_t col = 0; col < FIELD_WIDTH; col++) {
            if (!playfield[row][col]) {
                fullRow = 0;
                break;
            }
        }

        if (fullRow) {
            for (int8_t r = row; r > 0; r--) {
                memcpy(playfield[r], playfield[r - 1], sizeof(playfield[r]));
            }
            memset(playfield[0], 0, sizeof(playfield[0]));
            row++; // Recheck the row
        }
    }
}

bool CheckCollision(int8_t dx, int8_t dy) {
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                int newX = currentX + x + dx;
                int newY = currentY + y + dy;
                if (newX < 0 || newX >= FIELD_WIDTH || newY >= FIELD_HEIGHT) {
                    return true;
                }
                if (newY >= 0 && playfield[newY][newX]) {
                    return true;
                }
            }
        }
    }
    return false;
}

uint8_t MoveTetromino(int8_t dx, int8_t dy) {
    if (!CheckCollision(dx, dy)) {
        currentX += dx;
        currentY += dy;
        return 1;
    }
    return 0;
}

void RotateTetromino(void) {
    uint8_t nextRotation[4][4] = {0};
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            nextRotation[x][3 - y] = currentTetromino[y][x];
        }
    }

    uint8_t temp[4][4];
    memcpy(temp, currentTetromino, sizeof(currentTetromino));
    memcpy(currentTetromino, nextRotation, sizeof(currentTetromino));

    if (CheckCollision(0, 0)) {
        memcpy(currentTetromino, temp, sizeof(currentTetromino));
    }
}


void EndGame(void) {
    // Display game over message
	LCD_Clear(0, LCD_COLOR_LIGHT_BLUE);
    LCD_SetTextColor(LCD_COLOR_RED);
	LCD_SetFont(&Font16x24);
    LCD_DisplayString(50, 150, "GAME OVER");
    TimerStop();
    Timer_GetElapsedTime();
    uint32_t totalTime = Timer_GetElapsedTime(); // Use `elapsed_time` for total time
        uint32_t seconds = totalTime / 1000;
        uint32_t minutes = seconds / 60;
        seconds %= 60;

        char timeStr[20];
        sprintf(timeStr, "Time: %02lu:%02lu", minutes, seconds);

        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_SetFont(&Font12x12);
        LCD_DisplayString(50, 200, timeStr); // Display on LCD

        // Set game over flag
        gameOver = true;
    }

void Game_Update(void) {
    uint32_t currentTime = HAL_GetTick();

    // Drop the piece every FALL_SPEED ms
    if ((currentTime - lastFallTime) >= FALL_SPEED) {
        if (!MoveTetromino(0, 1)) {
            // Can't move down, lock piece into the field
            for (uint8_t y = 0; y < 4; y++) {
                for (uint8_t x = 0; x < 4; x++) {
                    if (currentTetromino[y][x]) {
                        uint8_t gridX = currentX + x;
                        uint8_t gridY = currentY + y;
                        if (gridY < FIELD_HEIGHT && gridX < FIELD_WIDTH) {
                            playfield[gridY][gridX] = tetrominoColor;
                        }
                    }
                }
            }
            ClearFullRows();
            SpawnTetromino();
        }
        lastFallTime = currentTime;
    }

    // Redraw field and piece
    Game_Draw_Field();
    Game_Draw_Tetromino();

}

