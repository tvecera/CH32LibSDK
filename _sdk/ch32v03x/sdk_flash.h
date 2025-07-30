
// ****************************************************************************
//
//                               Flash memory
//
// ****************************************************************************
// The internal RC oscillator (HSI) must be turned on when programming/erasing flash memory.
// Flash memory is organized to 248 pages of size 256 bytes (total 62 KB).

#ifndef _SDK_FLASH_H
#define _SDK_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

// Flash registers
typedef struct {
	io32	ACTLR;			// 0x00: control register
	io32	KEYR;			// 0x04: FPEC key register
	io32	OBKEYR;			// 0x08: OBKEY register
	io32	STATR;			// 0x0C: status register
	io32	CTLR;			// 0x10: configuration register
	io32	ADDR;			// 0x14: address register
	io32	res;			// 0x18: ... reserved
	io32	OBR;			// 0x1C: option byte register
	io32	WPR;			// 0x20: write protection register
	io32	MODEKEYR;		// 0x24: extended key register
	io32	BOOT_MODEKEYR;		// 0x28: unlock BOOT key register
} FLASH_t;
STATIC_ASSERT(sizeof(FLASH_t) == 0x2C, "Incorrect FLASH_t!");
#define FLASH	((FLASH_t*)FLASH_R_BASE)	// 0x40022000

// Set flash wait states 0..2 (register ACTLR.LATENCY)
// - Use 0 wait if sysclk <= 12MHz, 1 wait if sysclk > 12MHz and syclk <= 24MHz, 2 wait if sysclk > 24MHz
INLINE void Flash_Latency(int wait) { FLASH->ACTLR = (FLASH->ACTLR & ~3) | wait; }

#if USE_FLASH		// 1=use Flash programming

// Option bytes
//  - low 8-bits contain option byte
//  - high 8-bits contain inversion of option byte
typedef struct {
	io16	RDPR;			// 0x00: read protection, 0xA5 (0x5AA5 as 16-bit) = current code can be read, other value=cannot read
	io16	USER;			// 0x02: bit 0: IWDG_SW 1=is opened by software (default), 0=turned on by hardware
					//	 bit 1: STOPRST system reset in stop mode 1=not enabled (default), 0=enabled
					//	 bit 2: STANDYRST system reset in standby mode 1=not enabled (default), 0=enabled
					//	 bit 3-4: RST_MODE reset pin ignoring 0=128us, 1=1ms, 2=12ms (default), 3=PD7 as GPIO (default on 007)
					//	 bit 5: power-on startup 1=from BOOT (default), 0=from user
	io16	Data0;			// 0x04: user data (0xffff default) ... LCD display backlight 0x31..0x39, other = use default
	io16	Data1;			// 0x06: user data (0xffff default)
	io16	WRPR0;			// 0x08: write protect, every bit means 2KB, sector 0-15 (16KB), 1=no protection (default), 0=protected
	io16	WRPR1;			// 0x0A: write protect, every bit means 2KB, sector 16-31 (16KB), 1=no protection (default), 0=protected
	io16	WRPR2;			// 0x0C: write protect, every bit means 2KB, sector 32-47 (16KB), 1=no protection (default), 0=protected
	io16	WRPR3;			// 0x0E: write protect, every bit means 2KB, sector 48-62 (14KB), 1=no protection (default), 0=protected
} OB_t;
STATIC_ASSERT(sizeof(OB_t) == 0x10, "Incorrect OB_t!");
#define OB	((const OB_t*)OB_BASE)	// 0x1FFFF800

// Flash pages
#define FLASH_PAGEER_SHIFT	8			// shift of page erase size
#define FLASH_PAGEER_SIZE	BIT(FLASH_PAGEER_SHIFT)	// size of page erase (256 B)
#define FLASH_PAGEPG_SHIFT	8			// shift of page program size
#define FLASH_PAGEPG_SIZE	BIT(FLASH_PAGEPG_SHIFT)	// size of page program (256 B)

// Flash unlock keys
#define FLASH_KEY1	0x45670123
#define FLASH_KEY2	0xCDEF89AB

// get status register with safe write-only bits
#define FLASH_GETSTATR ((FLASH->STATR & ~(B4|B5))|B6)

// get control register with safe write-only bits
#define FLASH_GETCTLR ((FLASH->CTLR & ~(B6|B7|B15|B18|B19))|B9)

// --- Locks

// Unlock flash programming controller (register KEYR.KEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_Unlock(void);

// Lock flash programming controller (register CTLR.LOCK)
INLINE void Flash_Lock(void) { FLASH->CTLR = FLASH_GETCTLR | B7; }

// Check if flash programming controller is locked (register CTLR.LOCK)
INLINE Bool Flash_Locked(void) { return (FLASH->CTLR & B7) != 0; }

// Unlock fast programming (register MODEKEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_FastUnlock(void);

// Lock fast programing (register CTLR.FLOCK)
INLINE void Flash_FastLock(void) { FLASH->CTLR = FLASH_GETCTLR | B15; }

// Check if fast programming is locked (register CTLR.FLOCK)
INLINE Bool Flash_FastLocked(void) { return (FLASH->CTLR & B15) != 0; }

// Unlock option-byte programming (register OBKEYR.OBKEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_OBUnlock(void);

// Lock option-byte programming (register CTLR.OBWRE)
INLINE void Flash_OBLock(void) { FLASH->CTLR = FLASH_GETCTLR & ~B9; }

// Check if write to option-byte is locked (register CTLR.OBWRE)
INLINE Bool Flash_OBLocked(void) { return (FLASH->CTLR & B9) == 0; }

// Unlock boot area (register BOOT_MODEKEYR)
INLINE void Flash_BootUnlock(void) { FLASH->BOOT_MODEKEYR = FLASH_KEY1; FLASH->BOOT_MODEKEYR = FLASH_KEY2; }

// --- Flash status

// Check if Flash operation is busy (register STATR.BSY)
INLINE Bool Flash_Busy(void) { return (FLASH->STATR & B0) != 0; }

// Check/Clear write protection error flag (register STATR.WRPRTERR)
INLINE Bool Flash_WProtErr(void) { return (FLASH->STATR & B4) != 0; }
INLINE void Flash_WProtErrClr(void) { FLASH->STATR = FLASH_GETSTATR | B4; }

// Check/Clear end of operation flag (register STATR.EOP)
INLINE Bool Flash_EndOp(void) { return (FLASH->STATR & B5) != 0; }
INLINE void Flash_EndOpClr(void) { FLASH->STATR = FLASH_GETSTATR | B5; }

// Check/Clear wakeup flag (register STATR.FWAKE_FLAG)
INLINE Bool Flash_Wake(void) { return (FLASH->STATR & B6) != 0; }
INLINE void Flash_WakeClr(void) { FLASH->STATR = FLASH_GETSTATR & ~B6; }

// Check Turbo enable mode (register STATR.TURBO)
INLINE Bool Flash_Turbo(void) { return (FLASH->STATR & B7) != 0; }

// Check if booting from BOOT area is available (register STATR.BOOT_AVA)
INLINE Bool Flash_BootAva(void) { return (FLASH->STATR & B12) != 0; }

// Check source if booting from BOOT area (or from user area otherwise; register STATR.BOOT_STATUS)
INLINE Bool Flash_FromBoot(void) { return (FLASH->STATR & B13) != 0; }

// Select rebooting to BOOT or to user area after software reset (default user area; register STATR.BOOT_MODE)
INLINE void Flash_ToBoot(void) { FLASH->STATR = FLASH_GETSTATR | B14; }
INLINE void Flash_ToUser(void) { FLASH->STATR = FLASH_GETSTATR & ~B14; }

// Lock/Unlock write to select rebooting mode (register STATR.BOOT_LOCK)
INLINE void Flash_BootModeLock(void) { FLASH->STATR = FLASH_GETSTATR | B15; }
INLINE void Flash_BootModeUnlock(void) { FLASH->STATR = FLASH_GETSTATR & ~B15; }

// --- Configuration

// Enable/Disable standard programming operations (default disabled; register CTLR.PG)
//INLINE void Flash_PGEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B0; }
//INLINE void Flash_PGDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B0; }

// Enable/Disable 1K sector erase mode (default disabled; register CTLR.PER)
INLINE void Flash_SectEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B1; }
INLINE void Flash_SectDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B1; }

// Enable/Disable full erase of user area (default disabled; register CTLR.MER)
INLINE void Flash_FullEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B2; }
INLINE void Flash_FullDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B2; }

// Enable/Disable option byte programming (default disabled; register CTLR.OBPG)
//INLINE void Flash_OBPgEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B4; }
//INLINE void Flash_OBPgDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B4; }

// Enable/Disable option byte erase (default disabled; register CTLR.OBER)
INLINE void Flash_OBEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B5; }
INLINE void Flash_OBDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B5; }

// Start erase operation (register CTLR.STRT)
INLINE void Flash_Start(void) { FLASH->CTLR = FLASH_GETCTLR | B6; }

// Check if erasing operation is in progress (register CTLR.STRT)
INLINE Bool Flash_Erasing(void) { return (FLASH->CTLR & B6) != 0; }

// Enable/Disable error status interrupt (register CTLR.ERRIE)
INLINE void Flash_ErrIntEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B10; }
INLINE void Flash_ErrIntDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B10; }

// Enable/Disable end operation interrupt (register CTLR.EOPIE)
INLINE void Flash_EndOpIntEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B12; }
INLINE void Flash_EndOpIntDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B12; }

// Enable/Disable wake up interrupt (register CTLR.EOPIE)
INLINE void Flash_WakeIntEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B13; }
INLINE void Flash_WakeIntDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B13; }

// Enable/Disable quick page programming (register CTLR.FTPG)
INLINE void Flash_FastProgEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B16; }
INLINE void Flash_FastProgDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B16; }

// Enable/Disable fast page erase (register CTLR.FTER)
INLINE void Flash_FastEraseEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B17; }
INLINE void Flash_FastEraseDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B17; }

// Load cache data into buffer (register CTLR.BUFLOAD)
INLINE void Flash_BufLoad(void) { FLASH->CTLR = FLASH_GETCTLR | B18; }

// Start buffer reset (register CTLR.BUFRST)
INLINE void Flash_BufReset(void) { FLASH->CTLR = FLASH_GETCTLR | B19; }

// Enable/Disable 32KB block erase (only 32KB - erase 0x08000000-0x08007FFF; register CTLR.BER32)
INLINE void Flash_BlockEraseEnable(void) { FLASH->CTLR = FLASH_GETCTLR | B23; }
INLINE void Flash_BlockEraseDisable(void) { FLASH->CTLR = FLASH_GETCTLR & ~B23; }

// Set flash address (register ADDR.FAR)
INLINE void Flash_Addr(u32 addr) { FLASH->ADDR = addr; }

// --- Option byte

// Get option byte error - option byte does not match its inverse code (register OBR.OBERR)
INLINE Bool Flash_OBErr(void) { return (FLASH->OBR & B0) != 0; }

// Get read protection status (register OBR.RDPRT)
INLINE Bool Flash_GetReadProt(void) { return (FLASH->OBR & B1) != 0; }

// Check if independent watchdog is enabled (register OBR.IWDG_SW)
INLINE Bool Flash_GetIWDGEnabled(void) { return (FLASH->OBR & B2) == 0; }

// Check if system is reset and controlled in stop mode (register OBR.STOP_RST)
INLINE Bool Flash_GetStop(void) { return (FLASH->OBR & B3) == 0; }

// Check if system is reset and controlled in standby mode (register OBR.STANDY_RST)
INLINE Bool Flash_GetStandby(void) { return (FLASH->OBR & B4) == 0; }

// Get reset delay time
INLINE int Flash_GetRstDel(void) { return (FLASH->OBR >> 5) & 3; }

// Check startup mode from Boot (or from user area otherwise; register OBR.START_MODE)
INLINE Bool Flash_GetBootStart(void) { return (FLASH->OBR & B7) != 0; }

// Get data byte 0
INLINE u8 Flash_GetData0(void) { return (u8)(FLASH->OBR >> 10); }

// Get data byte 1
INLINE u8 Flash_GetData1(void) { return (u8)(FLASH->OBR >> 18); }

// Get write protection word
// - Each bit represents 2K bytes (8 pages), 1=no write protection, 0=write protected
INLINE u32 Flash_GetWProt(void) { return FLASH->WPR; }

// --- General operations

// Flash wait for last operation, with timeout in [ms] (returns False on timeout error or on write protection error)
Bool Flash_Wait(int ms);

// Erase flash memory range, with timeout in [ms] per 256 B page (address and size must be multiply of 256 B = 0x100; returns False on error)
Bool Flash_Erase(u32 addr, u32 size, int ms);

// Program flash memory (must be erased first), with timeout in [ms] per 256 B page (address and
// size must be multiply of 256 B = 0x100; source address must be u32 aligned; returns False on error)
Bool Flash_Program(u32 addr, const u32* src, u32 size, int ms);

// Verify flash memory - returns lowest address in Flash with a difference, or NULL if there is a match
//  Both addresses and sizes must be aligned to a 32-bit word.
const u32* Flash_Verify(u32 addr, const u32* src, u32 size);

// Search start of erased flash memory (address and size must be aligned to a 32-bit word)
const u32* Flash_Check(u32 addr, u32 size);

// Search start of erased flash memory in all memory
INLINE const u32* Flash_CheckAll(void) { return Flash_Check(FLASH_BASE, FLASHSIZE); }

// Erase option bytes, with timeout in [ms] (returns False on error)
Bool Flash_OBErase(int ms);

// Program option bytes (must be erased first), with timeout in [ms]
//   (source address must be u32 aligned; returns False on error)
Bool Flash_OBProgram(const OB_t* ob, int ms);

// Read option bytes from flash to buffer
void Flash_OBRead(OB_t* ob);

// Verify option bytes (returns False if different)
Bool Flash_OBVerify(OB_t* ob);

// Write option bytes from buffer to flash
void Flash_OBWrite(const OB_t* ob);

#endif // USE_FLASH

#ifdef __cplusplus
}
#endif

#endif // _SDK_FLASH_H
