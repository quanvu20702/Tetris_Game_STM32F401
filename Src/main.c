 /* File name: STM32_Project_Game_Tetris
 *
 * Description:xây dựng game tetris trên LCD
 *
 *
 * Last Changed By:  $Author: $ VTQUAN & BMNGOC
 * Revision:         $Revision: $
 * Last Changed:     $Date: $December 25, 2023
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <timer.h>
#include <system_stm32f4xx.h>
#include <lightsensor.h>
#include <Ucglib.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>
#include "stm32f401re_syscfg.h"
#include "stm32f401re_exti.h"


/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/
// Define GPIO_PIN*************************************************************/
// Button B2
#define BUTTON_LEFT_B2_PORT			GPIOB
#define BUTTON_LEFT_B2_PIN			GPIO_Pin_3

// Button B4
#define BUTTON_RIGHT_B4_PORT		GPIOB
#define BUTTON_RIGHT_B4_PIN			GPIO_Pin_0

// Button B1
#define BUTTON_UP_B1_PORT			GPIOB
#define BUTTON_UP_B1_PIN			GPIO_Pin_5

// Button B3
#define BUTTON_PLAY_B3_PORT			GPIOA
#define BUTTON_PLAY_B3_PIN			GPIO_Pin_4

// Define Screen Parameters************************************************/
#define SCREEN_WIDTH 12
#define SCREEN_HEIGHT 24


/* Private variables*******************************************************/
ucg_t ucg;
static int x_axis = 10;
static uint32_t idTimer = NO_TIMER;
uint32_t idTimerStopGame = NO_TIMER;
uint8_t eCurrentState;
static int size = 5;
static int currentX = 10;
static int currentY = -9;
int checkRow = 0;
int checkColumn = 0;
int deleteRow = 0;
static int screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static char str1[10] = "";
static int currentRotation = 1;
int currentBlock[4][4];
int nextBlock[4][4];
int blockIndex;
int (*block)[4] = currentBlock;
static int score = 0;
static int maxx;
static int minx;

/* Function prototypes*****************************************************/

static void initializeApplication(void);
void loadConfiguration(void);
void initScreen(void);
void initializeCurrentBlock(void);
void chooseBlock(void);
void drawBlockOnScreen(int x, int y,  int block[4][4]);
void eraseBlockOnScreen(int x, int y, int block[4][4]);
int checkForCollisions (int x, int y, int block[4][4]);
void fixBlockOnScreen(int x, int y, int block[4][4]);
void handleFallingBlock(int block[4][4]);
void startFallingBlockTimer(int block[4][4]);
void handleFullRows(void);
void checkForGameOver(void);
void updateScore(void);
int CalculateBlockMaxWidth(int block[4][4], int x0);
int calculateBlockWidth(int block[4][4]);
void rotateCurrentBlock(void);

static void initB1ButtonInterrupt(void);
static void initB2ButtonInterrupt(void);
static void initB3ButtonInterrupt(void);
static void initB4ButtonInterrupt(void);

/* Block Init**************************************************************/
static int block_T1[4][4] =
{
	{0,1,0,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_T2[4][4] =
{
	{0,1,0,0},
	{1,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_T3[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_T4[4][4] =
{
	{0,1,0,0},
	{0,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};

static int block_J1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{1,1,0,0},
	{0,0,0,0}
};
static int block_J2[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{0,0,1,0},
	{0,0,0,0}
};
static int block_J3[4][4] =
{
	{0,1,1,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_J4[4][4] =
{
	{1,0,0,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_L1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};
static int block_L2[4][4] =
{
	{0,0,1,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_L3[4][4] =
{
	{1,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_L4[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{1,0,0,0},
	{0,0,0,0}
};
static int block_Z1[4][4] =
{
	{0,0,0,0},
	{0,1,1,0},
	{0,0,1,1},
	{0,0,0,0}
};
static int block_Z2[4][4] =
{
	{0,0,1,0},
	{0,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_Z3[4][4] =
{
	{0,0,0,0},
	{0,0,1,1},
	{0,1,1,0},
	{0,0,0,0}
};
static int block_Z4[4][4] =
{
	{0,1,0,0},
	{0,1,1,0},
	{0,0,1,0},
	{0,0,0,0}
};
static int block_I1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0}
};
static int block_I2[4][4] =
{
	{0,0,0,0},
	{1,1,1,1},
	{0,0,0,0},
	{0,0,0,0}
};

static int block_O[4][4] =
{
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
/**
 * @func   initB1ButtonInterrupt
 * @brief  Initialize Interrupt for Button B1
 * @param  None
 * @retval None
 */
static void initB1ButtonInterrupt(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	// Enable Clock Port B;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_UP_B1_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//  Enable Clock Syscfg, Connect EXTI Line 5 to PB5 pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);

	//	Configuration Interrupt
	EXTI_InitStructure.EXTI_Line =  EXTI_Line5;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	// Configuration NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   initB2ButtonInterrupt
 * @brief  Initialize Interrupt for Button B2
 * @param  None
 * @retval None
 */
static void initB2ButtonInterrupt(void) {
    // Enable Clock Port B
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // Configure GPIO for Button B2
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT_B2_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Enable Clock for Syscfg, Connect EXTI Line 3 to PB3 pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);

    // Configure EXTI for Button B2
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC for Button B2
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   initB3ButtonInterrupt
 * @brief  Initialize Interrupt for Button B3
 * @param  None
 * @retval None
 */
static void initB3ButtonInterrupt(void) {
    // Enable Clock Port A
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Configure GPIO for Button B3
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PLAY_B3_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Enable Clock for Syscfg, Connect EXTI Line 4 to PA4 pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

    // Configure EXTI for Button B3
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC for Button B3
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   initB4ButtonInterrupt
 * @brief  Initialize Interrupt for Button B4
 * @param  None
 * @retval None
 */
static void initB4ButtonInterrupt(void) {
    // Enable Clock Port B
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // Configure GPIO for Button B4
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT_B4_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Enable Clock for Syscfg, Connect EXTI Line 0 to PB0 pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);

    // Configure EXTI for Button B4
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC for Button B4
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   loadConfiguration
 * @brief  Initializes the configuration for the Tetris game.
 * @param  None
 * @retval None
 */
void loadConfiguration(void) {
	ucg_DrawFrame(&ucg, 0, 0, 62, 122);					// Draw a frame for the game area
	ucg_DrawString(&ucg, 65, 12, 0, "Tetris Game");		// Display the Tetris Game title
	ucg_DrawString(&ucg, 65, 48, 0, "Score: ");			// Display the initial score
}

/**
 * @func   initScreen
 * @brief  Initializes the screen array
 * @param  None
 * @retval None
 */
void initScreen(void) {
	for (int i = -4; i < SCREEN_HEIGHT; i++) {
	        for (int j = 0; j < SCREEN_WIDTH; j++) {
	            screen[i][j] = 0;
	        }
	    }
}
// Init 19 blocks of game
int (*blockTypes[])[4] = {  block_T1, block_T2, block_T3, block_T4,
							block_J1, block_J2, block_J3, block_J4,
							block_Z1, block_Z2, block_Z3, block_Z4,
							block_L1, block_L2, block_L3, block_L4,
							block_I1, block_I2, block_O};
int (*block_T[])[4] = {block_T1, block_T2, block_T3, block_T4};
int (*block_J[])[4] = {block_J1, block_J2, block_J3, block_J4};
int (*block_Z[])[4] = {block_Z1, block_Z2, block_Z3, block_Z4};
int (*block_L[])[4] = {block_L1, block_L2, block_L3, block_L4};
int (*block_I[])[4] = {block_I1, block_I2};


/**
 * @func   chooseBlock
 * @brief  Random Block Selection
 * @param  None
 * @retval None
 */
void chooseBlock(void) {
    int randomBlockIndex = LightSensor_MeasureUseDMAMode() % 19;
    int (*randomBlock)[4] = blockTypes[randomBlockIndex];
    blockIndex = randomBlockIndex;
	// Copy the selected block to the nextBlock array
    memcpy(nextBlock, randomBlock, sizeof(nextBlock));
}

/**
 * @func   initializeCurrentBlock
 * @brief  Initializes the current block by choosing a new block.
 * @param  None
 * @retval None
 */
void initializeCurrentBlock(void) {
	chooseBlock();
	// Copy values from nextBlock to currentBlock
    memcpy(currentBlock, nextBlock, sizeof(currentBlock));
}

/**
 * @func   drawBlockOnDisplay
 * @brief  Draw a block on the graphics screen
  * @param  x: X-coordinate of the block's top-left corner
 *         	y: Y-coordinate of the block's top-left corner
 *         	block: 2D array representing the block to be drawn
 * @retval None
 */
void drawBlockOnScreen(int x, int y,  int block[4][4]) {
	ucg_SetColor(&ucg, 0, 0, 255, 255);
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1) {
				int px = x + j*size+1;
				int py = y + i*size;
				ucg_DrawBox(&ucg, px, py, size, size);
			}
		}
	}
}

/**
 * @func   eraseBlockOnScreen
 * @brief  Erase a block on the graphics screen
 * @param  x: X-coordinate of the block's top-left corner
 *         y: Y-coordinate of the block's top-left corner
 *         block: 2D array representing the block to be erased
 * @retval None
 */
void eraseBlockOnScreen(int x, int y, int block[4][4]) {
	ucg_SetColor(&ucg, 0, 0, 0, 0);
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1) {
				int px = x + j*size+1;
				int py = y + i*size;
				ucg_DrawBox(&ucg, px, py, size, size);
			}
		}
	}
}

/**
 * @func   checkForCollisions
 * @brief  Check for collisions between a block and the screen boundaries or other elements on the screen.
 * @param  x: X-coordinate of the block's top-left corner
 *         y: Y-coordinate of the block's top-left corner
 *         block: 2D array representing the block
 * @retval 1 if there is a collision, 0 otherwise
 */
int checkForCollisions(int x, int y, int block[4][4]) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1){
				int px = x + j*size;
				int py = y + i*size;
				// Check for collision with screen boundaries
                if (py >= SCREEN_HEIGHT * size) return 1;

                // Check for collision with existing elements on the screen
                if (screen[py / 5][px / 5] == 1) return 1;
			}
		}
	}
	return 0; // No collision
}

/**
 * @func   fixBlockOnScreen
 * @brief  Fix a block onto the game screen.
 * @param  x: X-coordinate of the block's top-left corner
 *         y: Y-coordinate of the block's top-left corner
 *         block: 2D array representing the block
 * @retval None
 */
void fixBlockOnScreen(int x, int y, int block[4][4]) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1){
				int px = x + j*size;
				int py = y + i*size;
				screen[py/5][px/5] = 1;
			}
		}
	}
}

/**
 * @func   updateScore
 * @brief  Update the displayed score on the graphical screen.
 * @param  None
 * @retval None
 */
void updateScore(){
	ucg_SetColor(&ucg, 0, 255, 0, 0);
	memset(str1,0,sizeof(str1));
	sprintf(str1,"Score: %d",score);
	ucg_DrawString(&ucg, 65, 48, 0, str1);
}

/**
 * @func   handleFullRows
 * @brief  Handle full rows in the game screen by updating the score and adjusting the screen.
 * @param  None
 * @retval None
 */
void handleFullRows(){
	ucg_SetColor(&ucg, 0, 0, 0, 0);

	for(int i = 0; i < SCREEN_HEIGHT; i++){

		for(int j = 0; j < SCREEN_WIDTH; j++){
			if(screen[i][j] == 1){
				checkRow++;

				if(checkRow == SCREEN_WIDTH){
					// Update score
					score+=5;
					// Erase the full row
					ucg_SetColor(&ucg, 0, 0, 0, 0);
					ucg_DrawBox(&ucg,1,i*5+1,SCREEN_WIDTH*5,5);
					// Clear the full row in the screen array
					for(int k = 0; k<SCREEN_WIDTH; k++){
						screen[i][k] = 0;
					}

					// Draw the remaining blocks above the cleared row
					ucg_SetColor(&ucg, 0, 0, 0, 0);
					for(int ii = 4; ii <= i; ii++){
						for(int jj = 0; jj < SCREEN_WIDTH; jj++) {
							if(screen[ii][jj] == 1) {
								int px =  jj*size+1;
								int py =  ii*size+1;
								ucg_DrawBox(&ucg, px, py, size, size);
							}
						}
					}

					// Shift the blocks above the clears row down
					for(int new_height = i; new_height >= 5; new_height--){
						for(int new_width = SCREEN_WIDTH - 1; new_width >= 0; new_width--){
							screen[new_height][new_width] = screen[new_height - 1][new_width];//neu day man hinh thi code nay sai}
						}
					}

					// Redraw the shifted blocks
					ucg_SetColor(&ucg,0,0,255,255);
					for(int ii = 4; ii <= i; ii++){
						for(int jj = 0; jj < SCREEN_WIDTH; jj++) {
							if(screen[ii][jj] == 1) {
								int px =  jj*size+1;
								int py =  ii*size+1;
								ucg_DrawBox(&ucg, px, py, size, size);
							}
						}
					}

					checkRow = 0;
				}
			}
			else{
				checkRow = 0;
				break;
			}
		}
	}
}

/**
 * @func   checkForGameOver
 * @brief  Check if the first row of the game screen has any block; if true, handle "Game Over".
 * @param  None
 * @retval None
 */
void checkForGameOver(){
	for(int i = 0 ; i < SCREEN_WIDTH;i++){
		if(screen[0][i] == 1){
			ucg_SetColor(&ucg, 0, 255, 0, 0);
			ucg_DrawString(&ucg, 4 ,60, 0, "Game Over");
			TimerStop(idTimer);
		}
	}
}

/**
 * @func   handleFallingBlock
 * @brief  Handle the falling and placement of a block in the game, including collision detection and game state updates.
 * @param  block: 2D array representing the falling block
 * @retval None
 */
void handleFallingBlock(int block[4][4]){
	drawBlockOnScreen(currentX, currentY, block);
	handleFullRows();
	// Check for collisions with the block below
	if(!checkForCollisions(currentX, currentY + size, block)) {
		// If no collision, erase the block and move it down
		eraseBlockOnScreen(currentX, currentY, block);
		currentY += size;
	}
	else {
		// If collision, fix the block on the screen, reset position, choose a new block
		fixBlockOnScreen(currentX, currentY, block);
		currentX = 25;
		currentY = -9;
		x_axis = currentX;
		chooseBlock();
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = nextBlock[i][j];
	        }
	    }
		// Check for game over
		checkForGameOver();
	}
	// Update the minx and maxx values based on block width and position
	minx = (CalculateBlockMaxWidth(block, x_axis) - calculateBlockWidth(block))*size;
	maxx = CalculateBlockMaxWidth(block, x_axis)*size;
	// Update the currentX position if it changed
	if(x_axis != currentX) {
		currentX = x_axis;
	}
	// Update the score display
	updateScore();
}

/**
 * @func   startFallingBlockTimer
 * @brief  Start a timer to handle the falling of a block at a specified interval.
 * @param  block: 2D array representing the falling block
 * @retval None
 */
void startFallingBlockTimer(int block[4][4]) {
	// Stop the timer if it's already running
	if (idTimer != NO_TIMER) {
		TimerStop(idTimer);
	}
	// Start a timer to handle the falling block at a specified interval
	idTimer = TimerStart("Fall", 500, TIMER_REPEAT_FOREVER, (void*) handleFallingBlock, block);
}

//Measure the distance of the blocks to the left and right borders so that the blocks do not go beyond the screen
/**
 * @func   InterruptPA4_Init
 * @brief  Init Interrupt
 * @param  None
 * @retval None
 */
int CalculateBlockMaxWidth(int block[4][4], int x0) {
    int max_width = -1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[i][j] == 1) {
                int cellX = x0 + j;
                if (cellX > max_width) {
                    max_width = cellX;
                }
            }
        }
    }

    return max_width - abs(x0) + 1;
}

/**
 * @func   calculateBlockWidth
 * @brief  Calculate the width of a block.
 * @param  block: 2D array representing the block
 * @retval Width of the block
 */
int calculateBlockWidth(int block[4][4]) {
    int width = 0;

    for (int i = 0; i < 4; i++) {
        int rowWidth = 0;
        for (int j = 0; j < 4; j++) {
            if (block[i][j] == 1) {
                rowWidth++;
            }
        }
        if (rowWidth > width) {
            width = rowWidth;
        }
    }

    return width;
}

/**
 * @func   rotateCurrentBlock
 * @brief  Rotate the current block based on its type and handle collisions.
 * @param  None
 * @retval None
 */
void rotateCurrentBlock(void) {
	int nextRotation = (currentRotation + 1)%4;
	int tempBlock[4][4];
	// Determine block type and perform rotation
	if (blockIndex >= 0 && blockIndex < 4) {
		blockIndex = (blockIndex + currentRotation)%4;
	    int (*rotBlock)[4] = block_T[blockIndex];
	    if (!checkForCollisions(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
	    }
	    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
	    currentRotation = nextRotation;
	}
	// Determine block type and perform rotation
	if (blockIndex >= 4 && blockIndex < 8) {
		blockIndex = (blockIndex - 4 + currentRotation)%4;
	    int (*rotBlock)[4] = block_J[blockIndex];
	    if (!checkForCollisions(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
	    }
	    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
	    currentRotation = nextRotation;
	    blockIndex += 4;
	}
	// Determine block type and perform rotation
	if (blockIndex >= 8 && blockIndex < 12) {
		blockIndex = (blockIndex - 8 + currentRotation)%4;
	    int (*rotBlock)[4] = block_Z[blockIndex];
	    if (!checkForCollisions(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
	    }
	    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
	    currentRotation = nextRotation;
	    blockIndex += 8;
	}
	// Determine block type and perform rotation
	if (blockIndex >= 12 && blockIndex < 16) {
		blockIndex = (blockIndex - 12 + currentRotation)%4;
	    int (*rotBlock)[4] = block_L[blockIndex];
	    if (!checkForCollisions(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
	    }
	    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
	    currentRotation = nextRotation;
	    blockIndex += 12;
	}
	// Determine block type and perform rotation
	if (blockIndex >= 16 && blockIndex < 18) {
		blockIndex = (blockIndex - 16 + currentRotation)%2;
		int (*rotBlock)[4] = block_I[blockIndex];
		if (!checkForCollisions(currentX, currentY, rotBlock)) {
			for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
		}
		for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
		currentRotation = nextRotation;
		blockIndex += 16;
		}
}


/**
 * @func   EXTI3_IRQHandler
 * @brief  Interrupt Service Routine for handling Button B2 to move left.
 * @param  None
 * @retval None
 */
void EXTI3_IRQHandler(void)//Button B2 for go left
{
	// Check if moving left is within the boundary
	if(EXTI_GetFlagStatus(EXTI_Line3) == SET)
	{
		if (x_axis + minx >= size) {
			x_axis -= size;
		}
	}
	// Clear the EXTI line 3 pending bit
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/**
 * @func   EXTI0_IRQHandler
 * @brief  Interrupt Service Routine for handling Button B4 to move right.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void) {
    if (EXTI_GetFlagStatus(EXTI_Line0) == SET) {
        // Check if moving right is within the boundary
        if (x_axis + maxx <= (SCREEN_WIDTH - 1) * size) {
            x_axis += size;
        }
    }
    // Clear the EXTI line 0 pending bit
    EXTI_ClearITPendingBit(EXTI_Line0);
}

/**
 * @func   EXTI4_IRQHandler
 * @brief  Interrupt Service Routine for handling Button B3 to start the game.
 * @param  None
 * @retval None
 */
void EXTI4_IRQHandler(void) {
    if (EXTI_GetFlagStatus(EXTI_Line4) == SET) {
        // Start the falling block timer when Button B3 is pressed
        startFallingBlockTimer(block);
    }
    // Clear the EXTI line 4 pending bit
    EXTI_ClearITPendingBit(EXTI_Line4);
}


/**
 * @func   EXTI5_IRQHandler
 * @brief  Interrupt Service Routine for handling Button B1 to rotate the current block.
 * @param  None
 * @retval None
 */
void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetFlagStatus(EXTI_Line5) == SET) {
        // Rotate the current block when Button B1 is pressed
        rotateCurrentBlock();
    }
    // Clear the EXTI line 5 pending bit
    EXTI_ClearITPendingBit(EXTI_Line5);
}

/**
 * @func   initializeApplication
 * @brief  Common initialization routine for the application.
 * @param  None
 * @retval None
 */
static void initializeApplication(void){
	// Update the system core clock
    SystemCoreClockUpdate();

    // Initialize timers
    TimerInit();

    // Initialize the current block
    initializeCurrentBlock();

    // Initialize the light sensor in DMA mode
    LightSensor_Init(ADC_READ_MODE_DMA);

    // Initialize interrupts for buttons B1, B2, B3, and B4
    initB1ButtonInterrupt();
    initB2ButtonInterrupt();
    initB4ButtonInterrupt();
    initB3ButtonInterrupt();

    // Begin the initialization of the LCD using the Ucglib4WireSWSPI library
    Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);

    // Clear the screen
    ucg_ClearScreen(&ucg);

    // Set the font and color for the LCD
    ucg_SetFont(&ucg, ucg_font_ncenR08_hr);
    ucg_SetColor(&ucg, 0, 255, 255, 255);
    ucg_SetColor(&ucg, 1, 0, 0, 0);

    // Rotate the display 180 degrees
    ucg_SetRotate180(&ucg);
}

int main(void) {
    // Initialize the application
    initializeApplication();

    // Load configuration settings
    loadConfiguration();

    // Main application loop
    while (1) {
        // Process the timer scheduler
        processTimerScheduler();
    }

    // The program should never reach this point
    return 0;
}
/**********************************************************************************/
