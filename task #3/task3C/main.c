#include <stdio.h>
#include <string.h>
#include "NUC1xx.h"
#include "GPIO.h"
#include "SPI.h"
#include "SYS.h"
#include "LCD.h"

#define  ONESHOT  0   // counting and interrupt when reach TCMPR number, then stop
#define  PERIODIC 1   // counting and interrupt when reach TCMPR number, then counting from 0 again
#define  TOGGLE   2   // keep counting and interrupt when reach TCMPR number, tout toggled (between 0 and 1)
#define  CONTINUOUS 3 // keep counting and interrupt when reach TCMPR number

volatile uint32_t ledState0 = 0; //tracks changes in LED state
volatile uint32_t ledState1 = 0;
volatile uint32_t ledState2 = 0;

static uint16_t Timer0Counter=0; //sets up counter for each timer
static uint16_t Timer1Counter=0;
static uint16_t Timer2Counter=0;


void InitTIMER0(void)
{         
	SYSCLK->CLKSEL1.TMR0_S = 0;	
	SYSCLK->APBCLK.TMR0_EN = 1;
	
	TIMER0->TCSR.MODE=PERIODIC;		

	TIMER0->TCSR.PRESCALE=255;	
	TIMER0->TCMPR = 46875; // 1 second
	
	TIMER0->TCSR.IE = 1;
	TIMER0->TISR.TIF = 1;		
	NVIC_EnableIRQ(TMR0_IRQn);

	TIMER0->TCSR.CRST = 1;		
	TIMER0->TCSR.CEN = 1;		
}

void TMR0_IRQHandler(void) 
{
	char TEXT0[16]="Time Cnt0: ";
	ledState0 = ~ ledState0;  // changing ON/OFF state
	if(ledState0)
	{
		DrvGPIO_ClrBit(E_GPC,12); //increases counter everytime GPC is LOW
		Timer0Counter+=1;
	}
	else DrvGPIO_SetBit(E_GPC,12);   
	sprintf(TEXT0+10,"%d",Timer0Counter);
	print_Line(1, TEXT0);
 	TIMER0->TISR.TIF =1;
}

void InitTIMER1(void)
{
	SYSCLK->CLKSEL1.TMR1_S = 0;	
  SYSCLK->APBCLK.TMR1_EN = 1;

	TIMER1->TCSR.MODE=TOGGLE;

	TIMER1->TCSR.PRESCALE=255;
	TIMER1->TCMPR = 93750; // 2 seconds							

	TIMER1->TCSR.IE = 1;
	TIMER1->TISR.TIF = 1;		
	NVIC_EnableIRQ(TMR1_IRQn);	

	TIMER1->TCSR.CRST = 1;
	TIMER1->TCSR.CEN = 1;
}

void TMR1_IRQHandler(void) 
{
	char TEXT1[16]="Time Cnt1: ";
	ledState1 = ~ ledState1;  // changing ON/OFF state
	if(ledState1) 
	{
		DrvGPIO_ClrBit(E_GPC,13);
		Timer1Counter+=1;
	}
	else DrvGPIO_SetBit(E_GPC,13); 
	
 	sprintf(TEXT1+10,"%d",Timer1Counter);
	print_Line(2, TEXT1);
 	TIMER1->TISR.TIF =1; 
}

void InitTIMER2(void)
{       
	SYSCLK->CLKSEL1.TMR2_S = 0;	
  SYSCLK->APBCLK.TMR2_EN = 1;

	TIMER2->TCSR.MODE=TOGGLE;

	TIMER2->TCSR.PRESCALE=255;
	TIMER2->TCMPR = 187500; // 4 seconds
							
	TIMER2->TCSR.IE = 1;
	TIMER2->TISR.TIF = 1;
	NVIC_EnableIRQ(TMR2_IRQn);

	TIMER2->TCSR.CRST = 1;
	TIMER2->TCSR.CEN = 1;
}

void TMR2_IRQHandler(void) // Timer2 interrupt subroutine 
{
	char TEXT2[16]="Time Cnt2: ";
	ledState2 = ~ ledState2;  // changing ON/OFF state	
	if(ledState2) 
	{
		DrvGPIO_ClrBit(E_GPC,14);
		Timer2Counter+=1;
	}
	else DrvGPIO_SetBit(E_GPC,14);
	
	sprintf(TEXT2+10,"%d",Timer2Counter);
	print_Line(3, TEXT2);
 	TIMER2->TISR.TIF =1;	
}

void INIT_LED()
{
	DrvGPIO_Open(E_GPC,12,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPC,13,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPC,14,E_IO_OUTPUT);

	DrvGPIO_SetBit(E_GPC,12);
	DrvGPIO_SetBit(E_GPC,13);
	DrvGPIO_SetBit(E_GPC,14);
}

int main(void)
{
	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1;//Enable 12MHz Crystal
	DrvSYS_Delay(5000); // waiting for 12MHz crystal stable
	SYSCLK->CLKSEL0.HCLK_S = 0;
	LOCKREG();
	
	init_LCD();
	INIT_LED();
	clear_LCD();
	
	InitTIMER0();
	InitTIMER1();
	InitTIMER2();	
	
	while(1)
	{
	}	
}