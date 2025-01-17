#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../tft_lib.h"
#include "ili9225.h"

void ili9225_lcdInit(TFT_t *dev, int width, int height, int offsetx, int offsety) {
	lcdInit(dev, 0x9225, width, height, offsetx, offsety);
	//printf("ili9225_lcdInit _model=%x\n", dev->_model);

	static const uint16_t regValues[] = {
		/* Set SS bit and direction output from S528 to S1 */
		0x10, 0x0000, // Set SAP,DSTB,STB
		0x11, 0x0000, // Set APON,PON,AON,VCI1EN,VC
		0x12, 0x0000, // Set BT,DC1,DC2,DC3
		0x13, 0x0000, // Set GVDD
		0x14, 0x0000, // Set VCOMH/VCOML voltage
		TFTLCD_DELAY16, 40,

		// Power-on sequence
		0x11, 0x0018, // Set APON,PON,AON,VCI1EN,VC
		0x12, 0x6121, // Set BT,DC1,DC2,DC3
		0x13, 0x006F, // Set GVDD	/*007F 0088 */
		0x14, 0x495F, // Set VCOMH/VCOML voltage
		0x10, 0x0800, // Set SAP,DSTB,STB
		TFTLCD_DELAY16, 10,
		0x11, 0x103B, // Set APON,PON,AON,VCI1EN,VC
		TFTLCD_DELAY16, 50,

		//0x01, 0x011C, // set the display line number and display direction
		0x01, 0x001C, // set the display line number and display direction
		//0x01, 0x021C, // set the display line number and display direction
		0x02, 0x0100, // set 1 line inversion
		0x03, 0x1010, // set GRAM write direction and BGR=1.
		//0x03, 0x1030, // set GRAM write direction and BGR=1.
		0x07, 0x0000, // Display off
		0x08, 0x0808, // set the back porch and front porch
		0x0B, 0x1100, // set the clocks number per line
		0x0C, 0x0000, // CPU interface
		0x0F, 0x0D01, // Set Osc  /*0e01*/
		0x15, 0x0020, // Set VCI recycling
		0x20, 0x0000, // Horizontal GRAM Address Set
		0x21, 0x0000, // Vertical GRAM Address Set

		/* Set GRAM area */
		0x30, 0x0000,
		0x31, 0x00DB,
		0x32, 0x0000,
		0x33, 0x0000,
		0x34, 0x00DB,
		0x35, 0x0000,
		0x36, 0x00AF,
		0x37, 0x0000,
		0x38, 0x00DB,
		0x39, 0x0000,

		/* Set GAMMA curve */
		0x50, 0x0000,
		0x51, 0x0808,
		0x52, 0x080A,
		0x53, 0x000A,
		0x54, 0x0A08,
		0x55, 0x0808,
		0x56, 0x0000,
		0x57, 0x0A00,
		0x58, 0x0710,
		0x59, 0x0710,

		0x07, 0x0012,
		TFTLCD_DELAY16, 50,
		0x07, 0x1017,
	};
	lcdInitTable16(dev, regValues, sizeof(regValues));

	// ili9225 custom function
	DrawPixel = ili9225_lcdDrawPixel;
	DrawFillRect = ili9225_lcdDrawFillRect;
	DisplayOff = ili9225_lcdDisplayOff;
	DisplayOn = ili9225_lcdDisplayOn;
	InversionOff = ili9225_lcdInversionOff;
	InversionOn = ili9225_lcdInversionOn;
}

// Draw pixel
// x:X coordinate
// y:Y coordinate
// color:color
void ili9225_lcdDrawPixel(TFT_t *dev, uint16_t x, uint16_t y, uint16_t color) {
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
void ili9225_lcdDrawFillRect(TFT_t *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
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
			//lcdWriteRegisterWord(dev, 0x0020, _x); // RAM Address Set 1
			//lcdWriteRegisterWord(dev, 0x0021, _y); // RAM Address Set 2
			//lcdWriteCommandWord(dev, 0x0022); // Write Data to GRAM
			lcdWriteDataWord(dev, color); // Write Data to GRAM
		}
	}
}

// Display OFF
void ili9225_lcdDisplayOff(TFT_t *dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x0000); // Set GON=0 DTE=0 D1=0, D0=0
}

// Display ON
void ili9225_lcdDisplayOn(TFT_t *dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x1017); // Set GON=1 DTE=1 D1=1, D0=1
}

// Display Inversion OFF
void ili9225_lcdInversionOff(TFT_t * dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x0013);
}

// Display Inversion ON
void ili9225_lcdInversionOn(TFT_t * dev) {
	lcdWriteRegisterWord(dev, 0x0007, 0x0017);
}
