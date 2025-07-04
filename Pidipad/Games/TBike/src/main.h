
#ifndef _MAIN_H
#define _MAIN_H

#define PROGMEM
INLINE u8 pgm_read_byte(const u8* addr) { return *addr; }

#define BLACK 0
#define WHITE 1

#define LOW 0
#define HIGH 1
typedef u8 uint8_t;
typedef s8 int8;
typedef s8 int8_t;
typedef u16 uint16_t;
typedef s16 int16_t;

INLINE int _abs(int n) { return (n < 0) ? -n : n; }


#define FULL_PIXEL

   #define PICO_OffsetX 0 //32
   #define PICO_OffsetY 8 //48

//#define BLACK 0
//#define WHITE 1

//#define LOW 0
//#define HIGH 1
#define uint8_t u8
#define int8 s8
#define uint16_t u16
#define int16_t s16

#define TINYJOYPAD_LEFT   (KeyPressed(KEY_LEFT))
#define TINYJOYPAD_RIGHT (KeyPressed(KEY_RIGHT))
#define TINYJOYPAD_DOWN (KeyPressed(KEY_DOWN))
#define TINYJOYPAD_UP  (KeyPressed(KEY_UP))
#define BUTTON_DOWN (KeyPressed(KEY_A))
#define BUTTON_UP (!KeyPressed(KEY_A))

//#define delay(Var_0) WaitMs(Var_0)
//#define _delay_ms(Var_1) WaitMs(Var_1)
#define _delay_us(Var_2) WaitUs(Var_2)

#define My_delay_ms(Var_3) WaitMs(Var_3)
#define My_delay_us(Var_4) WaitUs(Var_4)


// Game setup
//void setup();

// Main program loop
//void loop();

// delay [ms]
INLINE void delay(int ms) { WaitMs(ms); }
INLINE void _delay_ms(int ms) { WaitMs(ms); }

// play sound tone
void Sound(uint8_t freq, uint8_t dur);

void ResetSprite(void);
void intro_sound(void);
void End_Line_Win_sound(void);
void restore_start_line(void);
void Next_Level(void);
void RESET_FOR_NEW_GAME(void);
void Pause1(void);
uint8_t GAME_PLAY(void);
void TIME_TRACK(void);
void PLANTAGE(void);
void Higher_adj(uint8_t test);
void dinamic_adj(void);
void GRAVITY_ADJ(void);
void JUMP_ADJ(void);
void CheckCollision(void);
uint8_t  Trouver_Sprite_Collisionner(void);
void analise_minutieuse(void);
void ADD_LIVE(uint8_t SPR);
void CLIMB_ADJ3(void);
void Break_Gravity(void);
void CLIMB_ADJ0(void);
void CLIMB_ADJ1(void);
void CLIMB_ADJ2(void);
uint8_t RECUPE_X_SPRITE(uint8_t Type_Sprite);
uint8_t RECUPE_Y_SPRITE(uint8_t Type_Sprite);
uint8_t CHECK_SPEED_ADJ(float REC_INT);
void TRACK_RUN_ADJ(void);
uint8_t RECUPE_MAP_BYTE(const uint8_t *Level);
uint8_t SPLIT_MAP_BYTE(uint8_t BYTE,uint8_t L0_R1);
void RefreshPosSprite(void);
void INCREMENTE_SCROLL(void);
uint8_t CREATE_NEWSPRITE(void);
uint8_t  BACKGROUND(uint8_t xPASS,uint8_t yPASS);
uint8_t TOPBACK(uint8_t xPASS,uint8_t yPASS);
uint8_t FOUL1(void);
uint8_t GRADIN23(uint8_t yPASS);
uint8_t TIRE4(void);
uint8_t ROAD567(uint8_t yPASS);
uint8_t TABLEAU8(uint8_t xPASS);
uint8_t Return_live(uint8_t xPASS);
uint8_t Return_time(uint8_t xPASS);
uint8_t Return_Progress(uint8_t xPASS);
void AdjustVarScroll(void);
uint8_t Recupe(uint8_t xPASS,uint8_t yPASS);
uint8_t BLITZ_SPRITE_MAP(uint8_t xPASS,uint8_t yPASS);
uint8_t BIGSTEP_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t MINISTEP_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t Plantage(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t HUILE_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t LINE_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t START_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t SPEED_SPRITE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t NEW_LIVE(uint8_t numSprite,uint8_t xPASS,uint8_t yPASS);
uint8_t BIKE_SPRITE(uint8_t xPASS,uint8_t yPASS);
uint8_t blitzSprite(int8_t xPos,int8_t yPos,uint8_t xPASS,uint8_t yPASS,uint8_t FRAME,const uint8_t *SPRITES);
void RecupeDecalageY(uint8_t Valeur);
uint8_t SplitSpriteDecalageY(uint8_t decalage,uint8_t Input,uint8_t UPorDOWN);
void Tiny_Flip(uint8_t MODE);

#endif // _MAIN_H
