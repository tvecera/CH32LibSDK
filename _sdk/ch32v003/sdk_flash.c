
// ****************************************************************************
//
//                               Flash memory
//
// ****************************************************************************

#include "../_include.h"

#if USE_FLASH		// 1=use Flash programming

// Unlock flash program erase controller (register KEYR.KEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_Unlock(void)
{
	if (Flash_Locked())
	{
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

// Unlock fast programming (register MODEKEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_FastUnlock(void)
{
	if (Flash_FastLocked())
	{
		FLASH->MODEKEYR = FLASH_KEY1;
		FLASH->MODEKEYR = FLASH_KEY2;
	}
}

// Unlock option-byte programming (register OBKEYR.OBKEYR)
// - After unsuccessful unlock, controller will be locked until next reset.
void Flash_OBUnlock(void)
{
	if (Flash_OBLocked())
	{
		FLASH->OBKEYR = FLASH_KEY1;
		FLASH->OBKEYR = FLASH_KEY2;
	}
}

// Flash wait for last operation, with timeout in [ms] (returns False on timeout error or on write protection error)
Bool Flash_Wait(int ms)
{
	// first check completed operation without waiting
	if (!Flash_Busy()) return True;

	// check write protection error
	if (Flash_WProtErr()) return False;

	// recalculate ms to ticks
	ms *= HCLK_PER_MS;

	// get start time
	u32 start = Time();

	// wait
	while ((u32)(Time() - start) < ms)
	{
		// check completed operation
		if (!Flash_Busy()) return True;

		// check write pritection error
		if (Flash_WProtErr()) return False;
	}

	// timeout error
	return False;
}

// Erase flash memory, with timeout in [ms] per 64 B page (address and size must be multiply of 64 B = 0x40; returns False on error)
Bool Flash_Erase(u32 addr, u32 size, int ms)
{
	// Unlock flash programming controller
	Flash_Unlock();

	// Unlock fast programming
	Flash_FastUnlock();

	// clear write protection error
	Flash_WProtErrClr();

	// wait for last operation
	if (!Flash_Wait(ms)) return False;

	// disable standard programming B0, option byte programming B4, option byte erase B5, disable 1K sector erase B1, enable fast page erase B17
	FLASH->CTLR = (FLASH_GETCTLR & ~(B0|B4|B5|B1)) | B17;

	// loop pages
	while (size >= FLASH_PAGEER_SIZE)
	{
		// set flash page address
		Flash_Addr(addr);

		// start operation
		Flash_Start();

		// wait for operation
		if (!Flash_Wait(ms)) return False;

		// shift to next page
		addr += FLASH_PAGEER_SIZE;
		size -= FLASH_PAGEER_SIZE;
	}

	// disable fast page erase
	Flash_FastEraseDisable();

	// Lock fast programing
	Flash_FastLock();

	// Lock flash programming controller
	Flash_Lock();
	return True;
}

// Program flash memory (must be erased first), with timeout in [ms] per 64 B page (address and
// size must be multiply of 64 B = 0x40; source address must be u32 aligned; returns False on error)
Bool Flash_Program(u32 addr, const u32* src, u32 size, int ms)
{
	// Unlock flash programming controller
	Flash_Unlock();

	// Unlock fast programming
	Flash_FastUnlock();

	// clear write protection error
	Flash_WProtErrClr();

	// wait for last operation
	if (!Flash_Wait(ms)) return False;

	// enable fast programming
	Flash_FastProgEnable();

	// program pages
	while (size >= FLASH_PAGEPG_SIZE)
	{
		// clear internal 64-byte buffer
		Flash_BufReset();

		// wait for clearing the buffer
		if (!Flash_Wait(ms)) return False;

		// process 64 bytes (as 16 words u32)
		int i;
		for (i = 16; i > 0; i--)
		{
			// write 32-bit word to the cache
			*(u32*)addr = *src;

			// load word from cache to the buffer
			Flash_BufLoad();

			// wait for loading the buffer
			if (!Flash_Wait(ms)) return False;

			// shift to next word
			addr += 4;
			src++;
		}

		// set flash page address
		Flash_Addr(addr - FLASH_PAGEPG_SIZE);

		// start operation
		Flash_Start();

		// wait for operation
		if (!Flash_Wait(ms)) return False;

		// shift to next page
		size -= FLASH_PAGEPG_SIZE;
	}

	// disable fast programming
	Flash_FastProgDisable();

	// Lock fast programing
	Flash_FastLock();

	// Lock flash programming controller
	Flash_Lock();
	return True;
}

// Verify flash memory - returns lowest address in Flash with a difference, or NULL if there is a match
//  Both addresses and sizes must be aligned to a 32-bit word.
const u32* Flash_Verify(u32 addr, const u32* src, u32 size)
{
	const u32* dst = (const u32*)addr;
	while (size > 0)
	{
		if (*dst != *src) return dst;
		dst++;
		src++;
		size -= 4;
	}
	return NULL;
}

// Search start of erased flash memory (address and size must be aligned to a 32-bit word)
const u32* Flash_Check(u32 addr, u32 size)
{
	const u32* dst = (const u32*)(addr + size);
	while (size > 0)
	{
		if (dst[-1] != 0xffffffff) break;
		dst--;
		size -= 4;
	}
	return dst;
}

// Erase option bytes, with timeout in [ms] (returns False on error)
Bool Flash_OBErase(int ms)
{
	// Unlock flash programming controller
	Flash_Unlock();

	// clear write protection error
	Flash_WProtErrClr();

	// wait for last operation
	if (!Flash_Wait(ms)) return False;

	// Unlock option-byte programming
	Flash_OBUnlock();

	// disable standard programming B0, option byte programming B4, fast page erase B17, disable 1K sector erase B1, enable option byte erase B5
	FLASH->CTLR = (FLASH_GETCTLR & ~(B0|B4|B17|B1)) | B5;

	// Start erase operation
	Flash_Start();

	// wait for last operation
	if (!Flash_Wait(ms)) return False;

	// Disable option byte erase
	Flash_OBDisable();

	// Lock option-byte programming
	Flash_OBLock();

	// Lock flash programming controller
	Flash_Lock();
	return True;
}

// Program option bytes (must be erased first), with timeout in [ms]
//   (source address must be u16 aligned; returns False on error)
Bool Flash_OBProgram(const OB_t* ob, int ms)
{
	const u16* src = (const u16*)ob;

	// Unlock flash programming controller
	Flash_Unlock();

	// clear write protection error
	Flash_WProtErrClr();

	// wait for last operation
	if (!Flash_Wait(ms)) return False;

	// Enable option byte programming
	Flash_OBPgEnable();

	// process 16 bytes (as 8 words u16)
	int i;
	u32 addr = OB_BASE;
	for (i = 8; i > 0; i--)
	{
		// write 16-bit word to the cache
		*(u16*)addr = *src;

		// wait for loading the buffer
		if (!Flash_Wait(ms)) return False;

		// shift to next word
		addr += 2;
		src++;
	}

	// Disable option byte programming
	Flash_OBPgDisable();

	// Lock flash programming controller
	Flash_Lock();
	return True;
}

// Read option bytes from flash to buffer
void Flash_OBRead(OB_t* ob)
{
	memcpy(ob, OB, sizeof(OB_t));
}

// Verify option bytes (returns False if different)
Bool Flash_OBVerify(OB_t* ob)
{
	return memcmp(ob, OB, sizeof(OB_t)) == 0;
}

// Write option bytes from buffer to flash
void Flash_OBWrite(const OB_t* ob)
{
	// erase
	Flash_OBErase(1000);

	// write
	Flash_OBProgram(ob, 1000);
}

#endif // USE_FLASH
