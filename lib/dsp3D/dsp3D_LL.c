/******************************************************************************
Copyright (c) 2016 - Fabio Angeletti
e-mail: fabio.angeletti89@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dsp3D nor the names of its contributors may be used
  to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This work was inspired by the excellent tutorial series by David Rousset:
"learning how to write a 3D soft engine from scratch in C#, TypeScript
or JavaScript" - available on David's website https://www.davrous.com

******************************************************************************/

/******************************************************************************
The dsp3D_ll provides low level interface to the hardware.
******************************************************************************/

/******************************************************************************
Customize thw following to your needs
******************************************************************************/
#include "dsp3D_LL.h"

static uint32_t LCD_ActiveLayer = 1;
static uint32_t maxX = 0;
static uint32_t maxY = 0;
static uint32_t minX = SCREEN_WIDTH - 1;
static uint32_t minY = SCREEN_HEIGHT - 1;

void dsp3D_LL_init(void)
{

}

void dsp3D_LL_drawPoint(uint32_t x, uint32_t y, color32_t color)
{
	if(x < minX)
		minX = x;
	if(x > maxX)
		maxX = x;
	if(y < minY)
		minY = y;
	if(y > maxY)
		maxY = y;

	if (x >= (DIM_X - 16))
		return;
	if (y >= (DIM_Y - 0))
		return;
	// YOUR IMPLEMENTATION
	* colmain_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, x, y) = color; // dest address
}

static float32_t dbuf0 [DIM_Y][DIM_X];

void dsp3D_LL_clearScreen(color32_t color)
{
	// YOUR IMPLEMENTATION
	colmain_fillrect(colmain_fb_draw(), DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, color);
}

void dsp3D_LL_switchScreen(void)
{
	// YOUR IMPLEMENTATION
	display_nextfb();
}

void dsp3D_LL_writeToDepthBuffer(int32_t x, int32_t y, float32_t value)
{
	// YOUR IMPLEMENTATION
	if (x >= (DIM_X - 16))
		return;
	if (y >= (DIM_Y - 0))
		return;
	dbuf0 [y][x] = value;
}

float32_t dsp3D_LL_readFromDepthBuffer(int32_t x, int32_t y)
{
	return dbuf0 [y][x]; // YOUR IMPLEMENTATION
}

void dsp3D_LL_clearDepthBuffer(void)
{
	uint32_t x, y;

	for(x = minX; x <= maxX; x++)
		for(y = minY; y <= maxY; y++)
			dsp3D_LL_writeToDepthBuffer(x, y, FLT_MAX);

	maxX = 0;
	maxY = 0;
	minX = SCREEN_WIDTH - 1;
	minY = SCREEN_HEIGHT - 1;
}
