
// ****************************************************************************
//
//                                  DMA
//
// ****************************************************************************
/*
DMA1 data request connected to channels
---------------------------------------
channel1: ADC1, USART4_TX, TIM2_CH3, TIM4_CH1, TIM5_CH2
channel2: SPI1_RX, USART3_TX, TIM1_CH1, TIM2_UP, TIM3_CH3
channel3: SPI1_TX, USART3_RX, TIM1_CH2, TIM3_CH4, TIM3_UP
channel4: SPI2_RX, USART1_TX, I2C2_TX, TIM1_CH4, TIM1_TRIG, TIM1_COM, TIM4_CH2
channel5: SPI2_TX, USART1_RX, I2C2_RX, TIM1_UP, TIM2_CH1, TIM4_CH3
channel6: USART2_TX (USART2_RX), I2C1_TX, TIM1_CH3, TIM3_CH1, TIM3_TRIG, TIM5_CH4
channel7: USART2_RX (USART2_TX), I2C1_RX, TIM2_CH2, TIM2_CH4, TIM4_UP, TIM5_CH1, TIM5_TRIG
channel8: USART4_RX, TIM2_TRIG, TIM5_UP

DMA2 data request connected to channels
---------------------------------------
channel1: TIM5_CH4, TIM5_TRIG, TIM8_CH3, TIM8_UP, SPI/I2S3_RX
channel2: TIM5_CH3, TIM5_UP, TIM8_CH4, TIM8_TRIG, TIM8_COM, USART5_RX, SPI/I2S3_TX
channel3: TIM6_UP, TIM8_CH1, USART4_RX, DAC1
channel4: TIM5_CH2, TIM7_UP, USART5_TX, SDIO, DAC2
channel5: TIM5_CH1, TIM8_CH2, USART4_TX, ADC2
channel6: TIM9_UP, TIM10_CH4, USART6_TX
channel7: TIM9_CH1, TIM10_TRIG, TIM10_COM, USART6_RX
channel8: TIM9_CH4, TIM10_CH1, USART7_TX
channel9: TIM9_CH2, TIM10_CH3, USART7_RX
channel10: TIM9_TRIG, TIM9_COM, TIM10_CH2, USART8_TX
channel11: TIM9_CH3, TIM10_UP, USART8_RX
*/

#if USE_DMA		// 1=use DMA peripheral

#ifndef _SDK_DMA_H
#define _SDK_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

// DMA control
typedef struct {
	io32	INTFR;		// 0x00: DMA interrupt status regiser
	io32	INTFCR;		// 0x04: DMA interrupt flag clear register
} DMA_t;
STATIC_ASSERT(sizeof(DMA_t) == 0x08, "Incorrect DMA_t!");
#define DMA1	((DMA_t*)DMA1_BASE)		// 0x40020000
#define DMA2	((DMA_t*)DMA2_BASE)		// 0x40020400
#define DMA2EXT	((DMA_t*)DMA2_EXTEN_BASE)	// 0x400204D0

// DMA channel
typedef struct {
	io32	CFGR;		// 0x00: configuration register
	io32	CNTR;		// 0x04: count data register
	io32	PADDR;		// 0x08: peripheral address register
	io32	MADDR;		// 0x0C: memory address register
} DMAchan_t;
STATIC_ASSERT(sizeof(DMAchan_t) == 0x10, "Incorrect DMAchan_t!");

#define DMA1CH1		((DMAchan_t*)DMA1_Channel1_BASE)	// 0x40020008
#define DMA1CH2		((DMAchan_t*)DMA1_Channel2_BASE)	// 0x4002001C
#define DMA1CH3		((DMAchan_t*)DMA1_Channel3_BASE)	// 0x40020030
#define DMA1CH4		((DMAchan_t*)DMA1_Channel4_BASE)	// 0x40020044
#define DMA1CH5		((DMAchan_t*)DMA1_Channel5_BASE)	// 0x40020058
#define DMA1CH6		((DMAchan_t*)DMA1_Channel6_BASE)	// 0x4002006C
#define DMA1CH7		((DMAchan_t*)DMA1_Channel7_BASE)	// 0x40020080
#define DMA1CH8		((DMAchan_t*)DMA1_Channel8_BASE)	// 0x40020094

#define DMA2CH1		((DMAchan_t*)DMA2_Channel1_BASE)	// 0x40020408
#define DMA2CH2		((DMAchan_t*)DMA2_Channel2_BASE)	// 0x4002041C
#define DMA2CH3		((DMAchan_t*)DMA2_Channel3_BASE)	// 0x40020430
#define DMA2CH4		((DMAchan_t*)DMA2_Channel4_BASE)	// 0x40020444
#define DMA2CH5		((DMAchan_t*)DMA2_Channel5_BASE)	// 0x40020458
#define DMA2CH6		((DMAchan_t*)DMA2_Channel6_BASE)	// 0x4002046C
#define DMA2CH7		((DMAchan_t*)DMA2_Channel7_BASE)	// 0x40020480
#define DMA2CH8		((DMAchan_t*)DMA2_Channel8_BASE)	// 0x40020490
#define DMA2CH9		((DMAchan_t*)DMA2_Channel9_BASE)	// 0x400204A0
#define DMA2CH10	((DMAchan_t*)DMA2_Channel10_BASE)	// 0x400204B0
#define DMA2CH11	((DMAchan_t*)DMA2_Channel11_BASE)	// 0x400204C0

// get address of DMA1 channel 1..8
#define DMA1_CHAN(ch)	(DMAchan_t*)(DMA1_Channel1_BASE + ((ch)-1)*0x14)
INLINE DMAchan_t* DMA1_Chan(int ch) { return DMA1_CHAN(ch); }

// get address of DMA2 channel 1..11
#define DMA2_CHAN(ch)	(DMAchan_t*)( ((ch)<=7) ? (DMA2_Channel1_BASE+((ch)-1)*0x14) : (DMA2_Channel8_BASE+((ch)-8)*0x10) )
INLINE DMAchan_t* DMA2_Chan(int ch) { return DMA2_CHAN(ch); }

// DMA data size
#define DMA_SIZE_8	0	// 1 byte (8 bits)
#define DMA_SIZE_16	1	// 2 bytes (16 bits)
#define DMA_SIZE_32	2	// 4 bytes (32 bits)

// Control flags of CFGR configuration register
#define DMA_CFG_EN		B0		// channel enable
#define DMA_CFG_COMPINT		B1		// completion interrupt enable
#define DMA_CFG_HALFINT		B2		// over half interrupt enable
#define DMA_CFG_TRANSERRINT	B3		// transmission error interrupt enable
#define DMA_CFG_DIRFROMMEM	B4		// transfer direction from memory
#define DMA_CFG_DIRFROMPER	0		// ... transfer direction from peripheral
#define DMA_CFG_CIRC		B5		// circular mode enabled
#define DMA_CFG_PERINC		B6		// peripheral address increment
#define DMA_CFG_MEMINC		B7		// memory address increment
#define DMA_CFG_PSIZE(size)	((size)<<8)	// peripheral data size DMA_SIZE_*
#define DMA_CFG_PSIZE_8		0		// ... peripheral data size 8 bits
#define DMA_CFG_PSIZE_16	B8		// ... peripheral data size 16 bits
#define DMA_CFG_PSIZE_32	B9		// ... peripheral data size 32 bits
#define DMA_CFG_MSIZE(size)	((size)<<10)	// memory data size DMA_SIZE_*
#define DMA_CFG_MSIZE_8		0		// ... memory data size 8 bits
#define DMA_CFG_MSIZE_16	B10		// ... memory data size 16 bits
#define DMA_CFG_MSIZE_32	B11		// ... memory data size 32 bits
#define DMA_CFG_PRIOR(prior)	((prior)<<12)	// channel priority 0..3
#define DMA_CFG_PRIOR_LOW	0		// ... channel priority 0 low
#define DMA_CFG_PRIOR_MED	B12		// ... channel priority 1 medium
#define DMA_CFG_PRIOR_HIGH	B13		// ... channel priority 2 high
#define DMA_CFG_PRIOR_VERYHIGH	(B12|B13)	// ... channel priority 3 very high
#define DMA_CFG_MERM2MEM	B14		// memory-to-memory mode enable

// === DMA1/2 Interrupt

// Check/Clear global interrupt flag of DMA1 channel ch=1..8
INLINE Bool DMA1_Int(int ch) { return (DMA1->INTFR & (1 << ((ch-1)*4+0))) != 0; }
INLINE void DMA1_IntClr(int ch) { DMA1->INTFCR = 1 << ((ch-1)*4+0); }

// Check/Clear global interrupt flag of DMA2 channel ch=1..11
INLINE Bool DMA2_Int(int ch)
{
	if (ch < 8)
		return (DMA2->INTFR & (1 << ((ch-1)*4+0))) != 0;
	else
		return (DMA2EXT->INTFR & (1 << ((ch-8)*4+0))) != 0;
}
INLINE void DMA2_IntClr(int ch)
{
	if (ch < 8)
		DMA2->INTFCR = 1 << ((ch-1)*4+0);
	else
		DMA2EXT->INTFCR = 1 << ((ch-8)*4+0);
}

// Check/Clear transmission completion flag of DMA1 channel ch=1..8
INLINE Bool DMA1_Comp(int ch) { return (DMA1->INTFR & (1 << ((ch-1)*4+1))) != 0; }
INLINE void DMA1_CompClr(int ch) { DMA1->INTFCR = 1 << ((ch-1)*4+1); }

// Check/Clear transmission completion flag of DMA2 channel ch=1..11
INLINE Bool DMA2_Comp(int ch)
{
	if (ch < 8)
		return (DMA2->INTFR & (1 << ((ch-1)*4+1))) != 0;
	else
		return (DMA2EXT->INTFR & (1 << ((ch-8)*4+1))) != 0;
}
INLINE void DMA2_CompClr(int ch)
{
	if (ch < 8)
		DMA2->INTFCR = 1 << ((ch-1)*4+1);
	else
		DMA2EXT->INTFCR = 1 << ((ch-8)*4+1);
}

// Check/Clear transmission halfway flag of DMA1 channel ch=1..8
INLINE Bool DMA1_Half(int ch) { return (DMA1->INTFR & (1 << ((ch-1)*4+2))) != 0; }
INLINE void DMA1_HalfClr(int ch) { DMA1->INTFCR = 1 << ((ch-1)*4+2); }

// Check/Clear transmission halfway flag of DMA2 channel ch=1..11
INLINE Bool DMA2_Half(int ch)
{
	if (ch < 8)
		return (DMA2->INTFR & (1 << ((ch-1)*4+2))) != 0;
	else
		return (DMA2EXT->INTFR & (1 << ((ch-8)*4+2))) != 0;
}
INLINE void DMA2_HalfClr(int ch)
{
	if (ch < 8)
		DMA2->INTFCR = 1 << ((ch-1)*4+2);
	else
		DMA2EXT->INTFCR = 1 << ((ch-8)*4+2);
}

// Check/Clear transmission error flag of DMA1 channel ch=1..8
INLINE Bool DMA1_TransErr(int ch) { return (DMA1->INTFR & (1 << ((ch-1)*4+3))) != 0; }
INLINE void DMA1_TransErrClr(int ch) { DMA1->INTFCR = 1 << ((ch-1)*4+3); }

// Check/Clear transmission error flag of DMA2 channel ch=1..8
INLINE Bool DMA2_TransErr(int ch)
{
	if (ch < 8)
		return (DMA2->INTFR & (1 << ((ch-1)*4+3))) != 0;
	else
		return (DMA2EXT->INTFR & (1 << ((ch-8)*4+3))) != 0;
}
INLINE void DMA2_TransErrClr(int ch)
{
	if (ch < 8)
		DMA2->INTFCR = 1 << ((ch-1)*4+3);
	else
		DMA2EXT->INTFCR = 1 << ((ch-8)*4+3);
}

// === DMA channel configuration

// Enable/Disable DMA channel
INLINE void DMA_ChanEnable(DMAchan_t* ch) { ch->CFGR |= B0; }
INLINE void DMA_ChanDisable(DMAchan_t* ch) { ch->CFGR &= ~B0; }

// Enable/Disable transmission completion interrupt
INLINE void DMA_CompIntEnable(DMAchan_t* ch) { ch->CFGR |= B1; }
INLINE void DMA_CompIntDisable(DMAchan_t* ch) { ch->CFGR &= ~B1; }

// Enable/Disable transmission over half interrupt
INLINE void DMA_HalfIntEnable(DMAchan_t* ch) { ch->CFGR |= B2; }
INLINE void DMA_HalfIntDisable(DMAchan_t* ch) { ch->CFGR &= ~B2; }

// Enable/Disable transmission error interrupt
INLINE void DMA_TransErrIntEnable(DMAchan_t* ch) { ch->CFGR |= B3; }
INLINE void DMA_TransErrIntDisable(DMAchan_t* ch) { ch->CFGR &= ~B3; }

// Set data transfer direction read from memory or peripheral
INLINE void DMA_DirFromMem(DMAchan_t* ch) { ch->CFGR |= B4; }
INLINE void DMA_DirFromPer(DMAchan_t* ch) { ch->CFGR &= ~B4; }

// Enable/Disable DMA channel circular mode (not supported in memory-to-memory mode)
INLINE void DMA_CircEnable(DMAchan_t* ch) { ch->CFGR |= B5; }
INLINE void DMA_CircDisable(DMAchan_t* ch) { ch->CFGR &= ~B5; }

// Enable/Disable peripheral address increment
INLINE void DMA_PerIncEnable(DMAchan_t* ch) { ch->CFGR |= B6; }
INLINE void DMA_PerIncDisable(DMAchan_t* ch) { ch->CFGR &= ~B6; }

// Enable/Disable memory address increment
INLINE void DMA_MemIncEnable(DMAchan_t* ch) { ch->CFGR |= B7; }
INLINE void DMA_MemIncDisable(DMAchan_t* ch) { ch->CFGR &= ~B7; }

// Set peripheral data size DMA_SIZE_*
INLINE void DMA_PerDataSize(DMAchan_t* ch, int datasize) { ch->CFGR = (ch->CFGR & ~(3<<8)) | (datasize<<8); }

// Set memory data size DMA_SIZE_*
INLINE void DMA_MemDataSize(DMAchan_t* ch, int datasize) { ch->CFGR = (ch->CFGR & ~(3<<10)) | (datasize<<10); }

// Set channel priority 0..3 (low..high)
// - If equal, lower channel number has higher priority.
INLINE void DMA_Priority(DMAchan_t* ch, int priority) { ch->CFGR = (ch->CFGR & ~(3<<12)) | (priority<<12); }

// Enable/Disable memory-to-memory transfer (no pripheral request signal will be used)
INLINE void DMA_MemMemEnable(DMAchan_t* ch) { ch->CFGR |= B14; }
INLINE void DMA_MemMemDisable(DMAchan_t* ch) { ch->CFGR &= ~B14; }

// Set/Get configuration register (set combination of DMA_CFG_* flags)
INLINE void DMA_Cfg(DMAchan_t* ch, u32 cfg) { ch->CFGR = cfg; }
INLINE u32 DMA_GetCfg(DMAchan_t* ch) { return ch->CFGR; }

// Set/Get count of data (0..65535)
INLINE void DMA_Cnt(DMAchan_t* ch, int cnt) { ch->CNTR = cnt; }
INLINE u16 DMA_GetCnt(DMAchan_t* ch) { return (u16)ch->CNTR; }

// Set/Get peripheral address, or memory address in memory-to-memory mode
// - This address will require peripheral request signal. Or not, in memory-to-memory mode.
INLINE void DMA_PerAddr(DMAchan_t* ch, const volatile void* addr) { ch->PADDR = (u32)addr; }
INLINE void* DMA_GetPerAddr(DMAchan_t* ch) { return (void*)ch->PADDR; }

// Set/Get memory address
INLINE void DMA_MemAddr(DMAchan_t* ch, const volatile void* addr) { ch->MADDR = (u32)addr; }
INLINE void* DMA_GetMemAddr(DMAchan_t* ch) { return (void*)ch->MADDR; }

// === DMA1/2 Transfers

// DMA1 8-bit transfer memory to memory
//  ch ... channel number 1..8
//  dst ... destination address
//  src ... source address
//  cnt ... number of 8-bit bytes to transfer (max. 65535)
void DMA1_Copy8(int ch, u8* dst, const u8* src, int cnt);
void DMA2_Copy8(int ch, u8* dst, const u8* src, int cnt);

// MDA1/2 32-bit transfer memory to memory
//  ch ... channel number 1..8
//  dst ... destination address (must be 32-bit aligned)
//  src ... source address (must be 32-bit aligned)
//  cnt ... number of 32-bit words to transfer (max. 65535)
void DMA1_Copy32(int ch, u32* dst, const u32* src, int cnt);
void DMA2_Copy32(int ch, u32* dst, const u32* src, int cnt);

// DMA1/2 send 8-bit memory to peripheral
//  ch ... channel number 1..8
//  dst ... destination address of peripheral
//  src ... source address in memory
//  cnt ... number of bytes to transfer (max. 65535)
// It is necessary to enable DMA in the peripheral to generate a request for DMA.
void DMA1_Send8(int ch, volatile u32* dst, const u8* src, int cnt);
void DMA2_Send8(int ch, volatile u32* dst, const u8* src, int cnt);

#ifdef __cplusplus
}
#endif

#endif // _SDK_DMA_H

#endif // USE_DMA
