
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: attribute graphics with cell 4x4 pixels
// image width: 160 pixels
// image height: 120 lines
// image pitch: 20 bytes
// attribute width: 40 colors
// attribute height: 30 rows
// attribute pitch: 20 bytes
extern const u8 ImgIntro_Attr[600];
extern const u8 ImgIntro[2400];

// format: 1-bit pixel graphics
// image width: 128 pixels
// image height: 24 lines
// image pitch: 16 bytes
extern const u8 ImgPiecesBlack[384];
extern const u8 ImgPiecesWhite[384];
#define PIECESIMGWB	16	// width of image

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
