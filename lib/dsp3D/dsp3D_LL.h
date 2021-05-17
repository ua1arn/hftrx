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
	
#ifndef __DSP3D_LL_ENGINE__
#define __DSP3D_LL_ENGINE__

#include "hardware.h"
#include "src/display/display.h"
#include "display2.h"
#include <float.h>
#include "arm_math.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 			(DIM_X - 16)
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT			(DIM_Y)
#endif

#define color32_t uint32_t

/**
 * @brief      Initialize low level
 */
void dsp3D_LL_init(void);

/**
 * @brief      Draw a point at position (x, y) with color
 *
 * @param[in]  x      x position
 * @param[in]  y      y position
 * @param[in]  color  The color
 */
void dsp3D_LL_drawPoint(uint32_t x, uint32_t y, uint32_t color);

/**
 * @brief      Clear the screen with the specified color
 *
 * @param[in]  color  The color
 */
void dsp3D_LL_clearScreen(uint32_t color);

/**
 * @brief      Alternate screens (double buffering)
 */
void dsp3D_LL_switchScreen(void);

/**
 * @brief      Write a float to position within depth buffer
 *
 * @param[in]  pos    The position
 * @param[in]  value  The value
 */
void dsp3D_LL_writeToDepthBuffer(int32_t x, int32_t y, float32_t value);

/**
 * @brief      Read a float from position of depth buffer
 *
 * @param[in]  pos   The position
 *
 * @return     The value
 */
float32_t dsp3D_LL_readFromDepthBuffer(int32_t x, int32_t y);

/**
 * @brief      Clear the depth buffer
 */
void dsp3D_LL_clearDepthBuffer(void);

#endif
