
// ****************************************************************************
//
//                                  SPI
//
// ****************************************************************************

#if USE_SPI		// 1=use SPI peripheral

#ifndef _SDK_SPI_H
#define _SDK_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

// SPI registers
typedef struct {
	io32	CTLR1;		// 0x00: control register 1
	io32	CTLR2;		// 0x04: control register 2
	io32	STATR;		// 0x08: status register
	io16	DATAR;		// 0x0C: data register
	io16	res1;		// 0x0E: ... reserved
	io16	CRCR;		// 0x10: polynomial register
	io16	res2;		// 0x12: ... reserved
	io16	RCRCR;		// 0x14: receive CRC register
	io16	res3;		// 0x16: ... reserved
	io16	TCRCR;		// 0x18: transmit CRC register
	io16	res4;		// 0x1A: ... reserved
	io32	I2SCFGR;	// 0x1C: I2S configuration register
	io32	I2SPR;		// 0x20: I2S prescaler register
	io32	HSCR;		// 0x24: high-speed control register
} SPI_t;
STATIC_ASSERT(sizeof(SPI_t) == 0x28, "Incorrect SPI_t!");
#define SPI1	((SPI_t*)SPI1_BASE)	// 0x40013000
#define SPI2	((SPI_t*)SPI2_BASE)	// 0x40003800
#define SPI3	((SPI_t*)SPI3_BASE)	// 0x40003C00

// Baud rate setting divider
#define SPI_BAUD_DIV2		0	// Fhclk/2
#define SPI_BAUD_DIV4		1	// Fhclk/4
#define SPI_BAUD_DIV8		2	// Fhclk/8
#define SPI_BAUD_DIV16		3	// Fhclk/16
#define SPI_BAUD_DIV32		4	// Fhclk/32
#define SPI_BAUD_DIV64		5	// Fhclk/64
#define SPI_BAUD_DIV128		6	// Fhclk/128
#define SPI_BAUD_DIV256		7	// Fhclk/256

// I2S data length
#define I2S_DATALEN_16		0	// 16-bit data length
#define I2S_DATALEN_24		1	// 24-bit data length
#define I2S_DATALEN_32		2	// 32-bit data length

// I2S standard
#define I2S_STD_PHILIPS		0	// I2S Philips standard
#define I2S_STD_HIGHALIGN	1	// High byte alignment (left alignment)
#define I2S_STD_LOWALIGN	2	// Low byte alignment (right justified)
#define I2S_STD_PCM		3	// PCM standard

// I2S mode selection
#define I2S_MODE_SLAVETX	0	// slave device transmits
#define I2S_MODE_SLAVERX	1	// slave device receives
#define I2S_MODE_MASTERTX	2	// master device transmits
#define I2S_MODE_MASTERRX	3	// master device receives

// === Setup

// Set clock phase to data sampling on First/Second edge (default first edge; register CTLR1.CPHA)
INLINE void SPIx_ClockPhaseFirst(SPI_t* spi) { spi->CTLR1 &= ~B0; }
INLINE void SPI1_ClockPhaseFirst(void) { SPIx_ClockPhaseFirst(SPI1); }
INLINE void SPI2_ClockPhaseFirst(void) { SPIx_ClockPhaseFirst(SPI2); }
INLINE void SPI3_ClockPhaseFirst(void) { SPIx_ClockPhaseFirst(SPI3); }

INLINE void SPIx_ClockPhaseSecond(SPI_t* spi) { spi->CTLR1 |= B0; }
INLINE void SPI1_ClockPhaseSecond(void) { SPIx_ClockPhaseSecond(SPI1); }
INLINE void SPI2_ClockPhaseSecond(void) { SPIx_ClockPhaseSecond(SPI2); }
INLINE void SPI3_ClockPhaseSecond(void) { SPIx_ClockPhaseSecond(SPI3); }

// Select clock polarity - held Low or High in idle state (default low; register CTLR1.CPOL)
INLINE void SPIx_ClockPolLow(SPI_t* spi) { spi->CTLR1 &= ~B1; }
INLINE void SPI1_ClockPolLow(void) { SPIx_ClockPolLow(SPI1); }
INLINE void SPI2_ClockPolLow(void) { SPIx_ClockPolLow(SPI2); }
INLINE void SPI3_ClockPolLow(void) { SPIx_ClockPolLow(SPI3); }

INLINE void SPIx_ClockPolHigh(SPI_t* spi) { spi->CTLR1 |= B1; }
INLINE void SPI1_ClockPolHigh(void) { SPIx_ClockPolHigh(SPI1); }
INLINE void SPI2_ClockPolHigh(void) { SPIx_ClockPolHigh(SPI2); }
INLINE void SPI3_ClockPolHigh(void) { SPIx_ClockPolHigh(SPI3); }

// Select master or slave mode (default slave; register CTLR1.MSTR)
INLINE void SPIx_Master(SPI_t* spi) { spi->CTLR1 |= B2; }
INLINE void SPI1_Master(void) { SPIx_Master(SPI1); }
INLINE void SPI2_Master(void) { SPIx_Master(SPI2); }
INLINE void SPI3_Master(void) { SPIx_Master(SPI3); }

INLINE void SPIx_Slave(SPI_t* spi) { spi->CTLR1 &= ~B2; }
INLINE void SPI1_Slave(void) { SPIx_Slave(SPI1); }
INLINE void SPI2_Slave(void) { SPIx_Slave(SPI2); }
INLINE void SPI3_Slave(void) { SPIx_Slave(SPI3); }

// Set Baut rate setting divider SPI_BAUD_DIV* (default 2; register CTLR1.BR[2:0])
INLINE void SPIx_Baud(SPI_t* spi, int baud) { spi->CTLR1 = (spi->CTLR1 & ~(7 << 3)) | (baud << 3); }
INLINE void SPI1_Baud(int baud) { SPIx_Baud(SPI1, baud); }
INLINE void SPI2_Baud(int baud) { SPIx_Baud(SPI2, baud); }
INLINE void SPI3_Baud(int baud) { SPIx_Baud(SPI3, baud); }

// Enable/Disable SPI (default disable; register CTLR1.SPE)
INLINE void SPIx_Enable(SPI_t* spi) { spi->CTLR1 |= B6; }
INLINE void SPI1_Enable(void) { SPIx_Enable(SPI1); }
INLINE void SPI2_Enable(void) { SPIx_Enable(SPI2); }
INLINE void SPI3_Enable(void) { SPIx_Enable(SPI3); }

INLINE void SPIx_Disable(SPI_t* spi) { spi->CTLR1 &= ~B6; }
INLINE void SPI1_Disable(void) { SPIx_Disable(SPI1); }
INLINE void SPI2_Disable(void) { SPIx_Disable(SPI2); }
INLINE void SPI3_Disable(void) { SPIx_Disable(SPI3); }

// Select first transmitted bit LSB or MSB (default MSB; register CTLR1.LSBFIRST)
// LSB is only supported by SPI as host.
INLINE void SPIx_LSB(SPI_t* spi) { spi->CTLR1 |= B7; }
INLINE void SPI1_LSB(void) { SPIx_LSB(SPI1); }
INLINE void SPI2_LSB(void) { SPIx_LSB(SPI2); }
INLINE void SPI3_LSB(void) { SPIx_LSB(SPI3); }

INLINE void SPIx_MSB(SPI_t* spi) { spi->CTLR1 &= ~B7; }
INLINE void SPI1_MSB(void) { SPIx_MSB(SPI1); }
INLINE void SPI2_MSB(void) { SPIx_MSB(SPI2); }
INLINE void SPI3_MSB(void) { SPIx_MSB(SPI3); }

// Set level of NSS pin to Low or High (default low; register CTLR1.SSI)
INLINE void SPIx_NSSLow(SPI_t* spi) { spi->CTLR1 &= ~B8; }
INLINE void SPI1_NSSLow(void) { SPIx_NSSLow(SPI1); }
INLINE void SPI2_NSSLow(void) { SPIx_NSSLow(SPI2); }
INLINE void SPI3_NSSLow(void) { SPIx_NSSLow(SPI3); }

INLINE void SPIx_NSSHigh(SPI_t* spi) { spi->CTLR1 |= B8; }
INLINE void SPI1_NSSHigh(void) { SPIx_NSSHigh(SPI1); }
INLINE void SPI2_NSSHigh(void) { SPIx_NSSHigh(SPI2); }
INLINE void SPI3_NSSHigh(void) { SPIx_NSSHigh(SPI3); }

// Select whether NSS pin is controlled by software or hardware (default hardware; register CTLR1.SSM)
INLINE void SPIx_NSSSw(SPI_t* spi) { spi->CTLR1 |= B9; }
INLINE void SPI1_NSSSw(void) { SPIx_NSSSw(SPI1); }
INLINE void SPI2_NSSSw(void) { SPIx_NSSSw(SPI2); }
INLINE void SPI3_NSSSw(void) { SPIx_NSSSw(SPI3); }

INLINE void SPIx_NSSHw(SPI_t* spi) { spi->CTLR1 &= ~B9; }
INLINE void SPI1_NSSHw(void) { SPIx_NSSHw(SPI1); }
INLINE void SPI2_NSSHw(void) { SPIx_NSSHw(SPI2); }
INLINE void SPI3_NSSHw(void) { SPIx_NSSHw(SPI3); }

// Set receive-only mode in 2-wire mode, or full-duplex mode (default full-duplex; register CTLR1.RXONLY)
INLINE void SPIx_RxOnly(SPI_t* spi) { spi->CTLR1 |= B10; }
INLINE void SPI1_RxOnly(void) { SPIx_RxOnly(SPI1); }
INLINE void SPI2_RxOnly(void) { SPIx_RxOnly(SPI2); }
INLINE void SPI3_RxOnly(void) { SPIx_RxOnly(SPI3); }

INLINE void SPIx_Duplex(SPI_t* spi) { spi->CTLR1 &= ~B10; }
INLINE void SPI1_Duplex(void) { SPIx_Duplex(SPI1); }
INLINE void SPI2_Duplex(void) { SPIx_Duplex(SPI2); }
INLINE void SPI3_Duplex(void) { SPIx_Duplex(SPI3); }

// Select data format to 16-bit or 8-bit (default 8-bit; register CTLR1.DFF)
INLINE void SPIx_Data16(SPI_t* spi) { spi->CTLR1 |= B11; }
INLINE void SPI1_Data16(void) { SPIx_Data16(SPI1); }
INLINE void SPI2_Data16(void) { SPIx_Data16(SPI2); }
INLINE void SPI3_Data16(void) { SPIx_Data16(SPI3); }

INLINE void SPIx_Data8(SPI_t* spi) { spi->CTLR1 &= ~B11; }
INLINE void SPI1_Data8(void) { SPIx_Data8(SPI1); }
INLINE void SPI2_Data8(void) { SPIx_Data8(SPI2); }
INLINE void SPI3_Data8(void) { SPIx_Data8(SPI3); }

// Enable/Disable sending CRC register after next data transfer (default disabled; register CTLR1.CRCNEXT)
// - Should be enabled immediately after the last data is written to the data register.
INLINE void SPIx_CRCNextEnable(SPI_t* spi) { spi->CTLR1 |= B12; }
INLINE void SPI1_CRCNextEnable(void) { SPIx_CRCNextEnable(SPI1); }
INLINE void SPI2_CRCNextEnable(void) { SPIx_CRCNextEnable(SPI2); }
INLINE void SPI3_CRCNextEnable(void) { SPIx_CRCNextEnable(SPI3); }

INLINE void SPIx_CRCNextDisable(SPI_t* spi) { spi->CTLR1 &= ~B12; }
INLINE void SPI1_CRCNextDisable(void) { SPIx_CRCNextDisable(SPI1); }
INLINE void SPI2_CRCNextDisable(void) { SPIx_CRCNextDisable(SPI2); }
INLINE void SPI3_CRCNextDisable(void) { SPIx_CRCNextDisable(SPI3); }

// Enable/Disable hardware CRC (only in full-duplex mode; default disabled; register CTLR1.CRCEN)
INLINE void SPIx_CRCEnable(SPI_t* spi) { spi->CTLR1 |= B13; }
INLINE void SPI1_CRCEnable(void) { SPIx_CRCEnable(SPI1); }
INLINE void SPI2_CRCEnable(void) { SPIx_CRCEnable(SPI2); }
INLINE void SPI3_CRCEnable(void) { SPIx_CRCEnable(SPI3); }

INLINE void SPIx_CRCDisable(SPI_t* spi) { spi->CTLR1 &= ~B13; }
INLINE void SPI1_CRCDisable(void) { SPIx_CRCDisable(SPI1); }
INLINE void SPI2_CRCDisable(void) { SPIx_CRCDisable(SPI2); }
INLINE void SPI3_CRCDisable(void) { SPIx_CRCDisable(SPI3); }

// Select transmit-only or receive-only in 1-line bidirectional mode (default receive; register CTLR1.BIDIOE)
INLINE void SPIx_TxBidi(SPI_t* spi) { spi->CTLR1 |= B14; }
INLINE void SPI1_TxBidi(void) { SPIx_TxBidi(SPI1); }
INLINE void SPI2_TxBidi(void) { SPIx_TxBidi(SPI2); }
INLINE void SPI3_TxBidi(void) { SPIx_TxBidi(SPI3); }

INLINE void SPIx_RxBidi(SPI_t* spi) { spi->CTLR1 &= ~B14; }
INLINE void SPI1_RxBidi(void) { SPIx_RxBidi(SPI1); }
INLINE void SPI2_RxBidi(void) { SPIx_RxBidi(SPI2); }
INLINE void SPI3_RxBidi(void) { SPIx_RxBidi(SPI3); }

// Select 1-line or 2-line bidirectional mode (defalt 2-line; register CTLR1.BIDIMODE)
INLINE void SPIx_Bidi1(SPI_t* spi) { spi->CTLR1 |= B15; }
INLINE void SPI1_Bidi1(void) { SPIx_Bidi1(SPI1); }
INLINE void SPI2_Bidi1(void) { SPIx_Bidi1(SPI2); }
INLINE void SPI3_Bidi1(void) { SPIx_Bidi1(SPI3); }

INLINE void SPIx_Bidi2(SPI_t* spi) { spi->CTLR1 &= ~B15; }
INLINE void SPI1_Bidi2(void) { SPIx_Bidi2(SPI1); }
INLINE void SPI2_Bidi2(void) { SPIx_Bidi2(SPI2); }
INLINE void SPI3_Bidi2(void) { SPIx_Bidi2(SPI3); }

// Enable/Disable Rx buffer DMA (default disabled; register CTLR2.RXDMAEN)
INLINE void SPIx_RxDMAEnable(SPI_t* spi) { spi->CTLR2 |= B0; }
INLINE void SPI1_RxDMAEnable(void) { SPIx_RxDMAEnable(SPI1); }
INLINE void SPI2_RxDMAEnable(void) { SPIx_RxDMAEnable(SPI2); }
INLINE void SPI3_RxDMAEnable(void) { SPIx_RxDMAEnable(SPI3); }

INLINE void SPIx_RxDMADisable(SPI_t* spi) { spi->CTLR2 &= ~B0; }
INLINE void SPI1_RxDMADisable(void) { SPIx_RxDMADisable(SPI1); }
INLINE void SPI2_RxDMADisable(void) { SPIx_RxDMADisable(SPI2); }
INLINE void SPI3_RxDMADisable(void) { SPIx_RxDMADisable(SPI3); }

// Enable/Disable Tx buffer DMA (default disabled; register CTLR2.TXDMAEN)
INLINE void SPIx_TxDMAEnable(SPI_t* spi) { spi->CTLR2 |= B1; }
INLINE void SPI1_TxDMAEnable(void) { SPIx_TxDMAEnable(SPI1); }
INLINE void SPI2_TxDMAEnable(void) { SPIx_TxDMAEnable(SPI2); }
INLINE void SPI3_TxDMAEnable(void) { SPIx_TxDMAEnable(SPI3); }

INLINE void SPIx_TxDMADisable(SPI_t* spi) { spi->CTLR2 &= ~B1; }
INLINE void SPI1_TxDMADisable(void) { SPIx_TxDMADisable(SPI1); }
INLINE void SPI2_TxDMADisable(void) { SPIx_TxDMADisable(SPI2); }
INLINE void SPI3_TxDMADisable(void) { SPIx_TxDMADisable(SPI3); }

// Enable/Disable SS output (default disabled; register CTLR2.SSOE)
INLINE void SPIx_SSEnable(SPI_t* spi) { spi->CTLR2 |= B2; }
INLINE void SPI1_SSEnable(void) { SPIx_SSEnable(SPI1); }
INLINE void SPI2_SSEnable(void) { SPIx_SSEnable(SPI2); }
INLINE void SPI3_SSEnable(void) { SPIx_SSEnable(SPI3); }

INLINE void SPIx_SSDisable(SPI_t* spi) { spi->CTLR2 &= ~B2; }
INLINE void SPI1_SSDisable(void) { SPIx_SSDisable(SPI1); }
INLINE void SPI2_SSDisable(void) { SPIx_SSDisable(SPI2); }
INLINE void SPI3_SSDisable(void) { SPIx_SSDisable(SPI3); }

// Enable/Disable open-drain output (default disabled; register CTLR2.ODEN)
//INLINE void SPIx_OpenDrainEnable(SPI_t* spi) { spi->CTLR2 |= B15; }
//INLINE void SPI1_OpenDrainEnable(void) { SPIx_OpenDrainEnable(SPI1); }
//INLINE void SPI2_OpenDrainEnable(void) { SPIx_OpenDrainEnable(SPI2); }
//INLINE void SPI3_OpenDrainEnable(void) { SPIx_OpenDrainEnable(SPI3); }

//INLINE void SPIx_OpenDrainDisable(SPI_t* spi) { spi->CTLR2 &= ~B15; }
//INLINE void SPI1_OpenDrainDisable(void) { SPIx_OpenDrainDisable(SPI1); }
//INLINE void SPI2_OpenDrainDisable(void) { SPIx_OpenDrainDisable(SPI2); }
//INLINE void SPI3_OpenDrainDisable(void) { SPIx_OpenDrainDisable(SPI3); }

// === Interrupt

// Enable/Disable error interrupt (default disabled; register CTLR2.ERRIE)
INLINE void SPIx_ErrIntEnable(SPI_t* spi) { spi->CTLR2 |= B5; }
INLINE void SPI1_ErrIntEnable(void) { SPIx_ErrIntEnable(SPI1); }
INLINE void SPI2_ErrIntEnable(void) { SPIx_ErrIntEnable(SPI2); }
INLINE void SPI3_ErrIntEnable(void) { SPIx_ErrIntEnable(SPI3); }

INLINE void SPIx_ErrIntDisable(SPI_t* spi) { spi->CTLR2 &= ~B5; }
INLINE void SPI1_ErrIntDisable(void) { SPIx_ErrIntDisable(SPI1); }
INLINE void SPI2_ErrIntDisable(void) { SPIx_ErrIntDisable(SPI2); }
INLINE void SPI3_ErrIntDisable(void) { SPIx_ErrIntDisable(SPI3); }

// Enable/Disable Rx buffer not empty interrupt (default disabled; register CTLR2.RXNEIE)
INLINE void SPIx_RxReadyIntEnable(SPI_t* spi) { spi->CTLR2 |= B6; }
INLINE void SPI1_RxReadyIntEnable(void) { SPIx_RxReadyIntEnable(SPI1); }
INLINE void SPI2_RxReadyIntEnable(void) { SPIx_RxReadyIntEnable(SPI2); }
INLINE void SPI3_RxReadyIntEnable(void) { SPIx_RxReadyIntEnable(SPI3); }

INLINE void SPIx_RxReadyIntDisable(SPI_t* spi) { spi->CTLR2 &= ~B6; }
INLINE void SPI1_RxReadyIntDisable(void) { SPIx_RxReadyIntDisable(SPI1); }
INLINE void SPI2_RxReadyIntDisable(void) { SPIx_RxReadyIntDisable(SPI2); }
INLINE void SPI3_RxReadyIntDisable(void) { SPIx_RxReadyIntDisable(SPI3); }

// Enable/Disable Tx buffer empty interrupt (default disabled; register CTLR2.TXEIE)
INLINE void SPIx_TxEmptyIntEnable(SPI_t* spi) { spi->CTLR2 |= B7; }
INLINE void SPI1_TxEmptyIntEnable(void) { SPIx_TxEmptyIntEnable(SPI1); }
INLINE void SPI2_TxEmptyIntEnable(void) { SPIx_TxEmptyIntEnable(SPI2); }
INLINE void SPI3_TxEmptyIntEnable(void) { SPIx_TxEmptyIntEnable(SPI3); }

INLINE void SPIx_TxEmptyIntDisable(SPI_t* spi) { spi->CTLR2 &= ~B7; }
INLINE void SPI1_TxEmptyIntDisable(void) { SPIx_TxEmptyIntDisable(SPI1); }
INLINE void SPI2_TxEmptyIntDisable(void) { SPIx_TxEmptyIntDisable(SPI2); }
INLINE void SPI3_TxEmptyIntDisable(void) { SPIx_TxEmptyIntDisable(SPI3); }

// === Status

// Check if receive buffer not empty - received data are ready (register STATR.RXNE)
INLINE Bool SPIx_RxReady(SPI_t* spi) { return (spi->STATR & B0) != 0; }
INLINE Bool SPI1_RxReady(void) { return SPIx_RxReady(SPI1); }
INLINE Bool SPI2_RxReady(void) { return SPIx_RxReady(SPI2); }
INLINE Bool SPI3_RxReady(void) { return SPIx_RxReady(SPI3); }

// Check if transmit buffer is empty (register STATR.TXE)
INLINE Bool SPIx_TxEmpty(SPI_t* spi) { return (spi->STATR & B1) != 0; }
INLINE Bool SPI1_TxEmpty(void) { return SPIx_TxEmpty(SPI1); }
INLINE Bool SPI2_TxEmpty(void) { return SPIx_TxEmpty(SPI2); }
INLINE Bool SPI3_TxEmpty(void) { return SPIx_TxEmpty(SPI3); }

// Check if channel right has to be transmitted or has been received; or channel left otherwise (register STATR.CHSID)
INLINE Bool SPIx_ChRight(SPI_t* spi) { return (spi->STATR & B2) != 0; }
INLINE Bool SPI1_ChRight(void) { return SPIx_ChRight(SPI1); }
INLINE Bool SPI2_ChRight(void) { return SPIx_ChRight(SPI2); }
INLINE Bool SPI3_ChRight(void) { return SPIx_ChRight(SPI3); }

// Check/Clear underrun (register STATR.UDR)
INLINE Bool SPIx_Under(SPI_t* spi) { return (spi->STATR & B3) != 0; }
INLINE Bool SPI1_Under(void) { return SPIx_Under(SPI1); }
INLINE Bool SPI2_Under(void) { return SPIx_Under(SPI2); }
INLINE Bool SPI3_Under(void) { return SPIx_Under(SPI3); }

INLINE void SPIx_UnderClr(SPI_t* spi) { spi->STATR &= ~B3; }
INLINE void SPI1_UnderClr(void) { SPIx_UnderClr(SPI1); }
INLINE void SPI2_UnderClr(void) { SPIx_UnderClr(SPI2); }
INLINE void SPI3_UnderClr(void) { SPIx_UnderClr(SPI3); }

// Check/Clear CRC error (register STATR.CRCERR)
INLINE Bool SPIx_CRCErr(SPI_t* spi) { return (spi->STATR & B4) != 0; }
INLINE Bool SPI1_CRCErr(void) { return SPIx_CRCErr(SPI1); }
INLINE Bool SPI2_CRCErr(void) { return SPIx_CRCErr(SPI2); }
INLINE Bool SPI3_CRCErr(void) { return SPIx_CRCErr(SPI3); }

INLINE void SPIx_CRCErrClr(SPI_t* spi) { spi->STATR &= ~B4; }
INLINE void SPI1_CRCErrClr(void) { SPIx_CRCErrClr(SPI1); }
INLINE void SPI2_CRCErrClr(void) { SPIx_CRCErrClr(SPI2); }
INLINE void SPI3_CRCErrClr(void) { SPIx_CRCErrClr(SPI3); }

// Check/Clear mode fault error (register STATR.MODF)
INLINE Bool SPIx_ModeErr(SPI_t* spi) { return (spi->STATR & B5) != 0; }
INLINE Bool SPI1_ModeErr(void) { return SPIx_ModeErr(SPI1); }
INLINE Bool SPI2_ModeErr(void) { return SPIx_ModeErr(SPI2); }
INLINE Bool SPI3_ModeErr(void) { return SPIx_ModeErr(SPI3); }

INLINE void SPIx_ModeErrClr(SPI_t* spi) { spi->STATR &= ~B5; }
INLINE void SPI1_ModeErrClr(void) { SPIx_ModeErrClr(SPI1); }
INLINE void SPI2_ModeErrClr(void) { SPIx_ModeErrClr(SPI2); }
INLINE void SPI3_ModeErrClr(void) { SPIx_ModeErrClr(SPI3); }

// Check/Clear overrun (register STATR.OVR)
INLINE Bool SPIx_Over(SPI_t* spi) { return (spi->STATR & B6) != 0; }
INLINE Bool SPI1_Over(void) { return SPIx_Over(SPI1); }
INLINE Bool SPI2_Over(void) { return SPIx_Over(SPI2); }
INLINE Bool SPI3_Over(void) { return SPIx_Over(SPI3); }

INLINE void SPIx_OverClr(SPI_t* spi) { spi->STATR &= ~B6; }
INLINE void SPI1_OverClr(void) { SPIx_OverClr(SPI1); }
INLINE void SPI2_OverClr(void) { SPIx_OverClr(SPI2); }
INLINE void SPI3_OverClr(void) { SPIx_OverClr(SPI3); }

// Check busy flag (register STATR.BSY)
INLINE Bool SPIx_Busy(SPI_t* spi) { return (spi->STATR & B7) != 0; }
INLINE Bool SPI1_Busy(void) { return SPIx_Busy(SPI1); }
INLINE Bool SPI2_Busy(void) { return SPIx_Busy(SPI2); }
INLINE Bool SPI3_Busy(void) { return SPIx_Busy(SPI3); }

// === Data

// Send data
INLINE void SPIx_Write(SPI_t* spi, u16 data) { spi->DATAR = data; }
INLINE void SPI1_Write(u16 data) { SPIx_Write(SPI1, data); }
INLINE void SPI2_Write(u16 data) { SPIx_Write(SPI2, data); }
INLINE void SPI3_Write(u16 data) { SPIx_Write(SPI3, data); }

// Send data with wait
void SPIx_SendWait(SPI_t* spi, u16 data);
INLINE void SPI1_SendWait(u16 data) { SPIx_SendWait(SPI1, data); }
INLINE void SPI2_SendWait(u16 data) { SPIx_SendWait(SPI2, data); }
INLINE void SPI3_SendWait(u16 data) { SPIx_SendWait(SPI3, data); }

// Receive data
INLINE u16 SPIx_Read(SPI_t* spi) { return (u16)spi->DATAR; }
INLINE u16 SPI1_Read(void) { return SPIx_Read(SPI1); }
INLINE u16 SPI2_Read(void) { return SPIx_Read(SPI2); }
INLINE u16 SPI3_Read(void) { return SPIx_Read(SPI3); }

// Receive data with wait
u16 SPIx_RecvWait(SPI_t* spi);
INLINE u16 SPI1_RecvWait(void) { return SPIx_RecvWait(SPI1); }
INLINE u16 SPI2_RecvWait(void) { return SPIx_RecvWait(SPI2); }
INLINE u16 SPI3_RecvWait(void) { return SPIx_RecvWait(SPI3); }

// Set/Get CRC polynomial register (register CRCR)
INLINE void SPIx_Crc(SPI_t* spi, u16 crc) { spi->CRCR = crc; }
INLINE void SPI1_Crc(u16 crc) { SPIx_Crc(SPI1, crc); }
INLINE void SPI2_Crc(u16 crc) { SPIx_Crc(SPI2, crc); }
INLINE void SPI3_Crc(u16 crc) { SPIx_Crc(SPI3, crc); }

INLINE u16 SPIx_GetCrc(SPI_t* spi) { return (u16)spi->CRCR; }
INLINE u16 SPI1_GetCrc(void) { return SPIx_GetCrc(SPI1); }
INLINE u16 SPI2_GetCrc(void) { return SPIx_GetCrc(SPI2); }
INLINE u16 SPI3_GetCrc(void) { return SPIx_GetCrc(SPI3); }

// Get receive CRC register (register RCRCR)
INLINE u16 SPIx_RxCrc(SPI_t* spi) { return (u16)spi->RCRCR; }
INLINE u16 SPI1_RxCrc(void) { return SPIx_RxCrc(SPI1); }
INLINE u16 SPI2_RxCrc(void) { return SPIx_RxCrc(SPI2); }
INLINE u16 SPI3_RxCrc(void) { return SPIx_RxCrc(SPI3); }

// Get transmit CRC register (register TCRCR)
INLINE u16 SPIx_TxCrc(SPI_t* spi) { return (u16)spi->TCRCR; }
INLINE u16 SPI1_TxCrc(void) { return SPIx_TxCrc(SPI1); }
INLINE u16 SPI2_TxCrc(void) { return SPIx_TxCrc(SPI2); }
INLINE u16 SPI3_TxCrc(void) { return SPIx_TxCrc(SPI3); }

// Enable/Disable high-speed mode, when clock is divided by 2 (default disabled; register HSCR.HSRXEN)
INLINE void SPIx_HSEnable(SPI_t* spi) { spi->HSCR |= B0; }
INLINE void SPI1_HSEnable(void) { SPIx_HSEnable(SPI1); }
INLINE void SPI2_HSEnable(void) { SPIx_HSEnable(SPI2); }
INLINE void SPI3_HSEnable(void) { SPIx_HSEnable(SPI3); }

INLINE void SPIx_HSDisable(SPI_t* spi) { spi->HSCR &= ~B0; }
INLINE void SPI1_HSDisable(void) { SPIx_HSDisable(SPI1); }
INLINE void SPI2_HSDisable(void) { SPIx_HSDisable(SPI2); }
INLINE void SPI3_HSDisable(void) { SPIx_HSDisable(SPI3); }

// Enable/Disable high-speed mode 2, when clock is >= 120MHz (default disabled; register HSCR.HSRXEN2)
INLINE void SPIx_HS2Enable(SPI_t* spi) { spi->HSCR |= B2; }
INLINE void SPI1_HS2Enable(void) { SPIx_HS2Enable(SPI1); }
INLINE void SPI2_HS2Enable(void) { SPIx_HS2Enable(SPI2); }
INLINE void SPI3_HS2Enable(void) { SPIx_HS2Enable(SPI3); }

INLINE void SPIx_HS2Disable(SPI_t* spi) { spi->HSCR &= ~B2; }
INLINE void SPI1_HS2Disable(void) { SPIx_HS2Disable(SPI1); }
INLINE void SPI2_HS2Disable(void) { SPIx_HS2Disable(SPI2); }
INLINE void SPI3_HS2Disable(void) { SPIx_HS2Disable(SPI3); }

// === I2S

// Select I2S channel length 16-bits or 32-bits (register I2SCFGR.CHLEN; default 16-bits)
// - Only when data length is 16-bit, otherwise the channel is fixed to 32 bits by hardware.
INLINE void SPIx_I2S16bit(SPI_t* spi) { spi->I2SCFGR &= ~B0; }
INLINE void SPI1_I2S16bit(void) { SPIx_I2S16bit(SPI1); }
INLINE void SPI2_I2S16bit(void) { SPIx_I2S16bit(SPI2); }
INLINE void SPI3_I2S16bit(void) { SPIx_I2S16bit(SPI3); }

INLINE void SPIx_I2S32bit(SPI_t* spi) { spi->I2SCFGR |= B0; }
INLINE void SPI1_I2S32bit(void) { SPIx_I2S32bit(SPI1); }
INLINE void SPI2_I2S32bit(void) { SPIx_I2S32bit(SPI2); }
INLINE void SPI3_I2S32bit(void) { SPIx_I2S32bit(SPI3); }

// Setup I2S data length to I2S_DATALEN_* (register I2SCFGR.DATALEN[1:0]; default 16-bit)
INLINE void SPIx_I2SDataLen(SPI_t* spi, int datalen) { spi->I2SCFGR = (spi->I2SCFGR & ~(3 << 1)) | (datalen << 1); }
INLINE void SPI1_I2SDataLen(int datalen) { SPIx_I2SDataLen(SPI1, datalen); }
INLINE void SPI2_I2SDataLen(int datalen) { SPIx_I2SDataLen(SPI2, datalen); }
INLINE void SPI3_I2SDataLen(int datalen) { SPIx_I2SDataLen(SPI3, datalen); }

// Setup I2S quiescent clock polarity to Low or High (register I2SCFGR.CKPOL; default Low)
INLINE void SPIx_I2SClkPolLow(SPI_t* spi) { spi->I2SCFGR &= ~B3; }
INLINE void SPI1_I2SClkPolLow(void) { SPIx_I2SClkPolLow(SPI1); }
INLINE void SPI2_I2SClkPolLow(void) { SPIx_I2SClkPolLow(SPI2); }
INLINE void SPI3_I2SClkPolLow(void) { SPIx_I2SClkPolLow(SPI3); }

INLINE void SPIx_I2SClkPolHigh(SPI_t* spi) { spi->I2SCFGR |= B3; }
INLINE void SPI1_I2SClkPolHigh(void) { SPIx_I2SClkPolHigh(SPI1); }
INLINE void SPI2_I2SClkPolHigh(void) { SPIx_I2SClkPolHigh(SPI2); }
INLINE void SPI3_I2SClkPolHigh(void) { SPIx_I2SClkPolHigh(SPI3); }

// Select I2S standard I2S_STD_* (register I2SCFGR.I2SSTD[1:0]; default Philips standard)
INLINE void SPIx_I2SStd(SPI_t* spi, int std) { spi->I2SCFGR = (spi->I2SCFGR & ~(3 << 4)) | (std << 4); }
INLINE void SPI1_I2SStd(int std) { SPIx_I2SStd(SPI1, std); }
INLINE void SPI2_I2SStd(int std) { SPIx_I2SStd(SPI2, std); }
INLINE void SPI3_I2SStd(int std) { SPIx_I2SStd(SPI3, std); }

// Select I2S PCM frame synchronization to short or long (register I2SCFGR.PCMSYNC; default short)
INLINE void SPIx_I2SPCMShort(SPI_t* spi) { spi->I2SCFGR &= ~B7; }
INLINE void SPI1_I2SPCMShort(void) { SPIx_I2SPCMShort(SPI1); }
INLINE void SPI2_I2SPCMShort(void) { SPIx_I2SPCMShort(SPI2); }
INLINE void SPI3_I2SPCMShort(void) { SPIx_I2SPCMShort(SPI3); }

INLINE void SPIx_I2SPCMLong(SPI_t* spi) { spi->I2SCFGR |= B7; }
INLINE void SPI1_I2SPCMLong(void) { SPIx_I2SPCMLong(SPI1); }
INLINE void SPI2_I2SPCMLong(void) { SPIx_I2SPCMLong(SPI2); }
INLINE void SPI3_I2SPCMLong(void) { SPIx_I2SPCMLong(SPI3); }

// Select I2S mode I2S_MODE_* (register I2SCFGR.I2SCFG[1:0]; default slave transmits)
INLINE void SPIx_I2SMode(SPI_t* spi, int std) { spi->I2SCFGR = (spi->I2SCFGR & ~(3 << 4)) | (std << 4); }
INLINE void SPI1_I2SMode(int std) { SPIx_I2SMode(SPI1, std); }
INLINE void SPI2_I2SMode(int std) { SPIx_I2SMode(SPI2, std); }
INLINE void SPI3_I2SMode(int std) { SPIx_I2SMode(SPI3, std); }

// Enable/Disable I2S (register I2SCFGR.I2SE; default disabled)
INLINE void SPIx_I2SEnable(SPI_t* spi) { spi->I2SCFGR |= B10; }
INLINE void SPI1_I2SEnable(void) { SPIx_I2SEnable(SPI1); }
INLINE void SPI2_I2SEnable(void) { SPIx_I2SEnable(SPI2); }
INLINE void SPI3_I2SEnable(void) { SPIx_I2SEnable(SPI3); }

INLINE void SPIx_I2SDisable(SPI_t* spi) { spi->I2SCFGR &= ~B10; }
INLINE void SPI1_I2SDisable(void) { SPIx_I2SDisable(SPI1); }
INLINE void SPI2_I2SDisable(void) { SPIx_I2SDisable(SPI2); }
INLINE void SPI3_I2SDisable(void) { SPIx_I2SDisable(SPI3); }

// Select SPI or I2S mode (register I2SCFGR.I2SMOD; default SPI mode)
INLINE void SPIx_SPIMode(SPI_t* spi) { spi->I2SCFGR &= ~B11; }
INLINE void SPI1_SPIMode(void) { SPIx_SPIMode(SPI1); }
INLINE void SPI2_SPIMode(void) { SPIx_SPIMode(SPI2); }
INLINE void SPI3_SPIMode(void) { SPIx_SPIMode(SPI3); }

INLINE void SPIx_IS2Mode(SPI_t* spi) { spi->I2SCFGR |= B11; }
INLINE void SPI1_IS2Mode(void) { SPIx_IS2Mode(SPI1); }
INLINE void SPI2_IS2Mode(void) { SPIx_IS2Mode(SPI2); }
INLINE void SPI3_IS2Mode(void) { SPIx_IS2Mode(SPI3); }

// Set I2S divider 1..511 (registers I2SPR.I2SDIV and I2SPR.ODD; default 0 = divider is off)
INLINE void SPIx_I2SDiv(SPI_t* spi, int div)
{
	div = (div >> 1) | ((div & B0) << 8);
	spi->I2SPR = (spi->I2SPR & ~0x1ff) | div;
}
INLINE void SPI1_I2SDiv(int div) { SPIx_I2SDiv(SPI1, div); }
INLINE void SPI2_I2SDiv(int div) { SPIx_I2SDiv(SPI2, div); }
INLINE void SPI3_I2SDiv(int div) { SPIx_I2SDiv(SPI3, div); }

// Enable/Disable I2S master clock output (register I2SPR.MCKOE; default disabled)
INLINE void SPIx_I2SMClkEnable(SPI_t* spi) { spi->I2SPR |= B9; }
INLINE void SPI1_I2SMClkEnable(void) { SPIx_I2SMClkEnable(SPI1); }
INLINE void SPI2_I2SMClkEnable(void) { SPIx_I2SMClkEnable(SPI2); }
INLINE void SPI3_I2SMClkEnable(void) { SPIx_I2SMClkEnable(SPI3); }

INLINE void SPIx_I2SMClkDisable(SPI_t* spi) { spi->I2SPR &= ~B9; }
INLINE void SPI1_I2SMClkDisable(void) { SPIx_I2SMClkDisable(SPI1); }
INLINE void SPI2_I2SMClkDisable(void) { SPIx_I2SMClkDisable(SPI2); }
INLINE void SPI3_I2SMClkDisable(void) { SPIx_I2SMClkDisable(SPI3); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_SPI_H

#endif // USE_SPI
