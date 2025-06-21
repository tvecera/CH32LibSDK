
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// text screen
#define FONTW	8		// font width
#define FONTH	8		// font height
#define TEXTW	(WIDTH/8)	// width of text screen (=40)
#define TEXTH	(HEIGHT/FONTH)	// height of text screen (=15)

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
// image width: 64 pixels
// image height: 96 lines
// image pitch: 8 bytes
// attribute width: 32 colors
// attribute height: 48 rows
// attribute pitch: 16 bytes
extern const u8 ImgTiles_Attr[768];
extern const u8 ImgTiles[768];

// format: 1-bit pixel graphics
// image width: 64 pixels
// image height: 96 lines
// image pitch: 8 bytes
extern const u8 ImgTilesBlue[768];
extern const u8 ImgTilesRed[768];
extern const u8 ImgTilesYellow[768];

// main melody
extern const sMelodyNote MusicSound[];

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
