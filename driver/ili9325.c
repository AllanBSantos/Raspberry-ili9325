#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../tft_lib.h"
#include "ili9325.h"

void ili9325_lcdInit(TFT_t *dev, int width, int height, int offsetx, int offsety) {
	lcdInit(dev, 0x9325, width, height, offsetx, offsety);
	//printf("ili9325_lcdInit _model=%x\n", dev->_model);

	static const uint16_t regValues[] = {
		0x00E5, 0x78F0,		// set SRAM internal timing
		0x0001, 0x0100,		// set Driver Output Control
		0x0002, 0x0200,		// set 1 line inversion
		0x0003, 0x1030,		// set GRAM write direction and BGR=1.
		0x0004, 0x0000,		// Resize register
		0x0005, 0x0000,		// .kbv 16bits Data Format Selection
		0x0008, 0x0207,		// set the back porch and front porch
		0x0009, 0x0000,		// set non-display area refresh cycle ISC[3:0]
		0x000A, 0x0000,		// FMARK function
		0x000C, 0x0000,		// RGB interface setting
		0x000D, 0x0000,		// Frame marker Position
		0x000F, 0x0000,		// RGB interface polarity
		// ----------- Power On sequence ----------- //
		0x0010, 0x0000,		// SAP, BT[3:0], AP, DSTB, SLP, STB
		0x0011, 0x0007,		// DC1[2:0], DC0[2:0], VC[2:0]
		0x0012, 0x0000,		// VREG1OUT voltage
		0x0013, 0x0000,		// VDV[4:0] for VCOM amplitude
		0x0007, 0x0001,
		TFTLCD_DELAY, 200,	// Dis-charge capacitor power voltage
		0x0010, 0x1690,		// SAP=1, BT=6, APE=1, AP=1, DSTB=0, SLP=0, STB=0
		0x0011, 0x0227,		// DC1=2, DC0=2, VC=7
		TFTLCD_DELAY, 50,	// wait_ms 50ms
		0x0012, 0x000D,		// VCIRE=1, PON=0, VRH=5
		TFTLCD_DELAY, 50,	// wait_ms 50ms
		0x0013, 0x1200,		// VDV=28 for VCOM amplitude
		0x0029, 0x000A,		// VCM=10 for VCOMH
		0x002B, 0x000D,		// Set Frame Rate
		TFTLCD_DELAY, 50,	// wait_ms 50ms
		0x0020, 0x0000,		// GRAM horizontal Address
		0x0021, 0x0000,		// GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//

		0x0030, 0x0000,
		0x0031, 0x0404,
		0x0032, 0x0003,
		0x0035, 0x0405,
		0x0036, 0x0808,
		0x0037, 0x0407,
		0x0038, 0x0303,
		0x0039, 0x0707,
		0x003C, 0x0504,
		0x003D, 0x0808,

		//------------------ Set GRAM area ---------------//
		0x0060, 0x2700,	  // Gate Scan Line GS=0 [0xA700] 
		//0x0060, 0xA700,		// Gate Scan Line GS=0 [0xA700] 
		0x0061, 0x0001,		// NDL,VLE, REV .kbv
		0x006A, 0x0000,		// set scrolling line
		//-------------- Partial Display Control ---------//
		0x0080, 0x0000,
		0x0081, 0x0000,
		0x0082, 0x0000,
		0x0083, 0x0000,
		0x0084, 0x0000,
		0x0085, 0x0000,
		//-------------- Panel Control -------------------//
		0x0090, 0x0010,
		0x0092, 0x0000,
		0x0007, 0x0133,		// 262K color and display ON
	};
	lcdInitTable16(dev, regValues, sizeof(regValues));

	// ili9325 custom function
	DrawPixel = ili9325_lcdDrawPixel;
	DrawFillRect = ili9325_lcdDrawFillRect;
	DisplayOff = ili9325_lcdDisplayOff;
	DisplayOn = ili9325_lcdDisplayOn;
	InversionOff = ili9325_lcdInversionOff;
	InversionOn = ili9325_lcdInversionOn;
}

// Draw pixel
// x:X coordinate
// y:Y coordinate
// color:color
void ili9325_lcdDrawPixel(TFT_t *dev, uint16_t x, uint16_t y, uint16_t color) {
	if (x >= dev->_width) return;
	if (y >= dev->_height) return;

	uint16_t _x = x + dev->_offsetx;
	uint16_t _y = y + dev->_offsety;

	lcdWriteRegisterWord(dev, 0x0020, _x); // RAM Address Set 1
	lcdWriteRegisterWord(dev, 0x0021, _y); // RAM Address Set 2
	lcdWriteRegisterWord(dev, 0x0022, color); // Write Data to GRAM
}

// Draw rectangule of filling
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color
void ili9325_lcdDrawFillRect(TFT_t *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	if (x1 >= dev->_width) return;
	if (x2 >= dev->_width) x2 = dev->_width-1;
	if (y1 >= dev->_height) return;
	if (y2 >= dev->_height) y2 = dev->_height-1;

	uint16_t _x1 = x1 + dev->_offsetx;
	uint16_t _x2 = x2 + dev->_offsetx;
	uint16_t _y1 = y1 + dev->_offsety;
	uint16_t _y2 = y2 + dev->_offsety;
	int _y;
	for(_y=_y1;_y<=_y2;_y++){
		lcdWriteRegisterWord(dev, 0x0020, _x1); // RAM Address Set 1
		lcdWriteRegisterWord(dev, 0x0021, _y); // RAM Address Set 2
		lcdWriteCommandWord(dev, 0x0022); // Write Data to GRAM
		int _x;
		for(_x=_x1;_x<=_x2;_x++){
			lcdWriteDataWord(dev, color); // Write Data to GRAM
		}
	}
}

// Display OFF
void ili9325_lcdDisplayOff(TFT_t *dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x0000); // Set GON=0 DTE=0 D1=0, D0=0
}

// Display ON
void ili9325_lcdDisplayOn(TFT_t *dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x0173); // Set GON=1 DTE=1 D1=1, D0=1
}

// Display Inversion OFF
void ili9325_lcdInversionOff(TFT_t * dev) {
	lcdWriteRegisterWord(dev, 0x0061, 0x0001);
}

// Display Inversion ON
void ili9325_lcdInversionOn(TFT_t * dev) {
	lcdWriteRegisterWord(dev, 0x0061, 0x0000);
}
