
// ****************************************************************************
//
//                              GPIO pins indices
//
// ****************************************************************************

#ifndef _SDK_GPIO_PINS_H
#define _SDK_GPIO_PINS_H

// GPIO port index
#define PORTA_INX	0
#define PORTC_INX	2
#define PORTD_INX	3

// GPIO index
#define PA0	(PORTA_INX*8)
#define PA1	(PA0+1)
#define PA2	(PA0+2)
#define PA3	(PA0+3)
#define PA4	(PA0+4)
#define PA5	(PA0+5)
#define PA6	(PA0+6)
#define PA7	(PA0+7)

#define PC0	(PORTC_INX*8)
#define PC1	(PC0+1)
#define PC2	(PC0+2)
#define PC3	(PC0+3)
#define PC4	(PC0+4)
#define PC5	(PC0+5)
#define PC6	(PC0+6)
#define PC7	(PC0+7)

#define PD0	(PORTD_INX*8)
#define PD1	(PD0+1)
#define PD2	(PD0+2)
#define PD3	(PD0+3)
#define PD4	(PD0+4)
#define PD5	(PD0+5)
#define PD6	(PD0+6)
#define PD7	(PD0+7)

// get port index from GPIO index
#define GPIO_PORTINX(gpio) ((gpio)>>3)

// get port pin from GPIO index
#define GPIO_PIN(gpio) ((gpio)&0x07)

// compose GPIO index
#define GPIO_GPIO(port, pin) (((port)<<3)|(pin))

#endif // _SDK_GPIO_PINS_H
