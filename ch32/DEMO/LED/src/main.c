
#include "../include.h"

#define PIN	PD5

int main(void)
{
	GPIO_Mode(PIN, GPIO_MODE_OUT);

	while(True)
	{
		GPIO_Flip(PIN);
		WaitMs(500);
	}
}
