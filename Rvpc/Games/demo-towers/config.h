
// ****************************************************************************
//                                 
//                        Project library configuration
//
// ****************************************************************************

#ifndef _CONFIG_H
#define _CONFIG_H

// default font
#define FONT		    NULL	// default system font
#define FONT_CHAR_COUNT	128	                // number of characters in the font
#define FONTCOND	    NULL	            // default condensed font

// Videomodes (B&W mono):
//  0 ... graphics mode 128x64 pixels, required memory 1024 B (driver size 738 B in RAM)
//	1 ... graphics mode 160x120 pixels, required memory 2400 B (driver size 738 B in RAM) - Only for RVPC with CH32V002
//	8 ... text mode 23x18 characters of 8x8 pixels with custom fonts of 64/128 characters - 720x576 pixels (PAL),
//	required memory 414 bytes, font 8x8 pixels 4096/8192 bytes in Flash
#define VMODE       8
#define SOUND_MUTE  0

// ----------------------------------------------------------------------------
//                            Library modules
// ----------------------------------------------------------------------------

#define USE_CRC		0	// 1=use CRC library
#define USE_DECNUM	1	// 1=use decode number
#define USE_FAT		0	// 1=use FAT filesystem
#define USE_RAND	1	// 1=use random number generator
#define USE_SD		0	// 1=use SD card driver

// ----------------------------------------------------------------------------
//                             Device setup
// ----------------------------------------------------------------------------

#define USE_DRAW	0	// 1=use graphics drawing functions
#define USE_PRINT	1	// 1=use text printing functions
#define USE_KEY		1	// 1=use keyboard support
#define USE_SOUND	1	// 1=use sound support
#define USE_DISP	1	// 1=use display support

#endif // _CONFIG_H
