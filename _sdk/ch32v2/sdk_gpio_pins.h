
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
#define PORTD_INX	3
#define PORTE_INX	4

// GPIO index
#define PA0	(PORTA_INX*16)
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

#define PB0	(PORTB_INX*16)
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

#define PC0	(PORTC_INX*16)
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

#define PD0	(PORTD_INX*16)
#define PD1	(PD0+1)
#define PD2	(PD0+2)
#define PD3	(PD0+3)
#define PD4	(PD0+4)
#define PD5	(PD0+5)
#define PD6	(PD0+6)
#define PD7	(PD0+7)
#define PD8	(PD0+8)
#define PD9	(PD0+9)
#define PD10	(PD0+10)
#define PD11	(PD0+11)
#define PD12	(PD0+12)
#define PD13	(PD0+13)
#define PD14	(PD0+14)
#define PD15	(PD0+15)

#define PE0	(PORTE_INX*16)
#define PE1	(PE0+1)
#define PE2	(PE0+2)
#define PE3	(PE0+3)
#define PE4	(PE0+4)
#define PE5	(PE0+5)
#define PE6	(PE0+6)
#define PE7	(PE0+7)
#define PE8	(PE0+8)
#define PE9	(PE0+9)
#define PE10	(PE0+10)
#define PE11	(PE0+11)
#define PE12	(PE0+12)
#define PE13	(PE0+13)
#define PE14	(PE0+14)
#define PE15	(PE0+15)

// get port index from GPIO index
#define GPIO_PORTINX(gpio) ((gpio)>>4)

// get port pin from GPIO index
#define GPIO_PIN(gpio) ((gpio)&0x0f)

// compose GPIO index
#define GPIO_GPIO(port, pin) (((port)<<4)|(pin))

#endif // _SDK_GPIO_PINS_H
