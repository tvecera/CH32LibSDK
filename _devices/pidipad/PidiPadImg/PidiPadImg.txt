PidiPadImg version 1.0, (c) 2025 Miroslav Nemecek
Image converter to PidiPad formats.

Input image must be in Windows BMP format, in 1-bit or 4-bit color format.
Use pal_pidipad.act palettes to convert to 4-bit palleted format.
Switch OFF compression, switch ON "flip row order" (or flip the image vertically).

Syntax: input.bmp output.cpp name format
  'input.bmp' input image in BMP format
  'output.cpp' output file as C++ source
  'name' name of data array
  'format' specifies required output format:
	0 ... mono color format (requires 1-bit input image)\n");
	1 ... videomode 1, 160x120, attribute format
	2 ... videomode 2, 160x120, attribute format
	3 ... videomode 3, 160x120, attribute format
	4 ... videomode 4, 256x192, attribute format
	5 ... videomode 5, 144x96, full graphics 8 colors
	6 ... videomode 6, 80x60, text pseudographics, cell 2x2 pixels
	7 ... videomode 7, 80x60, text pseudographics, cell 2x2 pixels
	8 ... videomode 8, 160x60, text pseudographics, cell 2x2 pixels
