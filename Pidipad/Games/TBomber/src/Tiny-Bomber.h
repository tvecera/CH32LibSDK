
#ifndef _GAME_H
#define _GAME_H

typedef struct PERSONAGE
{
	uint8_t anim;//=0;
	uint8_t dead;//=0;
	uint8_t type;//=0;
	uint8_t DirectionAnim;//=0;
	uint8_t DirectionH;//=0;
	uint8_t DirectionV;//=0;
	uint8_t x;//=0;
	uint8_t Decalagey;//=0;
	int8_t y;//=0;
	uint8_t switchanim;//=0;
} PERSONAGE;
                                                                                                                                         
void LoadLevel(uint8_t LeveL);
void ResetVar(struct PERSONAGE *Sprite);
void resetMonster(struct PERSONAGE *Sprite);
void resetBomb(void);
void StartGame(void);
void loop();
uint8_t CheckLevelEnded(struct PERSONAGE *Sprite);
void DeadSong(void);
void DestroyBloc(void);
void DestroyEnemy(struct PERSONAGE *Sprite);
uint8_t COLLISION11(uint8_t x1,uint8_t y1,uint8_t w1,uint8_t h1,uint8_t x2,uint8_t y2,uint8_t w2,uint8_t h2);
uint8_t CollisionTiny2Caracter(struct PERSONAGE *Sprite);
void RefreshCaracter(struct PERSONAGE *Sprite);
uint8_t CheckCollisionWithBack(uint8_t SpriteCheck,uint8_t HorVcheck,struct PERSONAGE *Sprite);
uint8_t CheckCollisionWithBomb(uint8_t SpriteCheck,struct PERSONAGE *Sprite);
uint8_t CheckCollisionWithBlock(uint8_t SpriteCheck,struct PERSONAGE *Sprite);
uint8_t RecupeBacktoCompV(uint8_t SpriteCheck,struct PERSONAGE *Sprite);
uint8_t Trim(uint8_t Y1orY2,uint8_t TrimValue,uint8_t Decalage);
uint8_t ScanHRecupe(uint8_t UporDown,uint8_t Decalage);
uint8_t RecupeBacktoCompH(uint8_t SpriteCheck,struct PERSONAGE *Sprite);
void Tiny_Flip(uint8_t render0_picture1,struct PERSONAGE *Sprite);
uint8_t PrintLive(uint8_t x,uint8_t y);
uint8_t Explose(uint8_t x,uint8_t y);
uint8_t BombBlitz(uint8_t x,uint8_t y);
uint8_t BlockBomb(uint8_t x,uint8_t y,uint8_t Divx);
void BOOLWRITE0(uint8_t BoolNumber);
uint8_t BOOLREAD(uint8_t Numero);
uint8_t SplitSpriteDecalageY(uint8_t decalage,uint8_t Input,uint8_t UPorDOWN);
uint8_t SpriteMain(uint8_t x,uint8_t y,struct PERSONAGE  *Sprite);
uint8_t SpriteWrite(uint8_t x,uint8_t y,struct PERSONAGE *Sprite);
uint8_t return_if_sprite_present(uint8_t x,struct PERSONAGE  *Sprite,uint8_t SpriteNumber);
uint8_t background(uint8_t x,uint8_t y);

#endif // _GAME_H
