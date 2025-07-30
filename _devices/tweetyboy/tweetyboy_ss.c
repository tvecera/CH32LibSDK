
// ****************************************************************************
//
//                             TweetyBoy - Screen Shot
//
// ****************************************************************************

#include "../../includes.h"

#if USE_SCREENSHOT		// 1=use screen shot

Bool ScreenShotDiskWasMount = False; // disk was mounted
Bool ScreenShotIsOpen = False; // screen shot was open
sFile ScreenShotFile; // screen shot file

// BMP file header (size 70 bytes)
#pragma pack(push,1)
typedef struct {
	// BMP file header (size 14 bytes)
	u16	bfType;			// 0x00: magic, 'B' 'M' = 0x4D42
	u32	bfSize;			// 0x02: file size, aligned to DWORD = 54 + 256*4 + 160*80 + 2 = 13880 = 0x3638
	u16	bfReserved1;		// 0x06: = 0
	u16	bfReserved2;		// 0x08: = 0
	u32	bfOffBits;		// 0x0A: offset of data bits after file header = 1024 + 54 = 1078 (0x436)
	// BMP info header (size 40 bytes)
	u32	biSize;			// 0x0E: size of this info header = 40 (0x28)
	s32	biWidth;		// 0x12: width = 160 (0xA0)
	s32	biHeight;		// 0x16: height, negate if flip row order = -80 (0xFFFFFFB0)
	u16	biPlanes;		// 0x1A: planes = 1
	u16	biBitCount;		// 0x1C: number of bits per pixel = 8
	u32	biCompression;		// 0x1E: compression = 0 (none)
	u32	biSizeImage;		// 0x22: size of data of image + aligned file = 160*80 + 2 = 12802 = 0x3202
	s32	biXPelsPerMeter;	// 0x26: X pels per meter = 2834 (= 0xB12)
	s32	biYPelsPerMeter;	// 0x2A: Y pels per meter = 2834 (= 0xB12)
	u32	biClrUsed;		// 0x2E: number of user colors (0 = all)
	u32	biClrImportant;		// 0x32: number of important colors (0 = all)
	// u32	pal[256];		// 0x36: 256 palettes, size 1024 bytes
	//				// 0x436: image data
} sBmpSS;
#pragma pack(pop)

STATIC_ASSERT(sizeof(sBmpSS) == 0x36, "Incorrect sBmpSS!");

// BMP file header, 1 bit per pixel
const sBmpSS BmpHeader = {
	// BMP file header (size 14 bytes)
	0x4D42,			// u16	bfType;		// 0x00: magic, 'B' 'M' = 0x4D42
	0x36 + 256*4 + WIDTH*HEIGHT + 2, // u32	bfSize;	// 0x02: file size, aligned to DWORD = 54 + 256*4 + 160*80 + 2 = 13880 = 0x3638
	0,			// u16	bfReserved1;	// 0x06: = 0
	0,			// u16	bfReserved2;	// 0x08: = 0
	0x436,			// 0x0A: offset of data bits after file header = 1024 + 54 = 1078 (0x436)
	// BMP info header (size 40 bytes)
	40,			// u32	biSize;		// 0x0E: size of this info header = 40 (0x28)
	WIDTH,			// s32	biWidth;	// 0x12: width = 160 (0xA0)
	-HEIGHT,		// s32	biHeight;	// 0x16: height, negate if flip row order = -80 (0xFFFFFFB0)
	1,			// u16	biPlanes;	// 0x1A: planes = 1
	8,			// u16	biBitCount;	// 0x1C: number of bits per pixel = 8
	0,			// u32	biCompression;	// 0x1E: compression = 0 (none)
	WIDTH*HEIGHT + 2,	// u32	biSizeImage;	// 0x22: size of data of image + aligned file = 160*80 + 2 = 12802 = 0x3202
	2834,			// s32	biXPelsPerMeter; // 0x26: X pels per meter = 2834 (= 0xB12)
	2834,			// s32	biYPelsPerMeter; // 0x2A: Y pels per meter = 2834 (= 0xB12)
	0,			// u32	biClrUsed;	// 0x2E: number of user colors (0 = all)
	0,			// u32	biClrImportant;	// 0x32: number of important colors (0 = all)
	// u32	pal[256];	// 0x36: 256 palettes, size 1024 bytes
	//			// 0x436: image data
};

// open screenshot (returns False if cannot open)
Bool OpenScreenShot()
{
	int i;
	char fn[] = "/SCR00001.BMP";

	if (ScreenShotIsOpen) return False; // already open
	ScreenShotIsOpen = True;

	// mounted flag
	ScreenShotDiskWasMount = DiskMounted();

	Bool try = True;
	Bool res;

ScreenShotReset:

	// auto-mount disk
	res = DiskAutoMount();
	if (!res) DiskUnmount();
	if (DiskAutoMount())
	{
		// prepare file name
		while (FileExist(fn))
		{
			i = 8;
			for (;;)
			{
				fn[i]++;
				if (fn[i] <= '9') break;
				fn[i] = '0';
				i--;
			}
		}

		res = FileCreate(&ScreenShotFile, fn);
		if (!res && try)
		{
			DiskUnmount();
			try = False;
			goto ScreenShotReset;
		}

		if (res)
		{
			// write BMP file header
			FileWrite(&ScreenShotFile, &BmpHeader, sizeof(sBmpSS));

			// write palettes
			const u16* pal = Palette;
			u16 d;
			u8 r, g, b;
			for (i = 256; i > 0; i--)
			{
				d = *pal++;

				r = (d >> 11);
				r = (r << 3) | (r >> 2);
				if (r == 0x21) r = 0x24;
				if (r == 0x4A) r = 0x49;
				if (r == 0x6B) r = 0x6D;
				if (r == 0x94) r = 0x92;
				if (r == 0xB5) r = 0xB6;
				if (r == 0xDE) r = 0xDB;

				g = (d >> 5) & 0x3f;
				g = (g << 2) | (g >> 4);

				b = d & 0x1f;
				b = (b << 3) | (b >> 2);
				if (b == 0x52) b = 0x55;
				if (b == 0xAD) b = 0xAA;

				u32 k = b | ((u32)g << 8) | ((u32)r << 16);
				FileWrite(&ScreenShotFile, &k, 4);
			}

			// was open
			return True;
		}
	}

	// error - was not mounted
	ScreenShotIsOpen = False;
	if (!ScreenShotDiskWasMount) DiskUnmount();
	return False;
}

// write data to screenshot file
void WriteScreenShot(const void* data, int num)
{
	if (ScreenShotIsOpen)
	{
		// write image data
		FileWrite(&ScreenShotFile, data, num);
	}
}

// close screenshot
void CloseScreenShot()
{
	if (ScreenShotIsOpen)
	{
		// close file
		FileClose(&ScreenShotFile);

		// flush disk writes
		DiskFlush();

		// unmount
		if (!ScreenShotDiskWasMount) DiskUnmount();
		ScreenShotIsOpen = False;
	}
}

// Do one screen shot (generates file SCRxxxxx.BMP in root of SD card)
void ScreenShot()
{
	// open screen shot
	if (OpenScreenShot())
	{
		int n = WIDTH*HEIGHT;

		// write image data
		WriteScreenShot(FrameBuf, n + 2);

		// close screenshot
		CloseScreenShot();
	}
}

#endif // USE_SCREENSHOT
