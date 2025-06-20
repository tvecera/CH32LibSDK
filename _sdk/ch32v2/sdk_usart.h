
// ****************************************************************************
//
//                                   USART
//
// ****************************************************************************
// USART1/2/3 are USART (synchronous asynchronous),
// USART4/5/6/7/8 are UART (asynchronous).
// For CH32V203C8: USART4 is USART (synchronous asynchronous),

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
	io32	CTLR4;		// 0x1C: control register 4
} USART_t;
STATIC_ASSERT(sizeof(USART_t) == 0x20, "Incorrect USART_t!");
#define USART1	((USART_t*)USART1_BASE)	// 0x40013800
#define USART2	((USART_t*)USART2_BASE)	// 0x40004400
#define USART3	((USART_t*)USART3_BASE)	// 0x40004800
#define USART4	((USART_t*)USART4_BASE)	// 0x40004C00
#define USART5	((USART_t*)USART5_BASE)	// 0x40005000
#define USART6	((USART_t*)USART6_BASE)	// 0x40001800
#define USART7	((USART_t*)USART7_BASE)	// 0x40001C00
#define USART8	((USART_t*)USART8_BASE)	// 0x40002000

// Get USART periphery from USART index 1.. (returns NULL if not valid)
INLINE USART_t* USART_GetHw(int inx)
{
	if (inx == 1) return USART1;
	if (inx == 2) return USART2;
	if (inx == 3) return USART3;
	if (inx == 4) return USART4;
	if (inx == 5) return USART5;
	if (inx == 6) return USART6;
	if (inx == 7) return USART7;
	if (inx == 8) return USART8;
	return NULL;
}

// Get USART index 1.. from USART periphery (returns -1 if not valid)
INLINE int USART_GetInx(USART_t* usart)
{
	if (usart == USART1) return 1;
	if (usart == USART2) return 2;
	if (usart == USART3) return 3;
	if (usart == USART4) return 4;
	if (usart == USART5) return 5;
	if (usart == USART6) return 6;
	if (usart == USART7) return 7;
	if (usart == USART8) return 8;
	return -1;
}

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

// Word length ... word length 5 to 7 data bits is not applicable for all MCU models
#define USART_WORDLEN_5		0x0c	// 5 data bits
#define USART_WORDLEN_6		0x08	// 6 data bits
#define USART_WORDLEN_7		0x04	// 7 data bits
#define USART_WORDLEN_8		0x00	// 8 data bits
#define USART_WORDLEN_9		0x01	// 9 data bits

// MARK and SPACE checksum
#define USART_MS_OFF		0	// MARK and SPACE disabled
#define USART_MS_MARK		2	// enable MARK checksum
#define USART_MS_SPACE		3	// enable SPACE checksum

// === Get status

// Get parity error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_ParityError(USART_t* usart) { return (usart->STATR & B0) != 0; }
INLINE Bool USART1_ParityError(void) { return USARTx_ParityError(USART1); }
INLINE Bool USART2_ParityError(void) { return USARTx_ParityError(USART2); }
INLINE Bool USART3_ParityError(void) { return USARTx_ParityError(USART3); }
INLINE Bool USART4_ParityError(void) { return USARTx_ParityError(USART4); }
INLINE Bool USART5_ParityError(void) { return USARTx_ParityError(USART5); }
INLINE Bool USART6_ParityError(void) { return USARTx_ParityError(USART6); }
INLINE Bool USART7_ParityError(void) { return USARTx_ParityError(USART7); }
INLINE Bool USART8_ParityError(void) { return USARTx_ParityError(USART8); }

// Get frame error flag on receive (synchronization error)
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_FrameError(USART_t* usart) { return (usart->STATR & B1) != 0; }
INLINE Bool USART1_FrameError(void) { return USARTx_FrameError(USART1); }
INLINE Bool USART2_FrameError(void) { return USARTx_FrameError(USART2); }
INLINE Bool USART3_FrameError(void) { return USARTx_FrameError(USART3); }
INLINE Bool USART4_FrameError(void) { return USARTx_FrameError(USART4); }
INLINE Bool USART5_FrameError(void) { return USARTx_FrameError(USART5); }
INLINE Bool USART6_FrameError(void) { return USARTx_FrameError(USART6); }
INLINE Bool USART7_FrameError(void) { return USARTx_FrameError(USART7); }
INLINE Bool USART8_FrameError(void) { return USARTx_FrameError(USART8); }

// Get noise error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_NoiseError(USART_t* usart) { return (usart->STATR & B2) != 0; }
INLINE Bool USART1_NoiseError(void) { return USARTx_NoiseError(USART1); }
INLINE Bool USART2_NoiseError(void) { return USARTx_NoiseError(USART2); }
INLINE Bool USART3_NoiseError(void) { return USARTx_NoiseError(USART3); }
INLINE Bool USART4_NoiseError(void) { return USARTx_NoiseError(USART4); }
INLINE Bool USART5_NoiseError(void) { return USARTx_NoiseError(USART5); }
INLINE Bool USART6_NoiseError(void) { return USARTx_NoiseError(USART6); }
INLINE Bool USART7_NoiseError(void) { return USARTx_NoiseError(USART7); }
INLINE Bool USART8_NoiseError(void) { return USARTx_NoiseError(USART8); }

// Get overload error flag on receive (receive data buffer is full)
INLINE Bool USARTx_OverloadError(USART_t* usart) { return (usart->STATR & B3) != 0; }
INLINE Bool USART1_OverloadError(void) { return USARTx_OverloadError(USART1); }
INLINE Bool USART2_OverloadError(void) { return USARTx_OverloadError(USART2); }
INLINE Bool USART3_OverloadError(void) { return USARTx_OverloadError(USART3); }
INLINE Bool USART4_OverloadError(void) { return USARTx_OverloadError(USART4); }
INLINE Bool USART5_OverloadError(void) { return USARTx_OverloadError(USART5); }
INLINE Bool USART6_OverloadError(void) { return USARTx_OverloadError(USART6); }
INLINE Bool USART7_OverloadError(void) { return USARTx_OverloadError(USART7); }
INLINE Bool USART8_OverloadError(void) { return USARTx_OverloadError(USART8); }

// Get bus idle error flag on receive
// - Read this bit and then read data register to reset this bit.
INLINE Bool USARTx_IdleError(USART_t* usart) { return (usart->STATR & B4) != 0; }
INLINE Bool USART1_IdleError(void) { return USARTx_IdleError(USART1); }
INLINE Bool USART2_IdleError(void) { return USARTx_IdleError(USART2); }
INLINE Bool USART3_IdleError(void) { return USARTx_IdleError(USART3); }
INLINE Bool USART4_IdleError(void) { return USARTx_IdleError(USART4); }
INLINE Bool USART5_IdleError(void) { return USARTx_IdleError(USART5); }
INLINE Bool USART6_IdleError(void) { return USARTx_IdleError(USART6); }
INLINE Bool USART7_IdleError(void) { return USARTx_IdleError(USART7); }
INLINE Bool USART8_IdleError(void) { return USARTx_IdleError(USART8); }

// Check if received data is ready
// - Read data register to reset this bit, or use USART1_RxReadyClr() function.
INLINE Bool USARTx_RxReady(USART_t* usart) { return (usart->STATR & B5) != 0; }
INLINE Bool USART1_RxReady(void) { return USARTx_RxReady(USART1); }
INLINE Bool USART2_RxReady(void) { return USARTx_RxReady(USART2); }
INLINE Bool USART3_RxReady(void) { return USARTx_RxReady(USART3); }
INLINE Bool USART4_RxReady(void) { return USARTx_RxReady(USART4); }
INLINE Bool USART5_RxReady(void) { return USARTx_RxReady(USART5); }
INLINE Bool USART6_RxReady(void) { return USARTx_RxReady(USART6); }
INLINE Bool USART7_RxReady(void) { return USARTx_RxReady(USART7); }
INLINE Bool USART8_RxReady(void) { return USARTx_RxReady(USART8); }

// Clear flag "received data is ready"
INLINE void USARTx_RxReadyClr(USART_t* usart) { usart->STATR &= ~B5; }
INLINE void USART1_RxReadyClr(void) { USARTx_RxReadyClr(USART1); }
INLINE void USART2_RxReadyClr(void) { USARTx_RxReadyClr(USART2); }
INLINE void USART3_RxReadyClr(void) { USARTx_RxReadyClr(USART3); }
INLINE void USART4_RxReadyClr(void) { USARTx_RxReadyClr(USART4); }
INLINE void USART5_RxReadyClr(void) { USARTx_RxReadyClr(USART5); }
INLINE void USART6_RxReadyClr(void) { USARTx_RxReadyClr(USART6); }
INLINE void USART7_RxReadyClr(void) { USARTx_RxReadyClr(USART7); }
INLINE void USART8_RxReadyClr(void) { USARTx_RxReadyClr(USART8); }

// Check if sending is completed (frame from shift register was sent)
// - Read this bit and then write data register, of use USART_TxSentClr() function.
INLINE Bool USARTx_TxSent(USART_t* usart) { return (usart->STATR & B6) != 0; }
INLINE Bool USART1_TxSent(void) { return USARTx_TxSent(USART1); }
INLINE Bool USART2_TxSent(void) { return USARTx_TxSent(USART2); }
INLINE Bool USART3_TxSent(void) { return USARTx_TxSent(USART3); }
INLINE Bool USART4_TxSent(void) { return USARTx_TxSent(USART4); }
INLINE Bool USART5_TxSent(void) { return USARTx_TxSent(USART5); }
INLINE Bool USART6_TxSent(void) { return USARTx_TxSent(USART6); }
INLINE Bool USART7_TxSent(void) { return USARTx_TxSent(USART7); }
INLINE Bool USART8_TxSent(void) { return USARTx_TxSent(USART8); }

// Clear flag "sending is completed"
INLINE void USARTx_TxSentClr(USART_t* usart) { usart->STATR &= ~B6; }
INLINE void USART1_TxSentClr(void) { USARTx_TxSentClr(USART1); }
INLINE void USART2_TxSentClr(void) { USARTx_TxSentClr(USART2); }
INLINE void USART3_TxSentClr(void) { USARTx_TxSentClr(USART3); }
INLINE void USART4_TxSentClr(void) { USARTx_TxSentClr(USART4); }
INLINE void USART5_TxSentClr(void) { USARTx_TxSentClr(USART5); }
INLINE void USART6_TxSentClr(void) { USARTx_TxSentClr(USART6); }
INLINE void USART7_TxSentClr(void) { USARTx_TxSentClr(USART7); }
INLINE void USART8_TxSentClr(void) { USARTx_TxSentClr(USART8); }

// Check if sending buffer is empty (TDR register was transfered to shift register)
INLINE Bool USARTx_TxEmpty(USART_t* usart) { return (usart->STATR & B7) != 0; }
INLINE Bool USART1_TxEmpty(void) { return USARTx_TxEmpty(USART1); }
INLINE Bool USART2_TxEmpty(void) { return USARTx_TxEmpty(USART2); }
INLINE Bool USART3_TxEmpty(void) { return USARTx_TxEmpty(USART3); }
INLINE Bool USART4_TxEmpty(void) { return USARTx_TxEmpty(USART4); }
INLINE Bool USART5_TxEmpty(void) { return USARTx_TxEmpty(USART5); }
INLINE Bool USART6_TxEmpty(void) { return USARTx_TxEmpty(USART6); }
INLINE Bool USART7_TxEmpty(void) { return USARTx_TxEmpty(USART7); }
INLINE Bool USART8_TxEmpty(void) { return USARTx_TxEmpty(USART8); }

// Check if LIN Break was detected
INLINE Bool USARTx_BreakError(USART_t* usart) { return (usart->STATR & B8) != 0; }
INLINE Bool USART1_BreakError(void) { return USARTx_BreakError(USART1); }
INLINE Bool USART2_BreakError(void) { return USARTx_BreakError(USART2); }
INLINE Bool USART3_BreakError(void) { return USARTx_BreakError(USART3); }
INLINE Bool USART4_BreakError(void) { return USARTx_BreakError(USART4); }
INLINE Bool USART5_BreakError(void) { return USARTx_BreakError(USART5); }
INLINE Bool USART6_BreakError(void) { return USARTx_BreakError(USART6); }
INLINE Bool USART7_BreakError(void) { return USARTx_BreakError(USART7); }
INLINE Bool USART8_BreakError(void) { return USARTx_BreakError(USART8); }

// Clear LIN Break detection flag
INLINE void USARTx_BreakErrorClr(USART_t* usart) { usart->STATR &= ~B8; }
INLINE void USART1_BreakErrorClr(void) { USARTx_BreakErrorClr(USART1); }
INLINE void USART2_BreakErrorClr(void) { USARTx_BreakErrorClr(USART2); }
INLINE void USART3_BreakErrorClr(void) { USARTx_BreakErrorClr(USART3); }
INLINE void USART4_BreakErrorClr(void) { USARTx_BreakErrorClr(USART4); }
INLINE void USART5_BreakErrorClr(void) { USARTx_BreakErrorClr(USART5); }
INLINE void USART6_BreakErrorClr(void) { USARTx_BreakErrorClr(USART6); }
INLINE void USART7_BreakErrorClr(void) { USARTx_BreakErrorClr(USART7); }
INLINE void USART8_BreakErrorClr(void) { USARTx_BreakErrorClr(USART8); }

// Check CTS state change flag
INLINE void USARTx_CTSChange(USART_t* usart) { usart->STATR &= ~B9; }
INLINE void USART1_CTSChange(void) { USARTx_CTSChange(USART1); }
INLINE void USART2_CTSChange(void) { USARTx_CTSChange(USART2); }
INLINE void USART3_CTSChange(void) { USARTx_CTSChange(USART3); }
INLINE void USART4_CTSChange(void) { USARTx_CTSChange(USART4); }
INLINE void USART5_CTSChange(void) { USARTx_CTSChange(USART5); }
INLINE void USART6_CTSChange(void) { USARTx_CTSChange(USART6); }
INLINE void USART7_CTSChange(void) { USARTx_CTSChange(USART7); }
INLINE void USART8_CTSChange(void) { USARTx_CTSChange(USART8); }

// Clear CTS state change flag
INLINE void USARTx_CTSChangeClr(USART_t* usart) { usart->STATR &= ~B9; }
INLINE void USART1_CTSChangeClr(void) { USARTx_CTSChangeClr(USART1); }
INLINE void USART2_CTSChangeClr(void) { USARTx_CTSChangeClr(USART2); }
INLINE void USART3_CTSChangeClr(void) { USARTx_CTSChangeClr(USART3); }
INLINE void USART4_CTSChangeClr(void) { USARTx_CTSChangeClr(USART4); }
INLINE void USART5_CTSChangeClr(void) { USARTx_CTSChangeClr(USART5); }
INLINE void USART6_CTSChangeClr(void) { USARTx_CTSChangeClr(USART6); }
INLINE void USART7_CTSChangeClr(void) { USARTx_CTSChangeClr(USART7); }
INLINE void USART8_CTSChangeClr(void) { USARTx_CTSChangeClr(USART8); }

// Check if receiver is busy (this flag is not applicable for all MCU models)
INLINE Bool USARTx_RxBusy(USART_t* usart) { return (usart->STATR & B10) != 0; }
INLINE Bool USART1_RxBusy(void) { return USARTx_RxBusy(USART1); }
INLINE Bool USART2_RxBusy(void) { return USARTx_RxBusy(USART2); }
INLINE Bool USART3_RxBusy(void) { return USARTx_RxBusy(USART3); }
INLINE Bool USART4_RxBusy(void) { return USARTx_RxBusy(USART4); }
INLINE Bool USART5_RxBusy(void) { return USARTx_RxBusy(USART5); }
INLINE Bool USART6_RxBusy(void) { return USARTx_RxBusy(USART6); }
INLINE Bool USART7_RxBusy(void) { return USARTx_RxBusy(USART7); }
INLINE Bool USART8_RxBusy(void) { return USARTx_RxBusy(USART8); }

// Check MARK or SPACE checksum error (this flag is not applicable for all MCU models)
INLINE Bool USARTx_MSError(USART_t* usart) { return (usart->STATR & B10) != 0; }
INLINE Bool USART1_MSError(void) { return USARTx_MSError(USART1); }
INLINE Bool USART2_MSError(void) { return USARTx_MSError(USART2); }
INLINE Bool USART3_MSError(void) { return USARTx_MSError(USART3); }
INLINE Bool USART4_MSError(void) { return USARTx_MSError(USART4); }
INLINE Bool USART5_MSError(void) { return USARTx_MSError(USART5); }
INLINE Bool USART6_MSError(void) { return USARTx_MSError(USART6); }
INLINE Bool USART7_MSError(void) { return USARTx_MSError(USART7); }
INLINE Bool USART8_MSError(void) { return USARTx_MSError(USART8); }

// === Setup

// USART RCC clock Enable/Disable
INLINE void USART1_RCCClkEnable(void) { RCC_USART1ClkEnable(); }
INLINE void USART2_RCCClkEnable(void) { RCC_USART2ClkEnable(); }
INLINE void USART3_RCCClkEnable(void) { RCC_USART3ClkEnable(); }
INLINE void USART4_RCCClkEnable(void) { RCC_USART4ClkEnable(); }
INLINE void USART5_RCCClkEnable(void) { RCC_USART5ClkEnable(); }
INLINE void USART6_RCCClkEnable(void) { RCC_USART6ClkEnable(); }
INLINE void USART7_RCCClkEnable(void) { RCC_USART7ClkEnable(); }
INLINE void USART8_RCCClkEnable(void) { RCC_USART8ClkEnable(); }
void USARTx_RCCClkEnable(USART_t* usart);

INLINE void USART1_RCCClkDisable(void) { RCC_USART1ClkDisable(); }
INLINE void USART2_RCCClkDisable(void) { RCC_USART2ClkDisable(); }
INLINE void USART3_RCCClkDisable(void) { RCC_USART3ClkDisable(); }
INLINE void USART4_RCCClkDisable(void) { RCC_USART4ClkDisable(); }
INLINE void USART5_RCCClkDisable(void) { RCC_USART5ClkDisable(); }
INLINE void USART6_RCCClkDisable(void) { RCC_USART6ClkDisable(); }
INLINE void USART7_RCCClkDisable(void) { RCC_USART7ClkDisable(); }
INLINE void USART8_RCCClkDisable(void) { RCC_USART8ClkDisable(); }
void USARTx_RCCClkDisable(USART_t* usart);

// USART reset to default values
INLINE void USART1_Reset(void) { RCC_USART1Reset(); }
INLINE void USART2_Reset(void) { RCC_USART2Reset(); }
INLINE void USART3_Reset(void) { RCC_USART3Reset(); }
INLINE void USART4_Reset(void) { RCC_USART4Reset(); }
INLINE void USART5_Reset(void) { RCC_USART5Reset(); }
INLINE void USART6_Reset(void) { RCC_USART6Reset(); }
INLINE void USART7_Reset(void) { RCC_USART7Reset(); }
INLINE void USART8_Reset(void) { RCC_USART8Reset(); }
void USARTx_Reset(USART_t* usart);

// Set integer baudrate register in 1/16 fractions (in range 16..65535)
//  baudrate = HCLK/ibaud, ibaud = HCLK/baudrate
//  Example: HCLK=48000000, baudrate=115200, ibaud=48000000/115200=417, real baudrate=115109, error 0.08%
INLINE void USARTx_IBaud(USART_t* usart, int ibaud) { usart->BRR = ibaud; }
INLINE void USART1_IBaud(int ibaud) { USARTx_IBaud(USART1, ibaud); }
INLINE void USART2_IBaud(int ibaud) { USARTx_IBaud(USART2, ibaud); }
INLINE void USART3_IBaud(int ibaud) { USARTx_IBaud(USART3, ibaud); }
INLINE void USART4_IBaud(int ibaud) { USARTx_IBaud(USART4, ibaud); }
INLINE void USART5_IBaud(int ibaud) { USARTx_IBaud(USART5, ibaud); }
INLINE void USART6_IBaud(int ibaud) { USARTx_IBaud(USART6, ibaud); }
INLINE void USART7_IBaud(int ibaud) { USARTx_IBaud(USART7, ibaud); }
INLINE void USART8_IBaud(int ibaud) { USARTx_IBaud(USART8, ibaud); }

// Get integer baudrate register in 1/16 fractions 16..65535
//  baudrate = HCLK/ibaud, ibaud = HCLK/baudrate
INLINE int USARTx_GetIBaud(USART_t* usart) { return usart->BRR & 0xffff; }
INLINE int USART1_GetIBaud(void) { return USARTx_GetIBaud(USART1); }
INLINE int USART2_GetIBaud(void) { return USARTx_GetIBaud(USART2); }
INLINE int USART3_GetIBaud(void) { return USARTx_GetIBaud(USART3); }
INLINE int USART4_GetIBaud(void) { return USARTx_GetIBaud(USART4); }
INLINE int USART5_GetIBaud(void) { return USARTx_GetIBaud(USART5); }
INLINE int USART6_GetIBaud(void) { return USARTx_GetIBaud(USART6); }
INLINE int USART7_GetIBaud(void) { return USARTx_GetIBaud(USART7); }
INLINE int USART8_GetIBaud(void) { return USARTx_GetIBaud(USART8); }

// Set baudrate (in Bauds)
void USARTx_Baud(USART_t* usart, int baud);
INLINE void USART1_Baud(int baud) { USARTx_Baud(USART1, baud); }
INLINE void USART2_Baud(int baud) { USARTx_Baud(USART2, baud); }
INLINE void USART3_Baud(int baud) { USARTx_Baud(USART3, baud); }
INLINE void USART4_Baud(int baud) { USARTx_Baud(USART4, baud); }
INLINE void USART5_Baud(int baud) { USARTx_Baud(USART5, baud); }
INLINE void USART6_Baud(int baud) { USARTx_Baud(USART6, baud); }
INLINE void USART7_Baud(int baud) { USARTx_Baud(USART7, baud); }
INLINE void USART8_Baud(int baud) { USARTx_Baud(USART8, baud); }

// Get baudrate (in Bauds)
int USARTx_GetBaud(USART_t* usart);
INLINE int USART1_GetBaud(void) { return USARTx_GetBaud(USART1); }
INLINE int USART2_GetBaud(void) { return USARTx_GetBaud(USART2); }
INLINE int USART3_GetBaud(void) { return USARTx_GetBaud(USART3); }
INLINE int USART4_GetBaud(void) { return USARTx_GetBaud(USART4); }
INLINE int USART5_GetBaud(void) { return USARTx_GetBaud(USART5); }
INLINE int USART6_GetBaud(void) { return USARTx_GetBaud(USART6); }
INLINE int USART7_GetBaud(void) { return USARTx_GetBaud(USART7); }
INLINE int USART8_GetBaud(void) { return USARTx_GetBaud(USART8); }

// Send break character
INLINE void USARTx_SendBreak(USART_t* usart) { usart->CTLR1 |= B0; }
INLINE void USART1_SendBreak(void) { USARTx_SendBreak(USART1); }
INLINE void USART2_SendBreak(void) { USARTx_SendBreak(USART2); }
INLINE void USART3_SendBreak(void) { USARTx_SendBreak(USART3); }
INLINE void USART4_SendBreak(void) { USARTx_SendBreak(USART4); }
INLINE void USART5_SendBreak(void) { USARTx_SendBreak(USART5); }
INLINE void USART6_SendBreak(void) { USARTx_SendBreak(USART6); }
INLINE void USART7_SendBreak(void) { USARTx_SendBreak(USART7); }
INLINE void USART8_SendBreak(void) { USARTx_SendBreak(USART8); }

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
INLINE void USART5_SilentEnable(void) { USARTx_SilentEnable(USART5); }
INLINE void USART6_SilentEnable(void) { USARTx_SilentEnable(USART6); }
INLINE void USART7_SilentEnable(void) { USARTx_SilentEnable(USART7); }
INLINE void USART8_SilentEnable(void) { USARTx_SilentEnable(USART8); }

INLINE void USARTx_SilentDisable(USART_t* usart) { usart->CTLR1 &= ~B1; }
INLINE void USART1_SilentDisable(void) { USARTx_SilentDisable(USART1); }
INLINE void USART2_SilentDisable(void) { USARTx_SilentDisable(USART2); }
INLINE void USART3_SilentDisable(void) { USARTx_SilentDisable(USART3); }
INLINE void USART4_SilentDisable(void) { USARTx_SilentDisable(USART4); }
INLINE void USART5_SilentDisable(void) { USARTx_SilentDisable(USART5); }
INLINE void USART6_SilentDisable(void) { USARTx_SilentDisable(USART6); }
INLINE void USART7_SilentDisable(void) { USARTx_SilentDisable(USART7); }
INLINE void USART8_SilentDisable(void) { USARTx_SilentDisable(USART8); }

// Enable/Disable receiver (default disabled)
INLINE void USARTx_RxEnable(USART_t* usart) { usart->CTLR1 |= B2; }
INLINE void USART1_RxEnable(void) { USARTx_RxEnable(USART1); }
INLINE void USART2_RxEnable(void) { USARTx_RxEnable(USART2); }
INLINE void USART3_RxEnable(void) { USARTx_RxEnable(USART3); }
INLINE void USART4_RxEnable(void) { USARTx_RxEnable(USART4); }
INLINE void USART5_RxEnable(void) { USARTx_RxEnable(USART5); }
INLINE void USART6_RxEnable(void) { USARTx_RxEnable(USART6); }
INLINE void USART7_RxEnable(void) { USARTx_RxEnable(USART7); }
INLINE void USART8_RxEnable(void) { USARTx_RxEnable(USART8); }

INLINE void USARTx_RxDisable(USART_t* usart) { usart->CTLR1 &= ~B2; }
INLINE void USART1_RxDisable(void) { USARTx_RxDisable(USART1); }
INLINE void USART2_RxDisable(void) { USARTx_RxDisable(USART2); }
INLINE void USART3_RxDisable(void) { USARTx_RxDisable(USART3); }
INLINE void USART4_RxDisable(void) { USARTx_RxDisable(USART4); }
INLINE void USART5_RxDisable(void) { USARTx_RxDisable(USART5); }
INLINE void USART6_RxDisable(void) { USARTx_RxDisable(USART6); }
INLINE void USART7_RxDisable(void) { USARTx_RxDisable(USART7); }
INLINE void USART8_RxDisable(void) { USARTx_RxDisable(USART8); }

// Enable/Disable transmitter (default disabled)
INLINE void USARTx_TxEnable(USART_t* usart) { usart->CTLR1 |= B3; }
INLINE void USART1_TxEnable(void) { USARTx_TxEnable(USART1); }
INLINE void USART2_TxEnable(void) { USARTx_TxEnable(USART2); }
INLINE void USART3_TxEnable(void) { USARTx_TxEnable(USART3); }
INLINE void USART4_TxEnable(void) { USARTx_TxEnable(USART4); }
INLINE void USART5_TxEnable(void) { USARTx_TxEnable(USART5); }
INLINE void USART6_TxEnable(void) { USARTx_TxEnable(USART6); }
INLINE void USART7_TxEnable(void) { USARTx_TxEnable(USART7); }
INLINE void USART8_TxEnable(void) { USARTx_TxEnable(USART8); }

INLINE void USARTx_TxDisable(USART_t* usart) { usart->CTLR1 &= ~B3; }
INLINE void USART1_TxDisable(void) { USARTx_TxDisable(USART1); }
INLINE void USART2_TxDisable(void) { USARTx_TxDisable(USART2); }
INLINE void USART3_TxDisable(void) { USARTx_TxDisable(USART3); }
INLINE void USART4_TxDisable(void) { USARTx_TxDisable(USART4); }
INLINE void USART5_TxDisable(void) { USARTx_TxDisable(USART5); }
INLINE void USART6_TxDisable(void) { USARTx_TxDisable(USART6); }
INLINE void USART7_TxDisable(void) { USARTx_TxDisable(USART7); }
INLINE void USART8_TxDisable(void) { USARTx_TxDisable(USART8); }

// Set parity USART_PARITY_* (default none)
INLINE void USARTx_Parity(USART_t* usart, int parity) { usart->CTLR1 = (usart->CTLR1 & (3<<9)) | (parity<<9); }
INLINE void USART1_Parity(int parity) { USARTx_Parity(USART1, parity); }
INLINE void USART2_Parity(int parity) { USARTx_Parity(USART2, parity); }
INLINE void USART3_Parity(int parity) { USARTx_Parity(USART3, parity); }
INLINE void USART4_Parity(int parity) { USARTx_Parity(USART4, parity); }
INLINE void USART5_Parity(int parity) { USARTx_Parity(USART5, parity); }
INLINE void USART6_Parity(int parity) { USARTx_Parity(USART6, parity); }
INLINE void USART7_Parity(int parity) { USARTx_Parity(USART7, parity); }
INLINE void USART8_Parity(int parity) { USARTx_Parity(USART8, parity); }

// Set wake-up method to Address marker or Bus Idle (default marker)
INLINE void USARTx_WakeUpMarker(USART_t* usart) { usart->CTLR1 |= B11; }
INLINE void USART1_WakeUpMarker(void) { USARTx_WakeUpMarker(USART1); }
INLINE void USART2_WakeUpMarker(void) { USARTx_WakeUpMarker(USART2); }
INLINE void USART3_WakeUpMarker(void) { USARTx_WakeUpMarker(USART3); }
INLINE void USART4_WakeUpMarker(void) { USARTx_WakeUpMarker(USART4); }
INLINE void USART5_WakeUpMarker(void) { USARTx_WakeUpMarker(USART5); }
INLINE void USART6_WakeUpMarker(void) { USARTx_WakeUpMarker(USART6); }
INLINE void USART7_WakeUpMarker(void) { USARTx_WakeUpMarker(USART7); }
INLINE void USART8_WakeUpMarker(void) { USARTx_WakeUpMarker(USART8); }

INLINE void USARTx_WakeUpIdle(USART_t* usart) { usart->CTLR1 &= ~B11; }
INLINE void USART1_WakeUpIdle(void) { USARTx_WakeUpIdle(USART1); }
INLINE void USART2_WakeUpIdle(void) { USARTx_WakeUpIdle(USART2); }
INLINE void USART3_WakeUpIdle(void) { USARTx_WakeUpIdle(USART3); }
INLINE void USART4_WakeUpIdle(void) { USARTx_WakeUpIdle(USART4); }
INLINE void USART5_WakeUpIdle(void) { USARTx_WakeUpIdle(USART5); }
INLINE void USART6_WakeUpIdle(void) { USARTx_WakeUpIdle(USART6); }
INLINE void USART7_WakeUpIdle(void) { USARTx_WakeUpIdle(USART7); }
INLINE void USART8_WakeUpIdle(void) { USARTx_WakeUpIdle(USART8); }

// Set word length USART_WORDLEN_* (default 8 bits)
// - Word length 5 to 7 data bits is not applicable for all MCU models
INLINE void USARTx_WordLen(USART_t* usart, int len)
	{ usart->CTLR1 = (usart->CTLR1 & ~(B12|B14|B15)) | (len << 12); }
INLINE void USART1_WordLen(int len) { USARTx_WordLen(USART1, len); }
INLINE void USART2_WordLen(int len) { USARTx_WordLen(USART2, len); }
INLINE void USART3_WordLen(int len) { USARTx_WordLen(USART3, len); }
INLINE void USART4_WordLen(int len) { USARTx_WordLen(USART4, len); }
INLINE void USART5_WordLen(int len) { USARTx_WordLen(USART5, len); }
INLINE void USART6_WordLen(int len) { USARTx_WordLen(USART6, len); }
INLINE void USART7_WordLen(int len) { USARTx_WordLen(USART7, len); }
INLINE void USART8_WordLen(int len) { USARTx_WordLen(USART8, len); }

// Enable/Disable USART (default disabled)
INLINE void USARTx_Enable(USART_t* usart) { usart->CTLR1 |= B13; }
INLINE void USART1_Enable(void) { USARTx_Enable(USART1); }
INLINE void USART2_Enable(void) { USARTx_Enable(USART2); }
INLINE void USART3_Enable(void) { USARTx_Enable(USART3); }
INLINE void USART4_Enable(void) { USARTx_Enable(USART4); }
INLINE void USART5_Enable(void) { USARTx_Enable(USART5); }
INLINE void USART6_Enable(void) { USARTx_Enable(USART6); }
INLINE void USART7_Enable(void) { USARTx_Enable(USART7); }
INLINE void USART8_Enable(void) { USARTx_Enable(USART8); }

INLINE void USARTx_Disable(USART_t* usart) { usart->CTLR1 &= ~B13; }
INLINE void USART1_Disable(void) { USARTx_Disable(USART1); }
INLINE void USART2_Disable(void) { USARTx_Disable(USART2); }
INLINE void USART3_Disable(void) { USARTx_Disable(USART3); }
INLINE void USART4_Disable(void) { USARTx_Disable(USART4); }
INLINE void USART5_Disable(void) { USARTx_Disable(USART5); }
INLINE void USART6_Disable(void) { USARTx_Disable(USART6); }
INLINE void USART7_Disable(void) { USARTx_Disable(USART7); }
INLINE void USART8_Disable(void) { USARTx_Disable(USART8); }

// Set address of USART node in multiprocessor communication 0..15 (default 0)
INLINE void USARTx_Addr(USART_t* usart, int addr) { usart->CTLR2 = (usart->CTLR1 & ~0x0f) | addr; }
INLINE void USART1_Addr(int addr) { USARTx_Addr(USART1, addr); }
INLINE void USART2_Addr(int addr) { USARTx_Addr(USART2, addr); }
INLINE void USART3_Addr(int addr) { USARTx_Addr(USART3, addr); }
INLINE void USART4_Addr(int addr) { USARTx_Addr(USART4, addr); }
INLINE void USART5_Addr(int addr) { USARTx_Addr(USART5, addr); }
INLINE void USART6_Addr(int addr) { USARTx_Addr(USART6, addr); }
INLINE void USART7_Addr(int addr) { USARTx_Addr(USART7, addr); }
INLINE void USART8_Addr(int addr) { USARTx_Addr(USART8, addr); }

// Set LIN Break detection length to 10 or 11 bits (default 10 bits)
INLINE void USARTx_LinBreak10(USART_t* usart) { usart->CTLR2 &= ~B5; }
INLINE void USART1_LinBreak10(void) { USARTx_LinBreak10(USART1); }
INLINE void USART2_LinBreak10(void) { USARTx_LinBreak10(USART2); }
INLINE void USART3_LinBreak10(void) { USARTx_LinBreak10(USART3); }
INLINE void USART4_LinBreak10(void) { USARTx_LinBreak10(USART4); }
INLINE void USART5_LinBreak10(void) { USARTx_LinBreak10(USART5); }
INLINE void USART6_LinBreak10(void) { USARTx_LinBreak10(USART6); }
INLINE void USART7_LinBreak10(void) { USARTx_LinBreak10(USART7); }
INLINE void USART8_LinBreak10(void) { USARTx_LinBreak10(USART8); }

INLINE void USARTx_LinBreak11(USART_t* usart) { usart->CTLR2 |= B5; }
INLINE void USART1_LinBreak11(void) { USARTx_LinBreak11(USART1); }
INLINE void USART2_LinBreak11(void) { USARTx_LinBreak11(USART2); }
INLINE void USART3_LinBreak11(void) { USARTx_LinBreak11(USART3); }
INLINE void USART4_LinBreak11(void) { USARTx_LinBreak11(USART4); }
INLINE void USART5_LinBreak11(void) { USARTx_LinBreak11(USART5); }
INLINE void USART6_LinBreak11(void) { USARTx_LinBreak11(USART6); }
INLINE void USART7_LinBreak11(void) { USARTx_LinBreak11(USART7); }
INLINE void USART8_LinBreak11(void) { USARTx_LinBreak11(USART8); }

// Enable/Disable output last bit clock pulse (default disabled)
INLINE void USARTx_LastBitClkEnable(USART_t* usart) { usart->CTLR2 |= B8; }
INLINE void USART1_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART1); }
INLINE void USART2_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART2); }
INLINE void USART3_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART3); }
INLINE void USART4_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART4); }
INLINE void USART5_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART5); }
INLINE void USART6_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART6); }
INLINE void USART7_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART7); }
INLINE void USART8_LastBitClkEnable(void) { USARTx_LastBitClkEnable(USART8); }

INLINE void USARTx_LastBitClkDisable(USART_t* usart) { usart->CTLR2 &= ~B8; }
INLINE void USART1_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART1); }
INLINE void USART2_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART2); }
INLINE void USART3_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART3); }
INLINE void USART4_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART4); }
INLINE void USART5_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART5); }
INLINE void USART6_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART6); }
INLINE void USART7_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART7); }
INLINE void USART8_LastBitClkDisable(void) { USARTx_LastBitClkDisable(USART8); }

// Select clock phase of data capture to First/Second edge (default first edge)
// - This bit cannot be modified after enabling transmit.
INLINE void USARTx_ClockPhaseFirst(USART_t* usart) { usart->CTLR2 &= ~B9; }
INLINE void USART1_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART1); }
INLINE void USART2_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART2); }
INLINE void USART3_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART3); }
INLINE void USART4_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART4); }
INLINE void USART5_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART5); }
INLINE void USART6_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART6); }
INLINE void USART7_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART7); }
INLINE void USART8_ClockPhaseFirst(void) { USARTx_ClockPhaseFirst(USART8); }

INLINE void USARTx_ClockPhaseSecond(USART_t* usart) { usart->CTLR2 |= B9; }
INLINE void USART1_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART1); }
INLINE void USART2_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART2); }
INLINE void USART3_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART3); }
INLINE void USART4_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART4); }
INLINE void USART5_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART5); }
INLINE void USART6_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART6); }
INLINE void USART7_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART7); }
INLINE void USART8_ClockPhaseSecond(void) { USARTx_ClockPhaseSecond(USART8); }

// Select clock polarity - steady Low or High level (default low)
// - This bit cannot be modified after enabling transmit.
INLINE void USARTx_ClockPolLow(USART_t* usart) { usart->CTLR2 &= ~B10; }
INLINE void USART1_ClockPolLow(void) { USARTx_ClockPolLow(USART1); }
INLINE void USART2_ClockPolLow(void) { USARTx_ClockPolLow(USART2); }
INLINE void USART3_ClockPolLow(void) { USARTx_ClockPolLow(USART3); }
INLINE void USART4_ClockPolLow(void) { USARTx_ClockPolLow(USART4); }
INLINE void USART5_ClockPolLow(void) { USARTx_ClockPolLow(USART5); }
INLINE void USART6_ClockPolLow(void) { USARTx_ClockPolLow(USART6); }
INLINE void USART7_ClockPolLow(void) { USARTx_ClockPolLow(USART7); }
INLINE void USART8_ClockPolLow(void) { USARTx_ClockPolLow(USART8); }

INLINE void USARTx_ClockPolHigh(USART_t* usart) { usart->CTLR2 |= B10; }
INLINE void USART1_ClockPolHigh(void) { USARTx_ClockPolHigh(USART1); }
INLINE void USART2_ClockPolHigh(void) { USARTx_ClockPolHigh(USART2); }
INLINE void USART3_ClockPolHigh(void) { USARTx_ClockPolHigh(USART3); }
INLINE void USART4_ClockPolHigh(void) { USARTx_ClockPolHigh(USART4); }
INLINE void USART5_ClockPolHigh(void) { USARTx_ClockPolHigh(USART5); }
INLINE void USART6_ClockPolHigh(void) { USARTx_ClockPolHigh(USART6); }
INLINE void USART7_ClockPolHigh(void) { USARTx_ClockPolHigh(USART7); }
INLINE void USART8_ClockPolHigh(void) { USARTx_ClockPolHigh(USART8); }

// Enable/Disable clock (default disabled)
INLINE void USARTx_ClockEnable(USART_t* usart) { usart->CTLR2 |= B11; }
INLINE void USART1_ClockEnable(void) { USARTx_ClockEnable(USART1); }
INLINE void USART2_ClockEnable(void) { USARTx_ClockEnable(USART2); }
INLINE void USART3_ClockEnable(void) { USARTx_ClockEnable(USART3); }
INLINE void USART4_ClockEnable(void) { USARTx_ClockEnable(USART4); }
INLINE void USART5_ClockEnable(void) { USARTx_ClockEnable(USART5); }
INLINE void USART6_ClockEnable(void) { USARTx_ClockEnable(USART6); }
INLINE void USART7_ClockEnable(void) { USARTx_ClockEnable(USART7); }
INLINE void USART8_ClockEnable(void) { USARTx_ClockEnable(USART8); }

INLINE void USARTx_ClockDisable(USART_t* usart) { usart->CTLR2 &= ~B11; }
INLINE void USART1_ClockDisable(void) { USARTx_ClockDisable(USART1); }
INLINE void USART2_ClockDisable(void) { USARTx_ClockDisable(USART2); }
INLINE void USART3_ClockDisable(void) { USARTx_ClockDisable(USART3); }
INLINE void USART4_ClockDisable(void) { USARTx_ClockDisable(USART4); }
INLINE void USART5_ClockDisable(void) { USARTx_ClockDisable(USART5); }
INLINE void USART6_ClockDisable(void) { USARTx_ClockDisable(USART6); }
INLINE void USART7_ClockDisable(void) { USARTx_ClockDisable(USART7); }
INLINE void USART8_ClockDisable(void) { USARTx_ClockDisable(USART8); }

// Set number of stop bits USART_STOP_* (default 1 stop bit)
INLINE void USARTx_Stop(USART_t* usart, int stop) { usart->CTLR2 = (usart->CTLR2 & ~(3 << 12)) | (stop << 12); }
INLINE void USART1_Stop(int stop) { USARTx_Stop(USART1, stop); }
INLINE void USART2_Stop(int stop) { USARTx_Stop(USART2, stop); }
INLINE void USART3_Stop(int stop) { USARTx_Stop(USART3, stop); }
INLINE void USART4_Stop(int stop) { USARTx_Stop(USART4, stop); }
INLINE void USART5_Stop(int stop) { USARTx_Stop(USART5, stop); }
INLINE void USART6_Stop(int stop) { USARTx_Stop(USART6, stop); }
INLINE void USART7_Stop(int stop) { USARTx_Stop(USART7, stop); }
INLINE void USART8_Stop(int stop) { USARTx_Stop(USART8, stop); }

// Enable/Disable LIN mode (capability to send LIN Synch Breaks; default disabled)
INLINE void USARTx_LINEnable(USART_t* usart) { usart->CTLR2 |= B14; }
INLINE void USART1_LINEnable(void) { USARTx_LINEnable(USART1); }
INLINE void USART2_LINEnable(void) { USARTx_LINEnable(USART2); }
INLINE void USART3_LINEnable(void) { USARTx_LINEnable(USART3); }
INLINE void USART4_LINEnable(void) { USARTx_LINEnable(USART4); }
INLINE void USART5_LINEnable(void) { USARTx_LINEnable(USART5); }
INLINE void USART6_LINEnable(void) { USARTx_LINEnable(USART6); }
INLINE void USART7_LINEnable(void) { USARTx_LINEnable(USART7); }
INLINE void USART8_LINEnable(void) { USARTx_LINEnable(USART8); }

INLINE void USARTx_LINDisable(USART_t* usart) { usart->CTLR2 &= ~B14; }
INLINE void USART1_LINDisable(void) { USARTx_LINDisable(USART1); }
INLINE void USART2_LINDisable(void) { USARTx_LINDisable(USART2); }
INLINE void USART3_LINDisable(void) { USARTx_LINDisable(USART3); }
INLINE void USART4_LINDisable(void) { USARTx_LINDisable(USART4); }
INLINE void USART5_LINDisable(void) { USARTx_LINDisable(USART5); }
INLINE void USART6_LINDisable(void) { USARTx_LINDisable(USART6); }
INLINE void USART7_LINDisable(void) { USARTx_LINDisable(USART7); }
INLINE void USART8_LINDisable(void) { USARTx_LINDisable(USART8); }

// Enable/Disable IsDA infrared mode (default disabled)
INLINE void USARTx_IrDAEnable(USART_t* usart) { usart->CTLR3 |= B1; }
INLINE void USART1_IrDAEnable(void) { USARTx_IrDAEnable(USART1); }
INLINE void USART2_IrDAEnable(void) { USARTx_IrDAEnable(USART2); }
INLINE void USART3_IrDAEnable(void) { USARTx_IrDAEnable(USART3); }
INLINE void USART4_IrDAEnable(void) { USARTx_IrDAEnable(USART4); }
INLINE void USART5_IrDAEnable(void) { USARTx_IrDAEnable(USART5); }
INLINE void USART6_IrDAEnable(void) { USARTx_IrDAEnable(USART6); }
INLINE void USART7_IrDAEnable(void) { USARTx_IrDAEnable(USART7); }
INLINE void USART8_IrDAEnable(void) { USARTx_IrDAEnable(USART8); }

INLINE void USARTx_IrDADisable(USART_t* usart) { usart->CTLR3 &= ~B1; }
INLINE void USART1_IrDADisable(void) { USARTx_IrDADisable(USART1); }
INLINE void USART2_IrDADisable(void) { USARTx_IrDADisable(USART2); }
INLINE void USART3_IrDADisable(void) { USARTx_IrDADisable(USART3); }
INLINE void USART4_IrDADisable(void) { USARTx_IrDADisable(USART4); }
INLINE void USART5_IrDADisable(void) { USARTx_IrDADisable(USART5); }
INLINE void USART6_IrDADisable(void) { USARTx_IrDADisable(USART6); }
INLINE void USART7_IrDADisable(void) { USARTx_IrDADisable(USART7); }
INLINE void USART8_IrDADisable(void) { USARTx_IrDADisable(USART8); }

// Enable/Disable IsDA low-power (default disabled)
INLINE void USARTx_IRLPEnable(USART_t* usart) { usart->CTLR3 |= B2; }
INLINE void USART1_IRLPEnable(void) { USARTx_IRLPEnable(USART1); }
INLINE void USART2_IRLPEnable(void) { USARTx_IRLPEnable(USART2); }
INLINE void USART3_IRLPEnable(void) { USARTx_IRLPEnable(USART3); }
INLINE void USART4_IRLPEnable(void) { USARTx_IRLPEnable(USART4); }
INLINE void USART5_IRLPEnable(void) { USARTx_IRLPEnable(USART5); }
INLINE void USART6_IRLPEnable(void) { USARTx_IRLPEnable(USART6); }
INLINE void USART7_IRLPEnable(void) { USARTx_IRLPEnable(USART7); }
INLINE void USART8_IRLPEnable(void) { USARTx_IRLPEnable(USART8); }

INLINE void USARTx_IRLPDisable(USART_t* usart) { usart->CTLR3 &= ~B2; }
INLINE void USART1_IRLPDisable(void) { USARTx_IRLPDisable(USART1); }
INLINE void USART2_IRLPDisable(void) { USARTx_IRLPDisable(USART2); }
INLINE void USART3_IRLPDisable(void) { USARTx_IRLPDisable(USART3); }
INLINE void USART4_IRLPDisable(void) { USARTx_IRLPDisable(USART4); }
INLINE void USART5_IRLPDisable(void) { USARTx_IRLPDisable(USART5); }
INLINE void USART6_IRLPDisable(void) { USARTx_IRLPDisable(USART6); }
INLINE void USART7_IRLPDisable(void) { USARTx_IRLPDisable(USART7); }
INLINE void USART8_IRLPDisable(void) { USARTx_IRLPDisable(USART8); }

// Enable/Disable half-duplex (default disabled)
INLINE void USARTx_HalfDuplexEnable(USART_t* usart) { usart->CTLR3 |= B3; }
INLINE void USART1_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART1); }
INLINE void USART2_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART2); }
INLINE void USART3_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART3); }
INLINE void USART4_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART4); }
INLINE void USART5_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART5); }
INLINE void USART6_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART6); }
INLINE void USART7_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART7); }
INLINE void USART8_HalfDuplexEnable(void) { USARTx_HalfDuplexEnable(USART8); }

INLINE void USARTx_HalfDuplexDisable(USART_t* usart) { usart->CTLR3 &= ~B3; }
INLINE void USART1_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART1); }
INLINE void USART2_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART2); }
INLINE void USART3_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART3); }
INLINE void USART4_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART4); }
INLINE void USART5_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART5); }
INLINE void USART6_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART6); }
INLINE void USART7_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART7); }
INLINE void USART8_HalfDuplexDisable(void) { USARTx_HalfDuplexDisable(USART8); }

// Enable/Disable smartcard NACK (default disabled)
INLINE void USARTx_NACKEnable(USART_t* usart) { usart->CTLR3 |= B4; }
INLINE void USART1_NACKEnable(void) { USARTx_NACKEnable(USART1); }
INLINE void USART2_NACKEnable(void) { USARTx_NACKEnable(USART2); }
INLINE void USART3_NACKEnable(void) { USARTx_NACKEnable(USART3); }
INLINE void USART4_NACKEnable(void) { USARTx_NACKEnable(USART4); }
INLINE void USART5_NACKEnable(void) { USARTx_NACKEnable(USART5); }
INLINE void USART6_NACKEnable(void) { USARTx_NACKEnable(USART6); }
INLINE void USART7_NACKEnable(void) { USARTx_NACKEnable(USART7); }
INLINE void USART8_NACKEnable(void) { USARTx_NACKEnable(USART8); }

INLINE void USARTx_NACKDisable(USART_t* usart) { usart->CTLR3 &= ~B4; }
INLINE void USART1_NACKDisable(void) { USARTx_NACKDisable(USART1); }
INLINE void USART2_NACKDisable(void) { USARTx_NACKDisable(USART2); }
INLINE void USART3_NACKDisable(void) { USARTx_NACKDisable(USART3); }
INLINE void USART4_NACKDisable(void) { USARTx_NACKDisable(USART4); }
INLINE void USART5_NACKDisable(void) { USARTx_NACKDisable(USART5); }
INLINE void USART6_NACKDisable(void) { USARTx_NACKDisable(USART6); }
INLINE void USART7_NACKDisable(void) { USARTx_NACKDisable(USART7); }
INLINE void USART8_NACKDisable(void) { USARTx_NACKDisable(USART8); }

// Enable/Disable smartcard mode (default disabled)
INLINE void USARTx_SmartCardEnable(USART_t* usart) { usart->CTLR3 |= B5; }
INLINE void USART1_SmartCardEnable(void) { USARTx_SmartCardEnable(USART1); }
INLINE void USART2_SmartCardEnable(void) { USARTx_SmartCardEnable(USART2); }
INLINE void USART3_SmartCardEnable(void) { USARTx_SmartCardEnable(USART3); }
INLINE void USART4_SmartCardEnable(void) { USARTx_SmartCardEnable(USART4); }
INLINE void USART5_SmartCardEnable(void) { USARTx_SmartCardEnable(USART5); }
INLINE void USART6_SmartCardEnable(void) { USARTx_SmartCardEnable(USART6); }
INLINE void USART7_SmartCardEnable(void) { USARTx_SmartCardEnable(USART7); }
INLINE void USART8_SmartCardEnable(void) { USARTx_SmartCardEnable(USART8); }

INLINE void USARTx_SmartCardDisable(USART_t* usart) { usart->CTLR3 &= ~B5; }
INLINE void USART1_SmartCardDisable(void) { USARTx_SmartCardDisable(USART1); }
INLINE void USART2_SmartCardDisable(void) { USARTx_SmartCardDisable(USART2); }
INLINE void USART3_SmartCardDisable(void) { USARTx_SmartCardDisable(USART3); }
INLINE void USART4_SmartCardDisable(void) { USARTx_SmartCardDisable(USART4); }
INLINE void USART5_SmartCardDisable(void) { USARTx_SmartCardDisable(USART5); }
INLINE void USART6_SmartCardDisable(void) { USARTx_SmartCardDisable(USART6); }
INLINE void USART7_SmartCardDisable(void) { USARTx_SmartCardDisable(USART7); }
INLINE void USART8_SmartCardDisable(void) { USARTx_SmartCardDisable(USART8); }

// Enable/Disable receive DMA (default disabled)
INLINE void USARTx_RxDMAEnable(USART_t* usart) { usart->CTLR3 |= B6; }
INLINE void USART1_RxDMAEnable(void) { USARTx_RxDMAEnable(USART1); }
INLINE void USART2_RxDMAEnable(void) { USARTx_RxDMAEnable(USART2); }
INLINE void USART3_RxDMAEnable(void) { USARTx_RxDMAEnable(USART3); }
INLINE void USART4_RxDMAEnable(void) { USARTx_RxDMAEnable(USART4); }
INLINE void USART5_RxDMAEnable(void) { USARTx_RxDMAEnable(USART5); }
INLINE void USART6_RxDMAEnable(void) { USARTx_RxDMAEnable(USART6); }
INLINE void USART7_RxDMAEnable(void) { USARTx_RxDMAEnable(USART7); }
INLINE void USART8_RxDMAEnable(void) { USARTx_RxDMAEnable(USART8); }

INLINE void USARTx_RxDMADisable(USART_t* usart) { usart->CTLR3 &= ~B6; }
INLINE void USART1_RxDMADisable(void) { USARTx_RxDMADisable(USART1); }
INLINE void USART2_RxDMADisable(void) { USARTx_RxDMADisable(USART2); }
INLINE void USART3_RxDMADisable(void) { USARTx_RxDMADisable(USART3); }
INLINE void USART4_RxDMADisable(void) { USARTx_RxDMADisable(USART4); }
INLINE void USART5_RxDMADisable(void) { USARTx_RxDMADisable(USART5); }
INLINE void USART6_RxDMADisable(void) { USARTx_RxDMADisable(USART6); }
INLINE void USART7_RxDMADisable(void) { USARTx_RxDMADisable(USART7); }
INLINE void USART8_RxDMADisable(void) { USARTx_RxDMADisable(USART8); }

// Enable/Disable tramsmit DMA (default disabled)
INLINE void USARTx_TxDMAEnable(USART_t* usart) { usart->CTLR3 |= B7; }
INLINE void USART1_TxDMAEnable(void) { USARTx_TxDMAEnable(USART1); }
INLINE void USART2_TxDMAEnable(void) { USARTx_TxDMAEnable(USART2); }
INLINE void USART3_TxDMAEnable(void) { USARTx_TxDMAEnable(USART3); }
INLINE void USART4_TxDMAEnable(void) { USARTx_TxDMAEnable(USART4); }
INLINE void USART5_TxDMAEnable(void) { USARTx_TxDMAEnable(USART5); }
INLINE void USART6_TxDMAEnable(void) { USARTx_TxDMAEnable(USART6); }
INLINE void USART7_TxDMAEnable(void) { USARTx_TxDMAEnable(USART7); }
INLINE void USART8_TxDMAEnable(void) { USARTx_TxDMAEnable(USART8); }

INLINE void USARTx_TxDMADisable(USART_t* usart) { usart->CTLR3 &= ~B7; }
INLINE void USART1_TxDMADisable(void) { USARTx_TxDMADisable(USART1); }
INLINE void USART2_TxDMADisable(void) { USARTx_TxDMADisable(USART2); }
INLINE void USART3_TxDMADisable(void) { USARTx_TxDMADisable(USART3); }
INLINE void USART4_TxDMADisable(void) { USARTx_TxDMADisable(USART4); }
INLINE void USART5_TxDMADisable(void) { USARTx_TxDMADisable(USART5); }
INLINE void USART6_TxDMADisable(void) { USARTx_TxDMADisable(USART6); }
INLINE void USART7_TxDMADisable(void) { USARTx_TxDMADisable(USART7); }
INLINE void USART8_TxDMADisable(void) { USARTx_TxDMADisable(USART8); }

// Enable/Disable RTS flow control (default disabled)
INLINE void USARTx_RTSEnable(USART_t* usart) { usart->CTLR3 |= B8; }
INLINE void USART1_RTSEnable(void) { USARTx_RTSEnable(USART1); }
INLINE void USART2_RTSEnable(void) { USARTx_RTSEnable(USART2); }
INLINE void USART3_RTSEnable(void) { USARTx_RTSEnable(USART3); }
INLINE void USART4_RTSEnable(void) { USARTx_RTSEnable(USART4); }
INLINE void USART5_RTSEnable(void) { USARTx_RTSEnable(USART5); }
INLINE void USART6_RTSEnable(void) { USARTx_RTSEnable(USART6); }
INLINE void USART7_RTSEnable(void) { USARTx_RTSEnable(USART7); }
INLINE void USART8_RTSEnable(void) { USARTx_RTSEnable(USART8); }

INLINE void USARTx_RTSDisable(USART_t* usart) { usart->CTLR3 &= ~B8; }
INLINE void USART1_RTSDisable(void) { USARTx_RTSDisable(USART1); }
INLINE void USART2_RTSDisable(void) { USARTx_RTSDisable(USART2); }
INLINE void USART3_RTSDisable(void) { USARTx_RTSDisable(USART3); }
INLINE void USART4_RTSDisable(void) { USARTx_RTSDisable(USART4); }
INLINE void USART5_RTSDisable(void) { USARTx_RTSDisable(USART5); }
INLINE void USART6_RTSDisable(void) { USARTx_RTSDisable(USART6); }
INLINE void USART7_RTSDisable(void) { USARTx_RTSDisable(USART7); }
INLINE void USART8_RTSDisable(void) { USARTx_RTSDisable(USART8); }

// Enable/Disable CTS flow control (default disabled)
INLINE void USARTx_CTSEnable(USART_t* usart) { usart->CTLR3 |= B9; }
INLINE void USART1_CTSEnable(void) { USARTx_CTSEnable(USART1); }
INLINE void USART2_CTSEnable(void) { USARTx_CTSEnable(USART2); }
INLINE void USART3_CTSEnable(void) { USARTx_CTSEnable(USART3); }
INLINE void USART4_CTSEnable(void) { USARTx_CTSEnable(USART4); }
INLINE void USART5_CTSEnable(void) { USARTx_CTSEnable(USART5); }
INLINE void USART6_CTSEnable(void) { USARTx_CTSEnable(USART6); }
INLINE void USART7_CTSEnable(void) { USARTx_CTSEnable(USART7); }
INLINE void USART8_CTSEnable(void) { USARTx_CTSEnable(USART8); }

INLINE void USARTx_CTSDisable(USART_t* usart) { usart->CTLR3 &= ~B9; }
INLINE void USART1_CTSDisable(void) { USARTx_CTSDisable(USART1); }
INLINE void USART2_CTSDisable(void) { USARTx_CTSDisable(USART2); }
INLINE void USART3_CTSDisable(void) { USARTx_CTSDisable(USART3); }
INLINE void USART4_CTSDisable(void) { USARTx_CTSDisable(USART4); }
INLINE void USART5_CTSDisable(void) { USARTx_CTSDisable(USART5); }
INLINE void USART6_CTSDisable(void) { USARTx_CTSDisable(USART6); }
INLINE void USART7_CTSDisable(void) { USARTx_CTSDisable(USART7); }
INLINE void USART8_CTSDisable(void) { USARTx_CTSDisable(USART8); }

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
INLINE void USART5_Presc(int presc) { USARTx_Presc(USART5, presc); }
INLINE void USART6_Presc(int presc) { USARTx_Presc(USART6, presc); }
INLINE void USART7_Presc(int presc) { USARTx_Presc(USART7, presc); }
INLINE void USART8_Presc(int presc) { USARTx_Presc(USART8, presc); }

// Get prescaler value
INLINE int USARTx_GetPresc(USART_t* usart) { return usart->GPR & 0xff; }
INLINE int USART1_GetPresc(void) { return USARTx_GetPresc(USART1); }
INLINE int USART2_GetPresc(void) { return USARTx_GetPresc(USART2); }
INLINE int USART3_GetPresc(void) { return USARTx_GetPresc(USART3); }
INLINE int USART4_GetPresc(void) { return USARTx_GetPresc(USART4); }
INLINE int USART5_GetPresc(void) { return USARTx_GetPresc(USART5); }
INLINE int USART6_GetPresc(void) { return USARTx_GetPresc(USART6); }
INLINE int USART7_GetPresc(void) { return USARTx_GetPresc(USART7); }
INLINE int USART8_GetPresc(void) { return USARTx_GetPresc(USART8); }

// Set guard time value in terms of number of baud clocks, used in smartcard mode (default 0)
INLINE void USARTx_GuardTime(USART_t* usart, int guardtime) { usart->GPR = (usart->GPR & ~(0xff << 8)) | (guardtime << 8); }
INLINE void USART1_GuardTime(int guardtime) { USARTx_GuardTime(USART1, guardtime); }
INLINE void USART2_GuardTime(int guardtime) { USARTx_GuardTime(USART2, guardtime); }
INLINE void USART3_GuardTime(int guardtime) { USARTx_GuardTime(USART3, guardtime); }
INLINE void USART4_GuardTime(int guardtime) { USARTx_GuardTime(USART4, guardtime); }
INLINE void USART5_GuardTime(int guardtime) { USARTx_GuardTime(USART5, guardtime); }
INLINE void USART6_GuardTime(int guardtime) { USARTx_GuardTime(USART6, guardtime); }
INLINE void USART7_GuardTime(int guardtime) { USARTx_GuardTime(USART7, guardtime); }
INLINE void USART8_GuardTime(int guardtime) { USARTx_GuardTime(USART8, guardtime); }

// Get guard time value in terms of number of baud clocks
INLINE int USARTx_GetGuardTime(USART_t* usart) { return (usart->GPR >> 8) & 0xff; }
INLINE int USART1_GetGuardTime(void) { return USARTx_GetGuardTime(USART1); }
INLINE int USART2_GetGuardTime(void) { return USARTx_GetGuardTime(USART2); }
INLINE int USART3_GetGuardTime(void) { return USARTx_GetGuardTime(USART3); }
INLINE int USART4_GetGuardTime(void) { return USARTx_GetGuardTime(USART4); }
INLINE int USART5_GetGuardTime(void) { return USARTx_GetGuardTime(USART5); }
INLINE int USART6_GetGuardTime(void) { return USARTx_GetGuardTime(USART6); }
INLINE int USART7_GetGuardTime(void) { return USARTx_GetGuardTime(USART7); }
INLINE int USART8_GetGuardTime(void) { return USARTx_GetGuardTime(USART8); }

// Set MARK and SPACE checksums USART_MS_* (default disabled)
INLINE void USARTx_MarkSpace(USART_t* usart, int ms) { usart->CTLR4 = (usart->CTLR4 & ~(B2|B3)) | (ms << 2); }
INLINE void USART1_MarkSpace(int ms) { USARTx_MarkSpace(USART1, ms); }
INLINE void USART2_MarkSpace(int ms) { USARTx_MarkSpace(USART2, ms); }
INLINE void USART3_MarkSpace(int ms) { USARTx_MarkSpace(USART3, ms); }
INLINE void USART4_MarkSpace(int ms) { USARTx_MarkSpace(USART4, ms); }
INLINE void USART5_MarkSpace(int ms) { USARTx_MarkSpace(USART5, ms); }
INLINE void USART6_MarkSpace(int ms) { USARTx_MarkSpace(USART6, ms); }
INLINE void USART7_MarkSpace(int ms) { USARTx_MarkSpace(USART7, ms); }
INLINE void USART8_MarkSpace(int ms) { USARTx_MarkSpace(USART8, ms); }

// === Interrupt

// Enable/Disable IDLE interrupt (default disabled)
INLINE void USARTx_IdleIntEnable(USART_t* usart) { usart->CTLR1 |= B4; }
INLINE void USART1_IdleIntEnable(void) { USARTx_IdleIntEnable(USART1); }
INLINE void USART2_IdleIntEnable(void) { USARTx_IdleIntEnable(USART2); }
INLINE void USART3_IdleIntEnable(void) { USARTx_IdleIntEnable(USART3); }
INLINE void USART4_IdleIntEnable(void) { USARTx_IdleIntEnable(USART4); }
INLINE void USART5_IdleIntEnable(void) { USARTx_IdleIntEnable(USART5); }
INLINE void USART6_IdleIntEnable(void) { USARTx_IdleIntEnable(USART6); }
INLINE void USART7_IdleIntEnable(void) { USARTx_IdleIntEnable(USART7); }
INLINE void USART8_IdleIntEnable(void) { USARTx_IdleIntEnable(USART8); }

INLINE void USARTx_IdleIntDisable(USART_t* usart) { usart->CTLR1 &= ~B4; }
INLINE void USART1_IdleIntDisable(void) { USARTx_IdleIntDisable(USART1); }
INLINE void USART2_IdleIntDisable(void) { USARTx_IdleIntDisable(USART2); }
INLINE void USART3_IdleIntDisable(void) { USARTx_IdleIntDisable(USART3); }
INLINE void USART4_IdleIntDisable(void) { USARTx_IdleIntDisable(USART4); }
INLINE void USART5_IdleIntDisable(void) { USARTx_IdleIntDisable(USART5); }
INLINE void USART6_IdleIntDisable(void) { USARTx_IdleIntDisable(USART6); }
INLINE void USART7_IdleIntDisable(void) { USARTx_IdleIntDisable(USART7); }
INLINE void USART8_IdleIntDisable(void) { USARTx_IdleIntDisable(USART8); }

// Enable/Disable interrupt on "received data is ready" (default disabled)
INLINE void USARTx_RxReadyIntEnable(USART_t* usart) { usart->CTLR1 |= B5; }
INLINE void USART1_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART1); }
INLINE void USART2_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART2); }
INLINE void USART3_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART3); }
INLINE void USART4_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART4); }
INLINE void USART5_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART5); }
INLINE void USART6_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART6); }
INLINE void USART7_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART7); }
INLINE void USART8_RxReadyIntEnable(void) { USARTx_RxReadyIntEnable(USART8); }

INLINE void USARTx_RxReadyIntDisable(USART_t* usart) { usart->CTLR1 &= ~B5; }
INLINE void USART1_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART1); }
INLINE void USART2_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART2); }
INLINE void USART3_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART3); }
INLINE void USART4_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART4); }
INLINE void USART5_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART5); }
INLINE void USART6_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART6); }
INLINE void USART7_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART7); }
INLINE void USART8_RxReadyIntDisable(void) { USARTx_RxReadyIntDisable(USART8); }

// Enable/Disable interrupt on "sending is completed" (default disabled)
INLINE void USARTx_TxSentIntEnable(USART_t* usart) { usart->CTLR1 |= B6; }
INLINE void USART1_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART1); }
INLINE void USART2_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART2); }
INLINE void USART3_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART3); }
INLINE void USART4_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART4); }
INLINE void USART5_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART5); }
INLINE void USART6_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART6); }
INLINE void USART7_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART7); }
INLINE void USART8_TxSentIntEnable(void) { USARTx_TxSentIntEnable(USART8); }

INLINE void USARTx_TxSentIntDisable(USART_t* usart) { usart->CTLR1 &= ~B6; }
INLINE void USART1_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART1); }
INLINE void USART2_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART2); }
INLINE void USART3_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART3); }
INLINE void USART4_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART4); }
INLINE void USART5_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART5); }
INLINE void USART6_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART6); }
INLINE void USART7_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART7); }
INLINE void USART8_TxSentIntDisable(void) { USARTx_TxSentIntDisable(USART8); }

// Enable/Disable interrupt on "sending buffer is empty" (default disabled)
INLINE void USARTx_TxEmptyIntEnable(USART_t* usart) { usart->CTLR1 |= B7; }
INLINE void USART1_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART1); }
INLINE void USART2_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART2); }
INLINE void USART3_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART3); }
INLINE void USART4_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART4); }
INLINE void USART5_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART5); }
INLINE void USART6_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART6); }
INLINE void USART7_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART7); }
INLINE void USART8_TxEmptyIntEnable(void) { USARTx_TxEmptyIntEnable(USART8); }

INLINE void USARTx_TxEmptyIntDisable(USART_t* usart) { usart->CTLR1 &= ~B7; }
INLINE void USART1_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART1); }
INLINE void USART2_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART2); }
INLINE void USART3_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART3); }
INLINE void USART4_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART4); }
INLINE void USART5_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART5); }
INLINE void USART6_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART6); }
INLINE void USART7_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART7); }
INLINE void USART8_TxEmptyIntDisable(void) { USARTx_TxEmptyIntDisable(USART8); }

// Enable/Disable interrupt on parity error receive (default disabled)
INLINE void USARTx_ParityErrorIntEnable(USART_t* usart) { usart->CTLR1 |= B8; }
INLINE void USART1_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART1); }
INLINE void USART2_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART2); }
INLINE void USART3_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART3); }
INLINE void USART4_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART4); }
INLINE void USART5_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART5); }
INLINE void USART6_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART6); }
INLINE void USART7_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART7); }
INLINE void USART8_ParityErrorIntEnable(void) { USARTx_ParityErrorIntEnable(USART8); }

INLINE void USARTx_ParityErrorIntDisable(USART_t* usart) { usart->CTLR1 &= ~B8; }
INLINE void USART1_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART1); }
INLINE void USART2_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART2); }
INLINE void USART3_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART3); }
INLINE void USART4_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART4); }
INLINE void USART5_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART5); }
INLINE void USART6_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART6); }
INLINE void USART7_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART7); }
INLINE void USART8_ParityErrorIntDisable(void) { USARTx_ParityErrorIntDisable(USART8); }

// Enable/Disable LIN Break detection interrupt (default disabled)
INLINE void USARTx_LinBreakIntEnable(USART_t* usart) { usart->CTLR2 |= B6; }
INLINE void USART1_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART1); }
INLINE void USART2_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART2); }
INLINE void USART3_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART3); }
INLINE void USART4_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART4); }
INLINE void USART5_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART5); }
INLINE void USART6_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART6); }
INLINE void USART7_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART7); }
INLINE void USART8_LinBreakIntEnable(void) { USARTx_LinBreakIntEnable(USART8); }

INLINE void USARTx_LinBreakIntDisable(USART_t* usart) { usart->CTLR2 &= ~B6; }
INLINE void USART1_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART1); }
INLINE void USART2_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART2); }
INLINE void USART3_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART3); }
INLINE void USART4_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART4); }
INLINE void USART5_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART5); }
INLINE void USART6_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART6); }
INLINE void USART7_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART7); }
INLINE void USART8_LinBreakIntDisable(void) { USARTx_LinBreakIntDisable(USART8); }

// Enable/Disable error interrupt (default disabled)
INLINE void USARTx_ErrIntEnable(USART_t* usart) { usart->CTLR3 |= B0; }
INLINE void USART1_ErrIntEnable(void) { USARTx_ErrIntEnable(USART1); }
INLINE void USART2_ErrIntEnable(void) { USARTx_ErrIntEnable(USART2); }
INLINE void USART3_ErrIntEnable(void) { USARTx_ErrIntEnable(USART3); }
INLINE void USART4_ErrIntEnable(void) { USARTx_ErrIntEnable(USART4); }
INLINE void USART5_ErrIntEnable(void) { USARTx_ErrIntEnable(USART5); }
INLINE void USART6_ErrIntEnable(void) { USARTx_ErrIntEnable(USART6); }
INLINE void USART7_ErrIntEnable(void) { USARTx_ErrIntEnable(USART7); }
INLINE void USART8_ErrIntEnable(void) { USARTx_ErrIntEnable(USART8); }

INLINE void USARTx_ErrIntDisable(USART_t* usart) { usart->CTLR3 &= ~B0; }
INLINE void USART1_ErrIntDisable(void) { USARTx_ErrIntDisable(USART1); }
INLINE void USART2_ErrIntDisable(void) { USARTx_ErrIntDisable(USART2); }
INLINE void USART3_ErrIntDisable(void) { USARTx_ErrIntDisable(USART3); }
INLINE void USART4_ErrIntDisable(void) { USARTx_ErrIntDisable(USART4); }
INLINE void USART5_ErrIntDisable(void) { USARTx_ErrIntDisable(USART5); }
INLINE void USART6_ErrIntDisable(void) { USARTx_ErrIntDisable(USART6); }
INLINE void USART7_ErrIntDisable(void) { USARTx_ErrIntDisable(USART7); }
INLINE void USART8_ErrIntDisable(void) { USARTx_ErrIntDisable(USART8); }

// Enable/Disable CTS interrupt (default disabled)
INLINE void USARTx_CTSIntEnable(USART_t* usart) { usart->CTLR3 |= B10; }
INLINE void USART1_CTSIntEnable(void) { USARTx_CTSIntEnable(USART1); }
INLINE void USART2_CTSIntEnable(void) { USARTx_CTSIntEnable(USART2); }
INLINE void USART3_CTSIntEnable(void) { USARTx_CTSIntEnable(USART3); }
INLINE void USART4_CTSIntEnable(void) { USARTx_CTSIntEnable(USART4); }
INLINE void USART5_CTSIntEnable(void) { USARTx_CTSIntEnable(USART5); }
INLINE void USART6_CTSIntEnable(void) { USARTx_CTSIntEnable(USART6); }
INLINE void USART7_CTSIntEnable(void) { USARTx_CTSIntEnable(USART7); }
INLINE void USART8_CTSIntEnable(void) { USARTx_CTSIntEnable(USART8); }

INLINE void USARTx_CTSIntDisable(USART_t* usart) { usart->CTLR3 &= ~B10; }
INLINE void USART1_CTSIntDisable(void) { USARTx_CTSIntDisable(USART1); }
INLINE void USART2_CTSIntDisable(void) { USARTx_CTSIntDisable(USART2); }
INLINE void USART3_CTSIntDisable(void) { USARTx_CTSIntDisable(USART3); }
INLINE void USART4_CTSIntDisable(void) { USARTx_CTSIntDisable(USART4); }
INLINE void USART5_CTSIntDisable(void) { USARTx_CTSIntDisable(USART5); }
INLINE void USART6_CTSIntDisable(void) { USARTx_CTSIntDisable(USART6); }
INLINE void USART7_CTSIntDisable(void) { USARTx_CTSIntDisable(USART7); }
INLINE void USART8_CTSIntDisable(void) { USARTx_CTSIntDisable(USART8); }

// Enable/Disable MARK or SPACE checksum error interrupt (default disabled)
INLINE void USARTx_MSErrorIntEnable(USART_t* usart) { usart->CTLR4 |= B1; }
INLINE void USART1_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART1); }
INLINE void USART2_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART2); }
INLINE void USART3_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART3); }
INLINE void USART4_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART4); }
INLINE void USART5_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART5); }
INLINE void USART6_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART6); }
INLINE void USART7_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART7); }
INLINE void USART8_MSErrorIntEnable(void) { USARTx_MSErrorIntEnable(USART8); }

INLINE void USARTx_MSErrorIntDisable(USART_t* usart) { usart->CTLR4 &= ~B1; }
INLINE void USART1_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART1); }
INLINE void USART2_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART2); }
INLINE void USART3_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART3); }
INLINE void USART4_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART4); }
INLINE void USART5_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART5); }
INLINE void USART6_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART6); }
INLINE void USART7_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART7); }
INLINE void USART8_MSErrorIntDisable(void) { USARTx_MSErrorIntDisable(USART8); }

// === Simple transfer in poll direct mode

// send data character (8 or 9 bits; send buffer must be empty)
INLINE void USARTx_TxWrite(USART_t* usart, int ch) { usart->DATAR = ch; }
INLINE void USART1_TxWrite(int ch) { USARTx_TxWrite(USART1, ch); }
INLINE void USART2_TxWrite(int ch) { USARTx_TxWrite(USART2, ch); }
INLINE void USART3_TxWrite(int ch) { USARTx_TxWrite(USART3, ch); }
INLINE void USART4_TxWrite(int ch) { USARTx_TxWrite(USART4, ch); }
INLINE void USART5_TxWrite(int ch) { USARTx_TxWrite(USART5, ch); }
INLINE void USART6_TxWrite(int ch) { USARTx_TxWrite(USART6, ch); }
INLINE void USART7_TxWrite(int ch) { USARTx_TxWrite(USART7, ch); }
INLINE void USART8_TxWrite(int ch) { USARTx_TxWrite(USART8, ch); }

// receve data character (8 or 9 bits; received character must be ready)
INLINE int USARTx_RxRead(USART_t* usart) { return usart->DATAR & 0x1ff; }
INLINE int USART1_RxRead(void) { return USARTx_RxRead(USART1); }
INLINE int USART2_RxRead(void) { return USARTx_RxRead(USART2); }
INLINE int USART3_RxRead(void) { return USARTx_RxRead(USART3); }
INLINE int USART4_RxRead(void) { return USARTx_RxRead(USART4); }
INLINE int USART5_RxRead(void) { return USARTx_RxRead(USART5); }
INLINE int USART6_RxRead(void) { return USARTx_RxRead(USART6); }
INLINE int USART7_RxRead(void) { return USARTx_RxRead(USART7); }
INLINE int USART8_RxRead(void) { return USARTx_RxRead(USART8); }

// send data character in poll mode with wait (8 or 9 bits)
void USARTx_SendChar(USART_t* usart, int ch);
INLINE void USART1_SendChar(int ch) { USARTx_SendChar(USART1, ch); }
INLINE void USART2_SendChar(int ch) { USARTx_SendChar(USART2, ch); }
INLINE void USART3_SendChar(int ch) { USARTx_SendChar(USART3, ch); }
INLINE void USART4_SendChar(int ch) { USARTx_SendChar(USART4, ch); }
INLINE void USART5_SendChar(int ch) { USARTx_SendChar(USART5, ch); }
INLINE void USART6_SendChar(int ch) { USARTx_SendChar(USART6, ch); }
INLINE void USART7_SendChar(int ch) { USARTx_SendChar(USART7, ch); }
INLINE void USART8_SendChar(int ch) { USARTx_SendChar(USART8, ch); }

// receive data character in poll mode with wait (8 or 9 bits)
int USARTx_RecvChar(USART_t* usart);
INLINE int USART1_RecvChar(void) { return USARTx_RecvChar(USART1); }
INLINE int USART2_RecvChar(void) { return USARTx_RecvChar(USART2); }
INLINE int USART3_RecvChar(void) { return USARTx_RecvChar(USART3); }
INLINE int USART4_RecvChar(void) { return USARTx_RecvChar(USART4); }
INLINE int USART5_RecvChar(void) { return USARTx_RecvChar(USART5); }
INLINE int USART6_RecvChar(void) { return USARTx_RecvChar(USART6); }
INLINE int USART7_RecvChar(void) { return USARTx_RecvChar(USART7); }
INLINE int USART8_RecvChar(void) { return USARTx_RecvChar(USART8); }

// send 8-bit data buffer in poll mode
void USARTx_SendBuf(USART_t* usart, const u8* buf, int len);
INLINE void USART1_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART1, buf, len); }
INLINE void USART2_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART2, buf, len); }
INLINE void USART3_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART3, buf, len); }
INLINE void USART4_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART4, buf, len); }
INLINE void USART5_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART5, buf, len); }
INLINE void USART6_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART6, buf, len); }
INLINE void USART7_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART7, buf, len); }
INLINE void USART8_SendBuf(const u8* buf, int len) { USARTx_SendBuf(USART8, buf, len); }

// receive 8-bit data buffer in poll mode, with time-out in [us] 0..89sec (returns number of bytes)
#define USART_LONGTIMEOUT 20000000	// long time-out
int USARTx_RecvBuf(USART_t* usart, u8* buf, int len, u32 timeout);
INLINE int USART1_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART1, buf, len, timeout); }
INLINE int USART2_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART2, buf, len, timeout); }
INLINE int USART3_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART3, buf, len, timeout); }
INLINE int USART4_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART4, buf, len, timeout); }
INLINE int USART5_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART5, buf, len, timeout); }
INLINE int USART6_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART6, buf, len, timeout); }
INLINE int USART7_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART7, buf, len, timeout); }
INLINE int USART8_RecvBuf(u8* buf, int len, u32 timeout) { return USARTx_RecvBuf(USART8, buf, len, timeout); }

// send ASCIIZ text in poll mode
void USARTx_SendText(USART_t* usart, const char* buf);
INLINE void USART1_SendText(const char* buf) { USARTx_SendText(USART1, buf); }
INLINE void USART2_SendText(const char* buf) { USARTx_SendText(USART2, buf); }
INLINE void USART3_SendText(const char* buf) { USARTx_SendText(USART3, buf); }
INLINE void USART4_SendText(const char* buf) { USARTx_SendText(USART4, buf); }
INLINE void USART5_SendText(const char* buf) { USARTx_SendText(USART5, buf); }
INLINE void USART6_SendText(const char* buf) { USARTx_SendText(USART6, buf); }
INLINE void USART7_SendText(const char* buf) { USARTx_SendText(USART7, buf); }
INLINE void USART8_SendText(const char* buf) { USARTx_SendText(USART8, buf); }

// initialize simple transfer in polled direct mode (data length 8 bits)
//  baud ... baudrate in Bauds
//  wordlen ... word length USART_WORDLEN_*
//  parity ... parity USART_PARITY_*
//  stop ... stop bits USART_STOP_*
// Also set GPIOs as follows:
//  Remap USART1 (default 0) with GPIO_Remap_USART1(map):
//	0 ... PA9:TX, PA10:RX, PA8:CLK, PA11:CTS, PA12:RTS
//	1 ... PB6:TX, PB7:RX, PA8:CLK, PA11:CTS, PA12:RTS
//	2 ... PB15:TX, PA8:RX, PA10:CLK, PA5:CTS, PA9:RTS
//	3 ... PA6:TX, PA7:RX, PA5:CLK, PC4:CTS, PC5:RTS
//  Remap USART2 (default 0) with GPIO_Remap_USART2(map):
//	0 ... PA2:TX, PA3:RX, PA4:CLK, PA0:CTS, PA1:RTS
//	1 ... PD5:TX, PD6:RX, PD7:CLK, PD3:CTS, PD4:RTS
//  Remap USART3 (default 0) with GPIO_Remap_USART3(map):
//	0 ... PB10:TX, PB11:RX, PB12:CLK, PB13:CTS, PB14:RTS
//	1 ... PC10:TX, PC11:RX, PC12:CLK, PB13:CTS, PB14:RTS
//	2 ... PA13:TX, PA14:RX, PD10:CLK, PD11:CTS, PD12:RTS
//		The "2" remapping is not supported for lot numbers where
//		the fifth digit of the lot number is less than 2 and the
//		penultimate sixth digit of the lot number is 0.
//	3 ... PD8:TX, PD9:RX, PD10:CLK, PD11:CTS, PD12:RTS
//  Remap USART4 (default 0) with GPIO_Remap_USART4(map):
//	0 ... PC10:TX, PC11:RX
//	1 ... PB0:TX, PB1:RX
//	2,3 ... PE0:TX, PE1:RX
//    CH32V203 with 64 pins:
//	0, 2 ... PB0:TX, PB1:RX, PB2:CLK, PB3:CTS, PB4:RTS
//	1, 3 ... PA5:TX, PB5:RX, PA6:CLK, PA7:CTS, PA15:RTS
//  Remap USART5 (default 0) with GPIO_Remap_USART5(map):
//	0 ... PC12:TX, PD2:RX
//	1 ... PB4:TX, PB5:RX
//	2,3 ... PE8:TX, PE9:RX
//  Remap USART6 (default 0) with GPIO_Remap_USART6(map):
//	0 ... PC0:TX, PC1:RX
//	1 ... PB8:TX, PB9:RX
//	2,3 ... PE10:TX, PE11:RX
//  Remap USART7 (default 0) with GPIO_Remap_USART7(map):
//	0 ... PC2:TX, PC3:RX
//	1 ... PA6:TX, PA7:RX
//	2,3 ... PE12:TX, PE13:RX
//  Remap USART8 (default 0) with GPIO_Remap_USART8(map):
//	0 ... PC4:TX, PC5:RX
//	1 ... PA14:TX, PA15:RX
//	2,3 ... PE14:TX, PE15:RX
//  Set mode GPIO_Mode(gpio, mode) of TX to GPIO_MODE_AF
//  Set mode GPIO_Mode(gpio, mode) of RX to GPIO_MODE_IN
void USARTx_Init(USART_t* usart, int baud, int wordlen, int parity, int stop);
INLINE void USART1_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART1, baud, wordlen, parity, stop); }
INLINE void USART2_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART2, baud, wordlen, parity, stop); }
INLINE void USART3_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART3, baud, wordlen, parity, stop); }
INLINE void USART4_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART4, baud, wordlen, parity, stop); }
INLINE void USART5_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART5, baud, wordlen, parity, stop); }
INLINE void USART6_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART6, baud, wordlen, parity, stop); }
INLINE void USART7_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART7, baud, wordlen, parity, stop); }
INLINE void USART8_Init(int baud, int wordlen, int parity, int stop) { USARTx_Init(USART8, baud, wordlen, parity, stop); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_USART_H

#endif // USE_USART
