#include <stdio.h>
#include "NUC1xx.h"
#include "GPIO.h"
#include "SYS.h"
#include "ADC.h"
#include "LCD.h"

unsigned char FIG1[32*8]=
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xF0,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFC,0x1C,0x1F,0x07,0x07,0x03,0xFF,0xFF,0x03,0x07,0x07,0x1C,0xFC,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x1F,0x7F,0x73,0xE3,0xE3,0xC3,0xFF,0xFF,0xC3,0xE3,0xF3,0x73,0x3F,0x07,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x1F,0x1F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

int32_t main (void)
{
  int8_t x,y, locX, locY;
	uint16_t Vx, Vy;
	int DELAY_TIME = 100000;

 	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1; // enable external clock (12MHz)
	SYSCLK->CLKSEL0.HCLK_S = 0;	  // select external clock (12MHz)
	LOCKREG();

	init_LCD();  // initialize LCD pannel
	clear_LCD();  // clear LCD panel 
	x=32;
	y=16;
	draw_Bmp32x32(x,y,FG_COLOR,BG_COLOR,FIG1);

							 					 
	DrvADC_Open(ADC_SINGLE_END, ADC_SINGLE_CYCLE_OP, 0x03, INTERNAL_HCLK, 1); // ADC1 & ADC0	
	clear_LCD();  // clear LCD panel 
	
	while(1)
	{
		draw_Bmp32x32(x,y,FG_COLOR,BG_COLOR,FIG1);
		DrvSYS_Delay(DELAY_TIME);
		draw_Bmp32x32(x,y,BG_COLOR,BG_COLOR,FIG1);
		
		DrvADC_StartConvert();                   // start A/D conversion
    while(DrvADC_IsConversionDone()==FALSE) // wait till conversion is done
	  Vx = ADC->ADDR[0].RSLT & 0xFFF;
		Vy = ADC->ADDR[1].RSLT & 0xFFF;
		
		locX = Vx/48; //maps Vx and Vy value to LCD size parameters
		locY = Vy/128;
		
		x = locX; //continous movement
		y = locY;
	}
}
