
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: attribute graphics with cell 2x2 pixels
// image width: 160 pixels
// image height: 120 lines
// image pitch: 20 bytes
// attribute width: 80 colors
// attribute height: 60 rows
// attribute pitch: 40 bytes
extern const u8 ImgIntro_Attr[2400];
extern const u8 ImgIntro[2400];

// format: attribute graphics with cell 2x2 pixels
// image width: 14 pixels
// image height: 210 lines
// image pitch: 2 bytes
// attribute width: 7 colors
// attribute height: 105 rows
// attribute pitch: 4 bytes
extern const u8 ImgTiles_Attr[420];
extern const u8 ImgTiles[420];

// format: 1-bit pixel graphics
// image width: 14 pixels
// image height: 210 lines
// image pitch: 2 bytes
extern const u8 ImgTilesGreen[420];
extern const u8 ImgTilesRed[420];
extern const u8 ImgTilesWhite[420];
extern const u8 ImgTilesYellow[420];

// format: attribute graphics with cell 2x2 pixels
// image width: 48 pixels
// image height: 120 lines
// image pitch: 6 bytes
// attribute width: 24 colors
// attribute height: 60 rows
// attribute pitch: 12 bytes
extern const u8 ImgFrame_Attr[720];
extern const u8 ImgFrame[720];

// format: 1-bit pixel graphics
// image width: 48 pixels
// image height: 120 lines
// image pitch: 6 bytes
extern const u8 ImgFrameCyan[720];
extern const u8 ImgFrameRed[720];
extern const u8 ImgFrameWhite[720];
extern const u8 ImgFrameYellow[720];

// format: 1-bit pixel graphics
// image width: 4 pixels
// image height: 5 lines
// image pitch: 1 bytes
extern const u8 ImgComputer[5];

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
