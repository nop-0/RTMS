#include <stdio.h>
#include "NUC1xx.h"
#include "SYS.h"
#include "SPI.h"
#include "LCD.h"
#include "Scankey.h"


unsigned char FIG1[32*8]=
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xF0,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFC,0x1C,0x1F,0x07,0x07,0x03,0xFF,0xFF,0x03,0x07,0x07,0x1C,0xFC,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x1F,0x7F,0x73,0xE3,0xE3,0xC3,0xFF,0xFF,0xC3,0xE3,0xF3,0x73,0x3F,0x07,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x1F,0x1F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

int32_t main(void)
{
	int8_t x,y;
	int8_t keyIN=0;
	int8_t movX=3, dirX=0;
	int8_t movY=3, dirY=0;
	
	int DELAY_TIME = 100000;
	
	UNLOCKREG();
	DrvSYS_Open(50000000);
	LOCKREG();
	
	init_LCD();
	clear_LCD();
	OpenKeyPad();
	
	x=32;
	y=16;
	draw_Bmp32x32(x,y,FG_COLOR,BG_COLOR,FIG1);
	
	while(keyIN==0) keyIN=ScanKey();
	clear_LCD();
	
	while (TRUE)
	{
		draw_Bmp32x32(x,y,FG_COLOR,BG_COLOR,FIG1);
		DrvSYS_Delay(DELAY_TIME);
		draw_Bmp32x32(x,y,BG_COLOR,BG_COLOR,FIG1);
		
		keyIN=ScanKey();
		
		switch(keyIN)
		{
			case 1:	
				dirX=-1; 
				dirY=-1;
				break;
			
			case 2: 
				dirX=0;  
				dirY=-1;	
				break;
			
			case 3: 
				dirX=+1; 
				dirY=-1;			
				break;
			
			case 4:	
				dirX=-1; 
				dirY=0;		
				break;
			
			case 5: 
				dirX=0;  
				dirY=0;		
				break;
			
			case 6: 
				dirX=+1; 
				dirY=0;	
				break;
		  
			case 7:	
				dirX=-1; 
				dirY=+1;		
				break;
			
			case 8: 
				dirX=0;  
				dirY=+1;			
				break;
			
			case 9: 
				dirX=+1; 
				dirY=+1;		
				break;
			
			default: break;		
		}	
		x = x + dirX*movX; //continous movement
		y = y + dirY*movY;
		
//		if(x<0)	x=LCD_Xmax-16; //loops the movement pass the boundary
//		if(x>LCD_Xmax-16)	x=0; 
//		if(y<0)	y=LCD_Ymax-16;
//		if(y>LCD_Ymax-16)	y=0;
		
		if(x<0)	
		{
			dirX=0;
			dirY=0;
		}						//checks for boundaries
		if(x>LCD_Xmax-35)
		{
			dirX=0;
			dirY=0;
		}
		if(y<0)
		{
			dirX=0;
			dirY=0;
		}	
		if(y>LCD_Ymax-35)
		{
			dirX=0;
			dirY=0;
		}	
	}
}