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
The dsp3D is a powerful 3D rendering engine designed for ARM Cortex-M processor
based devices. It takes full advantage of the CMSIS DSP library to provide a
fast operation. A device equipped also with a hardware floating point unit is
recommended.

Four rendering methods are available:
	- Gouraud rendering
	- Flat surface rendering
	- Wireframe rendering
	- Point rendering

After rendering, the screen need to be drawn. Use dsp3D_present for this

It is easily extensible to support different face colors and maybe textures.
Tested on ST's 32F746-Discovery board and ST's 32F769-Discovery board
******************************************************************************/

#ifndef __DSP3D_ENGINE__
#define __DSP3D_ENGINE__

#include "dsp3D_LL.h"

#include <stdint.h>
#include "arm_math.h"

/* defines -----------------------------------------------------------------*/
	
/**
 * @brief      Set the camera position in the world
 *
 * @param[in]  x     x axis coordinate
 * @param[in]  y     y axis coordinate
 * @param[in]  z     z axis coordinate
 */
void dsp3D_setCameraPosition(float32_t x, float32_t y, float32_t z);

/**
 * @brief      Set the point to which the camera is pointing to
 *
 * @param[in]  x     x axis coordinate
 * @param[in]  y     y axis coordinate
 * @param[in]  z     z axis coordinate
 */
void dsp3D_setCameraTarget(float32_t x, float32_t y, float32_t z);

/**
 * @brief      Set the position of the mesh within the world
 *
 * @param[in]  x     x axis coordinate
 * @param[in]  y     y axis coordinate
 * @param[in]  z     z axis coordinate
 */
void dsp3D_setMeshPosition(float32_t x, float32_t y, float32_t z);	

/**
 * @brief      Set the rotation of the mesh
 *
 * @param[in]  yaw    The yaw
 * @param[in]  pitch  The pitch
 * @param[in]  roll   The roll
 */
void dsp3D_setMeshRotation(float32_t yaw, float32_t pitch, float32_t roll);

/**
 * @brief      Set the position of the light within the world
 * 			   N.B. is omnidirectional
 *
 * @param[in]  x     x axis coordinate
 * @param[in]  y     y axis coordinate
 * @param[in]  z     z axis coordinate
 */
void dsp3D_setLightPosition(float32_t x, float32_t y, float32_t z);	

/**
 * @brief      Init the dsp3D engine
 */
void dsp3D_init(void);

/**
 * @brief      Render the mesh with Gouraud shading
 *
 * @param      meshPointer  The mesh pointer
 */
void dsp3D_renderGouraud(const float32_t * dsp3dModel);

/**
 * @brief      Render the mesh as flat surfaces
 *
 * @param      meshPointer  The mesh pointer
 */
void dsp3D_renderFlat(const float32_t * dsp3dModel);

/**
 * @brief      Render the mesh as wireframe
 *
 * @param      meshPointer  The mesh pointer
 */
void dsp3D_renderWireframe(const float32_t * dsp3dModel);

/**
 * @brief      Render only the vertices of the mesh
 *
 * @param      meshPointer  The mesh pointer
 */
void dsp3D_renderPoints(const float32_t * dsp3dModel);

/**
 * @brief      Plot the rendered mesh on the current screen
 */
void dsp3D_present(void);

/**
 * @brief      Enable or disable backface culling
 *
 * @param      state  The desired state
 */
void dsp3D_setBackFaceCulling(uint32_t state);

/**
 * @brief      Render a single vertex within the world
 *
 * @param[in]  x     x axis coordinate
 * @param[in]  y     y axis coordinate
 * @param[in]  z     z axis coordinate
 */
void dsp3D_renderPoint(float32_t x, float32_t y, float32_t z);

#define LCD_COLOR_BLACK	COLORPIP_BLACK
#define LCD_COLOR_WHITE	COLORPIP_WHITE

extern float32_t meshRotation[3];
extern float32_t meshPosition[3];

#endif
