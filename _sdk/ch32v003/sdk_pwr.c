
// ****************************************************************************
//
//                               Power Control
//
// ****************************************************************************

#include "../../includes.h"	// globals

#if USE_PWR		// 1=use power control

// Enter standby mode
void PWR_EnterStandby(void)
{
	// select Standby mode
	PWR_PDDSStandby();

// @TODO !!!!!!!!!
//	NVIC->SCTLR |= B2;

	// WFI instruction
//	wfi();

// @TODO !!!!!!!!!
//	NVIC->SCTLR &= ~B2;
}

#endif // USE_PWR
