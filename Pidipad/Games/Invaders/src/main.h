
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: attribute graphics with cell 8x8 pixels
// image width: 256 pixels
// image height: 192 lines
// image pitch: 32 bytes
// attribute width: 32 colors
// attribute height: 24 rows
// attribute pitch: 16 bytes
extern const u8 ImgIntro_Attr[384];
extern const u8 ImgIntro[6144];

// format: 1-bit pixel graphics
// image width: 48 pixels
// image height: 56 lines
// image pitch: 6 bytes
extern const u8 ImgSprites[336];
#define IMGSPRITE_WBS	6

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
