
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROGMEM
//INLINE u8 pgm_read_byte(const u8* addr) { return *addr; }
INLINE u8 pgm_read_byte(const void* addr) { return *(u8*)addr; }

#define BLACK 0
#define WHITE 1

#define LOW 0
#define HIGH 1
typedef u8 uint8_t;
typedef s8 int8;
typedef s8 int8_t;
typedef u16 uint16_t;
typedef s16 int16_t;

INLINE u16 random(void ) { return RandU16(); }

/*
INLINE int _abs(int n) { return (n < 0) ? -n : n; }

#define TINYJOYPAD_LEFT   (KeyPressed(KEY_LEFT))
#define TINYJOYPAD_RIGHT (KeyPressed(KEY_RIGHT))
#define TINYJOYPAD_DOWN (KeyPressed(KEY_DOWN))
#define TINYJOYPAD_UP  (KeyPressed(KEY_UP))
#define BUTTON_DOWN ((KeyPressed(KEY_A)))//||(KeyPressed(KEY_B)))
#define BUTTON_UP ((!KeyPressed(KEY_A)))//&&(!KeyPressed(KEY_B)))
*/
// Game setup
void setup();

// Main program loop
void loop();

// delay [ms]
INLINE void delay(int ms) { WaitMs(ms); }
INLINE void _delay_ms(int ms) { WaitMs(ms); }

// play sound tone
void Sound(uint8_t freq, uint8_t dur);

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
