// Copyright (c) 2023-2025 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

// Images for PicoPad and PicoLibSDK
// PidiPadImg version 1.0, January 2025

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#pragma warning(disable : 4996) // unsafe fopen

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long int s32;		// on 64-bit system use "signed int"
typedef unsigned long int u32;		// on 64-bit system use "unsigned int"

typedef unsigned int BOOL;
#define TRUE  1
#define FALSE 0

// output formats
enum {
	FORMAT_0 = 0,	// '0': mono color format, requires 1-bit input image
	FORMAT_1,		// '1': videomode 1, 160x120, attribute format, cell 8x8 pixels
	FORMAT_2,		// '2': videomode 2, 160x120, attribute format, cell 4x4 pixels
	FORMAT_3,		// '3': videomode 3, 160x120, attribute format, cell 2x2 pixels
	FORMAT_4,		// '4': videomode 4, 256x192, attribute format, cell 8x8 pixels
	FORMAT_5,		// '5': videomode 5, 144x96, full graphics 8 colors
	FORMAT_6,		// '6': videomode 6, 80x60, text pseudographics, cell 2x2 pixels
	FORMAT_7,		// '7': videomode 7, 80x60, text pseudographics, cell 2x2 pixels
	FORMAT_8,		// '8': videomode 8, 160x60, text pseudographics, cell 2x2 pixels

	FORMAT_NUM
};

const char* SwitchTxt[] = {
	"0",	// FORMAT_0: mono color format, requires 1-bit input image
	"1",	// FORMAT_1: videomode 1: attribute format, cell 8x8 pixels
	"2",	// FORMAT_2: videomode 2: attribute format, cell 4x4 pixels
	"3",	// FORMAT_3: videomode 3: attribute format, cell 2x2 pixels
	"4",	// FORMAT_4: videomode 4: attribute format, cell 8x8 pixels
	"5",	// FORMAT_5: videomode 5: full graphics 8 colors
	"6",	// FORMAT_6: videomode 6, 80x60, text pseudographics, cell 2x2 pixels
	"7",	// FORMAT_7: videomode 7, 80x60, text pseudographics, cell 2x2 pixels
	"8",	// FORMAT_8: videomode 8, 160x60, text pseudographics, cell 2x2 pixels
};

#pragma pack(push,1)
typedef struct _bmpBITMAPFILEHEADER { // 14 bytes
		u16		bfType;
		u32		bfSize;
		u16		bfReserved1;
		u16		bfReserved2;
		u32		bfOffBits;
} bmpBITMAPFILEHEADER;

typedef struct _bmpBITMAPINFOHEADER{ // 40 bytes at offset 0x0E
		u32		biSize;
		s32		biWidth;
		s32		biHeight;
		u16		biPlanes;
		u16		biBitCount;
		u32		biCompression;
		u32		biSizeImage;
		s32		biXPelsPerMeter;
		s32		biYPelsPerMeter;
		u32		biClrUsed;
		u32		biClrImportant;
		// palettes at offset 54 = 0x36
} bmpBITMAPINFOHEADER;
#pragma pack(pop)

#define bmpBI_RGB	0
#define bmpBI_RLE8	1
#define bmpBI_RLE4	2

int OutFormat;	// output format FORMAT_*
u8* Img = NULL; // input file buffer
int W,H,WBS; // input file width and height
int B; // bit count
u8* D; // start of image line
int AttrCW;		// attribute compression - cell width (2, 4 or 8 pixels)
int AttrCH;		// attribute compression - cell height (2, 4 or 8 pixels)
int AttrW, AttrH; // attribute width and height in colors
int AttrWB;		// attribute bytes per line
u8* Attr = NULL; // attribute buffer

// search attributes ... use first non-black color in the cell. If gray, try to search another color.
void SearchAttr()
{
	int i, j, k, m;
	u8 b, b2;
	u8 *d, *s;

	for (i = 0; i < AttrH; i++)	// loop through attribute rows
	{
		d = &Attr[i*AttrW];	// destination buffer
		s = &D[i*AttrCH*WBS];	// source buffer
		for (j = 0; j < AttrW; j++) // loop through attribute colors
		{
			b = 8;	// preset black color
			for (k = 0; k < AttrCH; k++)
			{
				for (m = 0; m < AttrCW; m++)
				{
					b2 = s[m+k*WBS];
					if (b2 != 8) b = b2; // if not black, use this color
					if ((b != 0) && (b != 8)) break; // if not black nor gray, stop
				}
				if (m < AttrCW) break;
			}
			if (b == 8) b = 7; // no color, use white color

			*d++ = b;	// save color and shift destination pointer
			s += AttrCW; // shift source pointer
		}
	}
}

// load attributes
void LoadAttr(FILE* f, const char* name)
{
	int n, i, j;
	u8 b, b2;
	u8 *s;

	fprintf(f, "const u8 %s_Attr[%d] = {", name, AttrWB*AttrH);
	n = 0;
	for (i = 0; i < AttrH; i++)
	{
		s = &Attr[i*AttrW];
		for (j = 0; j < AttrWB; j++)
		{
			if ((n & 0x0f) == 0) fprintf(f, "\n\t");

			b = *s++ & 7;
			b = (b & 3) | ((b & 4) << 1);

			b2 = *s++ & 7;
			b2 = (b2 & 3) | ((b2 & 4) << 1);

			b = (b2 << 4) | b;

			fprintf(f, "0x%02X, ", b);
			n++;
		}
	}

	fprintf(f, "\n};\n\n");
}

void Help()
{
		printf("PidiPadImg version 1.0, (c) 2025 Miroslav Nemecek\n");
		printf("Syntax: input.bmp output.c name format\n");
		printf("  'input.bmp' input image in BMP format\n");
		printf("  'output.c' output file as C source\n");
		printf("  'name' name of data array\n");
		printf("  'format' specifies required output format:\n");
		printf("       0 ... mono color format (requires 1-bit input image)\n");
		printf("       1 ... videomode 1, 160x120, attribute format, cell 8x8 pixels\n");
		printf("       2 ... videomode 2, 160x120, attribute format, cell 4x4 pixels\n");
		printf("       3 ... videomode 3, 160x120, attribute format, cell 2x2 pixels\n");
		printf("       4 ... videomode 4, 256x192, attribute format, cell 8x8 pixels\n");
		printf("       5 ... videomode 5, 144x96, full graphics 8 colors\n");
		printf("       6 ... videomode 6, 80x60, text pseudographics, cell 2x2 pixels\n");
		printf("       7 ... videomode 7, 80x60, text pseudographics, cell 2x2 pixels\n");
		printf("       8 ... videomode 8, 160x60, text pseudographics, cell 2x2 pixels\n");
}

// unpack 4-bit image
void Unpack4()
{
	u8* s = &D[WBS*H];
	u8* d = &D[WBS*2*H];
	int i;
	for (i = WBS*H; i > 0; i--)
	{
		d--;
		s--;
		*d = *s & 0x0f;
		d--;
		*d = *s >> 4;
	}
	WBS *= 2;
}

// main function
int main(int argc, char* argv[])
{
	int i, j, k, wb, n, w, h;
	u8 b, b2;
	u8 *d, *s;

	if ((sizeof(_bmpBITMAPFILEHEADER) != 14) ||
		(sizeof(_bmpBITMAPINFOHEADER) != 40))
	{
		printf("Incorrect size of data types. Check typedef of s32/u32 or do 32-bit compilation.\n");
		return 1;
	}

	// base check syntax
	if (argc != 5)
	{
		Help();
		return 1;
	}

	// search flags
	for (i = 0; i < FORMAT_NUM; i++)
	{
		if (strcmp(argv[4], SwitchTxt[i]) == 0)
		{
			OutFormat = i;
			break;
		}
	}

	if (i == FORMAT_NUM)
	{
		Help();
		return 1;
	}

	// open main input file
	FILE* f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		printf("Error opening input file %s\n", argv[1]);
		return 1;
	}

	// size of input file
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (size < 40)
	{
		printf("Incorrect size of input file %s\n", argv[1]);
		return 1;
	}

	// create buffer (double size to unpack 4-bit format, + some more to align)
	Img = (u8*)malloc(size*2 + 16);
	Attr = (u8*)malloc(size*2 + 16);
	if ((Img == NULL) || (Attr == NULL))
	{
		printf("Memory error\n");
		return 1;
	}

	// read file
	int size2 = (int)fread(Img, 1, size, f);
	fclose(f);
	if (size2 != size)
	{
		printf("Error reading input file %s\n", argv[1]);
		return 1;
	}

	// check BMP header
	bmpBITMAPFILEHEADER* bmf = (bmpBITMAPFILEHEADER*)Img;
	bmpBITMAPINFOHEADER* bmi = (bmpBITMAPINFOHEADER*)&bmf[1];
	W = bmi->biWidth;
	H = bmi->biHeight;
	B = bmi->biBitCount;
	if (H < 0) H = -H;
	if ((bmf->bfType != 0x4d42) ||
		(bmf->bfOffBits < 0x30) || (bmf->bfOffBits > 0x440) ||
		(bmi->biCompression != bmpBI_RGB) ||
		(W < 4) || (W > 10000) || (H < 4) || (H > 10000) ||
		((B != 4) && (B != 1)))
	{
		printf("Incorrect format of input file %s, must be Windows-BMP\n", argv[1]);
		printf("  4-bit or 1-bit uncompressed.\n");
		return 1;
	}
	D = &Img[bmf->bfOffBits];

	if (OutFormat == FORMAT_0)
	{
		if (B != 1)
		{
			printf("Output format '0' requires 1-bit input image %s\n", argv[1]);
			return 1;
		}
	}
	else
	{
		if (B != 4)
		{
			printf("Output formats '1' to '5' require 4-bit input image %s\n", argv[1]);
			return 1;
		}
	}

	// source width in bytes
	WBS = (((W*B+7)/8) + 3)& ~3;

	// open output file
	f = fopen(argv[2], "wb");
	if (f == NULL)
	{
		printf("Error creating %s\n", argv[2]);
		return 1;
	}

	// export
	switch (OutFormat)
	{
	case FORMAT_0:	// "0": mono color format, requires 1-bit input image
		wb = (W+7)/8;

		// info header
		fprintf(f, "#include \"../include.h\"\n\n");
		fprintf(f, "// format: 1-bit pixel graphics\n");
		fprintf(f, "// image width: %d pixels\n", W);
		fprintf(f, "// image height: %d lines\n", H);
		fprintf(f, "// image pitch: %d bytes\n", wb);

		// image header
		fprintf(f, "const u8 %s[%d] = {", argv[3], wb*H);

		// load image
		n = 0;
		for (i = 0; i < H; i++)
		{
			for (j = 0; j < wb; j++)
			{
				if ((n & 0x0f) == 0) fprintf(f, "\n\t");
				fprintf(f, "0x%02X, ", D[j]);
				n++;
			}
			D += WBS;
		}
		break;

	case FORMAT_1:	// "1": videomode 1: attribute format, cell 8x8 pixels
	case FORMAT_4:	// "4": videomode 4: attribute format, cell 8x8 pixels (same as format '1')
		AttrCW = 8;
		AttrCH = 8;
		goto Attrib;

	case FORMAT_2:	// "2": videomode 2: attribute format, cell 4x4 pixels
		AttrCW = 4;
		AttrCH = 4;
		goto Attrib;

	case FORMAT_3:	// "3": videomode 3: attribute format, cell 2x2 pixels
		AttrCW = 2;
		AttrCH = 2;

Attrib:

		// unpack image to 8-bit format
		Unpack4();

		wb = (W+7)/8;	// destination width in bytes

		AttrW = (W+AttrCW-1)/AttrCW; // attribute width in colors
		AttrWB = (AttrW+1)/2;	// attribute width in bytes
		AttrH = (H+AttrCH-1)/AttrCH; // attribute height in colors

		// info header
		fprintf(f, "#include \"../include.h\"\n\n");
		fprintf(f, "// format: attribute graphics with cell %dx%d pixels\n", AttrCW, AttrCH);
		fprintf(f, "// image width: %d pixels\n", W);
		fprintf(f, "// image height: %d lines\n", H);
		fprintf(f, "// image pitch: %d bytes\n", wb);
		fprintf(f, "// attribute width: %d colors\n", AttrW);
		fprintf(f, "// attribute height: %d rows\n", AttrH);
		fprintf(f, "// attribute pitch: %d bytes\n", AttrWB);

		// search attributes
		SearchAttr();

		// load attributes
		LoadAttr(f, argv[3]);

		// image header
		fprintf(f, "const u8 %s[%d] = {", argv[3], wb*H);
		n = 0;
		for (i = 0; i < H; i++)	// loop through image lines
		{
			s = &D[i*WBS];	// source buffer
			for (j = 0; j < wb; j++) // loop through image bytes
			{
				b = 0;
				for (k = 0; k < 8; k++) // loop through cell bits
				{
					d = &Attr[i/AttrCH*AttrW + (j*8+k)/AttrCW];	// attribute
					b <<= 1;
					if (*s == *d) b |= 1;
					s++;
				}

				if ((n & 0x0f) == 0) fprintf(f, "\n\t");
				fprintf(f, "0x%02X, ", b);
				n++;
			}
		}
		break;

	case FORMAT_5:	// "5": videomode 5: full graphics 8 colors

		// unpack image to 8-bit format
		Unpack4();
		wb = (W+1)/2;

		// info header
		fprintf(f, "#include \"../include.h\"\n\n");
		fprintf(f, "// format: 4-bit pixel graphics\n");
		fprintf(f, "// image width: %d pixels\n", W);
		fprintf(f, "// image height: %d lines\n", H);
		fprintf(f, "// image pitch: %d bytes\n", wb);

		// image header
		fprintf(f, "const u8 %s[%d] = {", argv[3], wb*H);

		// load image
		n = 0;
		for (i = 0; i < H; i++)
		{
			for (j = 0; j < wb; j++)
			{
				if ((n & 0x0f) == 0) fprintf(f, "\n\t");
				b = D[j*2] & 7;
				b = (b & 3) | ((b & 4) << 1);

				b2 = D[j*2+1] & 7;
				b2 = (b2 & 3) | ((b2 & 4) << 1);

				b = (b2 << 4) | b;

				fprintf(f, "0x%02X, ", b);
				n++;
			}
			D += WBS;
		}
		break;

	case FORMAT_6:	// '6': videomode 6, 80x60, text pseudographics, cell 2x2 pixels
	case FORMAT_7:	// '7': videomode 7, 80x60, text pseudographics, cell 2x2 pixels
	case FORMAT_8:	// '8': videomode 8, 160x60, text pseudographics, cell 2x2 pixels

		AttrCW = 2;
		AttrCH = 2;

		// unpack image to 8-bit format
		Unpack4();

		w = (W + 1)/2;
		h = (H + 1)/2;
		wb = w;	// destination width in bytes

		AttrW = w; // attribute width in colors
		AttrWB = (AttrW+1)/2;	// attribute width in bytes
		AttrH = h; // attribute height in colors

		// info header
		fprintf(f, "#include \"../include.h\"\n\n");
		fprintf(f, "// format: pseudo graphics with cell 2x2 pixels\n");
		fprintf(f, "// text width: %d characters\n", w);
		fprintf(f, "// text height: %d rows\n", h);
		fprintf(f, "// text pitch: %d bytes\n", wb);
		fprintf(f, "// image width: %d pixels\n", w*2);
		fprintf(f, "// image height: %d lines\n", h*2);
		fprintf(f, "// attribute width: %d colors\n", AttrW);
		fprintf(f, "// attribute height: %d rows\n", AttrH);
		fprintf(f, "// attribute pitch: %d bytes\n", AttrWB);

		// search attributes
		SearchAttr();

		// load attributes
		LoadAttr(f, argv[3]);

		// text header
		fprintf(f, "const u8 %s[%d] = {", argv[3], wb*h);
		n = 0;
		for (i = 0; i < h; i++)	// loop through text rows
		{
			s = &D[i*2*WBS];	// source buffer
			d = &Attr[i*AttrW];	// attribute
			for (j = 0; j < wb; j++) // loop through text characters
			{
				b = 128;
				if (s[0] == d[0]) b |= 2;
				if (s[1] == d[0]) b |= 1;
				if (s[WBS] == d[0]) b |= 8;
				if (s[WBS+1] == d[0]) b |= 4;
				s += 2;
				d++;

				if ((n & 0x0f) == 0) fprintf(f, "\n\t");
				fprintf(f, "0x%02X, ", b);
				n++;
			}
		}
		break;
	};

	fprintf(f, "\n};\n");

	// close file
	fclose(f);
	free(Img);
	free(Attr);

	return 0;
}

