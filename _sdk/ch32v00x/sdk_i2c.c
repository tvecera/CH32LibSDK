
// ****************************************************************************
//
//                                  I2C
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_I2C		// 1=use I2C peripheral

// Set speed (use frequency in Hz - max. 3 MHz, or use predefined I2C_SPEED_*)
void I2Cx_SetSpeed(I2C_t* i2c, int speed)
{
	// get APB1 clock
	int clk = SystemCoreClock;

	// set I2C bus frequency domain (to check time-outs)
	//   Instead of the correct value, the smallest allowed value is used,
	//   which allows higher frequencies and longer timeouts to be used.
	I2Cx_Freq(i2c, (speed >= 1000000) ? 8 : (clk/1000000));

	// setup slow mode
	if (speed <= 110000)
	{
		// slow mode
		I2Cx_Slow(i2c);

		// setup divider, duty is 1:1
		int speed2 = speed * 2;
		int div = (clk + speed)/speed2;
		if (div < 4) div = 4;
		I2Cx_Div(i2c, div);
	}

	// setup fast mode
	else
	{
		// fast mode
		I2Cx_Fast(i2c);

		// select duty 2:1
		I2Cx_Duty21(i2c);
		speed *= 3;

		// setup divider
		int div = (clk + (speed >> 1))/speed;
		if (div < 1) div = 1;
		I2Cx_Div(i2c, div);
	}
}

// I2C initialize, use 7-bit address, use frequency in Hz (or use predefined I2C_SPEED_*)
//   Before it, setup I2C mapping and setup SDA and SCL pins to GPIO_MODE_AFOD mode.
void I2Cx_Init(I2C_t* i2c, int addr, int speed)
{
	// clock enable
	I2Cx_ClkEnable(i2c);

	// reset module
	I2Cx_Reset(i2c);

	// Set speed (use frequency in Hz, or use predefined I2C_SPEED_*)
	I2Cx_SetSpeed(i2c, speed);

	// enable ACK reply
	I2Cx_AckEnable(i2c);

	// set 7-bit address
	I2Cx_Addr7(i2c, addr);

	// enable module
	I2Cx_Enable(i2c);
}

// Clear all status
void I2Cx_StatusClr(I2C_t* i2c)
{
	// clear STAR1
	i2c->STAR1 &= ~(B8|B9|B10|B11|B12);

	// clear STAR2
	i2c->STAR2;
}

// I2C send 7-bit address and start master mode (dir = direction I2C_DIR_*)
// On master write mode: send data using I2Cx_TxEmpty() and I2Cx_Write(), then stop transfer with I2Cx_StopEnable().
void I2Cx_SendAddr(I2C_t* i2c, int addr, int dir)
{
	// sent start bit, start master mode
	I2Cx_StartEnable(i2c);

	// wait start bit sent
	while (!I2Cx_StartSent(i2c)) {}

	// write address + write/read mode
	I2Cx_Write(i2c, (addr << 1) | dir);

	// wait address acknowledge
	while (!I2Cx_AddrOk(i2c)) {}
}

// Write data byte with wait
void I2Cx_WriteWait(I2C_t* i2c, u8 data)
{
	// Wait for trasmitter data register is empty
	while (!I2Cx_TxEmpty(i2c)) {}

	// Write data byte
	I2Cx_Write(i2c, data);

	// wait transfer to finish
	while (!I2Cx_TxEmpty(i2c)) {}

	// Clear all status
	I2Cx_StatusClr(i2c);
}

// Read data byte with wait, timeout in [ms] (returns 'def' default value on time-out)
u8 I2Cx_ReadWait(I2C_t* i2c, int ms, u8 def)
{
	// recalculate ms to ticks
	u32 t = ms * HCLK_PER_MS;

	// get start time
	u32 start = Time();

	// Wait for received byte is ready
	while (!I2Cx_RxReady(i2c))
	{
		if ((u32)(Time() - start) >= ms) return def;
	}

	// Clear all status
	I2Cx_StatusClr(i2c);

	// Receive data
	return I2Cx_Read(i2c);
}

// I2C send data (master: after end of data stop transfer with I2Cx_StopEnable())
void I2Cx_SendData(I2C_t* i2c, const void* buf, int num)
{
	const u8* src = (const u8*)buf;
	for (; num > 0; num--) I2Cx_WriteWait(i2c, *src++);
}

// I2C receive data (master: after end of data stop transfer with I2Cx_StopEnable())
//  ms ... timeout in [ms]
// Returns number of received bytes.
int I2Cx_RecvData(I2C_t* i2c, void* buf, int num, int ms)
{
	// preset result value
	int res = 0;

	// recalculate ms to ticks
	u32 t = ms * HCLK_PER_MS;

	// receive data
	u8* dst = (u8*)buf;
	for (; num > 0; num--)
	{
		// get start time
		u32 start = Time();

		// Wait for received byte is ready
		while (!I2Cx_RxReady(i2c))
		{
			if ((u32)(Time() - start) >= ms) return res;
		}

		// Clear all status
		I2Cx_StatusClr(i2c);

		// Receive data
		*dst++ = I2Cx_Read(i2c);
		res++;
	}

	return res;
}

// Write device 8-bit register
void I2Cx_WriteReg8(I2C_t* i2c, int addr, u8 cmd, u8 val)
{
	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write command
	I2Cx_SendData(i2c, &cmd, 1);

	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write value
	I2Cx_SendData(i2c, &val, 1);

	// stop transfer
	I2Cx_StopEnable(i2c);
}

// Write device 16-bit register
void I2Cx_WriteReg16(I2C_t* i2c, int addr, u8 cmd, u16 val)
{
	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write command
	I2Cx_SendData(i2c, &cmd, 1);

	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write value
	I2Cx_SendData(i2c, &val, 2);

	// stop transfer
	I2Cx_StopEnable(i2c);
}

// Read device 8-bit register (with timeout in [ms] a default value on error)
u8 I2Cx_ReadReg8(I2C_t* i2c, int addr, u8 cmd, int ms, u8 def)
{
	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write command
	I2Cx_SendData(i2c, &cmd, 1);

	// send address, read mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_READ);

	// read value
	u8 val = I2Cx_ReadWait(i2c, ms, def);

	// stop transfer
	I2Cx_StopEnable(i2c);

	return val;
}

// Read device 16-bit register (with timeout in [ms] and default value on error)
u16 I2Cx_ReadReg16(I2C_t* i2c, int addr, u8 cmd, int ms, u16 def)
{
	// send address, write mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_WRITE);

	// write command
	I2Cx_SendData(i2c, &cmd, 1);

	// send address, read mode
	I2Cx_SendAddr(i2c, addr, I2C_DIR_READ);

	// read value
	u16 val;
	int n = I2Cx_RecvData(i2c, &val, 2, ms);
	if (n != 2) val = def;

	// stop transfer
	I2Cx_StopEnable(i2c);

	return val;
}

#endif // USE_I2C
