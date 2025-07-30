
// ****************************************************************************
//
//                             TweetyBoy - Drawing
//
// ****************************************************************************

#include "../../includes.h"

#if USE_DRAW || USE_PRINT || USE_DISP

const u8* DrawFont = FONT;		// current draw font (characters 8x8)
const u8* DrawFontCond = FONTCOND;	// current draw condensed font (characters 6x8)
int PrintPos = 0;			// current print position
int PrintRow = 0;			// current print row
u8 PrintCol = COL_WHITE;		// current print color
u8 PrintInv = 0;			// print inversion - offset added to characters (128 = inversion)

#if USE_DRAW || USE_PRINT
// clear screen
void DrawClear()
{
	memset(FrameBuf, COL_BLACK, FRAMESIZE);
	PrintPos = 0;
	PrintRow = 0;
	PrintCol = COL_WHITE;
	PrintInv = 0;
}
#endif // USE_DRAW || USE_PRINT

#if USE_DRAW	// 1=use graphics drawing functions

// ----------------------------------------------------------------------------
//                               Draw point
// ----------------------------------------------------------------------------

// draw pixel fast without limits
void DrawPointFast(int x, int y, u8 col)
{
	FrameBuf[x + y*WIDTHBYTE] = col;
}

// draw pixel
void DrawPoint(int x, int y, u8 col)
{
	if (((uint)x < (uint)WIDTH) && ((uint)y < (uint)HEIGHT)) FrameBuf[x + y*WIDTHBYTE] = col;
}

// get pixel color
u8 DrawGetPoint(int x, int y)
{
	if (((uint)x >= (uint)WIDTH) || ((uint)y >= (uint)HEIGHT)) return COL_BLACK;
	return FrameBuf[x + y*WIDTHBYTE];
}

// invert pixel fast without limits
void DrawPointInvFast(int x, int y)
{
	u8* d = &FrameBuf[x + y*WIDTHBYTE];
	*d = ~*d;
}

// invert pixel
void DrawPointInv(int x, int y)
{
	if (((uint)x < (uint)WIDTH) && ((uint)y < (uint)HEIGHT)) DrawPointInvFast(x, y);
}

// ----------------------------------------------------------------------------
//                            Draw rectangle
// ----------------------------------------------------------------------------

// draw rectangle
void DrawRect(int x, int y, int w, int h, u8 col)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > WIDTH) w = WIDTH - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > HEIGHT) h = HEIGHT - y;
	if (h <= 0) return;

	// draw rectangle
	u8* d = &FrameBuf[x + y*WIDTHBYTE];
	int wb = WIDTHBYTE - w;
	int w2;
	for (; h > 0; h--)
	{
		for (w2 = w; w2 > 0; w2--)
		{
			*d++ = col;
		}
		d += wb;
	}
}

// invert rectangle
void DrawRectInv(int x, int y, int w, int h)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > WIDTH) w = WIDTH - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > HEIGHT) h = HEIGHT - y;
	if (h <= 0) return;

	// draw rectangle
	u8* d = &FrameBuf[x + y*WIDTHBYTE];
	int wb = WIDTHBYTE - w;
	int w2;
	for (; h > 0; h--)
	{
		for (w2 = w; w2 > 0; w2--)
		{
			*d = ~*d;
			d++;
		}
		d += wb;
	}
}

// ----------------------------------------------------------------------------
//                 Draw horizontal line, vertical line, frame
// ----------------------------------------------------------------------------

// draw horizontal line
void DrawHLine(int x, int y, int w, u8 col) { DrawRect(x, y, w, 1, col); }

// invert horizontal line
void DrawHLineInv(int x, int y, int w) { DrawRectInv(x, y, w, 1); }

// draw vertical line
void DrawVLine(int x, int y, int h, u8 col) { DrawRect(x, y, 1, h, col); }

// invert vertical line
void DrawVLineInv(int x, int y, int h) { DrawRectInv(x, y, 1, h); }

// draw frame
void DrawFrame(int x, int y, int w, int h, u8 col)
{
	if (w <= 1)
	{
		DrawVLine(x, y, h, col);
	}
	else if (h <= 1)
	{
		DrawHLine(x, y, w, col);
	}
	else
	{
		DrawHLine(x, y, w, col);
		DrawHLine(x, y+h-1, w, col);
		DrawVLine(x, y+1, h-2, col);
		DrawVLine(x+w-1, y+1, h-2, col);
	}
}

// invert frame
void DrawFrameInv(int x, int y, int w, int h)
{
	if (w <= 1)
	{
		DrawVLineInv(x, y, h);
	}
	else if (h <= 1)
	{
		DrawHLineInv(x, y, w);
	}
	else
	{
		DrawHLineInv(x, y, w);
		DrawHLineInv(x, y+h-1, w);
		DrawVLineInv(x, y+1, h-2);
		DrawVLineInv(x+w-1, y+1, h-2);
	}
}

// ----------------------------------------------------------------------------
//                                  Draw line
// ----------------------------------------------------------------------------

// draw line
void DrawLine(int x1, int y1, int x2, int y2, u8 col)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			DrawPoint(x1, y1, col);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			DrawPoint(x1, y1, col);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// invert line
void DrawLineInv(int x1, int y1, int x2, int y2)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			DrawPointInv(x1, y1);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			DrawPointInv(x1, y1);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// ----------------------------------------------------------------------------
//                          Draw round (Filled circle)
// ----------------------------------------------------------------------------

// draw round (filled circle)
void DrawRound(int x0, int y0, int r, u8 col)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) DrawPoint(x+x0, y+y0, col);
		}
	}
}

// invert round (filled circle)
void DrawRoundInv(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) DrawPointInv(x+x0, y+y0);
		}
	}
}

// ----------------------------------------------------------------------------
//                               Draw circle
// ----------------------------------------------------------------------------

// draw circle
void DrawCircle(int x0, int y0, int r, u8 col)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		DrawPoint(x0+y, y0-x, col);
		DrawPoint(x0+x, y0-y, col);
		DrawPoint(x0-x, y0-y, col);
		DrawPoint(x0-y, y0-x, col);
		DrawPoint(x0-y, y0+x, col);
		DrawPoint(x0-x, y0+y, col);
		DrawPoint(x0+x, y0+y, col);
		DrawPoint(x0+y, y0+x, col);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// invert circle
void DrawCircleInv(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		DrawPointInv(x0+y, y0-x);
		DrawPointInv(x0+x, y0-y);
		DrawPointInv(x0-x, y0-y);
		DrawPointInv(x0-y, y0-x);
		DrawPointInv(x0-y, y0+x);
		DrawPointInv(x0-x, y0+y);
		DrawPointInv(x0+x, y0+y);
		DrawPointInv(x0+y, y0+x);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// ----------------------------------------------------------------------------
//                               Draw ring
// ----------------------------------------------------------------------------

// draw ring
void DrawRing(int x0, int y0, int rin, int rout, u8 col)
{
	int x, y, d;

	// draw circle
	if (rin == rout)
	{
		DrawCircle(x0, y0, rout, col);
		return;
	}

	// check radius order
	if (rin > rout)
	{
		x = rin;
		rin = rout;
		rout = x;
	}

	// inner radius is 0, draw round
	if (rin <= 0)
	{
		DrawRound(x0, y0, rout, col);
		return;
	}

	// prepare radius
	int rin2 = rin*(rin-1);
	int rout2 = rout*(rout-1);
	rout--;

	// full circle
	for (y = -rout; y <= rout; y++)
	{
		for (x = -rout; x <= rout; x++)
		{
			d = x*x + y*y;
			if ((d >= rin2) && (d <= rout2)) DrawPoint(x+x0, y+y0, col);
		}
	}
}

// invert ring
void DrawRingInv(int x0, int y0, int rin, int rout)
{
	int x, y, d;

	// draw circle
	if (rin == rout)
	{
		DrawCircleInv(x0, y0, rout);
		return;
	}

	// check radius order
	if (rin > rout)
	{
		x = rin;
		rin = rout;
		rout = x;
	}

	// inner radius is 0, draw round
	if (rin <= 0)
	{
		DrawRoundInv(x0, y0, rout);
		return;
	}

	// prepare radius
	int rin2 = rin*(rin-1);
	int rout2 = rout*(rout-1);
	rout--;

	// full circle
	for (y = -rout; y <= rout; y++)
	{
		for (x = -rout; x <= rout; x++)
		{
			d = x*x + y*y;
			if ((d >= rin2) && (d <= rout2)) DrawPointInv(x+x0, y+y0);
		}
	}
}

// ----------------------------------------------------------------------------
//                              Draw triangle
// ----------------------------------------------------------------------------

// draw triangle
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, u8 col)
{
	int x, y, k, xmin, xmax;

	// sort coordinates by Y (order from top to bottom: 1 / 2 / 3)
	if (y2 < y1)
	{
		k = x2; x2 = x1; x1 = k;
		k = y2; y2 = y1; y1 = k;
	}

	if (y3 < y1)
	{
		k = x3; x3 = x1; x1 = k;
		k = y3; y3 = y1; y1 = k;
	}
			
	if (y3 < y2)
	{
		k = x3; x3 = x2; x2 = k;
		k = y3; y3 = y2; y2 = k;
	}

	// top sub-triangle y1 <= y < y2 (without bottom y2)
	for (y = y1; y < y2; y++)
	{
		xmin = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

		if (xmax < xmin)
		{
			k = xmin - xmax + 1;
			if (k > 0) DrawHLine(xmax, y, k, col);
		}
		else
		{
			k = xmax - xmin + 1;
			if (k > 0) DrawHLine(xmin, y, k, col);
		}
	}

	// bottom y2 == y3
	if (y2 == y3)
	{
		xmin = x2;
		xmax = x3;

		if (xmax < xmin)
		{
			k = xmin - xmax + 1;
			if (k > 0) DrawHLine(xmax, y, k, col);
		}
		else
		{
			k = xmax - xmin + 1;
			if (k > 0) DrawHLine(xmin, y, k, col);
		}
	}

	// bottom sub-triangle y2 <= y <= y3
	else
	{
		for (; y <= y3; y++)
		{
			xmin = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
			xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

			if (xmax < xmin)
			{
				k = xmin - xmax + 1;
				if (k > 0) DrawHLine(xmax, y, k, col);
			}
			else
			{
				k = xmax - xmin + 1;
				if (k > 0) DrawHLine(xmin, y, k, col);
			}
		}
	}
}

// invert triangle
void DrawTriangleInv(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int x, y, k, xmin, xmax;

	// sort coordinates by Y (order from top to bottom: 1 / 2 / 3)
	if (y2 < y1)
	{
		k = x2; x2 = x1; x1 = k;
		k = y2; y2 = y1; y1 = k;
	}

	if (y3 < y1)
	{
		k = x3; x3 = x1; x1 = k;
		k = y3; y3 = y1; y1 = k;
	}
			
	if (y3 < y2)
	{
		k = x3; x3 = x2; x2 = k;
		k = y3; y3 = y2; y2 = k;
	}

	// top sub-triangle y1 <= y < y2 (without bottom y2)
	for (y = y1; y < y2; y++)
	{
		xmin = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

		if (xmax < xmin)
		{
			k = xmin - xmax + 1;
			if (k > 0) DrawHLineInv(xmax, y, k);
		}
		else
		{
			k = xmax - xmin + 1;
			if (k > 0) DrawHLineInv(xmin, y, k);
		}
	}

	// bottom y2 == y3
	if (y2 == y3)
	{
		xmin = x2;
		xmax = x3;

		if (xmax < xmin)
		{
			k = xmin - xmax + 1;
			if (k > 0) DrawHLineInv(xmax, y, k);
		}
		else
		{
			k = xmax - xmin + 1;
			if (k > 0) DrawHLineInv(xmin, y, k);
		}
	}

	// bottom sub-triangle y2 <= y <= y3
	else
	{
		for (; y <= y3; y++)
		{
			xmin = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
			xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

			if (xmax < xmin)
			{
				k = xmin - xmax + 1;
				if (k > 0) DrawHLineInv(xmax, y, k);
			}
			else
			{
				k = xmax - xmin + 1;
				if (k > 0) DrawHLineInv(xmin, y, k);
			}
		}
	}
}

// ----------------------------------------------------------------------------
//                               Text
// ----------------------------------------------------------------------------

// Draw character normal sized (no background, graphics coordinates)
void DrawChar(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) DrawPoint(x, y, col);
			m <<= 1;
			x++;
		}
		x -= 8;
		y++;
		src += 256;
	}
}

// Draw character normal sized, with background
void DrawCharBg(char ch, int x, int y, u8 col, u8 colbg)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 8; j > 0; j--)
		{
			DrawPoint(x, y, ((m & B7) != 0) ? col : colbg);
			m <<= 1;
			x++;
		}
		x -= 8;
		y++;
		src += 256;
	}
}

// Draw character condensed size 6x8 (no background, graphics coordinates)
void DrawCharCond(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFontCond[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 6; j > 0; j--)
		{
			if ((m & B7) != 0) DrawPoint(x, y, col);
			m <<= 1;
			x++;
		}
		x -= 6;
		y++;
		src += 256;
	}
}

// Draw character condensed size 6x8, with background
void DrawCharCondBg(char ch, int x, int y, u8 col, u8 colbg)
{
	const u8* src = &DrawFontCond[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 6; j > 0; j--)
		{
			DrawPoint(x, y, ((m & B7) != 0) ? col : colbg);
			m <<= 1;
			x++;
		}
		x -= 6;
		y++;
		src += 256;
	}
}

// Draw character condensed size 6x6 (no background, graphics coordinates)
void DrawCharCond6(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFontCond[(u8)ch];
	int i, j;
	u8 m;
	for (i = 6; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 6; j > 0; j--)
		{
			if ((m & B7) != 0) DrawPoint(x, y, col);
			m <<= 1;
			x++;
		}
		x -= 6;
		y++;
		src += 256;
	}
}

// Draw character condensed size 6x6, black background
void DrawCharCond6Bg(char ch, int x, int y, u8 col, u8 colbg)
{
	const u8* src = &DrawFontCond[(u8)ch];
	int i, j;
	u8 m;
	for (i = 6; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 6; j > 0; j--)
		{
			DrawPoint(x, y, ((m & B7) != 0) ? col : colbg);
			m <<= 1;
			x++;
		}
		x -= 6;
		y++;
		src += 256;
	}
}

// Draw character double-width (no background, graphics coordinates)
void DrawCharW(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPoint(x, y, col);
				DrawPoint(x+1, y, col);
			}
			m <<= 1;
			x += 2;
		}
		x -= 2*8;
		y++;
		src += 256;
	}
}

// Draw character double-height (no background, graphics coordinates)
void DrawCharH(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPoint(x, y, col);
				DrawPoint(x, y+1, col);
			}
			m <<= 1;
			x++;
		}
		x -= 8;
		y += 2;
		src += 256;
	}
}

// Draw character double-sized (no background, graphics coordinates)
void DrawChar2(char ch, int x, int y, u8 col)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		if (PrintInv != 0) m = ~m;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPoint(x, y, col);
				DrawPoint(x+1, y, col);
				DrawPoint(x, y+1, col);
				DrawPoint(x+1, y+1, col);
			}
			m <<= 1;
			x += 2;
		}
		x -= 2*8;
		y += 2;
		src += 256;
	}
}

// Invert character (background not changed, graphics coordinates)
void DrawCharInv(char ch, int x, int y)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) DrawPointInv(x, y);
			m <<= 1;
			x++;
		}
		x -= 8;
		y++;
		src += 256;
	}
}

// Invert character doble-width (background not changed, graphics coordinates)
void DrawCharWInv(char ch, int x, int y)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPointInv(x, y);
				DrawPointInv(x+1, y);
			}
			m <<= 1;
			x += 2;
		}
		x -= 2*8;
		y++;
		src += 256;
	}
}

// Invert character doble-height (background not changed, graphics coordinates)
void DrawCharHInv(char ch, int x, int y)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPointInv(x, y);
				DrawPointInv(x, y+1);
			}
			m <<= 1;
			x++;
		}
		x -= 8;
		y += 2;
		src += 256;
	}
}

// Invert character doble-sized (background not changed, graphics coordinates)
void DrawChar2Inv(char ch, int x, int y)
{
	const u8* src = &DrawFont[(u8)ch];
	int i, j;
	u8 m;
	for (i = 8; i > 0; i--)
	{
		m = *src;
		for (j = 8; j > 0; j--)
		{
			if ((m & B7) != 0) 
			{
				DrawPointInv(x, y);
				DrawPointInv(x+1, y);
				DrawPointInv(x, y+1);
				DrawPointInv(x+1, y+1);
			}
			m <<= 1;
			x += 2;
		}
		x -= 2*8;
		y += 2;
		src += 256;
	}
}

// Draw ASCIIZ text (no background, graphics coordinates)
void DrawText(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawChar(ch, x, y, col);
		x += 8;
	}
}

// Draw ASCIIZ text, with background
void DrawTextBg(const char* text, int x, int y, u8 col, u8 colbg)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharBg(ch, x, y, col, colbg);
		x += 8;
	}
}

// Draw text condensed size 6x8 (no background, graphics coordinates)
void DrawTextCond(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharCond(ch, x, y, col);
		x += 6;
	}
}

// Draw text condensed size 6x8, with background
void DrawTextCondBg(const char* text, int x, int y, u8 col, u8 colbg)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharCondBg(ch, x, y, col, colbg);
		x += 6;
	}
}

// Draw text condensed size 6x6 (no background, graphics coordinates)
void DrawTextCond6(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharCond6(ch, x, y, col);
		x += 6;
	}
}

// Draw text condensed size 6x6, with background
void DrawTextCond6Bg(const char* text, int x, int y, u8 col, u8 colbg)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharCond6Bg(ch, x, y, col, colbg);
		x += 6;
	}
}

// Draw ASCIIZ text double-width (no background, graphics coordinates)
void DrawTextW(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharW(ch, x, y, col);
		x += 2*8;
	}
}

// Draw ASCIIZ text double-height (no background, graphics coordinates)
void DrawTextH(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharH(ch, x, y, col);
		x += 8;
	}
}

// Draw ASCIIZ text double-sized (no background, graphics coordinates)
void DrawText2(const char* text, int x, int y, u8 col)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawChar2(ch, x, y, col);
		x += 2*8;
	}
}

// Invert ASCIIZ text (background not changed, graphics coordinates)
void DrawTextInv(const char* text, int x, int y)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharInv(ch, x, y);
		x += 8;
	}
}

// Invert ASCIIZ text double-width (background not changed, graphics coordinates)
void DrawTextWInv(const char* text, int x, int y)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharWInv(ch, x, y);
		x += 2*8;
	}
}

// Invert ASCIIZ text double-height (background not changed, graphics coordinates)
void DrawTextHInv(const char* text, int x, int y)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawCharHInv(ch, x, y);
		x += 8;
	}
}

// Invert ASCIIZ text double-sized (background not changed, graphics coordinates)
void DrawText2Inv(const char* text, int x, int y)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		DrawChar2Inv(ch, x, y);
		x += 2*8;
	}
}

// ----------------------------------------------------------------------------
//                               Draw image
// ----------------------------------------------------------------------------

// draw mono image, transparent background
void DrawMonoImg(const u8* img, int x, int y, int w, int h, int wsb, u8 col)
{
	int xd;
	int yd = y;
	int ys;
	int xs;
	int m;
	const u8* s;
	u8 b;
	for (ys = 0; ys < h; ys++)
	{
		s = &img[ys*wsb];
		xd = x;
		m = B7;
		b = *s++;
		for (xs = 0; xs < w; xs++)
		{
			if ((b & m) == 0) DrawPoint(xd, yd, col);
			m >>= 1;
			if (m == 0)
			{
				m = B7;
				b = *s++;
			}
			xd++;
		}
		yd++;
	}
}

// draw mono image, with background
void DrawMonoImgBg(const u8* img, int x, int y, int w, int h, int wsb, u8 col, u8 colbg)
{
	int xd;
	int yd = y;
	int ys;
	int xs;
	int m;
	const u8* s;
	u8 b;
	for (ys = 0; ys < h; ys++)
	{
		s = &img[ys*wsb];
		xd = x;
		m = B7;
		b = *s++;
		for (xs = 0; xs < w; xs++)
		{
			DrawPoint(xd, yd, ((b & m) == 0) ? col : colbg);
			m >>= 1;
			if (m == 0)
			{
				m = B7;
				b = *s++;
			}
			xd++;
		}
		yd++;
	}
}

// invert mono image
void DrawMonoImgInv(const u8* img, int x, int y, int w, int h, int wsb)
{
	int xd;
	int yd = y;
	int ys;
	int xs;
	int m;
	const u8* s;
	u8 b;
	for (ys = 0; ys < h; ys++)
	{
		s = &img[ys*wsb];
		xd = x;
		m = B7;
		b = *s++;
		for (xs = 0; xs < w; xs++)
		{
			if ((b & m) == 0) DrawPointInv(xd, yd);
			m >>= 1;
			if (m == 0)
			{
				m = B7;
				b = *s++;
			}
			xd++;
		}
		yd++;
	}
}

// draw color image
void DrawImg(const u8* img, int xd, int yd, int xs, int ys, int w, int h, int wsb)
{
	// limit x
	if (xs < 0) { w += xs; xd -= xs; xs = 0; }
	if (xd < 0) { w += xd; xs -= xd; xd = 0; }

	// limit w
	if (xs + w > wsb) w = wsb - xs;
	if (xd + w > WIDTH) w = WIDTH - xd;
	if (w <= 0) return;

	// limit y
	if (ys < 0) { h += ys; yd -= ys; ys = 0; }
	if (yd < 0) { h += yd; ys -= yd; yd = 0; }

	// limit h
	if (yd + h > HEIGHT) h = HEIGHT - yd;
	if (h <= 0) return;

	// draw image
	u8* d = &FrameBuf[xd + yd*WIDTHBYTE];
	const u8* s = &img[xs + ys*wsb];
	int wd = WIDTH;
	int ws = wsb;
	for (; h > 0; h--)
	{
		memcpy(d, s, w);
		d += wd;
		s += ws;
	}
}

// draw color image with transparent key color
void BlitImg(const u8* img, int xd, int yd, int xs, int ys, int w, int h, int wsb, u8 col)
{
	// limit x
	if (xs < 0) { w += xs; xd -= xs; xs = 0; }
	if (xd < 0) { w += xd; xs -= xd; xd = 0; }

	// limit w
	if (xs + w > wsb) w = wsb - xs;
	if (xd + w > WIDTH) w = WIDTH - xd;
	if (w <= 0) return;

	// limit y
	if (ys < 0) { h += ys; yd -= ys; ys = 0; }
	if (yd < 0) { h += yd; ys -= yd; yd = 0; }

	// limit h
	if (yd + h > HEIGHT) h = HEIGHT - yd;
	if (h <= 0) return;

	// draw image
	u8* d = &FrameBuf[xd + yd*WIDTHBYTE];
	const u8* s = &img[xs + ys*wsb];
	int wd = WIDTH - w;
	int ws = wsb - w;
	int i;
	u8 c;
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			c = *s;
			if (c != col) *d = c;
			d++;
			s++;
		}
		d += wd;
		s += ws;
	}
}

#endif // USE_DRAW

#if USE_PRINT		// 1=use text printing functions

// ----------------------------------------------------------------------------
//                               Printing
// ----------------------------------------------------------------------------

// scroll screen
void PrintScroll()
{
	PrintRow--;
	memmove(&FrameBuf[0], &FrameBuf[WIDTHBYTE*8], FRAMESIZE-WIDTHBYTE*8);
	memset(&FrameBuf[FRAMESIZE-WIDTHBYTE*8], COL_BLACK, WIDTHBYTE*8);
}

// print character at text position
void PrintCharAt(char ch, int x, int y)
{
	DrawCharBg(ch, x << 3, y << 3, PrintCol, COL_BLACK);
}

// print ASCIIZ text at text position
void PrintTextAt(const char* text, int x, int y)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		PrintCharAt(ch, x, y);
		x++;
	}
}

// print text with length at text position
void PrintTextLenAt(const char* text, int len, int x, int y)
{
	char ch;
	for (; len > 0; len--)
	{
		ch = *text++;
		PrintCharAt(ch, x, y);
		x++;
	}
}

// reset print position
void PrintHome(void)
{
	PrintPos = 0;
	PrintRow = 0;
	PrintCol = COL_WHITE;
}

// print new line
void PrintNewLine(void)
{
	PrintPos = 0; // reset position
	PrintRow++; // increase row

	// scroll screen
	if (PrintRow >= TEXTHEIGHT)
	{
		PrintScroll(); // scroll screen
		PrintRow = TEXTHEIGHT-1;
	}
}

// print character RAW at print position, without control characters
void PrintCharRaw(char ch)
{
	// print character
	PrintCharAt(ch, PrintPos, PrintRow);

	// move right
	PrintPos++; // increase position

	// new line
	if (PrintPos >= TEXTWIDTH) PrintNewLine();
}

// print character RAW repeat, without control characters
void PrintCharRawRep(char ch, int num)
{
	while (num-- > 0) PrintCharRaw(ch);
}

// print space character
void PrintSpc(void)
{
	PrintCharRaw(' ');
}

// print space character repeat
void PrintSpcRep(int num)
{
	while (num-- > 0) PrintSpc();
}

// print space to position
void PrintSpcTo(int pos)
{
	while (PrintPos < pos) PrintSpc();
}

// print character at print position
//   Control characters:
//     0x01 '\1' ^A ... set not-inverted text
//     0x02 '\2' ^B ... set inverted text (shift character code by 0x80)
//     0x07 '\a' ^G ... (bell) move cursor 1 position right (no print)
//     0x08 '\b' ^H ... (back space) move cursor 1 position left (no print)
//     0x09 '\t' ^I ... (tabulator) move cursor to next 8-character position, print spaces
//     0x0A '\n' ^J ... (new line) move cursor to start of next row
//     0x0B '\v' ^K ... (vertical tabulator) move cursor to start of previous row
//     0x0C '\f' ^L ... (form feed) clear screen, reset cursor position and set default color
//     0x0D '\r' ^M ... (carriage return) move cursor to start of current row
//     0x10 '\20' ^P ... set gray text color (COL_GRAY)
//     0x11 '\21' ^Q ... set blue text color (COL_BLUE)
//     0x12 '\22' ^R ... set green text color (COL_GREEN)
//     0x13 '\23' ^S ... set cyan text color (COL_CYAN)
//     0x14 '\24' ^T ... set red text color (COL_RED)
//     0x15 '\25' ^U ... set magenta text color (COL_MAGENTA)
//     0x16 '\26' ^V ... set yellow text color (COL_YELLOW)
//     0x17 '\27' ^W ... set white text color (COL_WHITE)
void PrintChar(char ch)
{
	switch ((u8)ch)
	{
	// printable character
	default:
		PrintCharAt(ch, PrintPos, PrintRow);

  // ! continue case 0x07 (move right)

	// '\a' ... (bell) move cursor 1 position right (no print)
	case 0x07:
		PrintPos++; // increase position
		if (PrintPos < TEXTWIDTH) break;

  // ! continue case 0x0A (new line)

	// '\n' ... (new line) move cursor to start of next row
	case 0x0A:
		PrintNewLine();
		break;

	// 'x01' ... set not-inverted text
	case 0x01:
		PrintInv = 0;
		break;

	// 'x02' ... set inverted text
	case 0x02:
		PrintInv = 0x80;
		break;

	// '\b' ... (back space) move cursor 1 position left (no print)
	case 0x08:
		if (PrintPos > 0) PrintPos--;
		break;

	// '\t' ... (tabulator) move cursor to next 8-character position, print spaces
	case 0x09:
		do PrintChar(' '); while ((PrintPos & 7) != 0); // print spaces
		break;		

	// '\v' ... (vertical tabulator) move cursor to start of previous row
	case 0x0B:
		if (PrintRow > 0) PrintRow--; // decrease row
		PrintPos = 0;
		break;

	// '\f' ... (form feed) clear screen, reset cursor position and set default color
	case 0x0C:
		DrawClear();
		break;

	// '\r' ... (carriage return) move cursor to start of current row
	case 0x0D:
		PrintPos = 0;
		break;
	// 'x10' ... set gray text color (COL_GRAY)
	case 0x10:
		PrintCol = COL_GRAY;
		break;

	// 'x11' ... set blue text color (COL_BLUE)
	case 0x11:
		PrintCol = COL_BLUE;
		break;

	// 'x12' ... set green text color (COL_GREEN)
	case 0x12:
		PrintCol = COL_GREEN;
		break;

	// 'x13' ... set cyan text color (COL_CYAN)
	case 0x13:
		PrintCol = COL_CYAN;
		break;

	// 'x14' ... set red text color (COL_RED)
	case 0x14:
		PrintCol = COL_RED;
		break;

	// 'x15' ... set magenta text color (COL_MAGENTA)
	case 0x15:
		PrintCol = COL_MAGENTA;
		break;

	// 'x16' ... set yellow text color (COL_YELLOW)
	case 0x16:
		PrintCol = COL_YELLOW;
		break;

	// 'x17' ... set white text color (COL_WHITE)
	case 0x17:
		PrintCol = COL_WHITE;
		break;
	}
}

/*
PrintText test sample:

	PrintText(
		"\f"					// clear screen (resolution: 40 characters x 15 rows)

		"- invisible row -\n"			// 0: this row will be scrolled-up

		"Hello world?\b!\n"			// 1: first visible print, overprint "?" -> "!"

		"First! row\r" "Second\n"		// 2: overprint "First! row" -> "Second row"

		"\3normal \4height \5width \6sized\n"	// 3: double-height row with font of all 4 sizes
		"\3font   \a\a\a\a\a\a\a\5font\3\n"	// 4: second half of previous double-height row

		"non-invert \2invert\1 non-invert\n"	// 5: inverted text

		"01234567012345670123"			// 6: print ruler 20 characters and move to next row (row is 40 columns width)
		"\t123\t12\tnext row\n"			// 7, 8: move to tab positions and move to next row after all

		"\20 normal \21 blue \22 green \23 cyan \n" // 9: print colors
		"\24 red \25 magenta \26 yellow \27 white \n" // 10:

		"\2"
		"\20 normal \21 blue \22 green \23 cyan \n" // 11: print colors inverted
		"\24 red \25 magenta \26 yellow \27 white \n" // 12:

		"\1\20"					// back to default state
		"%d 0x%08x ALL OK\n"			// 13: print numbers 12345678, 0x12345678

		"\nscrolling up",			// 14: this command scrolls screen up

		12345678, 0x12345678
	);
*/

// print character repeat
void PrintCharRep(char ch, int num)
{
	while (num-- > 0) PrintChar(ch);
}

// print ASCIIZ text
void PrintText(const char* text)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		PrintChar(ch);
	}
}

// print ASCIIZ text RAW, without control characters
void PrintTextRaw(const char* text)
{
	char ch;
	while ((ch = *text++) != 0)
	{
		PrintCharRaw(ch);
	}
}

// print text with length
void PrintTextLen(const char* text, int len)
{
	char ch;
	for (; len > 0; len--)
	{
		ch = *text++;
		PrintChar(ch);
	}
}

// print text with length RAW, without control characters
void PrintTextLenRaw(const char* text, int len)
{
	char ch;
	for (; len > 0; len--)
	{
		ch = *text++;
		PrintCharRaw(ch);
	}
}

#endif // USE_PRINT

#endif // USE_DRAW || USE_PRINT || USE_DISP
