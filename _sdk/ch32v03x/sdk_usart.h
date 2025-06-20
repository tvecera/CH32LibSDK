
// ****************************************************************************
//
//                                   USART
//
// ****************************************************************************

#if USE_USART		// 1=use USART peripheral

#ifndef _SDK_USART_H
#define _SDK_USART_H

#ifdef __cplusplus
extern "C" {
#endif

// USART registers
typedef struct {
	io32	STATR;		// 0x00: status register
	io32	DATAR;		// 0x04: data register
	io32	BRR;		// 0x08: baud rate register
	io32	CTLR1;		// 0x0C: control register 1
	io32	CTLR2;		// 0x10: control register 2
	io32	CTLR3;		// 0x14: control register 3
	io32	GPR;		// 0x18: protection and prescaler register
} USART_t;
STATIC_ASSERT(sizeof(USART_t) == 0x1C, "Incorrect USART_t!");
#define USART1	((USART_t*)USART1_BASE)	// 0x40013800
#define USART2	((USART_t*)USART2_BASE)	// 0x40004400
#define USART3	((USART_t*)USART3_BASE)	// 0x40004800
#define USART4	((USART_t*)USART4_BASE)	// 0x40004C00

// Get USART periphery from USART index 1.. (returns NULL if not valid)
INLINE USART_t* USART_GetHw(int inx) { return (inx == 1) ? USART1 : ((inx == 2) ? USART2 : NULL); }

// Get USART index 1.. from USART periphery (returns -1 if not valid)
INLINE int USART_GetInx(USART_t* usart) { return (usart == USART1) ? 1 : ((usart == USART2) ? 2 : -1); }

// Parity
#define USART_PARITY_NONE	0	// none parity
//#define USART_PARITY_NONE	1	// none parity
#define USART_PARITY_EVEN	2	// even parity
#define USART_PARITY_ODD	3	// odd parity

// Stop bits
#define USART_STOP_1		0	// 1 stop bit
#define USART_STOP_05		1	// 0.5 stop bit
#define USART_STOP_2		2	// 2 stop bits
#define USART_STOP_15		3	// 1.5 stop bit

// Word length
#define USART_WORDLEN_8		0x00	// 8 data bits
#define USART_WORDLEN_9		0x01	// 9 data bits

// === Get status

// Get parity error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_ParityError(USART_t* usart) { return (usart->STATR & B0) != 0; }
INLINE Bool USART1_ParityError(void) { return USARTx_ParityError(USART1); }
INLINE Bool USART2_ParityError(void) { return USARTx_ParityError(USART2); }
INLINE Bool USART3_ParityError(void) { return USARTx_ParityError(USART3); }
INLINE Bool USART4_ParityError(void) { return USARTx_ParityError(USART4); }

// Get frame error flag on receive (synchronization error)
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_FrameError(USART_t* usart) { return (usart->STATR & B1) != 0; }
INLINE Bool USART1_FrameError(void) { return USARTx_FrameError(USART1); }
INLINE Bool USART2_FrameError(void) { return USARTx_FrameError(USART2); }
INLINE Bool USART3_FrameError(void) { return USARTx_FrameError(USART3); }
INLINE Bool USART4_FrameError(void) { return USARTx_FrameError(USART4); }

// Get noise error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_NoiseError(USART_t* usart) { return (usart->STATR & B2) != 0; }
INLINE Bool USART1_NoiseError(void) { return USARTx_NoiseError(USART1); }
INLINE Bool USART2_NoiseError(void) { return USARTx_NoiseError(USART2); }
INLINE Bool USART3_NoiseError(void) { return USARTx_NoiseError(USART3); }
INLINE Bool USART4_NoiseError(void) { return USARTx_NoiseError(USART4); }

// Get overload error flag on receive (receive data buffer is full)
INLINE Bool USARTx_OverloadError(USART_t* usart) { return (usart->STATR & B3) != 0; }
INLINE Bool USART1_OverloadError(void) { return USARTx_OverloadError(USART1); }
INLINE Bool USART2_OverloadError(void) { return USARTx_OverloadError(USART2); }
INLINE Bool USART3_OverloadError(void) { return USARTx_OverloadError(USART3); }
INLINE Bool USART4_OverloadError(void) { return USARTx_OverloadError(USART4); }

// Get bus idle error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_IdleError(USART_t* usart) { return (usart->STATR & B4) != 0; }
INLINE Bool USART1_IdleError(void) { return USARTx_IdleError(USART1); }
INLINE Bool USART2_IdleError(void) { return USARTx_IdleError(USART2); }
INLINE Bool USART3_IdleError(void) { return USARTx_IdleError(USART3); }
INLINE Bool USART4_IdleError(void) { return USARTx_IdleError(USART4); }

// Check if received data is ready
// - Read data register to reset this bit, or use USART1_RxReadyClr() function.
INLINE Bool USARTx_RxReady(USART_t* usart) { return (usart->STATR & B5) != 0; }
INLINE Bool USART1_RxReady(void) { return USARTx_RxReady(USART1); }
INLINE Bool USART2_RxReady(void) { return USARTx_RxReady(USART2); }
INLINE Bool USART3_RxReady(void) { return USARTx_RxReady(USART3); }
INLINE Bool USART4_RxReady(void) { return USARTx_RxReady(USART4); }

// Clear flag "received data is ready"
INLINE void USARTx_RxReadyClr(USART_t* usart) { usart->STATR &= ~B5; }
INLINE void USART1_RxReadyClr(void) { USARTx_RxReadyClr(USART1); }
INLINE void USART2_RxReadyClr(void) { USARTx_RxReadyClr(USART2); }
INLINE void USART3_RxReadyClr(void) { USARTx_RxReadyClr(USART3); }
INLINE void USART4_RxReadyClr(void) { USARTx_RxReadyClr(USART4); }

// Check if sending is completed (frame from shift register was sent)
// - Read this bit and then write data register, of use USART_TxSentClr() function.
INLINE Bool USARTx_TxSent(USART_t* usart) { return (usart->STATR & B6) != 0; }
INLINE Bool USART1_TxSent(void) { return USARTx_TxSent(USART1); }
INLINE Bool USART2_TxSent(void) { return USARTx_TxSent(USART2); }
INLINE Bool USART3_TxSent(void) { return USARTx_TxSent(USART3); }
INLINE Bool USART4_TxSent(void) { return USARTx_TxSent(USART4); }

// Clear flag "sending is completed"
INLINE void USARTx_TxSentClr(USART_t* usart) { usart->STATR &= ~B6; }
INLINE void USART1_TxSentClr(void) { USARTx_TxSentClr(USART1); }
INLINE void USART2_TxSentClr(void) { USARTx_TxSentClr(USART2); }
INLINE void USART3_TxSentClr(void) { USARTx_TxSentClr(USART3); }
INLINE void USART4_TxSentClr(void) { USARTx_TxSentClr(USART4); }

// Check if sending buffer is empty (TDR register was transfered to shift register)
INLINE Bool USARTx_TxEmpty(USART_t* usart) { return (usart->STATR & B7) != 0; }
INLINE Bool USART1_TxEmpty(void) { return USARTx_TxEmpty(USART1); }
INLINE Bool USART2_TxEmpty(void) { return USARTx_TxEmpty(USART2); }
INLINE Bool USART3_TxEmpty(void) { return USARTx_TxEmpty(USART3); }
INLINE Bool USART4_TxEmpty(void) { return USARTx_TxEmpty(USART4); }

// Check if LIN Break was detected
INLINE Bool USARTx_BreakError(USART_t* usart) { return (usart->STATR & B8) != 0; }
INLINE Bool USART1_BreakError(void) { return USARTx_BreakError(USART1); }
INLINE Bool USART2_BreakError(void) { return USARTx_BreakError(USART2); }
INLINE Bool USART3_BreakError(void) { return USARTx_BreakError(USART3); }
INLINE Bool USART4_BreakError(void) { return USARTx_BreakError(USART4); }

// Clear LIN Break detection flag
INLINE void USARTx_BreakErrorClr(USART_t* usart) { usart->STATR &= ~B8; }
INLINE void USART1_BreakErrorClr(void) { USARTx_BreakErrorClr(USART1); }
INLINE void USART2_BreakErrorClr(void) { USARTx_BreakErrorClr(USART2); }
INLINE void USART3_BreakErrorClr(void) { USARTx_BreakErrorClr(USART3); }
INLINE void USART4_BreakErrorClr(void) { USARTx_BreakErrorClr(USART4); }

// Check CTS state change flag
INLINE void USARTx_CTSChange(USART_t* usart) { usart->STATR &= ~B9; }
INLINE void USART1_CTSChange(void) { USARTx_CTSChange(USART1); }
INLINE void USART2_CTSChange(void) { USARTx_CTSChange(USART2); }
INLINE void USART3_CTSChange(void) { USARTx_CTSChange(USART3); }
INLINE void USART4_CTSChange(void) { USARTx_CTSChange(USART4); }

// Clear CTS state change flag
INLINE void USARTx_CTSChangeClr(USART_t* usart) { usart->STATR &= ~B9; }
INLINE void USART1_CTSChangeClr(void) { USARTx_CTSChangeClr(USART1); }
INLINE void USART2_CTSChangeClr(void) { USARTx_CTSChangeClr(USART2); }
INLINE void USART3_CTSChangeClr(void) { USARTx_CTSChangeClr(USART3); }
INLINE void USART4_CTSChangeClr(void) { USARTx_CTSChangeClr(USART4); }

// === Setup

// USART RCC clock Enable/Disable
INLINE void USART1_RCCClkEnable(void) { RCC_USART1ClkEnable(); }
INLINE void USART2_RCCClkEnable(void) { RCC_USART2ClkEnable(); }
INLINE void USART3_RCCClkEnable(void) { RCC_USART3ClkEnable(); }
INLINE void USART4_RCCClkEnable(void) { RCC_USART4ClkEnable(); }
void USARTx_RCCClkEnable(USART_t* usart);

INLINE void USART1_RCCClkDisable(void) { RCC_USART1ClkDisable(); }
INLINE void USART2_RCCClkDisable(void) { RCC_USART2ClkDisable(); }
INLINE void USART3_RCCClkDisable(void) { RCC_USART3ClkDisable(); }
INLINE void USART4_RCCClkDisable(void) { RCC_USART4ClkDisable(); }
void USARTx_RCCClkDisable(USART_t* usart);

// USART reset to default values
INLINE void USART1_Reset(void) { RCC_USART1Reset(); }
INLINE void USART2_Reset(void) { RCC_USART2Reset(); }
INLINE void USART3_Reset(void) { RCC_USART3Reset(); }
INLINE void USART4_Reset(void) { RCC_USART4Reset(); }
void USARTx_Reset(USART_t* usart);

// Set integer baudrate register in 1/16 fractions (in range 16..65535)
//  baudrate = HCLK/ibaud, ibaud = HCLK/baudrate
//  Example: HCLK=48000000, baudrate=115200, ibaud=48000000/115200=417, real baudrate=115109, error 0.08%
INLINE void USARTx_IBaud(USART_t* usart, int ibaud) { usart->BRR = ibaud; }
INLINE void USART1_IBaud(int ibaud) { USARTx_IBaud(USART1, ibaud); }
INLINE void USART2_IBaud(int ibaud) { USARTx_IBaud(USART2, ibaud); }
INLINE void USART3_IBaud(int ibaud) { USARTx_IBaud(USART3, ibaud); }
INLINE void USART4_IBaud(int ibaud) { USARTx_IBaud(USART4, ibaud); }

// Get integer baudrate register in 1/16 fractions 16..65535
//  baudrate = HCLK/ibaud, ibaud = HCLK/baudrate
INLINE int USARTx_GetIBaud(USART_t* usart) { return usart->BRR & 0xffff; }
INLINE int USART1_GetIBaud(void) { return USARTx_GetIBaud(USART1); }
INLINE int USART2_GetIBaud(void) { return USARTx_GetIBaud(USART2); }
INLINE int USART3_GetIBaud(void) { return USARTx_GetIBaud(USART3); }
INLINE int USART4_GetIBaud(void) { return USARTx_GetIBaud(USART4); }

// Set baudrate (in Bauds)
void USARTx_Baud(USART_t* usart, int baud);
INLINE void USART1_Baud(int baud) { USARTx_Baud(USART1, baud); }
INLINE void USART2_Baud(int baud) { USARTx_Baud(USART2, baud); }
INLINE void USART3_Baud(int baud) { USARTx_Baud(USART3, baud); }
INLINE void USART4_Baud(int baud) { USARTx_Baud(USART4, baud); }

// Get baudrate (in Bauds)
int USARTx_GetBaud(USART_t* usart);
INLINE int USART1_GetBaud(void) { return USARTx_GetBaud(USART1); }
INLINE int USART2_GetBaud(void) { return USARTx_GetBaud(USART2); }
INLINE int USART3_GetBaud(void) { return USARTx_GetBaud(USART3); }
INLINE int USART4_GetBaud(void) { return USARTx_GetBaud(USART4); }

// Send break character
INLINE void USARTx_SendBreak(USART_t* usart) { usart->CTLR1 |= B0; }
INLINE void USART1_SendBreak(void) { USARTx_SendBreak(USART1); }
INLINE void USART2_SendBreak(void) { USARTx_SendBreak(USART2); }
INLINE void USART3_SendBreak(void) { USARTx_SendBreak(USART3); }
INLINE void USART4_SendBreak(void) { USARTx_SendBreak(USART4); }

// Enable/Disable receiver silent mode (receiver wakeup; default disabled)
// - Before enabling silent mode, the USART needs to receive a data byte first,
//   otherwise it cannot be woken up by bus idle in silent mode.
// - When configured as address mark wakeup, this flag cannot be modified
//   by software when RXNE (read data register non-empty) is set.
INLINE void USARTx_SilentEnable(USART_t* usart) { usart->CTLR1 |= B1; }
INLINE void USART1_SilentEnable(void) { USARTx_SilentEnable(USART1); }
INLINE void USART2_SilentEnable(void) { USARTx_SilentEnable(USART2); }
INLINE void USART3_SilentEnable(void) { USARTx_SilentEnable(USART3); }
INLINE void USART4_SilentEnable(void) { USARTx_SilentEnable(USART4); }

INLINE void USARTx_SilentDisable(USART_t* usart) { usart->CTLR1 &= ~B1; }
INLINE void USART1_SilentDisable(void) { USARTx_SilentDisable(USART1); }
INLINE void USART2_SilentDisable(void) { USARTx_SilentDisable(USART2); }
INLINE void USART3_SilentDisable(void) { USARTx_SilentDisable(USART3); }
INLINE void USART4_SilentDisable(void) { USARTx_SilentDisable(USART4); }

// Enable/Disable receiver (default disabled)
INLINE void USARTx_RxEnable(USART_t* usart) { usart->CTLR1 |= B2; }
INLINE void USART1_RxEnable(void) { USARTx_RxEnable(USART1); }
INLINE void USART2_RxEnable(void) { USARTx_RxEnable(USART2); }
INLINE void USART3_RxEnable(void) { USARTx_RxEnable(USART3); }
INLINE void USART4_RxEnable(void) { USARTx_RxEnable(USART4); }

INLINE void USARTx_RxDisable(USART_t* usart) { usart->CTLR1 &= ~B2; }
INLINE void USART1_RxDisable(void) { USARTx_RxDisable(USART1); }
INLINE void USART2_RxDisable(void) { USARTx_RxDisable(USART2); }
INLINE void USART3_RxDisable(void) { USARTx_RxDisable(USART3); }
INLINE void USART4_RxDisable(void) { USARTx_RxDisable(USART4); }

// Enable/Disable transmitter (default disabled)
INLINE void USARTx_TxEnable(USART_t* usart) { usart->CTLR1 |= B3; }
INLINE void USART1_TxEnable(void) { USARTx_TxEnable(USART1); }
INLINE void USART2_TxEnable(void) { USARTx_TxEnable(USART2); }
INLINE void USART3_TxEnable(void) { USARTx_TxEnable(USART3); }
INLINE void USART4_TxEnable(void) { USARTx_TxEnable(USART4); }

INLINE void USARTx_TxDisable(USART_t* usart) { usart->CTLR1 &= ~B3; }
INLINE void USART1_TxDisable(void) { USARTx_TxDisable(USART1); }
INLINE void USART2_TxDisable(void) { USARTx_TxDisable(USART2); }
INLINE void USART3_TxDisable(void) { USARTx_TxDisable(USART3); }
INLINE void USART4_TxDisable(void) { USARTx_TxDisable(USART4); }

// Set parity USART_PARITY_* (default none)
INLINE void USARTx_Parity(USART_t* usart, int parity) { usart->CTLR1 = (usart->CTLR1 & (3<<9)) | (parity<<9); }
INLINE void USART1_Parity(int parity) { USARTx_Parity(USART1, parity); }
INLINE void USART2_Parity(int parity) { USARTx_Parity(USART2, parity); }
INLINE void USART3_Parity(int parity) { USARTx_Parity(USART3, parity); }
INLINE void USART4_Parity(int parity) { USARTx_Parity(USART4, parity); }

// Set wake-up method to Address marker or Bus Idle (default marker)
INLINE void USARTx_WakeUpMarker(USART_t* usart) { usart->CTLR1 |= B11; }
INLINE void USART1_WakeUpMarker(void) { USARTx_WakeUpMarker(USART1); }
INLINE void USART2_WakeUpMarker(void) { USARTx_WakeUpMarker(USART2); }
INLINE void USART3_WakeUpMarker(void) { USARTx_WakeUpMarker(USART3); }
INLINE void USART4_WakeUpMarker(void) { USARTx_WakeUpMarker(USART4); }

INLINE void USARTx_WakeUpIdle(USART_t* usart) { usart->CTLR1 &= ~B11; }
INLINE void USART1_WakeUpIdle(void) { USARTx_WakeUpIdle(USART1); }
INLINE void USART2_WakeUpIdle(void) { USARTx_WakeUpIdle(USART2); }
INLINE void USART3_WakeUpIdle(void) { USARTx_WakeUpIdle(USART3); }
INLINE void USART4_WakeUpIdle(void) { USARTx_WakeUpIdle(USART4); }

// Set word length USART_WORDLEN_* (default 8 bits)
INLINE void USARTx_WordLen(USART_t* usart, int len)
	{ usart->CTLR1 = (usart->CTLR1 & ~(B12|B14|B15)) | (len << 12); }
INLINE void USART1_WordLen(int len) { USARTx_WordLen(USART1, len); }
INLINE void USART2_WordLen(int len) { USARTx_WordLen(USART2, len); }
INLINE void USART3_WordLen(int len) { USARTx_WordLen(USART3, len); }
INLINE void USART4_WordLen(int len) { USARTx_WordLen(USART4, len); }

// Enable/Disable USART (default disabled)
INLINE void USARTx_Enable(USART_t* usart) { usart->CTLR1 |= B13; }
INLINE void USART1_Enable(void) { USARTx_Enable(USART1); }
INLINE void USART2_Enable(void) { USARTx_Enable(USART2); }
INLINE void USART3_Enable(void) { USARTx_Enable(USART3); }
INLINE void USART4_Enable(void) { USARTx_Enable(USART4); }

INLINE void USARTx_Disable(USART_t* usart) { usart->CTLR1 &= ~B13; }
INLINE void USART1_Disable(void) { USARTx_Disable(USART1); }
INLINE void USART2_Disable(void) { USARTx_Disable(USART2); }
INLINE void USART3_Disable(void) { USARTx_Disable(USART3); }
INLINE void USART4_Disable(void) { USARTx_Disable(USART4); }

// Set address of USART node in multiprocessor communication 0..15 (default 0)
INLINE void USARTx_Addr(USART_t* usart, int addr) { usart->CTLR2 = (usart->CTLR1 & ~0x0f) | addr; }
INLINE void USART1_Addr(int addr) { USARTx_Addr(USART1, addr); }
INLINE void USART2_Addr(int addr) { USARTx_Addr(USART2, addr); }
INLINE void USART3_Addr(int addr) { USARTx_Addr(USART3, addr); }
INLINE void USART4_Addr(int addr) { USARTx_Addr(USART4, addr); }

// Set LIN Break detection length to 10 or 11 bits (default 10 bits)
INLINE void USARTx_LinBreak10(USART_t* usart) { usart->CTLR2 &= ~B5; }
INLINE void USART1_LinBreak10(void) { USARTx_LinBreak10(USART1); }
INLINE void USART2_LinBreak10(void) { USARTx_LinBreak10(USART2); }
INLINE void USART3_LinBreak10(void) { USARTx_LinBreak10(USART3); }
INLINE void USART4_LinBreak10(void) { USARTx_LinBreak10(USART4); }

INLINE void USARTx_LinBreak11(USART_t* usart) { usart->CTLR2 |= B5; }
INLINE void USART1_LinBreak11(void) { USARTx_LinBreak11(USART1); }
INLINE void USART2_LinBreak11(void) { USARTx_LinBreak11(USART2); }
INLINE void USART3_LinBreak11(void) { USARTx_LinBreak11(USART3); }
INLINE void USART4_LinBreak11(void) { USARTx_LinBreak11(USART4); }

// Enable/Disable output last bit clock pulse (default disabled)
INLINE void USARTx_LastBitClkEnable(USART_t* usart) { usart->CTLR2 |= B8; }
INLINE void USART1_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART1); }
INLINE void USART2_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART2); }
INLINE void USART3_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART3); }
INLINE void USART4_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART4); }

INLINE void USARTx_LastBitClkDisable(USART_t* usart) { usart->CTLR2 &= ~B8; }
INLINE void USART1_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART1); }
INLINE void USART2_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART2); }
INLINE void USART3_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART3); }
INLINE void USART4_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART4); }

// Select clock phase of data capture to First/Second edge (default first edge)
// - This bit cannot be modified after enabling transmit.
INLINE void USARTx_ClockPhaseFirst(USART_t* usart) { usart->CTLR2 &= ~B9; }
INLINE void USART1_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART1); }
INLINE void USART2_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART2); }
INLINE void USART3_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART3); }
INLINE void USART4_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART4); }

INLINE void USARTx_ClockPhaseSecond(USART_t* usart) { usart->CTLR2 |= B9; }
INLINE void USART1_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART1); }
INLINE void USART2_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART2); }
INLINE void USART3_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART3); }
INLINE void USART4_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART4); }

// Select clock polarity - steady Low or High level (default low)
// - This bit cannot be modified after enabling transmit.
INLINE void USARTx_ClockPolLow(USART_t* usart) { usart->CTLR2 &= ~B10; }
INLINE void USART1_ClockPolLow(void) { USARTx_ClockPolLow(USART1); }
INLINE void USART2_ClockPolLow(void) { USARTx_ClockPolLow(USART2); }
INLINE void USART3_ClockPolLow(void) { USARTx_ClockPolLow(USART3); }
INLINE void USART4_ClockPolLow(void) { USARTx_ClockPolLow(USART4); }

INLINE void USARTx_ClockPolHigh(USART_t* usart) { usart->CTLR2 |= B10; }
INLINE void USART1_ClockPolHigh(void) { USARTx_ClockPolHigh(USART1); }
INLINE void USART2_ClockPolHigh(void) { USARTx_ClockPolHigh(USART2); }
INLINE void USART3_ClockPolHigh(void) { USARTx_ClockPolHigh(USART3); }
INLINE void USART4_ClockPolHigh(void) { USARTx_ClockPolHigh(USART4); }

// Enable/Disable clock (default disabled)
INLINE void USARTx_ClockEnable(USART_t* usart) { usart->CTLR2 |= B11; }
INLINE void USART1_ClockEnable(void) { USARTx_ClockEnable(USART1); }
INLINE void USART2_ClockEnable(void) { USARTx_ClockEnable(USART2); }
INLINE void USART3_ClockEnable(void) { USARTx_ClockEnable(USART3); }
INLINE void USART4_ClockEnable(void) { USARTx_ClockEnable(USART4); }

INLINE void USARTx_ClockDisable(USART_t* usart) { usart->CTLR2 &= ~B11; }
INLINE void USART1_ClockDisable(void) { USARTx_ClockDisable(USART1); }
INLINE void USART2_ClockDisable(void) { USARTx_ClockDisable(USART2); }
INLINE void USART3_ClockDisable(void) { USARTx_ClockDisable(USART3); }
INLINE void USART4_ClockDisable(void) { USARTx_ClockDisable(USART4); }

// Set number of stop bits USART_STOP_* (default 1 stop bit)
INLINE void USARTx_Stop(USART_t* usart, int stop) { usart->CTLR2 = (usart->CTLR2 & ~(3 << 12)) | (stop << 12); }
INLINE void USART1_Stop(int stop) { USARTx_Stop(USART1, stop); }
INLINE void USART2_Stop(int stop) { USARTx_Stop(USART2, stop); }
INLINE void USART3_Stop(int stop) { USARTx_Stop(USART3, stop); }
INLINE void USART4_Stop(int stop) { USARTx_Stop(USART4, stop); }

// Enable/Disable LIN mode (capability to send LIN Synch Breaks; default disabled)
INLINE void USARTx_LINEnable(USART_t* usart) { usart->CTLR2 |= B14; }
INLINE void USART1_LINEnable(void) { USARTx_LINEnable(USART1); }
INLINE void USART2_LINEnable(void) { USARTx_LINEnable(USART2); }
INLINE void USART3_LINEnable(void) { USARTx_LINEnable(USART3); }
INLINE void USART4_LINEnable(void) { USARTx_LINEnable(USART4); }

INLINE void USARTx_LINDisable(USART_t* usart) { usart->CTLR2 &= ~B14; }
INLINE void USART1_LINDisable(void) { USARTx_LINDisable(USART1); }
INLINE void USART2_LINDisable(void) { USARTx_LINDisable(USART2); }
INLINE void USART3_LINDisable(void) { USARTx_LINDisable(USART3); }
INLINE void USART4_LINDisable(void) { USARTx_LINDisable(USART4); }

// Enable/Disable IsDA infrared mode (default disabled)
INLINE void USARTx_IrDAEnable(USART_t* usart) { usart->CTLR3 |= B1; }
INLINE void USART1_IrDAEnable(void) { USARTx_IrDAEnable(USART1); }
INLINE void USART2_IrDAEnable(void) { USARTx_IrDAEnable(USART2); }
INLINE void USART3_IrDAEnable(void) { USARTx_IrDAEnable(USART3); }
INLINE void USART4_IrDAEnable(void) { USARTx_IrDAEnable(USART4); }

INLINE void USARTx_IrDADisable(USART_t* usart) { usart->CTLR3 &= ~B1; }
INLINE void USART1_IrDADisable(void) { USARTx_IrDADisable(USART1); }
INLINE void USART2_IrDADisable(void) { USARTx_IrDADisable(USART2); }
INLINE void USART3_IrDADisable(void) { USARTx_IrDADisable(USART3); }
INLINE void USART4_IrDADisable(void) { USARTx_IrDADisable(USART4); }

// Enable/Disable IsDA low-power (default disabled)
INLINE void USARTx_IRLPEnable(USART_t* usart) { usart->CTLR3 |= B2; }
INLINE void USART1_IRLPEnable(void) { USARTx_IRLPEnable(USART1); }
INLINE void USART2_IRLPEnable(void) { USARTx_IRLPEnable(USART2); }
INLINE void USART3_IRLPEnable(void) { USARTx_IRLPEnable(USART3); }
INLINE void USART4_IRLPEnable(void) { USARTx_IRLPEnable(USART4); }

INLINE void USARTx_IRLPDisable(USART_t* usart) { usart->CTLR3 &= ~B2; }
INLINE void USART1_IRLPDisable(void) { USARTx_IRLPDisable(USART1); }
INLINE void USART2_IRLPDisable(void) { USARTx_IRLPDisable(USART2); }
INLINE void USART3_IRLPDisable(void) { USARTx_IRLPDisable(USART3); }
INLINE void USART4_IRLPDisable(void) { USARTx_IRLPDisable(USART4); }

// Enable/Disable half-duplex (default disabled)
INLINE void USARTx_HalfDuplexEnable(USART_t* usart) { usart->CTLR3 |= B3; }
INLINE void USART1_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART1); }
INLINE void USART2_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART2); }
INLINE void USART3_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART3); }
INLINE void USART4_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART4); }

INLINE void USARTx_HalfDuplexDisable(USART_t* usart) { usart->CTLR3 &= ~B3; }
INLINE void USART1_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART1); }
INLINE void USART2_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART2); }
INLINE void USART3_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART3); }
INLINE void USART4_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART4); }

// Enable/Disable smartcard NACK (default disabled)
INLINE void USARTx_NACKEnable(USART_t* usart) { usart->CTLR3 |= B4; }
INLINE void USART1_NACKEnable(void) { USARTx_NACKEnable(USART1); }
INLINE void USART2_NACKEnable(void) { USARTx_NACKEnable(USART2); }
INLINE void USART3_NACKEnable(void) { USARTx_NACKEnable(USART3); }
INLINE void USART4_NACKEnable(void) { USARTx_NACKEnable(USART4); }

INLINE void USARTx_NACKDisable(USART_t* usart) { usart->CTLR3 &= ~B4; }
INLINE void USART1_NACKDisable(void) { USARTx_NACKDisable(USART1); }
INLINE void USART2_NACKDisable(void) { USARTx_NACKDisable(USART2); }
INLINE void USART3_NACKDisable(void) { USARTx_NACKDisable(USART3); }
INLINE void USART4_NACKDisable(void) { USARTx_NACKDisable(USART4); }

// Enable/Disable smartcard mode (default disabled)
INLINE void USARTx_SmartCardEnable(USART_t* usart) { usart->CTLR3 |= B5; }
INLINE void USART1_SmartCardEnable(void) { USARTx_SmartCardEnable(USART1); }
INLINE void USART2_SmartCardEnable(void) { USARTx_SmartCardEnable(USART2); }
INLINE void USART3_SmartCardEnable(void) { USARTx_SmartCardEnable(USART3); }
INLINE void USART4_SmartCardEnable(void) { USARTx_SmartCardEnable(USART4); }

INLINE void USARTx_SmartCardDisable(USART_t* usart) { usart->CTLR3 &= ~B5; }
INLINE void USART1_SmartCardDisable(void) { USARTx_SmartCardDisable(USART1); }
INLINE void USART2_SmartCardDisable(void) { USARTx_SmartCardDisable(USART2); }
INLINE void USART3_SmartCardDisable(void) { USARTx_SmartCardDisable(USART3); }
INLINE void USART4_SmartCardDisable(void) { USARTx_SmartCardDisable(USART4); }

// Enable/Disable receive DMA (default disabled)
INLINE void USARTx_RxDMAEnable(USART_t* usart) { usart->CTLR3 |= B6; }
INLINE void USART1_RxDMAEnable(void) { USARTx_RxDMAEnable(USART1); }
INLINE void USART2_RxDMAEnable(void) { USARTx_RxDMAEnable(USART2); }
INLINE void USART3_RxDMAEnable(void) { USARTx_RxDMAEnable(USART3); }
INLINE void USART4_RxDMAEnable(void) { USARTx_RxDMAEnable(USART4); }

INLINE void USARTx_RxDMADisable(USART_t* usart) { usart->CTLR3 &= ~B6; }
INLINE void USART1_RxDMADisable(void) { USARTx_RxDMADisable(USART1); }
INLINE void USART2_RxDMADisable(void) { USARTx_RxDMADisable(USART2); }
INLINE void USART3_RxDMADisable(void) { USARTx_RxDMADisable(USART3); }
INLINE void USART4_RxDMADisable(void) { USARTx_RxDMADisable(USART4); }

// Enable/Disable tramsmit DMA (default disabled)
INLINE void USARTx_TxDMAEnable(USART_t* usart) { usart->CTLR3 |= B7; }
INLINE void USART1_TxDMAEnable(void) { USARTx_TxDMAEnable(USART1); }
INLINE void USART2_TxDMAEnable(void) { USARTx_TxDMAEnable(USART2); }
INLINE void USART3_TxDMAEnable(void) { USARTx_TxDMAEnable(USART3); }
INLINE void USART4_TxDMAEnable(void) { USARTx_TxDMAEnable(USART4); }

INLINE void USARTx_TxDMADisable(USART_t* usart) { usart->CTLR3 &= ~B7; }
INLINE void USART1_TxDMADisable(void) { USARTx_TxDMADisable(USART1); }
INLINE void USART2_TxDMADisable(void) { USARTx_TxDMADisable(USART2); }
INLINE void USART3_TxDMADisable(void) { USARTx_TxDMADisable(USART3); }
INLINE void USART4_TxDMADisable(void) { USARTx_TxDMADisable(USART4); }

// Enable/Disable RTS flow control (default disabled)
INLINE void USARTx_RTSEnable(USART_t* usart) { usart->CTLR3 |= B8; }
INLINE void USART1_RTSEnable(void) { USARTx_RTSEnable(USART1); }
INLINE void USART2_RTSEnable(void) { USARTx_RTSEnable(USART2); }
INLINE void USART3_RTSEnable(void) { USARTx_RTSEnable(USART3); }
INLINE void USART4_RTSEnable(void) { USARTx_RTSEnable(USART4); }

INLINE void USARTx_RTSDisable(USART_t* usart) { usart->CTLR3 &= ~B8; }
INLINE void USART1_RTSDisable(void) { USARTx_RTSDisable(USART1); }
INLINE void USART2_RTSDisable(void) { USARTx_RTSDisable(USART2); }
INLINE void USART3_RTSDisable(void) { USARTx_RTSDisable(USART3); }
INLINE void USART4_RTSDisable(void) { USARTx_RTSDisable(USART4); }

// Enable/Disable CTS flow control (default disabled)
INLINE void USARTx_CTSEnable(USART_t* usart) { usart->CTLR3 |= B9; }
INLINE void USART1_CTSEnable(void) { USARTx_CTSEnable(USART1); }
INLINE void USART2_CTSEnable(void) { USARTx_CTSEnable(USART2); }
INLINE void USART3_CTSEnable(void) { USARTx_CTSEnable(USART3); }
INLINE void USART4_CTSEnable(void) { USARTx_CTSEnable(USART4); }

INLINE void USARTx_CTSDisable(USART_t* usart) { usart->CTLR3 &= ~B9; }
INLINE void USART1_CTSDisable(void) { USARTx_CTSDisable(USART1); }
INLINE void USART2_CTSDisable(void) { USARTx_CTSDisable(USART2); }
INLINE void USART3_CTSDisable(void) { USARTx_CTSDisable(USART3); }
INLINE void USART4_CTSDisable(void) { USARTx_CTSDisable(USART4); }

// Set prescaler value (default 0)
// - In IrDA Low-power mode, source clock is divided by this value with a value of 0 indicating retention.
// - In normal IrDA mode, this can only be set to 1.
// - In smartcard mode, source clock is divided by twice this value to clock
//     the smart card, with a value of 0 indicating retention
INLINE void USARTx_Presc(USART_t* usart, int presc) { usart->GPR = (usart->GPR & ~0xff) | presc; }
INLINE void USART1_Presc(int presc) { USARTx_Presc(USART1, presc); }
INLINE void USART2_Presc(int presc) { USARTx_Presc(USART2, presc); }
INLINE void USART3_Presc(int presc) { USARTx_Presc(USART3, presc); }
INLINE void USART4_Presc(int presc) { USARTx_Presc(USART4, presc); }

// Get prescaler value
INLINE int USARTx_GetPresc(USART_t* usart) { return usart->GPR & 0xff; }
INLINE int USART1_GetPresc(void) { return USARTx_GetPresc(USART1); }
INLINE int USART2_GetPresc(void) { return USARTx_GetPresc(USART2); }
INLINE int USART3_GetPresc(void) { return USARTx_GetPresc(USART3); }
INLINE int USART4_GetPresc(void) { return USARTx_GetPresc(USART4); }

// Set guard time value in terms of number of baud clocks, used in smartcard mode (default 0)
INLINE void USARTx_GuardTime(USART_t* usart, int guardtime) { usart->GPR = (usart->GPR & ~(0xff << 8)) | (guardtime << 8); }
INLINE void USART1_GuardTime(int guardtime) { USARTx_GuardTime(USART1, guardtime); }
INLINE void USART2_GuardTime(int guardtime) { USARTx_GuardTime(USART2, guardtime); }
INLINE void USART3_GuardTime(int guardtime) { USARTx_GuardTime(USART3, guardtime); }
INLINE void USART4_GuardTime(int guardtime) { USARTx_GuardTime(USART4, guardtime); }

// Get guard time value in terms of number of baud clocks
INLINE int USARTx_GetGuardTime(USART_t* usart) { return (usart->GPR >> 8) & 0xff; }
INLINE int USART1_GetGuardTime(void) { return USARTx_GetGuardTime(USART1); }
INLINE int USART2_GetGuardTime(void) { return USARTx_GetGuardTime(USART2); }
INLINE int USART3_GetGuardTime(void) { return USARTx_GetGuardTime(USART3); }
INLINE int USART4_GetGuardTime(void) { return USARTx_GetGuardTime(USART4); }

// === Interrupt

// Enable/Disable IDLE interrupt (default disabled)
INLINE void USARTx_IdleIntEnable(USART_t* usart) { usart->CTLR1 |= B4; }
INLINE void USART1_IdleIntEnable(void) { USARTx_IdleIntEnable(USART1); }
INLINE void USART2_IdleIntEnable(void) { USARTx_IdleIntEnable(USART2); }
INLINE void USART3_IdleIntEnable(void) { USARTx_IdleIntEnable(USART3); }
INLINE void USART4_IdleIntEnable(void) { USARTx_IdleIntEnable(USART4); }

INLINE void USARTx_IdleIntDisable(USART_t* usart) { usart->CTLR1 &= ~B4; }
INLINE void USART1_IdleIntDisable(void) { USARTx_IdleIntDisable(USART1); }
INLINE void USART2_IdleIntDisable(void) { USARTx_IdleIntDisable(USART2); }
INLINE void USART3_IdleIntDisable(void) { USARTx_IdleIntDisable(USART3); }
INLINE void USART4_IdleIntDisable(void) { USARTx_IdleIntDisable(USART4); }

// Enable/Disable interrupt on "received data is ready" (default disabled)
INLINE void USARTx_RxReadyIntEnable(USART_t* usart) { usart->CTLR1 |= B5; }
INLINE void USART1_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART1); }
INLINE void USART2_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART2); }
INLINE void USART3_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART3); }
INLINE void USART4_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART4); }

INLINE void USARTx_RxReadyIntDisable(USART_t* usart) { usart->CTLR1 &= ~B5; }
INLINE void USART1_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART1); }
INLINE void USART2_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART2); }
INLINE void USART3_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART3); }
INLINE void USART4_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART4); }

// Enable/Disable interrupt on "sending is completed" (default disabled)
INLINE void USARTx_TxSentIntEnable(USART_t* usart) { usart->CTLR1 |= B6; }
INLINE void USART1_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART1); }
INLINE void USART2_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART2); }
INLINE void USART3_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART3); }
INLINE void USART4_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART4); }

INLINE void USARTx_TxSentIntDisable(USART_t* usart) { usart->CTLR1 &= ~B6; }
INLINE void USART1_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART1); }
INLINE void USART2_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART2); }
INLINE void USART3_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART3); }
INLINE void USART4_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART4); }

// Enable/Disable interrupt on "sending buffer is empty" (default disabled)
INLINE void USARTx_TxEmptyIntEnable(USART_t* usart) { usart->CTLR1 |= B7; }
INLINE void USART1_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART1); }
INLINE void USART2_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART2); }
INLINE void USART3_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART3); }
INLINE void USART4_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART4); }

INLINE void USARTx_TxEmptyIntDisable(USART_t* usart) { usart->CTLR1 &= ~B7; }
INLINE void USART1_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART1); }
INLINE void USART2_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART2); }
INLINE void USART3_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART3); }
INLINE void USART4_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART4); }

// Enable/Disable interrupt on parity error receive (default disabled)
INLINE void USARTx_ParityErrorIntEnable(USART_t* usart) { usart->CTLR1 |= B8; }
INLINE void USART1_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART1); }
INLINE void USART2_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART2); }
INLINE void USART3_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART3); }
INLINE void USART4_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART4); }

INLINE void USARTx_ParityErrorIntDisable(USART_t* usart) { usart->CTLR1 &= ~B8; }
INLINE void USART1_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART1); }
INLINE void USART2_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART2); }
INLINE void USART3_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART3); }
INLINE void USART4_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART4); }

// Enable/Disable LIN Break detection interrupt (default disabled)
INLINE void USARTx_LinBreakIntEnable(USART_t* usart) { usart->CTLR2 |= B6; }
INLINE void USART1_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART1); }
INLINE void USART2_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART2); }
INLINE void USART3_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART3); }
INLINE void USART4_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART4); }

INLINE void USARTx_LinBreakIntDisable(USART_t* usart) { usart->CTLR2 &= ~B6; }
INLINE void USART1_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART1); }
INLINE void USART2_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART2); }
INLINE void USART3_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART3); }
INLINE void USART4_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART4); }

// Enable/Disable error interrupt (default disabled)
INLINE void USARTx_ErrIntEnable(USART_t* usart) { usart->CTLR3 |= B0; }
INLINE void USART1_ErrIntEnable(void) { USARTx_ErrIntEnable(USART1); }
INLINE void USART2_ErrIntEnable(void) { USARTx_ErrIntEnable(USART2); }
INLINE void USART3_ErrIntEnable(void) { USARTx_ErrIntEnable(USART3); }
INLINE void USART4_ErrIntEnable(void) { USARTx_ErrIntEnable(USART4); }

INLINE void USARTx_ErrIntDisable(USART_t* usart) { usart->CTLR3 &= ~B0; }
INLINE void USART1_ErrIntDisable(void) { USARTx_ErrIntDisable(USART1); }
INLINE void USART2_ErrIntDisable(void) { USARTx_ErrIntDisable(USART2); }
INLINE void USART3_ErrIntDisable(void) { USARTx_ErrIntDisable(USART3); }
INLINE void USART4_ErrIntDisable(void) { USARTx_ErrIntDisable(USART4); }

// Enable/Disable CTS interrupt (default disabled)
INLINE void USARTx_CTSIntEnable(USART_t* usart) { usart->CTLR3 |= B10; }
INLINE void USART1_CTSIntEnable(void) { USARTx_CTSIntEnable(USART1); }
INLINE void USART2_CTSIntEnable(void) { USARTx_CTSIntEnable(USART2); }
INLINE void USART3_CTSIntEnable(void) { USARTx_CTSIntEnable(USART3); }
INLINE void USART4_CTSIntEnable(void) { USARTx_CTSIntEnable(USART4); }

INLINE void USARTx_CTSIntDisable(USART_t* usart) { usart->CTLR3 &= ~B10; }
INLINE void USART1_CTSIntDisable(void) { USARTx_CTSIntDisable(USART1); }
INLINE void USART2_CTSIntDisable(void) { USARTx_CTSIntDisable(USART2); }
INLINE void USART3_CTSIntDisable(void) { USARTx_CTSIntDisable(USART3); }
INLINE void USART4_CTSIntDisable(void) { USARTx_CTSIntDisable(USART4); }

// === Simple transfer in poll direct mode

// send data character (8 or 9 bits; send buffer must be empty)
INLINE void USARTx_TxWrite(USART_t* usart, int ch) { usart->DATAR = ch; }
INLINE void USART1_TxWrite(int ch) { USARTx_TxWrite(USART1, ch); }
INLINE void USART2_TxWrite(int ch) { USARTx_TxWrite(USART2, ch); }
INLINE void USART3_TxWrite(int ch) { USARTx_TxWrite(USART3, ch); }
INLINE void USART4_TxWrite(int ch) { USARTx_TxWrite(USART4, ch); }

// receve data character (8 or 9 bits; received character must be ready)
INLINE int USARTx_RxRead(USART_t* usart) { return usart->DATAR & 0x1ff; }
INLINE int USART1_RxRead(void) { return USARTx_RxRead(USART1); }
INLINE int USART2_RxRead(void) { return USARTx_RxRead(USART2); }
INLINE int USART3_RxRead(void) { return USARTx_RxRead(USART3); }
INLINE int USART4_RxRead(void) { return USARTx_RxRead(USART4); }

// send data character in poll mode with wait (8 or 9 bits)
void USARTx_SendChar(USART_t* usart, int ch);
INLINE void USART1_SendChar(int ch) { USARTx_SendChar(USART1, ch); }
INLINE void USART2_SendChar(int ch) { USARTx_SendChar(USART2, ch); }
INLINE void USART3_SendChar(int ch) { USARTx_SendChar(USART3, ch); }
INLINE void USART4_SendChar(int ch) { USARTx_SendChar(USART4, ch); }

// receive data character in poll mode with wait (8 or 9 bits)
int USARTx_RecvChar(USART_t* usart);
INLINE int USART1_RecvChar(void) { return USARTx_RecvChar(USART1); }
INLINE int USART2_RecvChar(void) { return USARTx_RecvChar(USART2); }
INLINE int USART3_RecvChar(void) { return USARTx_RecvChar(USART3); }
INLINE int USART4_RecvChar(void) { return USARTx_RecvChar(USART4); }

// send 8-bit data buffer in poll mode
void USARTx_SendBuf(USART_t* usart, const u8* buf, int len);
INLINE void USART1_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART1, buf, len); }
INLINE void USART2_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART2, buf, len); }
INLINE void USART3_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART3, buf, len); }
INLINE void USART4_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART4, buf, len); }

// receive 8-bit data buffer in poll mode, with time-out in [us] 0..89sec (returns number of bytes)
#define USART_LONGTIMEOUT 20000000	// long time-out
int USARTx_RecvBuf(USART_t* usart, u8* buf, int len, u32 timeout);
INLINE int USART1_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART1, buf, len, timeout); }
INLINE int USART2_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART2, buf, len, timeout); }
INLINE int USART3_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART3, buf, len, timeout); }
INLINE int USART4_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART4, buf, len, timeout); }

// send ASCIIZ text in poll mode
void USARTx_SendText(USART_t* usart, const char* buf);
INLINE void USART1_SendText(const char* buf) { USARTx_SendText(USART1, buf); }
INLINE void USART2_SendText(const char* buf) { USARTx_SendText(USART2, buf); }
INLINE void USART3_SendText(const char* buf) { USARTx_SendText(USART3, buf); }
INLINE void USART4_SendText(const char* buf) { USARTx_SendText(USART4, buf); }

// initialize simple transfer in polled direct mode (data length 8 bits)
//  baud ... baudrate in Bauds
//  wordlen ... word length USART_WORDLEN_*
//  parity ... parity USART_PARITY_*
//  stop ... stop bits USART_STOP_*
// Also set GPIOs as follows:
//  Remap USART1 (default 0) with GPIO_Remap_USART1(map):
//	0 ... PB10:TX, PB11:RX, PB9:CLK, PC16:CTS, PC17:RTS
//	1 ... PA10:TX, PA11:RX, PB9:CLK, PC16:CTS, PC17:RTS
//	2 ... PB10:TX, PB11:RX, PB5:CLK, PA9:CTS, PA8:RTS
//	3 ... PA7:TX, PB2:RX, PB12:CLK, PA13:CTS, PA14:RTS
//  Remap USART2 (default 0) with GPIO_Remap_USART2(map):
//	0 ... PA2:TX, PA3:RX, PA4:CLK, PA0:CTS, PA1:RTS
//	1 ... PA20:TX, PA19:RX, PA23:CLK, PA1:CTS, PA2:RTS
//	2 ... PA15:TX, PA16:RX, PA22:CLK, PA17:CTS, PA21:RTS
//	3 ... PC0:TX, PC1:RX, PB20:CLK, PC2:CTS, PC3:RTS
//	4-7 ... PA15:TX, PA16:RX, PA22:CLK, PA17:CTS, PC3:RTS
//  Remap USART3 (default 0) with GPIO_Remap_USART3(map):
//	0 ... PB3:TX, PB4:RX, PB5:CLK, PB6:CTS, PB7:RTS
//	1 ... PC18:TX, PC19:RX, PB5:CLK, PB6:CTS, PB7:RTS
//	2 ... PA18:TX, PB14:RX, PB8:CLK, PA3:CTS, PA4:RTS
//	3 ... PB16:TX, PB17:RX, (none CLK), PB18:CTS, PB19:RTS
//  Remap USART4 (default 0) with GPIO_Remap_USART4(map):
//	0 ... PB0:TX, PB1:RX, PB2:CLK, PB15:CTS, PA8:RTS
//	1 ... PA5:TX, PA9:RX, PA6:CLK, PA7:CTS, PB21:RTS
//	2 ... PC16:TX, PC17:RX, PB2:CLK, PB15:CTS, PA8:RTS
//	3 ... PB9:TX, PA10:RX, PB8:CLK, PA14:CTS, PA13:RTS
//	4,6 ... PB13:TX, PC19:RX, PA8:CLK, PA5:CTS, PA6:RTS
//	5,7 ... PC17:TX, PC16:RX, PB2:CLK, PB15:CTS, PA8:RTS
//  Set mode GPIO_Mode(gpio, mode) of TX to GPIO_MODE_AF
//  Set mode GPIO_Mode(gpio, mode) of RX to GPIO_MODE_IN
void USARTx_Init(USART_t* usart, int baud, int wordlen, int parity, int stop);
INLINE void USART1_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART1, baud, wordlen, parity, stop); }
INLINE void USART2_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART2, baud, wordlen, parity, stop); }
INLINE void USART3_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART3, baud, wordlen, parity, stop); }
INLINE void USART4_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART4, baud, wordlen, parity, stop); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_USART_H

#endif // USE_USART
