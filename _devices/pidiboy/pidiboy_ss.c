
// ****************************************************************************
//
//                             PidiBoy - Screen Shot
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
	u32	bfSize;			// 0x02: file size, aligned to DWORD = 62 + (128+7)/8*64 + 2 = 1088 = 0x440
	u16	bfReserved1;		// 0x06: = 0
	u16	bfReserved2;		// 0x08: = 0
	u32	bfOffBits;		// 0x0A: offset of data bits after file header = 62 (0x3E)
	// BMP info header (size 40 bytes)
	u32	biSize;			// 0x0E: size of this info header = 40 (0x28)
	s32	biWidth;		// 0x12: width = 128 (0x80)
	s32	biHeight;		// 0x16: height, negate if flip row order = -64 (0xFFFFFFC0)
	u16	biPlanes;		// 0x1A: planes = 1
	u16	biBitCount;		// 0x1C: number of bits per pixel = 1
	u32	biCompression;		// 0x1E: compression = 0 (none)
	u32	biSizeImage;		// 0x22: size of data of image + aligned file = (128+7)/8*64 + 2 = 1026 = 0x402
	s32	biXPelsPerMeter;	// 0x26: X pels per meter = 2834 (= 0xB12)
	s32	biYPelsPerMeter;	// 0x2A: Y pels per meter = 2834 (= 0xB12)
	u32	biClrUsed;		// 0x2E: number of user colors (0 = all)
	u32	biClrImportant;		// 0x32: number of important colors (0 = all)
	u32	pal0;			// 0x36: palette of color 0
	u32	pal1;			// 0x3A: palette of color 1
					// 0x3E:
} sBmpSS;
#pragma pack(pop)

STATIC_ASSERT(sizeof(sBmpSS) == 0x3E, "Incorrect sBmpSS!");

// BMP file header, 1 bit per pixel
const sBmpSS BmpHeader = {
	// BMP file header (size 14 bytes)
	0x4D42,			// u16	bfType;		// 0x00: magic, 'B' 'M' = 0x4D42
	0x3e + (WIDTH+7)/8*HEIGHT + 2, // u32	bfSize;	// 0x02: file size, aligned to DWORD = 62 + (128+7)/8*64 + 2 = 1088 = 0x440
	0,			// u16	bfReserved1;	// 0x06: = 0
	0,			// u16	bfReserved2;	// 0x08: = 0
	0x3e,			// 0x0A: offset of data bits after file header = 62 (0x3E)
	// BMP info header (size 40 bytes)
	40,			// u32	biSize;		// 0x0E: size of this info header = 40 (0x28)
	WIDTH,			// s32	biWidth;	// 0x12: width = 128 (0x80)
	-HEIGHT,		// s32	biHeight;	// 0x16: height, negate if flip row order = -64 (0xFFFFFFC0)
	1,			// u16	biPlanes;	// 0x1A: planes = 1
	1,			// u16	biBitCount;	// 0x1C: number of bits per pixel = 1
	0,			// u32	biCompression;	// 0x1E: compression = 0 (none)
	(WIDTH+7)/8*HEIGHT + 2,	// u32	biSizeImage;	// 0x22: size of data of image + aligned file = (128+7)/8*64 + 2 = 1026 = 0x402
	2834,			// s32	biXPelsPerMeter; // 0x26: X pels per meter = 2834 (= 0xB12)
	2834,			// s32	biYPelsPerMeter; // 0x2A: Y pels per meter = 2834 (= 0xB12)
	0,			// u32	biClrUsed;	// 0x2E: number of user colors (0 = all)
	0,			// u32	biClrImportant;	// 0x32: number of important colors (0 = all)
	0xffffff,		// u32  pal0;		// 0x36: palette of color 0
	0,			// u32  pal1;		// 0x3A: palette of color 1
							// 0x3E:
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
		int n = (WIDTH+7)/8*HEIGHT;
		int i;

		// write image data
		for (i = 0; i < n; i++) FrameBuf[i] = ~FrameBuf[i];
		WriteScreenShot(FrameBuf, n + 2);
		for (i = 0; i < n; i++) FrameBuf[i] = ~FrameBuf[i];

		// close screenshot
		CloseScreenShot();
	}
}

#endif // USE_SCREENSHOT
