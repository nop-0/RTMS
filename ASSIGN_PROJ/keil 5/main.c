/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "sys.c"
#include "clk.c"
#include "gpio.c"
#include "spi.c"
#include "ADC.c"
////////////////////////////////////////////////////////////////////////////////
#include "Seven_Segment_Display.c"
#include "Keypad.c"
#include "lcd.c"
#include "Variable_Resistor.c"
#include "drive.c"
////////////////////////////////////////////////////////////////////////////////
extern int Init_Thread(void);

int KeypadVal;
int Score = 0, Lives = 10, gameFlag = 1;   
float ADCVal = 0;

char Title[10] = "[SKYFALL]";
char guide1[20] = ">Avoid the Aliens<";
char guide2[30] = "Press any button to start";
////////////////////////////////////////////////////////////////////////////////


void thread1_task(void const *argument) // dedicated thread for handling 7SegDisp
{
	seven_segment_begin();
}

void thread2_task(void const *argument)
{
	while(1)
	{
		KeypadVal = Keypad_Return();				// captures Keypad input
		ADCVal = Variable_Resistor_Value(); // returns VR1 value from 0-255
		gameFlag = gameOver(gameFlag, Lives, Score);
	}

}

void thread3_task(void const *argument)
{
	while(gameFlag)
	{
		Lives = livesUpdate(Lives);
		Score = scoresUpdate(Score);
		seven_segment_value_input(Score);				
		
		if (Lives <= 9) PC12=0;        
		if (Lives <= 6) PC13=0;                   
		if (Lives <= 3) PC14=0;
		if (Lives <= 1) PC15=0;
		
	}
}

void thread4_task(void const *argument)
{
	while(gameFlag)
	{
			gameLoop2(ADCVal);
			//gameLoop(KeypadVal);
	}
}
////////////////////////////////////////////////////////////////////////
osThreadDef(thread1_task,osPriorityNormal,1,0); //declaring threads
osThreadDef(thread2_task,osPriorityNormal,1,0);
osThreadDef(thread3_task,osPriorityNormal,1,0);
osThreadDef(thread4_task,osPriorityNormal,1,0);
////////////////////////////////////////////////////////////////////////

int main (void) {															//main loop
  SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
	CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_Msk;
	SYS_LockReg();
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	InitVRADC();														//initialize VR1 and LCD
	init_LCD();
	clear_LCD();
	
	draw_Bmp16x8(0,0,FG_COLOR,BG_COLOR, Alien2);	  //start menu
	draw_Bmp16x8(10,10,FG_COLOR,BG_COLOR, Alien3);	
	draw_Bmp16x8(100,5,FG_COLOR,BG_COLOR, Alien4);
	draw_Bmp16x8(100,40,FG_COLOR,BG_COLOR, Alien2);		
	draw_Bmp16x8(10,40,FG_COLOR,BG_COLOR, Alien5);	
	draw_Bmp16x8(110,20,FG_COLOR,BG_COLOR, Alien1);
	draw_Bmp16x8(0,30,FG_COLOR,BG_COLOR, Alien1);
	printS(30, 9, Title);
	printS_5x7(18, 34, guide1);
	printS_5x7(0, 50, guide2);
	
	while(KeypadVal == 0 ) {KeypadVal = Keypad_Return();}	//starts the game
	clear_LCD();
		
	Init_Thread();													//initialize Threading
	
	osThreadCreate(osThread(thread1_task),NULL); //create threads
	osThreadCreate(osThread(thread2_task),NULL);
	osThreadCreate(osThread(thread3_task),NULL);
	osThreadCreate(osThread(thread4_task),NULL);
	
  osKernelStart ();                         // start thread execution 
}
