
// ****************************************************************************
//
//                                Interrupt
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_IRQ		// 1=use IRQ interrupt support

// Set event - execute WFI command as WFE instruction
void sev(void)
{
	// save old state of the register
	u32 tmp = NVIC->SCTLR;

	// Set flags: B3:WFITOWFE execute WFI instruction as WFE, B5:SETEVENT set event to wake up WFE
	NVIC->SCTLR = tmp | (B3|B5);

	// Restore old state except SETEVENT flag
	NVIC->SCTLR = (NVIC->SCTLR & ~B5) | (tmp & B5);
}

// wait for event
void wfe(void)
{
	// temporary to save event enable regsier
	u32 t = 0;

	// check Deep Sleep mode
	if (NVIC_IsDeepSleep())
	{
		// disable global interrupt
		di();

		// save current event enable register
		t = EXTI->EVENR;

		// enable events in the same way as interrupts are enabled
		EXTI->EVENR = EXTI->INTENR;
	}

	// not Deep Sleep mode
	else
	{
		// set SEVONPEND flag - all interrupts can wake up the system
		NVIC->SCTLR |= B4;

		// disable global interrupt
		di();
	}

	// set to execute WFI command (=continue to interrupt) as if it were a WFE (=continue to program)
	WFItoWFE();

	// execute WFI instruction (as WFE instruction)
	_WFE_();

	// check Deep Sleep mode
	if (NVIC_IsDeepSleep())
		// restore event enable register
		EXTI->EVENR = t;
	else
		// restore SEVONPEND flag
		NVIC->SCTLR &= ~B4;

	// enable global interrupt
	ei();
}

// wait for event doubled, with set event
void wfe2(void)
{
	// temporary to save event enable regsier
	u32 t = 0;

	// check Deep Sleep mode
	if (NVIC_IsDeepSleep())
	{
		// disable global interrupt
		di();

		// save current event enable register
		t = EXTI->EVENR;

		// enable events in the same way as interrupts are enabled
		EXTI->EVENR = EXTI->INTENR;
	}

	// not Deep Sleep mode
	else
	{
		// set SEVONPEND flag - all interrupts can wake up the system
		NVIC->SCTLR |= B4;

		// disable global interrupt
		di();
	}

	// set event
	sev();

	// set to execute WFI command (=continue to interrupt) as if it were a WFE (=continue to program)
	WFItoWFE();

	// execute WFI instruction (as WFE instruction)
	_WFE_();

	// set to execute WFI command (=continue to interrupt) as if it were a WFE (=continue to program)
	WFItoWFE();

	// execute WFI instruction (as WFE instruction)
	_WFE_();

	// set to execute WFI command (=continue to interrupt) as if it were a WFE (=continue to program)
	WFItoWFE();

	// execute WFI instruction (as WFE instruction)
	_WFE_();

	// check Deep Sleep mode
	if (NVIC_IsDeepSleep())
		// restore event enable register
		EXTI->EVENR = t;
	else
		// restore SEVONPEND flag
		NVIC->SCTLR &= ~B4;

	// enable global interrupt
	ei();
}

// Setup and enable VTF (Vector Table Free), channel 0..1 - set IRQ and handler address (must be two-byte alignment)
// - Need to mark handler function with HANDLER attribute.
void NVIC_VTFEnable(int ch, int irq, irq_handler_t handler)
{
	// disable current handler
	NVIC->VTFADDR[ch] = 0;
	cb();

	// set IRQ
	NVIC->VTFIDR[ch] = irq;
	cb();

	// set handler address, enable channel
	NVIC->VTFADDR[ch] = (u32)handler | 1;
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
