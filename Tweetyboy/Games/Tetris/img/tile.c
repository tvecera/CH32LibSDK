#include "../include.h"

// format: 1-bit pixel graphics
// image width: 5 pixels
// image height: 5 lines
// image pitch: 1 bytes
const u8 ImgTile[5] __attribute__ ((aligned(4))) = {
	0x00, 0x77, 0x70, 0x70, 0x00, 
};
