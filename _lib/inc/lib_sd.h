
// ****************************************************************************
//
//                                 SD card
//
// ****************************************************************************
//   Motorola format: MSB first, master mode, SCK low when idle (leading edge rising, trailing
//	edge falling), clock phase: leading edge sample, trailing edge setup, 8 data bits

// Define GPIOs:
//#define SD_CS_GPIO	PD3	// CS output to SD card
//#define SD_CLK_GPIO	PD4	// CLK output to SD card
//#define SD_MISO_GPIO	PD5	// MISO input from SD card
//#define SD_MOSI_GPIO	PD6	// MOSI output to SD card

#if USE_SD		// 1=use SD card driver

#ifndef _LIB_SD_H
#define _LIB_SD_H

#ifdef __cplusplus
extern "C" {
#endif

// defaults - only to not report compilation error
#ifndef SD_CS_GPIO
#define SD_CS_GPIO	PA0	// CS output to SD card
#endif

#ifndef SD_CLK_GPIO
#define SD_CLK_GPIO	PA1	// CLK output to SD card
#endif

#ifndef SD_MISO_GPIO
#define SD_MISO_GPIO	PA2	// MISO input from SD card
#endif

#ifndef SD_MOSI_GPIO
#define SD_MOSI_GPIO	PA3	// MOSI output to SD card
#endif

#ifndef SD_SPEED_INIT
#define SD_SPEED_INIT	(HCLK_PER_US*2)	// SD speed on init: wait delay "HCLK_PER_US*2" = 250 kbps
#endif

#ifndef SD_SPEED_READ
#define SD_SPEED_READ	(HCLK_PER_US/8)	// SD speed on read: wait delay "HCLK_PER_US/8" = 4 Mbps
#endif

#ifndef SD_SPEED_WRITE
#define SD_SPEED_WRITE	(HCLK_PER_US/2)	// SD speed on read: wait delay "HCLK_PER_US/2" = 1 Mbps
#endif

// SD card type
enum {
	SD_NONE = 0,	// unknown type
	SD_MMC = 1,	// MMC
	SD_SD1 = 2,	// SD v1
	SD_SD2 = 3, 	// SD v2
	SD_SDHC = 4, 	// SDHC, block device
};

// current SD speed - number of HCLK cycles of one half-pulse
extern int SD_SpeedDelay;

// SD card type (SD_NONE,...)
extern u8 SD_Type;

// get SD card type name
const char* SD_GetName();

// SD transfer one byte
u8 SD_Byte(u8 val);

// connect to SD card after inserting (returns False on error)
Bool SD_Connect();

// disconnect SD card
void SD_Disconnect();

// read one sector from SD card (returns False on error)
Bool SD_ReadSect(u32 sector, u8* buffer);

// write one sector to SD card (returns False on error)
Bool SD_WriteSect(u32 sector, const u8* buffer);

// get media size (in number of sectors; returns 0 on error)
u32 SD_MediaSize();

// check if disk is valid
INLINE Bool DiskValid() { return SD_Type != SD_NONE; }

// initialize SD card interface
void SD_Init(void);

// Terminate SD card interface
void SD_Term(void);

#ifdef __cplusplus
}
#endif

#endif // _LIB_SD_H

#endif // USE_SD
