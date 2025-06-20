
// ****************************************************************************
//
//                                Interrupt
//
// ****************************************************************************

#include "../_include.h"

#if USE_IRQ		// 1=use IRQ interrupt support

// Setup fast interrupt service, channel 0..3
// - Need to mark handler function with HANDLER attribute.
// - Handler address must be in form 0xN00NNNNN (bits 20..27 must be 0).
// - Highest 4 bits of address of all 4 channels must be the same.
void NVIC_FastIRQ(int ch, int irq, irq_handler_t handler)
{
	// set base (higher 4 bits) of handler address
	// - This forms base addresses 0x0xxxxxxx, 0x1xxxxxxx... 0xFxxxxxxx
	NVIC->VTFBADDRR = (u32)handler;

	// set offset (lower 20 bits) and ID of the handler
	// - This forms offsets 0xNxx00000-0xNxxFFFFF
	NVIC->VTFADDRR[ch] = ((u32)irq << 24) | ((u32)handler & 0xfffff);
}

// Disable all interrupts
void NVIC_IRQAllDisable(void)
{
	int i;
	for (i = 0; i < 8; i++) NVIC->IRER[i] = ~0;
}

// Clear all interrupt pending states
void NVIC_IRQAllClear(void)
{
	int i;
	for (i = 0; i < 8; i++) NVIC->IPRR[i] = ~0;
	isb();
}

#endif // USE_IRQ
