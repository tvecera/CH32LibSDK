
// ****************************************************************************
//
//                                 CPU control
//
// ****************************************************************************

#ifndef _SDK_CPU_H
#define _SDK_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

// compiler barrier
INLINE void cb(void) { __asm volatile ("" ::: "memory"); }

// instruction synchronization barrier
INLINE void isb(void) { __asm volatile (" fence.i\n" ::: "memory"); }

// data memory barrier
INLINE void dmb(void) { __asm volatile (" fence rw, rw\n" ::: "memory"); }

// data synchronization barrier
INLINE void dsb(void) { __asm volatile (" fence rw, rw\n" ::: "memory"); }

// no operation (nop instruction)
INLINE void nop(void) { __asm volatile (" nop\n" ::: "memory"); }
INLINE void nop2(void) { __asm volatile (" .word 0x13\n" ::: "memory"); }

// disable global interrupts
INLINE void di(void)
{
	__asm volatile (" csrc mstatus,%0\n" :: "r" (0x88) : "memory");
	isb();
}

// enable global interrupts
INLINE void ei(void) { __asm volatile (" csrs mstatus,%0\n" :: "r" (0x88) : "memory"); }

// get global interrupts (returns True if interrupts are enabled)
INLINE Bool geti(void)
{
	u32 val;
	__asm volatile (" csrr %0, mstatus\n" : "=r" (val) :: "memory");
	return (val & B3) != 0;
}

// set interrupts (True to enable interrupts)
INLINE void seti(Bool enable) { if (enable) ei(); else di(); }

// save and disable interrupts (returns interrupt state 1=enabled, 0=disabled)
INLINE Bool LockIRQ(void)
{
	u32 state;
	__asm volatile (" csrrci %0, mstatus, 0x8\n" : "=r" (state) :: "memory");
	isb();
	return (state & B3) != 0;
}

// restore interrupts
INLINE void UnlockIRQ(Bool state) { if (state) ei(); else di(); }

// IRQ lock (temporary disables interrupt)
#define IRQ_LOCK Bool irq_state = LockIRQ()	// lock on begin of critical section
#define IRQ_RELOCK irq_state = LockIRQ()	// lock again on begin of critical section
#define IRQ_UNLOCK UnlockIRQ(irq_state)		// unlock on end of critical section

// Get machine status register
INLINE u32 get_MSTATUS(void)
{
	u32 res;
	__asm volatile(" csrr %0, mstatus\n": "=r" (res));
	return res;
}

// Set machine status register
INLINE void set_MSTATUS(u32 val)
{
	__asm volatile(" csrw mstatus, %0\n" : : "r" (val) : "memory");
}

// Get machine ISA register
INLINE u32 get_MISA(void)
{
	u32 res;
	__asm volatile(" csrr %0, misa\n": "=r" (res));
	return res;
}

// Set machine ISA register
INLINE void set_MISA(u32 val)
{
	__asm volatile(" csrw misa, %0\n" : : "r" (val) : "memory");
}

// Get machine Trap-Vector base address register
INLINE u32 get_MTVEC(void)
{
	u32 res;
	__asm volatile(" csrr %0, mtvec\n": "=r" (res));
	return res;
}

// Set machine Trap-Vector base address register
INLINE void set_MTVEC(u32 val)
{
	__asm volatile(" csrw mtvec, %0\n" : : "r" (val) : "memory");
}

// Get machine scratch register
INLINE u32 get_MSCRATCH(void)
{
	u32 res;
	__asm volatile(" csrr %0, mscratch\n": "=r" (res));
	return res;
}

// Set machine scratch register
INLINE void set_MSCRATCH(u32 val)
{
	__asm volatile(" csrw mscratch, %0\n" : : "r" (val) : "memory");
}

// Get machine exception program register
INLINE u32 get_MEPC(void)
{
	u32 res;
	__asm volatile(" csrr %0, mepc\n": "=r" (res));
	return res;
}

// Set machine exception program register
INLINE void set_MEPC(u32 val)
{
	__asm volatile(" csrw mepc, %0\n" : : "r" (val) : "memory");
}

// Get machine cause register
INLINE u32 get_MCAUSE(void)
{
	u32 res;
	__asm volatile(" csrr %0, mcause\n": "=r" (res));
	return res;
}

// Set machine cause register
INLINE void set_MCAUSE(u32 val)
{
	__asm volatile(" csrw mcause, %0\n" : : "r" (val) : "memory");
}

// Get machine vendor ID register
INLINE u32 get_MVENDORID(void)
{
	u32 res;
	__asm volatile(" csrr %0, mvendorid\n": "=r" (res));
	return res;
}

// Get machine architecture ID register
INLINE u32 get_MARCHID(void)
{
	u32 res;
	__asm volatile(" csrr %0, marchid\n": "=r" (res));
	return res;
}

// Get machine implementation ID register
INLINE u32 get_MIMPID(void)
{
	u32 res;
	__asm volatile(" csrr %0, mimpid\n": "=r" (res));
	return res;
}

// Get machine hard ID register
INLINE u32 get_MHARTID(void)
{
	u32 res;
	__asm volatile(" csrr %0, mhartid\n": "=r" (res));
	return res;
}

// Get SP register
INLINE u32 get_SP(void)
{
	u32 res;
	__asm volatile(" mv %0, sp\n": "=r" (res));
	return res;
}

// WFI instruction (use _WFI() instead of direct instruction)
INLINE void wfi_ins(void) { __asm volatile (" wfi\n" ::: "memory"); }

// Wait for event with delay (t = delay 48MHz:19, 24MHz:11, 16MHz:8, 8MHz:5)
// - Go to sleep, wait for event, wake up and continue the previously stopped program flow.
INLINE void WFE_T(u32 t)
{
	__asm volatile (" wfi\n");
	__asm volatile ("1: addi a0,a0,-1\n bnez a0,1b\n");
}
#define _WFE_()   WFE_T(19) //48M
//#define _WFE_()   WFE_T(11) //24M
//#define _WFE_()   WFE_T(8) //16M
//#define _WFE_()   WFE_T(5) //8M

// reverse byte order of DWORD (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u32 Endian(u32 val)
{
	return __builtin_bswap32(val);
//	u32 res;
//	__asm volatile (" rev8 %0,%1\n" : "=r" (res) : "r" (val));
//	return res;
}

// swap bytes of WORD (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u16 Swap(u16 val)
{
	return __builtin_bswap16(val);
}

// swap bytes in two WORDs (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u32 Swap2(u32 val)
{
	return __builtin_bswap16((u16)val) | ((u32)__builtin_bswap16((u16)(val >> 16)) << 16);
}

// rotate bits in DWORD right by 'num' bits (carry lower bits to higher bits)
INLINE u32 Ror(u32 val, u8 num)
{
	num &= 0x1f;
	val = (val >> num) | (val << (32 - num));
	return val;
}

// rotate bits in DWORD left by 'num' bits (carry higher bits to lower bits)
INLINE u32 Rol(u32 val, u8 num)
{
	num &= 0x1f;
	val = (val << num) | (val >> (32 - num));
	return val;
}

// reverse 32 bits
INLINE u32 Reverse32(u32 val)
{
	u32 res;
	__asm volatile (" rev8 %0,%1\n brev8 %0,%0\n" : "=r" (res) : "r" (val));
	return res;
}

INLINE u32 reverse(u32 val) { return Reverse32(val); }
INLINE u32 __rev(u32 val) { return Reverse32(val); }

// reverse 16 bits
INLINE u16 Reverse16(u16 val) { return (u16)(Reverse32(val) >> 16); }

// reverse 8 bits
INLINE u8 Reverse8(u8 val)
{
	u32 val2 = val;
	u32 res;
	__asm volatile (" brev8 %0,%1\n" : "=r" (res) : "r" (val2));
	return (u8)res;
}

// reverse 64 bits
INLINE u64 Reverse64(u64 val)
{
	return ((u64)Reverse32((u32)val) << 32) | Reverse32((u32)(val >> 32));
}

INLINE u64 __revll(u64 val) { return Reverse64(val); }

// get number of leading zeros of u32 number
INLINE u32 Clz(u32 val)
{
	return __builtin_clz(val);
//	u32 res;
//	__asm volatile (" clz %0,%1\n" : "=r" (res) : "r" (val));
//	return res;
}

INLINE u32 clz(u32 val) { return Clz(val); }

// get number of leading zeros of u64 number
INLINE u32 Clz64(u64 num)
{
	if (num >= 0x100000000ULL)
		return clz((u32)(num >> 32));
	else
		return clz((u32)num) + 32;
}

INLINE u32 clz64(u64 val) { return Clz64(val); }

// get number of trailing zeros of u32 number
INLINE u32 Ctz(u32 val)
{
	u32 res;
	__asm volatile (" ctz %0,%1\n" : "=r" (res) : "r" (val));
	return res;
}

INLINE u32 ctz(u32 val) { return Ctz(val); }

// get number of trailing zeros of u64 number
INLINE u32 Ctz64(u64 num)
{
	if ((u32)num == 0)
		return ctz((u32)(num >> 32)) + 32;
	else
		return ctz((u32)num);
}

INLINE u32 ctz64(u64 val) { return Ctz64(val); }

// counts '1' bits (population count)
INLINE u32 Popcount(u32 val) { return __builtin_popcount(val); }
INLINE u32 popcount(u32 val) { return Popcount(val); }
INLINE u32 Popcount64(u64 val) { return Popcount((u32)val) + Popcount((u32)(val >> 32)); }

// get bit order of 8-bit value (logarithm, returns position of highest bit + 1: 1..8, 0=no bit)
INLINE u32 Order8(u8 val) { return 32 - Clz((u32)(u8)val); }

// get bit order of value (logarithm, returns position of highest bit + 1: 1..32, 0=no bit)
INLINE u32 Order(u32 val) { return 32 - Clz(val); }

// get bit order of 64-bit value (logarithm, returns position of highest bit + 1: 1..64, 0=no bit)
INLINE u32 Order64(u64 val)
{
	if (val >= 0x100000000ULL)
		return Order((u32)(val >> 32)) + 32;
	else
		return Order((u32)val);
}

// get mask of value (0x123 returns 0x1FF)
INLINE u32 Mask(u32 val) { return ((u32)-1) >> Clz(val); }

// range mask - returns bits set to '1' on range 'first' to 'last' (RangeMask(7,14) returns 0x7F80)
INLINE u32 RangeMask(int first, int last) { return (~(((u32)-1) << (last+1))) & (((u32)-1) << first); }

// range mask - returns bits set to '1' on range 'first' to 'last' (RangeMask(7,14) returns 0x7F80)
INLINE u64 RangeMask64(int first, int last) { return (~(((u64)-1) << (last+1))) & (((u64)-1) << first); }

// check if integer number is power of 2
INLINE Bool IsPow2(u32 a) { return ((a & (a-1)) == 0); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_CPU_H
