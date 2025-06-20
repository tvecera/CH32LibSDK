
#include "../include.h"

#define PIN	PA0

int main(void)
{
	GPIO_Mode(PIN, GPIO_MODE_OUT);

	while(True)
	{
		GPIO_Flip(PIN);
		WaitMs(1000);
	}
}
