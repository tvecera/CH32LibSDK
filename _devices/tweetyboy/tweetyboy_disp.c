
// ****************************************************************************
//
//                        TweetyBoy - Display driver
//
// ****************************************************************************

#include "../../includes.h"

#if USE_DISP		// 1=use software display driver, 2=use hardware display driver (0=no driver)

// ST7735 commands
#define ST7735_NOP		0x00	// no operation
#define ST7735_SWRESET		0x01	// software reset
#define ST7735_RDDID		0x04	// read display ID
#define ST7735_RDDST		0x09	// read display status
#define ST7735_RDDPM		0x0A	// read display power mode
#define ST7735_RDDMADCTL	0x0B	// read display MADCTL
#define ST7735_RDDCOLMOD	0x0C	// read display pixel format
#define ST7735_RDDIM		0x0D	// read display image mode
#define ST7735_RDDSM		0x0E	// read display signal mode

#define ST7735_SLPIN		0x10	// enable sleep mode, sleep mode IN
#define ST7735_SLPOUT		0x11	// disable sleep mode, sleep mode OUT
#define ST7735_PTLON		0x12	// partial display mode on
#define ST7735_NORON		0x13	// partial display mode off

#define ST7735_INVOFF		0x20	// disable inversion mode, display inversion off
#define ST7735_INVON		0x21	// enable inversion mode, display inversion on
#define ST7735_GAMSET		0x26	// gamma set
#define ST7735_DISPOFF		0x28	// disable display
#define ST7735_DISPON		0x29	// enable display
#define ST7735_CASET		0x2A	// set start and end column of draw window
#define ST7735_RASET		0x2B	// set start and end row of draw window
#define ST7735_RAMWR		0x2C	// start write data to draw window
#define ST7735_RAMRD		0x2E	// start read data from draw window

#define ST7735_PTLAR		0x30	// partial start/end address set
#define ST7735_TEOFF		0x34	// tearing effect line off
#define ST7735_TEON		0x35	// tearing effect line on
#define ST7735_MADCTL		0x36	// set rotation mode, memory data access control (data RotationTab)
#define ST7735_IDMOFF		0x38	// idle mode off
#define ST7735_IDMON		0x39	// idle mode on
#define ST7735_COLMOD		0x3A	// set color mode, interface pixel format (data COLOR_MODE_*)

//#define ST7735_RAMCTRL	0xB0	// RAM control, data 2 bytes:
// 1st byte (unused bits are 0; default 0x00):
//   B1,B0: interface 0 MCU, 1 RGB, 2 VSYNC
//   B4: ram access from 0 MCU, 1 RGB
// 2nd byte (default 0xF0):
//   B1,B0: method of pixel data transfer
//   B2: RGB interface bus width 0=18 bits, 1=6 bits
//   B3: endian 0=big MSB first, 1=little LSB first (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
//   B5,B4: align 65K data with bit 0: 0=equ 0, 1=equ 1, 2=equ high bit, 3=equ green bit 0
//   B7,B6: 1

#define ST7735_FRMCTR1		0xB1	// Frame Rate Control (In normal mode/ Full colors)
#define ST7735_FRMCTR2		0xB2	// Frame Rate Control (In Idle mode/ 8-colors)
#define ST7735_FRMCTR3		0xB3	// Frame Rate Control (In Partial mode/ full colors)
#define ST7735_INVCTR		0xB4	// Display Inversion Control
#define ST7735_DISSET5		0xB6	// Display Function setting

#define ST7735_PWCTR1		0xC0	// Power Control 1
#define ST7735_PWCTR2		0xC1	// Power Control 2
#define ST7735_PWCTR3		0xC2	// Power Control 3 (in Normal mode/ Full colors)
#define ST7735_PWCTR4		0xC3	// Power Control 4 (in Idle mode/ 8-colors)
#define ST7735_PWCTR5		0xC4	// Power Control 5 (in Partial mode/ full-colors)
#define ST7735_VMCTR1		0xC5	// VCOM Control 1
#define ST7735_VMOFCTR		0xC7	// set VCOM offset control

#define ST7735_WRID2		0xD1	// set LCM version code
#define ST7735_WRID3		0xD2	// customer Project code
#define ST7735_NVCTR1		0xD9	// EEPROM control status
#define ST7735_RDID1		0xDA	// read ID1
#define ST7735_RDID2		0xDB	// read ID2
#define ST7735_RDID3		0xDC	// read ID3
#define ST7735_NVCTR2		0xDE	// EEPROM Read Command
#define ST7735_NVCTR3		0xDF	// EEPROM Write Command

#define ST7735_GAMCTRP1		0xE0	// set Gamma adjustment (+ polarity)
#define ST7735_GAMCTRN1		0xE1	// set Gamma adjustment (- polarity)

#define ST7735_EXTCTRL		0xF0	// extension command control

#define ST7735_PWCTR6		0xFC	// Power Control 6 (In partial mode + Idle)

#define ST7735_VCOM4L		0xFF	// Vcom 4 level control

// Parameters of ST7735_MADCTL command
#define ST7735_MADCTL_MY	0x80	// B7: page address order (0=top to bottom, 1=bottom to top)
#define ST7735_MADCTL_MX	0x40	// B6: column address order (0=left to right, 1=right to left)
#define ST7735_MADCTL_MV	0x20	// B5: page/column order (0=normal mode, 1=reverse mode)
#define ST7735_MADCTL_ML	0x10	// B4: line address order (LCD refresh 0=top to bottom, 1=bottom to top)
#define ST7735_MADCTL_RGB	0x08	// B3: RGB/BGR order (0=RGB, 1=BGR)
#define ST7735_MADCTL_MH	0x04	// B2: display latch order (LD refresh 0=left to right, 1=right to left)

// color mode ST7735_COLMOD (select both color space and data format)
#define COLOR_MODE_65K		0x50	// 65K-color space (RGB 5-6-5)
#define COLOR_MODE_262K		0x60	// 262K-color space (RGB 6-6-6)
#define COLOR_MODE_12BIT	0x03	// 12-bit data, 4K-color space (RGB 4-4-4)
#define COLOR_MODE_16BIT	0x05	// 16-bit data (RGB 5-6-5)
#define COLOR_MODE_18BIT	0x06	// 18-bit data (RGB 6-6-6)
//#define COLOR_MODE_16M		0x07	// 24-bit data (RGB 8-8-8) ... not supported

// signals
#define CS_ON	GPIO_Out0(DISP_CS_GPIO); cb()	// activate chip selection
#define CS_OFF	cb(); GPIO_Out1(DISP_CS_GPIO)	// deactivate chip selection
#define RES_ON	GPIO_Out0(DISP_RES_GPIO); cb()	// activate reset
#define RES_OFF	cb(); GPIO_Out1(DISP_RES_GPIO)	// deactivate reset
#define DC_CMD	GPIO_Out0(DISP_DC_GPIO); cb()	// set command mode
#define DC_DATA	GPIO_Out1(DISP_DC_GPIO); cb()	// set data mode

// clock delay (it has also compiler barrier effect)
#if DISP_SPEED <= 0
#define DISP_CLK_WAIT()		cb()
#elif DISP_SPEED == 1
#define DISP_CLK_WAIT()		nop()
#elif DISP_SPEED == 2
#define DISP_CLK_WAIT()		{ nop(); nop(); }
#elif DISP_SPEED == 3
#define DISP_CLK_WAIT()		{ nop(); nop(); nop(); }
#elif DISP_SPEED == 4
#define DISP_CLK_WAIT()		{ nop(); nop(); nop(); nop(); }
#elif DISP_SPEED == 5
#define DISP_CLK_WAIT()		{ nop(); nop(); nop(); nop(); nop(); }
#elif DISP_SPEED == 6
#define DISP_CLK_WAIT()		{ nop(); nop(); nop(); nop(); nop(); nop(); }
#else
#define DISP_CLK_WAIT()		WaitClk(DISP_SPEED)
#endif

u8 FrameBuf[FRAMESIZE];		// display graphics buffer (1 pixel = 1 byte)
#include "pal/pal332.h"		// const u16 DefPalette[256];	// default palettes RGB332 in RGB565 format
const u16* Palette = PALETTE;	// pointer to palettes in RGB565 format

int DispUpdateX = 0;		// display update X coordinate
int DispUpdateY = 0;		// display update Y coordinate
int DispUpdateW = WIDTH;	// display update width
int DispUpdateH = HEIGHT;	// display update height

// rotation mode for ST7735_MADCTL
const u8 RotationTab[4] = {
	0 + ST7735_MADCTL_RGB,						// 0: Portrait
	ST7735_MADCTL_MX + ST7735_MADCTL_MV + ST7735_MADCTL_RGB,	// 1: Landscape
	ST7735_MADCTL_MY + ST7735_MADCTL_MX + ST7735_MADCTL_RGB,	// 2: Inverted Portrait
	ST7735_MADCTL_MY + ST7735_MADCTL_MV + ST7735_MADCTL_RGB,	// 3: Inverted Landscape
};

u8 Disp_OutCol = COL_WHITE; // current output color
u8 Disp_OutPage = 0;	// current output page (0..7)
u8 Disp_OutX = 0;	// current output X (0..127)

// display connect (activate chip selection)
void DispConnect(void)
{
	CS_ON;		// activate chip selection
}

// display disconnect (deactivate chip selection)
void DispDisconnect(void)
{
	CS_OFF; 	// deactivate chip selection
}

// write a byte to the display
void DispWriteByte(u8 data)
{
#if USE_DISP == 1	// 1=use software display driver, 2=use hardware display driver (0=no driver)
// Software driver:

	// bit 7
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B7)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 6
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B6)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 5
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B5)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 4
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B4)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 3
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B3)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 2
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B2)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 1
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B1)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

	// bit 0
	GPIOx_OutFast(GPIO_PORT(DISP_MOSI_GPIO), GPIO_PIN(DISP_MOSI_GPIO), (data & B0)); cb();
	GPIO_Out1(DISP_SCK_GPIO); cb();
	DISP_CLK_WAIT(); cb();
	GPIO_Out0(DISP_SCK_GPIO); cb();
//	DISP_CLK_WAIT(); cb();

#else
// Hardware driver:

	// write data byte
	SPI1_SendWait(data);
	while (SPI1_Busy()) {}	// wait transfer to end

#endif
}

// write command to display (display must be connected)
void DispWriteCmd(u8 cmd)
{
	DC_CMD;		// set command mode
	DispWriteByte(cmd); // send command to SPI
}

// write data to display (display must be connected)
void DispWriteData(u8 data)
{
	DC_DATA;	// set data mode
	DispWriteByte(data); // send data to SPI
}

// Display select simulated I2C page
void DispI2C_SelectPage(int page)
{
	Disp_OutPage = page;
	Disp_OutX = 0;
}

// write a byte over simulated I2C (write to frame buffer), using Disp_OutCol color
// - After write all data, send image to display with DispUpdate().
void DispI2C_Write(u8 data)
{
	int i;
	int y = Disp_OutPage << 3;
	int x = Disp_OutX;
	u8 col = Disp_OutCol;
	u8* d = &FrameBuf[x + 16 + (y + 8)*WIDTHBYTE];
	if ((data & B0) != 0) d[0*WIDTHBYTE] = col; else d[0*WIDTHBYTE] = COL_BLACK;
	if ((data & B1) != 0) d[1*WIDTHBYTE] = col; else d[1*WIDTHBYTE] = COL_BLACK;
	if ((data & B2) != 0) d[2*WIDTHBYTE] = col; else d[2*WIDTHBYTE] = COL_BLACK;
	if ((data & B3) != 0) d[3*WIDTHBYTE] = col; else d[3*WIDTHBYTE] = COL_BLACK;
	if ((data & B4) != 0) d[4*WIDTHBYTE] = col; else d[4*WIDTHBYTE] = COL_BLACK;
	if ((data & B5) != 0) d[5*WIDTHBYTE] = col; else d[5*WIDTHBYTE] = COL_BLACK;
	if ((data & B6) != 0) d[6*WIDTHBYTE] = col; else d[6*WIDTHBYTE] = COL_BLACK;
	if ((data & B7) != 0) d[7*WIDTHBYTE] = col; else d[7*WIDTHBYTE] = COL_BLACK;

	x++;
	if (x >= 128)
	{
		x = 0;
		Disp_OutPage = (Disp_OutPage + 1) & 7;
	}

	Disp_OutX = x;
}

// add byte over simulated I2C (write to frame buffer), write only '1' bits
// - After write all data, send image to display with DispUpdate().
void DispI2C_Add(int x, int y, u8 data, u8 col)
{
	y <<= 3;
	u8* d = &FrameBuf[x + 16 + (y + 8)*WIDTHBYTE];
	if ((data & B0) != 0) d[0*WIDTHBYTE] = col;
	if ((data & B1) != 0) d[1*WIDTHBYTE] = col;
	if ((data & B2) != 0) d[2*WIDTHBYTE] = col;
	if ((data & B3) != 0) d[3*WIDTHBYTE] = col;
	if ((data & B4) != 0) d[4*WIDTHBYTE] = col;
	if ((data & B5) != 0) d[5*WIDTHBYTE] = col;
	if ((data & B6) != 0) d[6*WIDTHBYTE] = col;
	if ((data & B7) != 0) d[7*WIDTHBYTE] = col;
}

// set byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Set(int x, int y, u8 data, u8 col)
{
	y <<= 3;
	u8* d = &FrameBuf[x + 16 + (y + 8)*WIDTHBYTE];
	if ((data & B0) != 0) d[0*WIDTHBYTE] = col; else d[0*WIDTHBYTE] = COL_BLACK;
	if ((data & B1) != 0) d[1*WIDTHBYTE] = col; else d[1*WIDTHBYTE] = COL_BLACK;
	if ((data & B2) != 0) d[2*WIDTHBYTE] = col; else d[2*WIDTHBYTE] = COL_BLACK;
	if ((data & B3) != 0) d[3*WIDTHBYTE] = col; else d[3*WIDTHBYTE] = COL_BLACK;
	if ((data & B4) != 0) d[4*WIDTHBYTE] = col; else d[4*WIDTHBYTE] = COL_BLACK;
	if ((data & B5) != 0) d[5*WIDTHBYTE] = col; else d[5*WIDTHBYTE] = COL_BLACK;
	if ((data & B6) != 0) d[6*WIDTHBYTE] = col; else d[6*WIDTHBYTE] = COL_BLACK;
	if ((data & B7) != 0) d[7*WIDTHBYTE] = col; else d[7*WIDTHBYTE] = COL_BLACK;
}

// clear byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Clr(int x, int y)
{
	y <<= 3;
	u8* d = &FrameBuf[x + 16 + (y + 8)*WIDTHBYTE];
	u8 col = COL_BLACK;
	d[0*WIDTHBYTE] = col;
	d[1*WIDTHBYTE] = col;
	d[2*WIDTHBYTE] = col;
	d[3*WIDTHBYTE] = col;
	d[4*WIDTHBYTE] = col;
	d[5*WIDTHBYTE] = col;
	d[6*WIDTHBYTE] = col;
	d[7*WIDTHBYTE] = col;
}

// set draw window, start sending data (display must be connected)
void DispWindow(u16 x1, u16 x2, u16 y1, u16 y2)
{
	// setup virtual window
	x1 = x1 + 1;
	x2 = x2 + 1 - 1;
	y1 = y1 + 26;
	y2 = y2 + 26 - 1;

	// set columns
	DispWriteCmd(ST7735_CASET);
	DispWriteData((u8)(x1>>8));
	DispWriteData((u8)x1);
	DispWriteData((u8)(x2>>8));
	DispWriteData((u8)x2);

	// set rows
	DispWriteCmd(ST7735_RASET);
	DispWriteData((u8)(y1>>8));
	DispWriteData((u8)y1);
	DispWriteData((u8)(y2>>8));
	DispWriteData((u8)y2);

	// send command to start sending data
	DispWriteCmd(ST7735_RAMWR);
}

// set backlight 1..9
// Total current, by backlight:
// 9: 256 ... 23mA (6+17mA)
// 8: 128 ... 16mA (6+10mA)
// 7: 64 .... 12mA (6+6mA)
// 6: 32 .... 9mA (6+3mA) *default*
// 5: 16 .... 8mA (6+2mA)
// 4: 8 ..... 7mA (6+1mA)
// 3: 4 ..... 7mA (6+1mA)
// 2: 2 ..... 7mA (6+1mA)
// 1: 1 ..... 6mA (6+0mA)
void DispBacklight(u8 backlight)
{
	if (backlight > BACKLIGHT_MAX) backlight = BACKLIGHT_MAX;
	if (backlight < BACKLIGHT_MIN) backlight = BACKLIGHT_MIN;
	int k = BIT(backlight-1);
	TIM3_Comp1(k);
}

// read backlight setup level 1..9 from Flash
u8 DispBacklightGet(void)
{
	u8 backlight = OB->Data0 - BACKLIGHT_FLASHOFF;
	if ((backlight < BACKLIGHT_MIN) || (backlight > BACKLIGHT_MAX)) backlight = BACKLIGHT_DEF;
	return backlight;
}

// write backlight setup level 1..9 to Flash
void DispBacklightSet(u8 backlight)
{
#if USE_FLASH		// 1=use Flash programming
	if (backlight > BACKLIGHT_MAX) backlight = BACKLIGHT_MAX;
	if (backlight < BACKLIGHT_MIN) backlight = BACKLIGHT_MIN;
	if (backlight != DispBacklightGet())
	{
		OB_t ob;
		Flash_OBRead(&ob);	// read option bytes
		ob.Data0 = backlight + BACKLIGHT_FLASHOFF; // set backlight level
		Flash_OBWrite(&ob);	// write new option bytes
	}
#endif
}

// clear backlight setup level to default value to Flash
void DispBacklightSetDef()
{
#if USE_FLASH		// 1=use Flash programming
	if (OB->Data0 != 0xff)
	{
		OB_t ob;
		Flash_OBRead(&ob);	// read option bytes
		ob.Data0 = 0xff;	// set default backlight level
		Flash_OBWrite(&ob);	// write new option bytes
	}
#endif
}

// update backlight from Flash setup
void DispBacklightUpdate(void)
{
	DispBacklight(DispBacklightGet());
}

// Display initialize (port clock must be enabled)
void DispInit(void)
{
#if USE_DISP == 1	// 1=use software display driver, 2=use hardware display driver (0=no driver)
// Software driver:

	// setup SPI pins
	GPIO_Out0(DISP_SCK_GPIO); GPIO_Mode(DISP_SCK_GPIO, GPIO_MODE_OUT);
	GPIO_Out0(DISP_MOSI_GPIO); GPIO_Mode(DISP_MOSI_GPIO, GPIO_MODE_OUT);

#else
// Hardware driver:

	// SPI setup
	RCC_SPI1ClkEnable();		// SPI1 clock enable
	RCC_SPI1Reset();		// SPI1 reset
	GPIO_Remap_SPI1(DISP_SPI_MAP);	// set SPI mapping
	SPI1_Baud(DISP_SPI_DIV);	// set SPI speed
	SPI1_Data8();			// 8-bit
	SPI1_MSB();			// MSB bit first
	SPI1_NSSSw();			// NSS controlled by software
	SPI1_NSSHigh();			// need set NSS to HIGH to enable master output
	SPI1_SSDisable();		// disable SS output
	SPI1_Bidi1();			// set 1-line mode
	SPI1_TxBidi();			// transmit only
	SPI1_Master();			// set master mode
	SPI1_ClockPhaseFirst();		// sampling on first edge
	SPI1_ClockPolLow();		// clock polarity low
	SPI1_Enable();			// SPI enable

	// setup SPI pins
	GPIO_Mode(DISP_SCK_GPIO, GPIO_MODE_AF);
	GPIO_Mode(DISP_MOSI_GPIO, GPIO_MODE_AF);

#endif

	// setup control pins
	GPIO_Out1(DISP_RES_GPIO); GPIO_Mode(DISP_RES_GPIO, GPIO_MODE_OUT);
	GPIO_Out1(DISP_DC_GPIO); GPIO_Mode(DISP_DC_GPIO, GPIO_MODE_OUT);
	GPIO_Out1(DISP_CS_GPIO); GPIO_Mode(DISP_CS_GPIO, GPIO_MODE_OUT);
	GPIO_Out1(DISP_BLK_GPIO); GPIO_Mode(DISP_BLK_GPIO, GPIO_MODE_AF);

	// Remap Timer 3 channel 1 mapping 1 (output CH1 to PB4)
	// 1: PB4:CH1, PB5:CH2
	GPIO_Remap_TIM3(1);

	// setup Timer 3 PWM
	TIM3_InitPWM(1, BACKLIGHT_DIV, BIT(BACKLIGHT_LOOP)-1, BIT(BACKLIGHT_DEF-1), True);
	DispBacklightUpdate();	// update backlight

	// display connect (activate chip selection)
	DispConnect();

	// hard reset display
	RES_ON;		// activate reset
	WaitMs(1);	// reset delay
	RES_OFF;	// deactivate reset
	WaitMs(120);	// wait for initialization, datasheet requires minimum 120ms

	// soft reset display
	DispWriteCmd(ST7735_SWRESET);
	WaitMs(120);	// wait for initialization, datasheet requires minimum 120ms

	// disable sleep mode
	DispWriteCmd(ST7735_SLPOUT);
	WaitMs(120);	// wait for initialization, datasheet required minimum 120ms

	// enable inversion
	DispWriteCmd(ST7735_INVON);

	// select gamma correction
	DispWriteCmd(ST7735_GAMSET);
	DispWriteData(DISP_GAMMA);

	// set display rotation - landscape
	DispWriteCmd(ST7735_MADCTL);
	DispWriteData(RotationTab[1]);

	// set color mode
	DispWriteCmd(ST7735_COLMOD);
	DispWriteData(COLOR_MODE_65K | COLOR_MODE_16BIT); // set color mode to RGB 16-bit 565

	// display disconnect (deactivate chip selection)
	DispDisconnect();

	// clear display
	memset(FrameBuf, 0, sizeof(FrameBuf));

	// display update
	DispUpdateX = 0;	// display update X coordinate
	DispUpdateY = 0;	// display update Y coordinate
	DispUpdateW = WIDTH;	// display update width
	DispUpdateH = HEIGHT;	// display update height
	DispUpdate();

	// enable display
	DispConnect();
	DispWriteCmd(ST7735_DISPON);
	DispDisconnect();
}

// Display terminate
void DispTerm(void)
{
	// disable display
	DispConnect();
	DispWriteCmd(ST7735_DISPOFF);
	DispDisconnect();

#if USE_DISP == 2	// 1=use software display driver, 2=use hardware display driver (0=no driver)
	RCC_SPI1Reset();		// SPI1 reset
#endif

	// terminate pins
	GPIO_PinReset(DISP_BLK_GPIO);
	GPIO_PinReset(DISP_CS_GPIO);
	GPIO_PinReset(DISP_DC_GPIO);
	GPIO_PinReset(DISP_RES_GPIO);
	GPIO_PinReset(DISP_MOSI_GPIO);
	GPIO_PinReset(DISP_SCK_GPIO);
}

// set display update window (default full screen is DispUpdateSetup(0, 0, WIDTH, HEIGHT))
void DispUpdateSetup(int x, int y, int w, int h)
{
	DispUpdateX = x;	// display update X coordinate
	DispUpdateY = y;	// display update Y coordinate
	DispUpdateW = w;	// display update width
	DispUpdateH = h;	// display update height
}

// Display update - send frame buffer to the display
void DispUpdate()
{
	// synchronize external display (to start waiting for active CS)
	DispWriteCmd(0xff);

	// display connect (activate chip selection)
	DispConnect();

	// get display update window
	int x = DispUpdateX;	// display update X coordinate
	int y = DispUpdateY;	// display update Y coordinate
	int w = DispUpdateW;	// display update width
	int h = DispUpdateH;	// display update height

	// set draw window
	DispWindow(x, x+w, y, y+h);

	// send data from frame buffer
	const u16* pal = Palette;
	const u8* s;
	int i;
	u16 ww;
	DC_DATA;	// set data mode
	for (; h > 0; h--)
	{
		s = &FrameBuf[x + y*WIDTHBYTE];
		for (i = w; i > 0; i--)
		{
			ww = pal[*s++];
			DispWriteByte((u8)(ww>>8));
			DispWriteByte((u8)ww);
		}
		y++;
	}

	// display disconnect (deactivate chip selection)
	DispDisconnect();
}

#endif // USE_DISP
