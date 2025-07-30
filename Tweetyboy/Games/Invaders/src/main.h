
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 80 lines
// image pitch: 160 bytes
//extern const u16 ImgIntro_Pal[256] __attribute__ ((aligned(4)));
extern const u8 ImgIntro[12800] __attribute__ ((aligned(4)));

// format: 1-bit pixel graphics
// image width: 48 pixels
// image height: 56 lines
// image pitch: 6 bytes
extern const u8 ImgSprites[336] __attribute__ ((aligned(4)));
#define IMGSPRITE_WBS	6

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
