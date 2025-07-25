
// ****************************************************************************
//
//                                  I2C
//
// ****************************************************************************
// Master mode
// -----------
// - set clock I2C1_Freq() and I2C1_Div()
// - select address mode 7-bit I2C1_Addr7(x) or 10-bit I2C1_Addr10(x)
// - enable I2C I2C1_Enable()
// - generate start event I2Cx_StartEnable()
// - wait to I2C1_StartSent()
// 10-bit address:
//   - write 1st byte of 10-bit address I2C1_Write()
//   - wait I2C1_Add10Sent()
//   - write 2nd byte of 10-bit address I2C1_Write()
//   - wait I2C1_AddrOk()
// 7-bit address:
//   - write 7-bit address I2C1_Write(), lowest bit 0=write or 1=read
//   - wait I2C1_AddrOk()
// Master send data:
//   - continue send data
//   - generate stop event I2C1_StopEnable()
// Master receive data:
//   - re-send start condition
//
// Slave mode
// ----------
// - select address 7-bit I2C1_Addr7(x) or 10-bit I2C1_Addr10(x)
// - enable I2C I2C1_Enable()
// - wait I2C1_AddrOk()
// Slave send data:
//

#if USE_I2C		// 1=use I2C peripheral

#ifndef _SDK_I2C_H
#define _SDK_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

// I2C registers
typedef struct {
	io16	CTLR1;		// 0x00: control register 1
	io16	res0;		// 0x02:
	io16	CTLR2;		// 0x04: control register 2
	io16	res1;		// 0x06:
	io16	OADDR1;		// 0x08: address register 1
	io16	res2;		// 0x0A:
	io16	OADDR2;		// 0x0C: address register 2
	io16	res3;		// 0x0E:
	io16	DATAR;		// 0x10: data register
	io16	res4;		// 0x12:
	io16	STAR1;		// 0x14: status register 1
	io16	res5;		// 0x16:
	io16	STAR2;		// 0x18: status register 2
	io16	res6;		// 0x1A:
	io16	CKCFGR;		// 0x1C: clock register
	io16	res7;		// 0x1E:
} I2C_t;
STATIC_ASSERT(sizeof(I2C_t) == 0x20, "Incorrect I2C_t!");
#define I2C1	((I2C_t*)I2C1_BASE)	// 0x40005400

#define I2C_SPEED_SLOW	100000	// slow speed (standard, 100kHz)
#define I2C_SPEED_FAST	400000	// fast speed (400kHz)

// transfer direction
#define I2C_DIR_WRITE	0		// write direction
#define I2C_DIR_READ	1		// read direction

// Events
#define I2C_EVT_START	B0		// start bit was sent
#define I2C_EVT_ADDR	B1		// address was sent, received address matches
#define I2C_EVT_TRANS	B2		// byte transmission end (byte was sent or byte was received)
#define I2C_EVT_ADDR10	B3		// first address byte of 10-bit address was sent
#define I2C_EVT_STOP	B4		// stop event detected
#define I2C_EVT_RXREADY	B6		// received byte is ready
#define I2C_EVT_TXEMPTY	B7		// transmit data register is empty
#define I2C_EVT_BUS	B8		// bus error
#define I2C_EVT_ARB	B9		// arbitration loss
#define I2C_EVT_ANS	B10		// answer failure
#define I2C_EVT_OVER	B11		// overload or underload
#define I2C_EVT_CRC	B12		// CRC error

#define I2C_EVT_MASTER	(B0<<16)	// master mode (start bit was sent)
#define I2C_EVT_BUSY	(B1<<16)	// bus busy (SDA or SCL is in low level)
#define I2C_EVT_DIRTX	(B2<<16)	// data direction transmitted (bit is determined by address byte)
#define I2C_EVT_BROAD	(B4<<16)	// broadcast address received
#define I2C_EVT_DUAL	(B7<<16)	// received address matches 2nd address instead of 1st address

#define I2C_EVT_ALL	0x00971FDF	// mask of all events (low 16 bits = STAR1, high 16 bits = STAR2)

// === Setup

// I2C enable clock
INLINE void I2C1_ClkEnable(void) { RCC_I2C1ClkEnable(); }
INLINE void I2Cx_ClkEnable(I2C_t* i2c) { if (i2c == I2C1) I2C1_ClkEnable(); }

// I2C Reset
INLINE void I2C1_Reset(void) { RCC_I2C1Reset(); }
INLINE void I2Cx_Reset(I2C_t* i2c) { if (i2c == I2C1) I2C1_Reset(); }

// Enable/Disable I2C module (default disable; register CTLR1.PE)
INLINE void I2Cx_Enable(I2C_t* i2c) { i2c->CTLR1 |= B0; }
INLINE void I2C1_Enable(void) { I2Cx_Enable(I2C1); }

INLINE void I2Cx_Disable(I2C_t* i2c) { i2c->CTLR1 &= ~B0; }
INLINE void I2C1_Disable(void) { I2Cx_Disable(I2C1); }

// Enable/Disable PEC (packet error checking) CRC8 calculation (default disable; register CTLR1.ENPEC)
//  CRC8 = X^8 + X^2 + X + 1
INLINE void I2Cx_CrcCalcEnable(I2C_t* i2c) { i2c->CTLR1 |= B5; }
INLINE void I2C1_CrcCalcEnable(void) { I2Cx_CrcCalcEnable(I2C1); }

INLINE void I2Cx_CrcCalcDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B5; }
INLINE void I2C1_CrcCalcDisable(void) { I2Cx_CrcCalcDisable(I2C1); }

// Enable/Disable broadcast call - address 0x00 (default disable; register CTLR1.ENGC)
INLINE void I2Cx_BroadEnable(I2C_t* i2c) { i2c->CTLR1 |= B6; }
INLINE void I2C1_BroadEnable(void) { I2Cx_BroadEnable(I2C1); }

INLINE void I2Cx_BroadDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B6; }
INLINE void I2C1_BroadDisable(void) { I2Cx_BroadDisable(I2C1); }

// Enable/Disable clock extension (default enable; register CTLR1.NOSTRETCH)
INLINE void I2Cx_StretchEnable(I2C_t* i2c) { i2c->CTLR1 &= ~B7; }
INLINE void I2C1_StretchEnable(void) { I2Cx_StretchEnable(I2C1); }

INLINE void I2Cx_StretchDisable(I2C_t* i2c) { i2c->CTLR1 |= B7; }
INLINE void I2C1_StretchDisable(void) { I2Cx_StretchDisable(I2C1); }

// Enable/Disable start event generation (default disable; register CTLR1.START)
INLINE void I2Cx_StartEnable(I2C_t* i2c) { i2c->CTLR1 |= B8; }
INLINE void I2C1_StartEnable(void) { I2Cx_StartEnable(I2C1); }

INLINE void I2Cx_StartDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B8; }
INLINE void I2C1_StartDisable(void) { I2Cx_StartDisable(I2C1); }

// Enable/Disable stop event generation (default disable; register CTLR1.STOP)
INLINE void I2Cx_StopEnable(I2C_t* i2c) { i2c->CTLR1 |= B9; }
INLINE void I2C1_StopEnable(void) { I2Cx_StopEnable(I2C1); }

INLINE void I2Cx_StopDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B9; }
INLINE void I2C1_StopDisable(void) { I2Cx_StopDisable(I2C1); }

// Enable/Disable ACK reply (default disable; register CTLR1.ACK)
INLINE void I2Cx_AckEnable(I2C_t* i2c) { i2c->CTLR1 |= B10; }
INLINE void I2C1_AckEnable(void) { I2Cx_AckEnable(I2C1); }

INLINE void I2Cx_AckDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B10; }
INLINE void I2C1_AckDisable(void) { I2Cx_AckDisable(I2C1); }

// Select position of PEC crc to next received data or this received data (default this; register CTLR1.POS)
INLINE void I2Cx_PosThis(I2C_t* i2c) { i2c->CTLR1 &= ~B11; }
INLINE void I2C1_PosThis(void) { I2Cx_PosThis(I2C1); }

INLINE void I2Cx_PosNext(I2C_t* i2c) { i2c->CTLR1 |= B11; }
INLINE void I2C1_PosNext(void) { I2Cx_PosNext(I2C1); }

// Enable/Disable PEC (packet error checking) error detection (default disable; register CTLR1.PEC)
INLINE void I2Cx_CrcEnable(I2C_t* i2c) { i2c->CTLR1 |= B12; }
INLINE void I2C1_CrcEnable(void) { I2Cx_CrcEnable(I2C1); }

INLINE void I2Cx_CrcDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B12; }
INLINE void I2C1_CrcDisable(void) { I2Cx_CrcDisable(I2C1); }

// Enable/Disable software reset (default disable; register CTLR1.SWRST)
INLINE void I2Cx_SwRstEnable(I2C_t* i2c) { i2c->CTLR1 |= B15; }
INLINE void I2C1_SwRstEnable(void) { I2Cx_SwRstEnable(I2C1); }

INLINE void I2Cx_SwRstDisable(I2C_t* i2c) { i2c->CTLR1 &= ~B15; }
INLINE void I2C1_SwRstDisable(void) { I2Cx_SwRstDisable(I2C1); }

// Setup I2C module clock frequency domain - should be equal to current clock in MHz, set 8-48 value [MHz], can use HCLK_PER_US (default 0; register CTLR2.FREQ)
// - Used to to check time-outs, not to generate CLK.
INLINE void I2Cx_Freq(I2C_t* i2c, int mhz) { i2c->CTLR2 = (i2c->CTLR2 & ~0x1f) | mhz; }
INLINE void I2C1_Freq(int mhz) { I2Cx_Freq(I2C1, mhz); }

// Set clock frequency division 0..4095 (default 0; register CKCFGR.CCR[11:0])
INLINE void I2Cx_Div(I2C_t* i2c, int div) { i2c->CKCFGR = (i2c->CKCFGR & ~0x3ff) | div; }
INLINE void I2C1_Div(int div) { I2Cx_Div(I2C1, div); }

// Select clock duty cycle in fast mode to Tlow/Thigh = 2:1 or 16:9 (default 2:1; register CKCFGR.DUTY)
INLINE void I2Cx_Duty21(I2C_t* i2c) { i2c->CKCFGR &= ~B14; }
INLINE void I2C1_Duty21(void) { I2Cx_Duty21(I2C1); }

INLINE void I2Cx_Duty169(I2C_t* i2c) { i2c->CKCFGR |= B14; }
INLINE void I2C1_Duty169(void) { I2Cx_Duty169(I2C1); }

// Select master mode selection bit to fast (max. 4 MHz, duty 2:1 or 16:9) or slow (max. 2 MHz, duty 1:1) mode (default slow; register CKCFGR.F/S)
//  Standard (slow) mode: Tlow >= 4.7us, Thigh >= 4.0us (= max. clock 115 kHz, or 100 kHz at ratio 1:1)
INLINE void I2Cx_Slow(I2C_t* i2c) { i2c->CKCFGR &= ~B15; }
INLINE void I2C1_Slow(void) { I2Cx_Slow(I2C1); }

// Fast mode: Tlow >= 1.3us, Thigh >= 0.6us (= max. clock 526 kHz, or 512 kHz at ratio 2:1, or 490 kHz at ratio 16:9)
// Fast mode Plus: Tlow >= 0.5us, Thigh >= 0.26us (= max. clock 1.32 MHz, or 1.28 MHz at ratio 2:1 and 16:9, or 1 MHz at ratio 1:1) 
INLINE void I2Cx_Fast(I2C_t* i2c) { i2c->CKCFGR |= B15; }
INLINE void I2C1_Fast(void) { I2Cx_Fast(I2C1); }

// Set speed (use frequency in Hz, or use predefined I2C_SPEED_*)
void I2Cx_SetSpeed(I2C_t* i2c, int speed);
INLINE void I2C1_SetSpeed(int speed) { I2Cx_SetSpeed(I2C1, speed); }

// === Interrupt

// Enable/Disable error interrupt (default disable; register CTLR2.ITERREN)
INLINE void I2Cx_IntErrEnable(I2C_t* i2c) { i2c->CTLR2 |= B8; }
INLINE void I2C1_IntErrEnable(void) { I2Cx_IntErrEnable(I2C1); }

INLINE void I2Cx_IntErrDisable(I2C_t* i2c) { i2c->CTLR2 &= ~B8; }
INLINE void I2C1_IntErrDisable(void) { I2Cx_IntErrDisable(I2C1); }

// Enable/Disable event interrupt (default disable; register CTLR2.ITEVTEN)
INLINE void I2Cx_IntEvtEnable(I2C_t* i2c) { i2c->CTLR2 |= B9; }
INLINE void I2C1_IntEvtEnable(void) { I2Cx_IntEvtEnable(I2C1); }

INLINE void I2Cx_IntEvtDisable(I2C_t* i2c) { i2c->CTLR2 &= ~B9; }
INLINE void I2C1_IntEvtDisable(void) { I2Cx_IntEvtDisable(I2C1); }

// Enable/Disable buffer interrupt (default disable; register CTLR2.ITBUFEN)
INLINE void I2Cx_IntBufEnable(I2C_t* i2c) { i2c->CTLR2 |= B10; }
INLINE void I2C1_IntBufEnable(void) { I2Cx_IntBufEnable(I2C1); }

INLINE void I2Cx_IntBufDisable(I2C_t* i2c) { i2c->CTLR2 &= ~B10; }
INLINE void I2C1_IntBufDisable(void) { I2Cx_IntBufDisable(I2C1); }

// Enable/Disable DMA request (default disable; register CTLR2.DMAEN)
INLINE void I2Cx_DMAEnable(I2C_t* i2c) { i2c->CTLR2 |= B11; }
INLINE void I2C1_DMAEnable(void) { I2Cx_DMAEnable(I2C1); }

INLINE void I2Cx_DMADisable(I2C_t* i2c) { i2c->CTLR2 &= ~B11; }
INLINE void I2C1_DMADisable(void) { I2Cx_DMADisable(I2C1); }

// Enable/Disable DMA last transfer (default not last; register CTLR2.LAST)
INLINE void I2Cx_LastEnable(I2C_t* i2c) { i2c->CTLR2 |= B12; }
INLINE void I2C1_LastEnable(void) { I2Cx_LastEnable(I2C1); }

INLINE void I2Cx_LastDisable(I2C_t* i2c) { i2c->CTLR2 &= ~B12; }
INLINE void I2C1_LastDisable(void) { I2Cx_LastDisable(I2C1); }

// === Status

// Check if start bit sent (register STAR1.SB)
INLINE Bool I2Cx_StartSent(I2C_t* i2c) { return (i2c->STAR1 & B0) != 0; }
INLINE Bool I2C1_StartSent(void) { return I2Cx_StartSent(I2C1); }

// Check if address is sent (master) or address matches (slave) (register STAR1.ADDR)
INLINE Bool I2Cx_AddrOk(I2C_t* i2c) { return (i2c->STAR1 & B1) != 0; }
INLINE Bool I2C1_AddrOk(void) { return I2Cx_AddrOk(I2C1); }

// Check if byte transmission end (register STAR1.BTF)
INLINE Bool I2Cx_TransEnd(I2C_t* i2c) { return (i2c->STAR1 & B2) != 0; }
INLINE Bool I2C1_TransEnd(void) { return I2Cx_TransEnd(I2C1); }

// Check if 1st byte of 10-bit address has been sent (register STAR1.ADD10)
INLINE Bool I2Cx_Add10Sent(I2C_t* i2c) { return (i2c->STAR1 & B3) != 0; }
INLINE Bool I2C1_Add10Sent(void) { return I2Cx_Add10Sent(I2C1); }

// Check stop event detected (register STAR1.STOPF)
INLINE Bool I2Cx_StopEvt(I2C_t* i2c) { return (i2c->STAR1 & B4) != 0; }
INLINE Bool I2C1_StopEvt(void) { return I2Cx_StopEvt(I2C1); }

// Check if received byte is ready - data register on reading is not empty (register STAR1.RxNE)
INLINE Bool I2Cx_RxReady(I2C_t* i2c) { return (i2c->STAR1 & B6) != 0; }
INLINE Bool I2C1_RxReady(void) { return I2Cx_RxReady(I2C1); }

// Check if transmit data register is empty (register STAR1.TxE)
INLINE Bool I2Cx_TxEmpty(I2C_t* i2c) { return (i2c->STAR1 & B7) != 0; }
INLINE Bool I2C1_TxEmpty(void) { return I2Cx_TxEmpty(I2C1); }

// Check bus error (register STAR1.BERR)
INLINE Bool I2Cx_BusErr(I2C_t* i2c) { return (i2c->STAR1 & B8) != 0; }
INLINE Bool I2C1_BusErr(void) { return I2Cx_BusErr(I2C1); }

// Clear bus error flag (register STAR1.BERR)
INLINE void I2Cx_BusErrClr(I2C_t* i2c) { i2c->STAR1 &= ~B8; }
INLINE void I2C1_BusErrCr(void) { I2Cx_BusErrClr(I2C1); }

// Check arbitration loss (register STAR1.ARLO)
INLINE Bool I2Cx_ArbLos(I2C_t* i2c) { return (i2c->STAR1 & B9) != 0; }
INLINE Bool I2C1_ArbLos(void) { return I2Cx_ArbLos(I2C1); }

// Clear arbitration loss flag (register STAR1.ARLO)
INLINE void I2Cx_ArbLosClr(I2C_t* i2c) { i2c->STAR1 &= ~B9; }
INLINE void I2C1_ArbLosClr(void) { I2Cx_ArbLosClr(I2C1); }

// Check answer failure (register STAR1.AF)
INLINE Bool I2Cx_AnsErr(I2C_t* i2c) { return (i2c->STAR1 & B10) != 0; }
INLINE Bool I2C1_AnsErr(void) { return I2Cx_AnsErr(I2C1); }

// Clear answer failure flag (register STAR1.AF)
INLINE void I2Cx_AnsErrClr(I2C_t* i2c) { i2c->STAR1 &= ~B10; }
INLINE void I2C1_AnsErrClr(void) { I2Cx_AnsErrClr(I2C1); }

// Check overload/underload event (register STAR1.OVR)
INLINE Bool I2Cx_OvrErr(I2C_t* i2c) { return (i2c->STAR1 & B11) != 0; }
INLINE Bool I2C1_OvrErr(void) { return I2Cx_OvrErr(I2C1); }

// Clear overload/underload event flag (register STAR1.OVR)
INLINE void I2Cx_OvrErrClr(I2C_t* i2c) { i2c->STAR1 &= ~B11; }
INLINE void I2C1_OvrErrClr(void) { I2Cx_OvrErrClr(I2C1); }

// Check CRC error (register STAR1.PECERR)
INLINE Bool I2Cx_CrcErr(I2C_t* i2c) { return (i2c->STAR1 & B12) != 0; }
INLINE Bool I2C1_CrcErr(void) { return I2Cx_CrcErr(I2C1); }

// Clear CRC error flag (register STAR1.PECERR)
INLINE void I2Cx_CrcErrClr(I2C_t* i2c) { i2c->STAR1 &= ~B12; }
INLINE void I2C1_CrcErrClr(void) { I2Cx_CrcErrClr(I2C1); }

// Check if interface is in master mode (start bit has been sent) (register STAR2.MSL)
INLINE Bool I2Cx_IsMaster(I2C_t* i2c) { return (i2c->STAR2 & B0) != 0; }
INLINE Bool I2C1_IsMaster(void) { return I2Cx_IsMaster(I2C1); }

// Check if bus is busy (register STAR2.BUSY)
INLINE Bool I2Cx_Busy(I2C_t* i2c) { return (i2c->STAR2 & B1) != 0; }
INLINE Bool I2C1_Busy(void) { return I2Cx_Busy(I2C1); }

// Check data direction flag, if data is transmitted (register STAR2.TRA)
INLINE Bool I2Cx_IsTrans(I2C_t* i2c) { return (i2c->STAR2 & B2) != 0; }
INLINE Bool I2C1_IsTrans(void) { return I2Cx_IsTrans(I2C1); }

// Check if broadcast address received (register STAR2.GENCALL)
INLINE Bool I2Cx_IsBroad(I2C_t* i2c) { return (i2c->STAR2 & B4) != 0; }
INLINE Bool I2C1_IsBroad(void) { return I2Cx_IsBroad(I2C1); }

// Check if received address matches dual address (register STAR2.DUALF)
INLINE Bool I2Cx_IsDual(I2C_t* i2c) { return (i2c->STAR2 & B7) != 0; }
INLINE Bool I2C1_IsDual(void) { return I2Cx_IsDual(I2C1); }

// Get CRC if CRC calculation is enabled (register STAR2.PEC[7:0])
INLINE u8 I2Cx_GetCrc(I2C_t* i2c) { return (u8)(i2c->STAR2 >> 8); }
INLINE u8 I2C1_GetCrc(void) { return (u8)I2Cx_GetCrc(I2C1); }

// Get all events (use #define I2C_EVT_* to mask and check events)
INLINE u16 I2Cx_Evt(I2C_t* i2c) { return (i2c->STAR1 | ((u32)i2c->STAR2 << 16)) & I2C_EVT_ALL; }
INLINE u16 I2C1_Evt(void) { return I2Cx_Evt(I2C1); }

// Clear all status
void I2Cx_StatusClr(I2C_t* i2c);
INLINE void I2C1_StatusClr(void) { I2Cx_StatusClr(I2C1); }

// === Data transfer

// Set 7-bit slave interface address 0..127 and select 7-bit address mode (default 7-bit address 0; register OADDR1.ADDMODE)
//  Example: OLED SSD1306 uses 7-bit address 0x3C or 0x3D. Sometimes the address is specified in 8 bits,
//  with the lowest bit 0=write and 1=read. For example, for OLED SSD1306 addresses 0x78/0x79 and 0x7A/0x7B.
//  In this case, enter the address divided by 2 (or shifted >> 1).
INLINE void I2Cx_Addr7(I2C_t* i2c, int addr) { i2c->OADDR1 = addr << 1; }
INLINE void I2C1_Addr7(int addr) { I2Cx_Addr7(I2C1, addr); }

// Set 10-bit slave interface address 0..1023 and select 10-bit address mode (default 7-bit address 0; register OADDR1.ADDMODE)
INLINE void I2Cx_Addr10(I2C_t* i2c, int addr) { i2c->OADDR1 = addr | B15; }
INLINE void I2C1_Addr10(int addr) { I2Cx_Addr10(I2C1, addr); }

// Set 2nd 7-bit slave interface address 0..127 and enable dual address mode (default disabled; register OADDR2)
INLINE void I2Cx_Addr7Dual(I2C_t* i2c, int addr) { i2c->OADDR2 = (addr << 1) | B0; }
INLINE void I2C1_Addr7Dual(int addr) { I2Cx_Addr7Dual(I2C1, addr); }

// Disable dual address mode (default disabled; register OADDR2)
INLINE void I2Cx_AddrDualDisable(I2C_t* i2c) { i2c->OADDR2 = 0; }
INLINE void I2C1_AddrDualDisable(void) { I2Cx_AddrDualDisable(I2C1); }

// Write data byte (register DATAR)
INLINE void I2Cx_Write(I2C_t* i2c, u8 data) { i2c->DATAR = data; }
INLINE void I2C1_Write(u8 data) { I2Cx_Write(I2C1, data); }

// Write data byte with wait
void I2Cx_WriteWait(I2C_t* i2c, u8 data);
INLINE void I2C1_WriteWait(u8 data) { I2Cx_WriteWait(I2C1, data); }

// Read data byte (register DATAR)
INLINE u8 I2Cx_Read(I2C_t* i2c) { return (u8)i2c->DATAR; }
INLINE u8 I2C1_Read(void) { return I2Cx_Read(I2C1); }

// Read data byte with wait, timeout in [ms] (returns 'def' default value on time-out)
u8 I2Cx_ReadWait(I2C_t* i2c, int ms, u8 def);
INLINE u8 I2C1_ReadWait(int ms, u8 def) { return I2Cx_ReadWait(I2C1, ms, def); }

// I2C initialize, use 7-bit address, use frequency in Hz (or use predefined I2C_SPEED_*)
//   Before it, setup I2C mapping and setup SDA and SCL pins to GPIO_MODE_AFOD mode.
// On master mode - continue with I2Cx_SendAddr().
// On slave mode - continue with waiting on address using I2Cx_AddrOk(), then receive data with I2Cx_ReadWait().
void I2Cx_Init(I2C_t* i2c, int addr, int speed);
INLINE void I2C1_Init(int addr, int speed) { I2Cx_Init(I2C1, addr, speed); }

// I2C send 7-bit address and start master mode (dir = direction I2C_DIR_*)
// On master write mode: send data using I2Cx_WriteWait(), then stop transfer with I2Cx_StopEnable().
// On master read mode: receive data using I2Cx_ReadWait(), then stop transfer with I2Cx_StopEnable().
void I2Cx_SendAddr(I2C_t* i2c, int addr, int dir);
INLINE void I2C1_SendAddr(int addr, int dir) { I2Cx_SendAddr(I2C1, addr, dir); }

// I2C send data (master: after end of data stop transfer with I2Cx_StopEnable())
void I2Cx_SendData(I2C_t* i2c, const void* buf, int num);
INLINE void I2C1_SendData(const void* buf, int num) { I2Cx_SendData(I2C1, buf, num); }

// I2C receive data (master: after end of data stop transfer with I2Cx_StopEnable())
//  ms ... timeout in [ms]
// Returns number of received bytes.
int I2Cx_RecvData(I2C_t* i2c, void* buf, int num, int ms);
INLINE int I2C1_RecvData(void* buf, int num, int ms) { return I2Cx_RecvData(I2C1, buf, num, ms); }

// Write device 8-bit register
void I2Cx_WriteReg8(I2C_t* i2c, int addr, u8 cmd, u8 val);
INLINE void I2C1_WriteReg8(int addr, u8 cmd, u8 val) { I2Cx_WriteReg8(I2C1, addr, cmd, val); }

// Write device 16-bit register
void I2Cx_WriteReg16(I2C_t* i2c, int addr, u8 cmd, u16 val);
INLINE void I2C1_WriteReg16(int addr, u8 cmd, u8 val) { I2Cx_WriteReg16(I2C1, addr, cmd, val); }

// Read device 8-bit register (with timeout in [ms] a default value on error)
u8 I2Cx_ReadReg8(I2C_t* i2c, int addr, u8 cmd, int ms, u8 def);
INLINE u8 I2C1_ReadReg8(int addr, u8 cmd, int ms, u8 def) { return I2Cx_ReadReg8(I2C1, addr, cmd, ms, def); }

// Read device 16-bit register (with timeout in [ms] and default value on error)
u16 I2Cx_ReadReg16(I2C_t* i2c, int addr, u8 cmd, int ms, u16 def);
INLINE u16 I2C1_ReadReg16(int addr, u8 cmd, int ms, u16 def) { return I2Cx_ReadReg16(I2C1, addr, cmd, ms, def); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_I2C_H

#endif // USE_I2C
