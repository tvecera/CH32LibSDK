
// ****************************************************************************
//
//                                   USART
//
// ****************************************************************************

#include "../_include.h"

#if USE_USART		// 1=use USART peripheral

// Set baudrate (in Bauds)
void USARTx_Baud(USART_t* usart, int baud)
{
	// get HCLK clock in Hz
	int clk = RCC_GetFreq(CLK_HCLK);

	// calculate divider
	int div = (clk + baud/2) / baud;

	// limit range
	if (div < 16) div = 16;
	if (div > 65535) div = 65535;

	// set baudrate
	USARTx_IBaud(usart, div);
}

// Get baudrate (in Bauds)
int USARTx_GetBaud(USART_t* usart)
{
	// get HCLK clock in Hz
	int clk = RCC_GetFreq(CLK_HCLK);

	// get integer baudrate
	int div = USARTx_GetIBaud(usart);
	if (div < 16) div = 16;

	// calculate baudrate
	return (clk + div/2) / div;
}

// send data character in poll mode with wait (8 or 9 bits)
void USARTx_SendChar(USART_t* usart, int ch)
{
	while (!USARTx_TxEmpty(usart)){}
	USARTx_TxWrite(usart, ch);
}

// receive data character in poll mode with wait (8 or 9 bits)
int USARTx_RecvChar(USART_t* usart)
{
	while (!USARTx_RxReady(usart)){}
	return USARTx_RxRead(usart);
}

// send 8-bit data buffer in poll mode
void USARTx_SendBuf(USART_t* usart, const u8* buf, int len)
{
	for (; len > 0; len--) USARTx_SendChar(usart, *buf++);
}

// receive 8-bit data buffer in poll mode, with time-out in [us] 0..89sec (returns number of bytes)
int USARTx_RecvBuf(USART_t* usart, u8* buf, int len, u32 timeout)
{
	// recalculate us to ticks
	timeout *= HCLK_PER_US;

	// loop of bytes
	int num = 0;
	for (; len > 0; len--)
	{
		// get start time
		u32 start = Time();

		// wait with timeout
		while (!USARTx_RxReady(usart))
		{
			// timeout
			if ((u32)(Time() - start) >= timeout) return num;
		}

		// receive one byte
		*buf++ = (u8)USARTx_RxRead(usart);
		num++;
	}
	return num;
}

// send ASCIIZ text in poll mode
void USARTx_SendText(USART_t* usart, const char* buf)
{
	char ch;
	while ((ch = *buf) != 0)
	{
		USARTx_SendChar(usart, ch);
		buf++;
	}
}

// initialize simple transfer in polled direct mode (data length 8 bits)
//  baud ... baudrate in Bauds
//  wordlen ... word length USART_WORDLEN_*
//  parity ... parity USART_PARITY_*
//  stop ... stop bits USART_STOP_*
void USARTx_Init(USART_t* usart, int baud, int wordlen, int parity, int stop)
{
	USARTx_RCCClkEnable(usart);	// enable RCC clock
	USARTx_Reset(usart);		// reset USART to default values
	USARTx_Baud(usart, baud);	// set baudrate
	USARTx_WordLen(usart, wordlen);	// set word length
	USARTx_Parity(usart, parity);	// set parity
	USARTx_Stop(usart, stop);	// set stop bits
	USARTx_RxEnable(usart);		// Enable receiver
	USARTx_TxEnable(usart);		// Enable transmitter
	USARTx_Enable(usart);		// enable USART
}

#endif // USE_USART
