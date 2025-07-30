#include "../include.h"

// format: 1-bit pixel graphics
// image width: 5 pixels
// image height: 5 lines
// image pitch: 1 bytes
const u8 ImgBrick[5] __attribute__ ((aligned(4))) = {
	0x08, 0x0F, 0xF8, 0x40, 0xF8, 
};
