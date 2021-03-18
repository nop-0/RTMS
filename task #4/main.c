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
////////////////////////////////////////////////////////////////////////////////
extern int Init_Thread(void);
 
int KeypadVal, volume;
float ADCVal = 0,lastADC = 0;
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
	}

}

void thread3_task(void const *argument)
{
	while(1)
	{
		seven_segment_value_input_VR(ADCVal);					// 7SegDisp for VR1 value 
		seven_segment_value_input_keypad(KeypadVal);	// modified VR1 exclusive 7SegDisp function to intake keypad values		
	}
}

void thread4_task(void const *argument)
{
	while(1)
	{
		lastADC = ADCVal;
	
		volume = (lastADC/255)*127;														// mapping VR1 to LCD for the volume bar
		RectangleFill(0,16,volume,29,FG_COLOR, BG_COLOR);			// controls the black bar
		RectangleFill(127,16,volume+2,29,BG_COLOR, FG_COLOR); // controls the white bar 
																																	
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
	Init_Thread();													//initialize Threading
	
	osThreadCreate(osThread(thread1_task),NULL); //create threads
	osThreadCreate(osThread(thread2_task),NULL);
	osThreadCreate(osThread(thread3_task),NULL);
	osThreadCreate(osThread(thread4_task),NULL);
	
  osKernelStart ();                         // start thread execution 
}
