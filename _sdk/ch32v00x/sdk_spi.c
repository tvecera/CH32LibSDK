
// ****************************************************************************
//
//                                  SPI
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_SPI		// 1=use SPI peripheral

// Send data with wait
void SPIx_SendWait(SPI_t* spi, u16 data)
{
	while (!SPIx_TxEmpty(spi)) {}
	SPIx_Write(spi, data);
}

// Receive data with wait
u16 SPIx_RecvWait(SPI_t* spi)
{
	while (SPIx_RxReady(spi)) {}
	return SPIx_Read(spi);
}

#endif // USE_SPI
