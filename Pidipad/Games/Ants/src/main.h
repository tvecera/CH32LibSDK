
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: pseudo graphics with cell 2x2 pixels
// text width: 80 characters
// text height: 30 rows
// text pitch: 80 bytes
// image width: 160 pixels
// image height: 60 lines
// attribute width: 80 colors
// attribute height: 30 rows
// attribute pitch: 40 bytes
extern const u8 ImgIntro_Attr[1200];
extern const u8 ImgIntro[2400];

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
