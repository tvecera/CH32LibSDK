
// ****************************************************************************
//
//                              Application support
//
// ****************************************************************************

#ifndef _SDK_RUNTIME_H
#define _SDK_RINTUME_H

#ifdef __cplusplus
extern "C" {
#endif

// default value - if device does not use boot loader
#ifndef BOOTLOADER_SIZE
#define BOOTLOADER_SIZE	0x5000	// size of boot loader (20 KB; must be multiply of sector size 512 B)
#endif

// application info header
#define APPINFO_OFF		(BOOTLOADER_SIZE + 0xA4) // application info header offset
#define APPINFO_OFF_CRC		(APPINFO_OFF + 0x0c)	// start of computing CRC
typedef struct {
	char	magic1[4];	// 0x00: magic 1: text "INFO", use APPINFO_MAGIC1 to check
	u32	len;		// 0x04: application length of data after "AppInfoHeader_StartCRC" label, on init: 0x01234567
	u16	crc;		// 0x08: application CRC of data after "AppInfoHeader_StartCRC" label, init: 0x89AB
				//	 - use CRC-16 CCITT (CRC16C, use Crc16CFast() with INIT=0x1D0F)
	char	mcu[2];		// 0x0A: MCU serie identifier, use APPINFO_MCU to check
				//		"03" = CH32V003
				//		"0X" = CH32V00x
				//		"3X" = CH32X03x
				//		"V1" = CH32V103
				//		"L1" = CH32L103
				//		"V2" = CH32V2xx
				//		"V3" = CH32V3xx
				//		"V4" = CH32V4xx
//AppInfoHeader_StartCRC: ... start of computing CRC
	u32	stack;		// 0x0C: end of user stack, to setup SP register
	u32	start;		// 0x10: start address, to jump to application
	char	magic2[4];	// 0x14: magic 2: text "info", use APPINFO_MAGIC2 to check
} sAppInfoHeader;
STATIC_ASSERT(sizeof(sAppInfoHeader) == 0x18, "Incorrect sAppInfoHeader!");
#define AppInfo	((const sAppInfoHeader*)(FLASH_BASE+APPINFO_OFF))	// pointer to application info

#define APPINFO_MAGIC1	0x4F464E49	// magic 1 "INFO" in hex format
#define APPINFO_MCU	0x5830		// this MCU serie identifier in hex format (= "0X")
#define APPINFO_MAGIC2	0x6F666E69	// magic 2 "info" in hex format

// check application in memory
Bool CheckApp();

// Loader resident data
typedef struct {
	u16	crc;		// 0x00: CRC-16C CCITT checksum of following 14 bytes of this structure (use Crc16CFast())
	u8	flags;		// 0x02: flags	bit 7: selected file is directory
				//		bit 6: feedback flag from the application - return to the loader
				//		bit 0..5: relative offset of selected file on the page (to get top file of the file list)
	u8	lastnamelen;	// 0x03: length of name of selected file
	u8	lastname[8];	// 0x04: name of last selected file (without terminating 0 and without file extension ".BIN")
	u32	curdirclust;	// 0x0C: cluster of current directory (0 = ROOT)
} sLoaderData;
STATIC_ASSERT(sizeof(sLoaderData) == 16, "Incorrect sLoaderData!");
extern sLoaderData _sloaderdata;
#define LoaderData	(&_sloaderdata) // pointer to loader data

// Setup CRC of loader data
void LoaderData_CRC(void);

// Check CRC of loader data
Bool LoaderData_Check(void);

// System initialize
void SystemInit(void);

// System terminate
void SystemTerm(void);

// exit application and reset to boot loader
void ResetToBootLoader(void);

#ifdef __cplusplus
}
#endif

#endif // _SDK_RUNTIME_H
