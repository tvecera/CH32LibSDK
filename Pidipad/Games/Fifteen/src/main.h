
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

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
