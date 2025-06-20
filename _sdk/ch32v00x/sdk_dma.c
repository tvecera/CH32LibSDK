
// ****************************************************************************
//
//                                  DMA
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_DMA		// 1=use DMA peripheral

// DMA1 8-bit transfer memory to memory
//  ch ... channel number 1..7
//  dst ... destination address
//  src ... source address
//  cnt ... number of 8-bit bytes to transfer (max. 65535)
void DMA1_Copy8(int ch, u8* dst, const u8* src, int cnt)
{
	// enable DMA1 clock
	RCC_DMA1ClkEnable();

	// get address of DMA1 channel
	DMAchan_t* chan = DMA1_Chan(ch);

	// set source address (as peripheral address)
	DMA_PerAddr(chan, src);

	// set destination addres (as memory address)
	DMA_MemAddr(chan, dst);

	// set number of entries
	DMA_Cnt(chan, cnt);

	// configure DMA channel
	DMA_Cfg(chan,
	//	DMA_CFG_EN |			// channel enable
	//	DMA_CFG_COMPINT |		// completion interrupt enable
	//	DMA_CFG_HALFINT |		// over half interrupt enable
	//	DMA_CFG_TRANSERRINT |		// transmission error interrupt enable
	//	DMA_CFG_DIRFROMMEM |		// transfer direction from memory
		DMA_CFG_DIRFROMPER |		// ... transfer direction from peripheral
	//	DMA_CFG_CIRC |			// circular mode enabled
		DMA_CFG_PERINC |		// peripheral address increment
		DMA_CFG_MEMINC |		// memory address increment
	//	DMA_CFG_PSIZE(size) |		// peripheral data size DMA_SIZE_*
		DMA_CFG_PSIZE_8	|		// ... peripheral data size 8 bits
	//	DMA_CFG_PSIZE_16 |		// ... peripheral data size 16 bits
	//	DMA_CFG_PSIZE_32 |		// ... peripheral data size 32 bits
	//	DMA_CFG_MSIZE(size) |		// memory data size DMA_SIZE_*
		DMA_CFG_MSIZE_8	|		// ... memory data size 8 bits
	//	DMA_CFG_MSIZE_16 |		// ... memory data size 16 bits
	//	DMA_CFG_MSIZE_32 |		// ... memory data size 32 bits
	//	DMA_CFG_PRIOR(prior) |		// channel priority 0..3
	//	DMA_CFG_PRIOR_LOW |		// ... channel priority 0 low
		DMA_CFG_PRIOR_MED |		// ... channel priority 1 medium
	//	DMA_CFG_PRIOR_HIGH |		// ... channel priority 2 high
	//	DMA_CFG_PRIOR_VERYHIGH |	// ... channel priority 3 very high
		DMA_CFG_MERM2MEM |		// memory-to-memory mode enable
		0);

	// clear trasmission completion flag
	DMA1_CompClr(ch);

	// enable channel to start transfer
	DMA_ChanEnable(chan);

	// wait for transfer to complete
	while (!DMA1_Comp(ch)) {}
}

// DMA1 32-bit transfer memory to memory
//  ch ... channel number 1..7
//  dst ... destination address (must be 32-bit aligned)
//  src ... source address (must be 32-bit aligned)
//  cnt ... number of 32-bit words to transfer (max. 65535)
void DMA1_Copy32(int ch, u32* dst, const u32* src, int cnt)
{
	// enable DMA1 clock
	RCC_DMA1ClkEnable();

	// get address of DMA1 channel
	DMAchan_t* chan = DMA1_Chan(ch);

	// set source address (as peripheral address)
	DMA_PerAddr(chan, src);

	// set destination addres (as memory address)
	DMA_MemAddr(chan, dst);

	// set number of entries
	DMA_Cnt(chan, cnt);

	// configure DMA channel
	DMA_Cfg(chan,
	//	DMA_CFG_EN |			// channel enable
	//	DMA_CFG_COMPINT |		// completion interrupt enable
	//	DMA_CFG_HALFINT |		// over half interrupt enable
	//	DMA_CFG_TRANSERRINT |		// transmission error interrupt enable
	//	DMA_CFG_DIRFROMMEM |		// transfer direction from memory
		DMA_CFG_DIRFROMPER |		// ... transfer direction from peripheral
	//	DMA_CFG_CIRC |			// circular mode enabled
		DMA_CFG_PERINC |		// peripheral address increment
		DMA_CFG_MEMINC |		// memory address increment
	//	DMA_CFG_PSIZE(size) |		// peripheral data size DMA_SIZE_*
	//	DMA_CFG_PSIZE_8	|		// ... peripheral data size 8 bits
	//	DMA_CFG_PSIZE_16 |		// ... peripheral data size 16 bits
		DMA_CFG_PSIZE_32 |		// ... peripheral data size 32 bits
	//	DMA_CFG_MSIZE(size) |		// memory data size DMA_SIZE_*
	//	DMA_CFG_MSIZE_8	|		// ... memory data size 8 bits
	//	DMA_CFG_MSIZE_16 |		// ... memory data size 16 bits
		DMA_CFG_MSIZE_32 |		// ... memory data size 32 bits
	//	DMA_CFG_PRIOR(prior) |		// channel priority 0..3
	//	DMA_CFG_PRIOR_LOW |		// ... channel priority 0 low
		DMA_CFG_PRIOR_MED |		// ... channel priority 1 medium
	//	DMA_CFG_PRIOR_HIGH |		// ... channel priority 2 high
	//	DMA_CFG_PRIOR_VERYHIGH |	// ... channel priority 3 very high
		DMA_CFG_MERM2MEM |		// memory-to-memory mode enable
		0);

	// clear trasmission completion flag
	DMA1_CompClr(ch);

	// enable channel to start transfer
	DMA_ChanEnable(chan);

	// wait for transfer to complete
	while (!DMA1_Comp(ch)) {}
}

// DMA1 Send 8-bit memory to peripheral
//  ch ... channel number 1..7
//  dst ... destination address of peripheral
//  src ... source address in memory
//  cnt ... number of bytes to transfer (max. 65535)
// It is necessary to enable DMA in the peripheral to generate a request for DMA.
void DMA1_Send8(int ch, volatile u32* dst, const u8* src, int cnt)
{
	// enable DMA1 clock
	RCC_DMA1ClkEnable();

	// get address of DMA1 channel
	DMAchan_t* chan = DMA1_Chan(ch);

	// set destination address (as peripheral address)
	DMA_PerAddr(chan, dst);

	// set source addres (as memory address)
	DMA_MemAddr(chan, src);

	// set number of entries
	DMA_Cnt(chan, cnt);

	// configure DMA channel
	DMA_Cfg(chan,
	//	DMA_CFG_EN |			// channel enable
	//	DMA_CFG_COMPINT |		// completion interrupt enable
	//	DMA_CFG_HALFINT |		// over half interrupt enable
	//	DMA_CFG_TRANSERRINT |		// transmission error interrupt enable
		DMA_CFG_DIRFROMMEM |		// transfer direction from memory
	//	DMA_CFG_DIRFROMPER |		// ... transfer direction from peripheral
	//	DMA_CFG_CIRC |			// circular mode enabled
	//	DMA_CFG_PERINC |		// peripheral address increment
		DMA_CFG_MEMINC |		// memory address increment
	//	DMA_CFG_PSIZE(size) |		// peripheral data size DMA_SIZE_*
	//	DMA_CFG_PSIZE_8	|		// ... peripheral data size 8 bits
	//	DMA_CFG_PSIZE_16 |		// ... peripheral data size 16 bits
		DMA_CFG_PSIZE_32 |		// ... peripheral data size 32 bits
	//	DMA_CFG_MSIZE(size) |		// memory data size DMA_SIZE_*
		DMA_CFG_MSIZE_8	|		// ... memory data size 8 bits
	//	DMA_CFG_MSIZE_16 |		// ... memory data size 16 bits
	//	DMA_CFG_MSIZE_32 |		// ... memory data size 32 bits
	//	DMA_CFG_PRIOR(prior) |		// channel priority 0..3
	//	DMA_CFG_PRIOR_LOW |		// ... channel priority 0 low
		DMA_CFG_PRIOR_MED |		// ... channel priority 1 medium
	//	DMA_CFG_PRIOR_HIGH |		// ... channel priority 2 high
	//	DMA_CFG_PRIOR_VERYHIGH |	// ... channel priority 3 very high
	//	DMA_CFG_MERM2MEM |		// memory-to-memory mode enable
		0);

	// clear trasmission completion flag
	DMA1_CompClr(ch);

	// enable channel to start transfer
	DMA_ChanEnable(chan);

	// wait for transfer to complete
	while (!DMA1_Comp(ch)) {}
}

#endif // USE_DMA
