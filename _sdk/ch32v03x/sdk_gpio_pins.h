
// ****************************************************************************
//
//                              GPIO pins indices
//
// ****************************************************************************

#ifndef _SDK_GPIO_PINS_H
#define _SDK_GPIO_PINS_H

// GPIO port index
#define PORTA_INX	0
#define PORTB_INX	1
#define PORTC_INX	2

// GPIO index
#define PA0	(PORTA_INX*32)
#define PA1	(PA0+1)
#define PA2	(PA0+2)
#define PA3	(PA0+3)
#define PA4	(PA0+4)
#define PA5	(PA0+5)
#define PA6	(PA0+6)
#define PA7	(PA0+7)
#define PA8	(PA0+8)
#define PA9	(PA0+9)
#define PA10	(PA0+10)
#define PA11	(PA0+11)
#define PA12	(PA0+12)
#define PA13	(PA0+13)
#define PA14	(PA0+14)
#define PA15	(PA0+15)
#define PA16	(PA0+16)
#define PA17	(PA0+17)
#define PA18	(PA0+18)
#define PA19	(PA0+19)
#define PA20	(PA0+20)
#define PA21	(PA0+21)
#define PA22	(PA0+22)
#define PA23	(PA0+23)

#define PB0	(PORTB_INX*32)
#define PB1	(PB0+1)
#define PB2	(PB0+2)
#define PB3	(PB0+3)
#define PB4	(PB0+4)
#define PB5	(PB0+5)
#define PB6	(PB0+6)
#define PB7	(PB0+7)
#define PB8	(PB0+8)
#define PB9	(PB0+9)
#define PB10	(PB0+10)
#define PB11	(PB0+11)
#define PB12	(PB0+12)
#define PB13	(PB0+13)
#define PB14	(PB0+14)
#define PB15	(PB0+15)
#define PB16	(PB0+16)
#define PB17	(PB0+17)
#define PB18	(PB0+18)
#define PB19	(PB0+19)
#define PB20	(PB0+20)
#define PB21	(PB0+21)
#define PB22	(PB0+22)
#define PB23	(PB0+23)

#define PC0	(PORTC_INX*32)
#define PC1	(PC0+1)
#define PC2	(PC0+2)
#define PC3	(PC0+3)
#define PC4	(PC0+4)
#define PC5	(PC0+5)
#define PC6	(PC0+6)
#define PC7	(PC0+7)
#define PC8	(PC0+8)
#define PC9	(PC0+9)
#define PC10	(PC0+10)
#define PC11	(PC0+11)
#define PC12	(PC0+12)
#define PC13	(PC0+13)
#define PC14	(PC0+14)
#define PC15	(PC0+15)
#define PC16	(PC0+16)
#define PC17	(PC0+17)
#define PC18	(PC0+18)
#define PC19	(PC0+19)
#define PC20	(PC0+20)
#define PC21	(PC0+21)
#define PC22	(PC0+22)
#define PC23	(PC0+23)

// get port index from GPIO index
#define GPIO_PORTINX(gpio) ((gpio)>>5)

// get port pin from GPIO index
#define GPIO_PIN(gpio) ((gpio)&0x1f)

// compose GPIO index
#define GPIO_GPIO(port, pin) (((port)<<5)|(pin))

#endif // _SDK_GPIO_PINS_H
