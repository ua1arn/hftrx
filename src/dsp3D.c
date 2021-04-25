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

After rendering, the screen need to be drawn. Use dsp3D_present

It is easily extensible to support different face colors and maybe textures.
Tested on ST's 32F746-Discovery board and ST's 32F769-Discovery board
******************************************************************************/

#include "dsp3D.h"

#define ASSEMBLE_ARGB(A,R,G,B) ((A) << 24 | (R) << 16 | (G) << 8 | (B))
#define SCREEN_ASPECT_RATIO		(SCREEN_WIDTH / (float32_t) SCREEN_HEIGHT)

#define ABS(x)   		((x) > 0 ? (x) : -(x))
#define MIN(x, y)		((x) > (y) ? (y) : (x))
#define MAX(x, y)		((x) < (y) ? (y) : (x))
#define ROUND(x) 		((x)>=0?(int32_t)((x)+(float32_t)0.5):(int32_t)((x)-(float32_t)0.5))

float32_t cameraPosition[3] = 			{0, 0, 10};
float32_t cameraTarget[3] = 			{0, 0, 0};

float32_t meshRotation[3] = 			{0, 0, 0};
float32_t meshPosition[3] = 			{0, 0, 0};

float32_t lightPosition[3] = 			{0, 10, 10};

float32_t unitX[3] = 					{1, 0, 0};
float32_t unitY[3] = 					{0, 1, 0};
float32_t unitZ[3] = 					{0, 0, 1};

float32_t matrix_view[16] = 			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_projection[16] = 		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_rotation[16] = 		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_translation[16] = 		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_world[16] = 			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_worldView[16] = 		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_transform[16] = 		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t matrix_transformhelper[16] = 	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t lastRenderingType, culling;

arm_matrix_instance_f32 instance_matrix_view;
arm_matrix_instance_f32 instance_matrix_rotation;
arm_matrix_instance_f32 instance_matrix_translation;
arm_matrix_instance_f32 instance_matrix_transform;
arm_matrix_instance_f32 instance_matrix_transformhelper;
arm_matrix_instance_f32 instance_matrix_projection;
arm_matrix_instance_f32 instance_matrix_world;
arm_matrix_instance_f32 instance_matrix_worldView;

float32_t dsp3D_clamp(float32_t value);
float32_t dsp3D_interpolate(float32_t min, float32_t max, float32_t gradient);
float32_t dsp3D_computeNDotL(float32_t *vertex, float32_t *normal, float32_t *lightPosition);
void dsp3D_vectorNormalTransform(float32_t *v, float32_t *m, float32_t *result);
void dsp3D_vectorCrossProduct(float32_t *a, float32_t *b, float32_t *v);
void dsp3D_vectorNorm(const float32_t *a, float32_t *v);
void dsp3D_transformVertex(const float32_t *v, const float32_t *m, float32_t tv [3]);
void dsp3D_generateLookAtMatrixLH(float32_t *cameraPosition, float32_t *cameraTarget, float32_t *cameraUpVector, float32_t *m);
void dsp3D_generatePerspectiveFovMatrixLH(float32_t fov, float32_t aspect, float32_t znear, float32_t zfar, float32_t* m);
void dsp3D_generateRotationMatrix(float32_t yaw, float32_t pitch, float32_t roll, float32_t *m);
void dsp3D_generateTranslationMatrix(float32_t xaxis, float32_t yaxis, float32_t zaxis, float32_t *m);
void dsp3D_generateScalingMatrix(float32_t xaxis, float32_t yaxis, float32_t zaxis, float32_t *m);

void dsp3D_projectVertex(const float32_t *coord, float32_t *m);
void dsp3D_projectVertexComplete(float32_t *vertex, float32_t *vertexNormal, float32_t *m);
void dsp3D_drawPoint(int32_t x, int32_t y, color32_t color);
void dsp3D_drawPointDepthBuffer(int32_t x, int32_t y, float32_t z, color32_t color);
void dsp3D_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, color32_t color);
void dsp3D_processScanLineGouraud(int32_t y, float32_t *ndotl, float32_t* pa, float32_t* pb, float32_t* pc, float32_t* pd, color32_t color);
void dsp3D_processScanLineFlat(int32_t y, float32_t ndotl, const float32_t* pa, const float32_t* pb, float32_t* pc, float32_t* pd, color32_t color);
void dsp3D_swapArray(float32_t *a, float32_t *b);
void dsp3D_drawFaceFlat(float32_t *p1, float32_t *p2, float32_t *p3, color32_t color);
void dsp3D_drawFaceGouraud(float32_t *p1, float32_t *p2, float32_t *p3, color32_t color);
void dsp3D_calculateFaceNormal(const float32_t *a, const float32_t *b, const float32_t *c, float32_t *m, float32_t *n);

void dsp3D_generateMatrices(void);

void dsp3D_setCameraPosition(float32_t x, float32_t y, float32_t z)
{
	cameraPosition[0] = x;
	cameraPosition[1] = y;
	cameraPosition[2] = z;

	dsp3D_generateLookAtMatrixLH(cameraPosition, cameraTarget, unitY, matrix_view);
}

void dsp3D_setCameraTarget(float32_t x, float32_t y, float32_t z)
{
	cameraTarget[0] = x;
	cameraTarget[1] = y;
	cameraTarget[2] = z;

	dsp3D_generateLookAtMatrixLH(cameraPosition, cameraTarget, unitY, matrix_view);
}

void dsp3D_setMeshPosition(float32_t x, float32_t y, float32_t z)
{
	meshPosition[0] = x;
	meshPosition[1] = y;
	meshPosition[2] = z;

	dsp3D_generateTranslationMatrix(meshPosition[0], meshPosition[1], meshPosition[2], matrix_translation);
}

void dsp3D_setMeshRotation(float32_t yaw, float32_t pitch, float32_t roll)
{
	meshRotation[0] = yaw;
	meshRotation[1] = pitch;
	meshRotation[2] = roll;

	dsp3D_generateRotationMatrix(meshRotation[0], meshRotation[1], meshRotation[2], matrix_rotation);
}

void dsp3D_setLightPosition(float32_t x, float32_t y, float32_t z)
{
	lightPosition[0] = x;
	lightPosition[1] = y;
	lightPosition[2] = z;
}

float32_t dsp3D_clamp(float32_t value)
{
	return MAX(0, MIN(value, 1));
}

float32_t dsp3D_interpolate(float32_t minv, float32_t maxv, float32_t gradient)
{
	return (minv + (maxv - minv) * dsp3D_clamp(gradient));
}

float32_t dsp3D_computeNDotL(float32_t *vertex, float32_t *normal, float32_t *lightPosition)
{
	float32_t lightDirection[3];
	float32_t normalNorm[3];
	float32_t lightDirectionNorm[3];
	float32_t dotProd;

	arm_sub_f32(lightPosition, vertex, lightDirection, 3);
	dsp3D_vectorNorm(normal, normalNorm);
	dsp3D_vectorNorm(lightDirection, lightDirectionNorm);
	arm_dot_prod_f32(normalNorm, lightDirectionNorm, 3, &dotProd);

	// the minus sign should not be here!
	return MAX(0.0, -dotProd);
}

void dsp3D_vectorNormalTransform(float32_t *v, float32_t *m, float32_t *result)
{
	float32_t vectorNormal [3];

	dsp3D_vectorNorm(v, vectorNormal);
	dsp3D_transformVertex(vectorNormal, m, result);
}

void dsp3D_vectorCrossProduct(float32_t *a, float32_t *b, float32_t *v)
{
	v[0] = a[1] * b[2] - a[2] * b[1];
	v[1] = a[2] * b[0] - a[0] * b[2];
	v[2] = a[0] * b[1] - a[1] * b[0];
}

void dsp3D_vectorNorm(const float32_t *a, float32_t *v)
{
	float32_t norm;
	arm_status st;

	st = arm_sqrt_f32(a[0] * a[0] + a[1] * a[1] + a[2] * a[2], &norm);

	if (st == ARM_MATH_SUCCESS && norm != 0)
	{
		v[0] = a[0] / norm;
		v[1] = a[1] / norm;
		v[2] = a[2] / norm;
	}
	else
	{
		v[0] = 1;
		v[1] = 1;
		v[2] = 1;
	}
}

// OPTIMIZE IT!
void dsp3D_transformVertex(const float32_t *v, const float32_t *m, float32_t tv [3])
{
	float32_t w;
	
	tv[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + m[12];
	tv[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + m[13];
	tv[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + m[14];

	w = (float32_t) 1 / (v[0]*m[3] + v[1]*m[7] + v[2]*m[11] + m[15]);

	tv[0] *= w;
	tv[1] *= w;
	tv[2] *= w;
}

void dsp3D_generateLookAtMatrixLH(float32_t *cameraPosition, float32_t *cameraTarget, float32_t *cameraUpVector, float32_t *m)
{
	float32_t xaxis[3];
	float32_t yaxis[3];
	float32_t zaxis[3];

	float32_t deltaVect[3];
	float32_t crossProd[3];

	float32_t dotProd;

	arm_sub_f32(cameraTarget, cameraPosition, deltaVect, 3);
	dsp3D_vectorNorm(deltaVect, zaxis);

	dsp3D_vectorCrossProduct(cameraUpVector, zaxis, crossProd);
	dsp3D_vectorNorm(crossProd, xaxis);

	dsp3D_vectorCrossProduct(zaxis, xaxis, yaxis);

	m[0] = xaxis[0];
	m[1] = yaxis[0];
	m[2] = zaxis[0];
	m[3] = 0;
	m[4] = xaxis[1];
	m[5] = yaxis[1];
	m[6] = zaxis[1];
	m[7] = 0;
	m[8] = xaxis[2];
	m[9] = yaxis[2];
	m[10] = zaxis[2];
	m[11] = 0;
	arm_dot_prod_f32(xaxis, cameraPosition, 3, &dotProd);
	m[12] = -dotProd;
	arm_dot_prod_f32(yaxis, cameraPosition, 3, &dotProd);
	m[13] = -dotProd;
	arm_dot_prod_f32(zaxis, cameraPosition, 3, &dotProd);
	m[14] = -dotProd;
	m[15] = 1;
}

void dsp3D_generatePerspectiveFovMatrixLH(float32_t fov, float32_t aspect, float32_t znear, float32_t zfar, float32_t* m)
{
	int32_t x, y;
	float32_t yScale, q;
	
	yScale = 1.0 / tan(fov * 0.5);
	q = zfar / (zfar - znear);

	for(x = 0; x < 4; x++)
  		for(y = 0; y < 4; y++)
   			m[x * 4 + y] = 0;

	m[0] = yScale / aspect;
	m[5] = yScale;
	m[10] = q;
	m[11] = 1.0;
	m[14] = - q * znear;
}

void dsp3D_generateRotationMatrix(float32_t yaw, float32_t pitch, float32_t roll, float32_t *m)
{
	float32_t s_y = arm_sin_f32(yaw * 0.5);
	float32_t c_y = arm_cos_f32(yaw * 0.5);
	float32_t s_p = arm_sin_f32(pitch * 0.5);
	float32_t c_p = arm_cos_f32(pitch * 0.5);
	float32_t s_r = arm_sin_f32(roll * 0.5);
	float32_t c_r = arm_cos_f32(roll * 0.5);

	float32_t x = c_y * s_p * c_r + s_y * c_p * s_r;
	float32_t y = s_y * c_p * c_r - c_y * s_p * s_r;
	float32_t z = c_y * c_p * s_r - s_y * s_p * c_r;
	float32_t w = c_y * c_p * c_r + s_y * s_p * s_r;
	
	m[0] = 1.0 - (2.0 * (y * y + z * z));
	m[1] = 2.0 * (x * y + z * w);
	m[2] = 2.0 * (z * x - y * w);
	m[3] = 0;
	m[4] = 2.0 * (x * y - z * w);
	m[5] = 1.0 - (2.0 * (z * z + x * x));
	m[6] = 2.0 * (y * z + x * w);
	m[7] = 0;
	m[8] = 2.0 * (z * x + y * w);
	m[9] = 2.0 * (y * z - x * w);
	m[10] = 1.0 - (2.0 * (y * y + x * x));
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void dsp3D_generateTranslationMatrix(float32_t xaxis, float32_t yaxis, float32_t zaxis, float32_t *m)
{
	int32_t x, y;
	
	for(x = 0; x < 4; x++)
		for(y = 0; y < 4; y++)
			if(x == y)
				m[x * 4 + y] = 1;
			else
				m[x * 4 + y] = 0;
	
	m[12] = xaxis;
	m[13] = yaxis;
	m[14] = zaxis;
}

void dsp3D_generateScalingMatrix(float32_t xaxis, float32_t yaxis, float32_t zaxis, float32_t *m)
{
	int32_t x,y;
	
	for(x = 0; x < 4; x++)
		for(y = 0; y < 4; y++)
			m[x * 4 + y] = 0;
	
	m[0] = xaxis;
	m[5] = yaxis;
	m[10] = zaxis;
	m[15] = 1;
}

void dsp3D_projectVertex(const float32_t *vertex, float32_t *m)
{
	float32_t coordinates[4];
	
	dsp3D_transformVertex(vertex, matrix_transform, coordinates);
	
	m[0] = coordinates[0] * (float32_t)SCREEN_WIDTH + (float32_t)SCREEN_WIDTH / 2;
	m[1] = - coordinates[1] * (float32_t)SCREEN_HEIGHT + (float32_t)SCREEN_HEIGHT / 2;
	m[2] = coordinates[2];
}

void dsp3D_projectVertexComplete(float32_t *vertex, float32_t *vertexNormal, float32_t *m)
{
	float32_t coordinates[4];
	float32_t pointWorld[4];
	float32_t pointNormalWorld[4];
	
	dsp3D_transformVertex(vertex, matrix_transform, coordinates);
	dsp3D_transformVertex(vertex, matrix_world, pointWorld);
	dsp3D_transformVertex(vertexNormal, matrix_world, pointNormalWorld);
	
	m[0] = coordinates[0] * (float32_t)SCREEN_WIDTH + (float32_t)SCREEN_WIDTH / 2;
	m[1] = -coordinates[1] * (float32_t)SCREEN_HEIGHT + (float32_t)SCREEN_HEIGHT / 2;
	m[2] = coordinates[2];
	m[3] = pointWorld[0];
	m[4] = pointWorld[1];
	m[5] = pointWorld[2];
	m[6] = pointNormalWorld[0];
	m[7] = pointNormalWorld[1];
	m[8] = pointNormalWorld[2];
}

void dsp3D_drawPoint(int32_t x, int32_t y, color32_t color)
{
	if((x > -1) && (x < SCREEN_WIDTH) && (y > -1) && (y < SCREEN_HEIGHT))
		dsp3D_LL_drawPoint(x, y, color);
}

void dsp3D_drawPointDepthBuffer(int32_t x, int32_t y, float32_t z, color32_t color)
{
	if((x > -1) && (x < SCREEN_WIDTH) && (y > -1) && (y < SCREEN_HEIGHT))
	{
		if(dsp3D_LL_readFromDepthBuffer(x, y) < z)
			return;

		dsp3D_LL_writeToDepthBuffer(x, y, z);
		dsp3D_LL_drawPoint(x, y, color);
	}
}

void dsp3D_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, color32_t color)
{
	int32_t dx = ABS(x1 - x0);
	int32_t sx = x0 < x1 ? 1 : -1;
	int32_t dy = ABS(y1 - y0);
	int32_t sy = y0 < y1 ? 1 : -1;
	int32_t err = (dx > dy ? dx : -dy) / 2;
	int32_t e2;

	for (;;)
	{
		dsp3D_drawPoint(x0, y0, color);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = err;

		if (e2 > -dx)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void dsp3D_processScanLineGouraud(int32_t y, float32_t *ndotl, float32_t* pa, float32_t* pb, float32_t* pc, float32_t* pd, color32_t color)
{
    int32_t x, sx, ex;
    uint8_t a, r, g, b;
    float32_t z, z1, z2; 
    float32_t ndl, snl, enl;
    float32_t gradient, gradient1, gradient2;

    if(pa[1] != pb[1])
    	gradient1 = (y - pa[1]) / (pb[1] - pa[1]);
    else
    	gradient1 = 1;

    if(pc[1] != pd[1])
    	gradient2 = (y - pc[1]) / (pd[1] - pc[1]);
    else
    	gradient2 = 1;
            
    sx = dsp3D_interpolate(pa[0], pb[0], gradient1);
    ex = dsp3D_interpolate(pc[0], pd[0], gradient2);

    z1 = dsp3D_interpolate(pa[2], pb[2], gradient1);
    z2 = dsp3D_interpolate(pc[2], pd[2], gradient2);

    snl = dsp3D_interpolate(ndotl[0], ndotl[1], gradient1);
    enl = dsp3D_interpolate(ndotl[2], ndotl[3], gradient2);

    for (x = sx; x < ex; x++)
    {
    	gradient = (float32_t)(x - sx) / (float32_t)(ex - sx);

    	z = dsp3D_interpolate(z1, z2, gradient);
    	ndl = dsp3D_interpolate(snl, enl, gradient);

    	a = (color >> 24);
    	r = (color >> 16);
    	g = (color >> 8);
    	b = (color);
    	
    	r = (uint8_t)((float32_t)r * ndl);
    	g = (uint8_t)((float32_t)g * ndl);
    	b = (uint8_t)((float32_t)b * ndl);

    	dsp3D_drawPointDepthBuffer(x, y, z, ASSEMBLE_ARGB(a, r, g, b));
    }
}

void dsp3D_processScanLineFlat(int32_t y, float32_t ndotl, const float32_t* pa, const float32_t* pb, float32_t* pc, float32_t* pd, color32_t color)
{
    int32_t x, sx, ex;
    uint8_t a, r, g, b;
    float32_t z1, z2;
    float32_t gradient1, gradient2;

    if(pa[1] != pb[1])
    	gradient1 = (y - pa[1]) / (pb[1] - pa[1]);
    else
    	gradient1 = 1;

    if(pc[1] != pd[1])
    	gradient2 = (y - pc[1]) / (pd[1] - pc[1]);
    else
    	gradient2 = 1;
            
    sx = dsp3D_interpolate(pa[0], pb[0], gradient1);
    ex = dsp3D_interpolate(pc[0], pd[0], gradient2);

    z1 = dsp3D_interpolate(pa[2], pb[2], gradient1);
    z2 = dsp3D_interpolate(pc[2], pd[2], gradient2);

    for (x = sx; x < ex; x++)
    {
    	float32_t gradient, z;
    	gradient = (float32_t)(x - sx) / (float32_t)(ex - sx);

    	z = dsp3D_interpolate(z1, z2, gradient);

    	a = (color >> 24);
    	r = (color >> 16);
    	g = (color >> 8);
    	b = (color);

    	r = (float32_t)r * ndotl;
    	g = (float32_t)g * ndotl;
    	b = (float32_t)b * ndotl;

    	dsp3D_drawPointDepthBuffer(x, y, z, ASSEMBLE_ARGB(a, r, g, b));
    }
}

void dsp3D_swapArray(float32_t *a, float32_t *b)
{
	float32_t temp[9];
	uint32_t index;

	for(index = 0; index < 9; index++)
	{
		temp[index] = a[index];
		a[index] = b[index];
		b[index] = temp[index];
	}
}

void dsp3D_drawFaceGouraud(float32_t *v1, float32_t *v2, float32_t *v3, color32_t color)
{
	float32_t ndotl[4];
	float32_t nl1, nl2, nl3;
	float32_t dP1P2, dP1P3;
	int32_t y;

	if (v1[1] > v2[1])
    	dsp3D_swapArray(v1, v2);

    if (v2[1] > v3[1])
    {
    	dsp3D_swapArray(v2, v3);
    }

    if (v1[1] > v2[1])
    {
    	dsp3D_swapArray(v1, v2);
    }
	
	nl1 = dsp3D_computeNDotL(&v1[3], &v1[6], lightPosition);
	nl2 = dsp3D_computeNDotL(&v2[3], &v2[6], lightPosition);
	nl3 = dsp3D_computeNDotL(&v3[3], &v3[6], lightPosition);

    if ((v2[1] - v1[1]) > 0)
        dP1P2 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
    else
        dP1P2 = 0;

    if ((v3[1] - v1[1]) > 0.0)
        dP1P3 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
    else
        dP1P3 = 0;

    if (dP1P2 > dP1P3)
    {
        for (y = v1[1]; y <= v3[1]; y++)
            if (y < v2[1])
            {
            	ndotl[0] = nl1;
            	ndotl[1] = nl3;
            	ndotl[2] = nl1;
            	ndotl[3] = nl2;
                dsp3D_processScanLineGouraud(y, ndotl, v1, v3, v1, v2, color);
            }
            else
            {
            	ndotl[0] = nl1;
            	ndotl[1] = nl3;
            	ndotl[2] = nl2;
            	ndotl[3] = nl3;
                dsp3D_processScanLineGouraud(y, ndotl, v1, v3, v2, v3, color);
            }
    }
    else
    {
        for (y = v1[1]; y <= v3[1]; y++)
            if (y < v2[1])
            {
            	ndotl[0] = nl1;
            	ndotl[1] = nl2;
            	ndotl[2] = nl1;
            	ndotl[3] = nl3;
                dsp3D_processScanLineGouraud(y, ndotl, v1, v2, v1, v3, color);
            }
            else
            {
            	ndotl[0] = nl2;
            	ndotl[1] = nl3;
            	ndotl[2] = nl1;
            	ndotl[3] = nl3;
                dsp3D_processScanLineGouraud(y, ndotl, v2, v3, v1, v3, color);
            }
    }
}

void dsp3D_drawFaceFlat(float32_t *v1, float32_t *v2, float32_t *v3, color32_t color)
{
	float32_t ndotl;
	float32_t vnFace[3];
	float32_t centerPoint[3];
	float32_t dP1P2, dP1P3;
	int32_t y;

	if (v1[1] > v2[1])
    	dsp3D_swapArray(v1, v2);

    if (v2[1] > v3[1])
    	dsp3D_swapArray(v2, v3);

    if (v1[1] > v2[1])
    	dsp3D_swapArray(v1, v2);

    vnFace[0] = (v1[6] + v2[6] + v3[6]) / 3.0;
	vnFace[1] = (v1[7] + v2[7] + v3[7]) / 3.0;
	vnFace[2] = (v1[8] + v2[8] + v3[8]) / 3.0;

	centerPoint[0] = (v1[3] + v2[3] + v3[3]) / 3.0;
	centerPoint[1] = (v1[4] + v2[4] + v3[4]) / 3.0;
	centerPoint[2] = (v1[5] + v2[5] + v3[5]) / 3.0;

	ndotl = dsp3D_computeNDotL(centerPoint, vnFace, lightPosition);

    if ((v2[1] - v1[1]) > 0.0)
        dP1P2 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
    else
        dP1P2 = 0.0;

    if ((v3[1] - v1[1]) > 0.0)
        dP1P3 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
    else
        dP1P3 = 0.0;

    if (dP1P2 > dP1P3)
        for (y = v1[1]; y <= v3[1]; y++)
            if (y < v2[1])
                dsp3D_processScanLineFlat(y, ndotl, v1, v3, v1, v2, color);
            else
                dsp3D_processScanLineFlat(y, ndotl, v1, v3, v2, v3, color);
    else
        for (y = v1[1]; y <= v3[1]; y++)
            if (y < v2[1])
                dsp3D_processScanLineFlat(y, ndotl, v1, v2, v1, v3, color);
            else
                dsp3D_processScanLineFlat(y, ndotl, v2, v3, v1, v3, color);
}

void dsp3D_calculateFaceNormal(const float32_t *a, const float32_t *b, const float32_t *c, float32_t *m, float32_t *n)
{
	float32_t h[3];
	float32_t hn[3];

	h[0] = (a[0] + b[0] + c[0]) / 3;
	h[1] = (a[1] + b[1] + c[1]) / 3;
	h[2] = (a[2] + b[2] + c[2]) / 3;

	dsp3D_vectorNorm(h, hn);

	n[0] = hn[0] * m[0] + hn[1] * m[4] + hn[2] * m[8];
	n[1] = hn[0] * m[1] + hn[1] * m[5] + hn[2] * m[9];
	n[2] = hn[0] * m[2] + hn[1] * m[6] + hn[2] * m[10];

}

void dsp3D_init(void)
{
	dsp3D_LL_init();
	
	arm_mat_init_f32(& instance_matrix_view, 4, 4, matrix_view);
	arm_mat_init_f32(& instance_matrix_rotation, 4, 4, matrix_rotation);
	arm_mat_init_f32(& instance_matrix_translation, 4, 4, matrix_translation);
	arm_mat_init_f32(& instance_matrix_transform, 4, 4, matrix_transform);
	arm_mat_init_f32(& instance_matrix_transformhelper, 4, 4, matrix_transformhelper);
	arm_mat_init_f32(& instance_matrix_projection, 4, 4, matrix_projection);
	arm_mat_init_f32(& instance_matrix_world, 4, 4, matrix_world);
	arm_mat_init_f32(& instance_matrix_worldView, 4, 4, matrix_worldView);

	dsp3D_generateMatrices();

	lastRenderingType = 0;
	culling = 0;
}

void dsp3D_renderGouraud(const float32_t * dsp3dModel)
{
	uint32_t i;
	uint32_t a, b, c;
	uint32_t numVert, numFaces;
	uint8_t RGBr, RGBg, RGBb;
	
	float32_t vertex_transform_a[9];
	float32_t vertex_transform_b[9];
	float32_t vertex_transform_c[9];
	float32_t vertex_a[3];
	float32_t vertex_b[3];
	float32_t vertex_c[3];
	float32_t vertex_norm_a[3];
	float32_t vertex_norm_b[3];
	float32_t vertex_norm_c[3];
	float32_t face_norm[3];

	float32_t camToPointVector[3];
	float32_t faceNormalNormalized[3];
	float32_t camToPointVectorNormalized[3];
	float32_t cullingAngle;

	dsp3D_generateMatrices();

	numVert = dsp3dModel[0];
	numFaces = dsp3dModel[1];

	for(i = 0; i < numFaces; i++)
	{
		a = dsp3dModel[2 + numVert * 6 + i * 6 + 0];
		b = dsp3dModel[2 + numVert * 6 + i * 6 + 1];
		c = dsp3dModel[2 + numVert * 6 + i * 6 + 2];

		RGBr = dsp3dModel[2 + numVert * 6 + i * 6 + 3];
		RGBg = dsp3dModel[2 + numVert * 6 + i * 6 + 4];
		RGBb = dsp3dModel[2 + numVert * 6 + i * 6 + 5];

		vertex_a[0] = dsp3dModel[2 + a * 6 + 0];
		vertex_a[1] = dsp3dModel[2 + a * 6 + 1];
		vertex_a[2] = dsp3dModel[2 + a * 6 + 2];
		vertex_norm_a[0] = dsp3dModel[2 + a * 6 + 3];
		vertex_norm_a[1] = dsp3dModel[2 + a * 6 + 4];
		vertex_norm_a[2] = dsp3dModel[2 + a * 6 + 5];

		vertex_b[0] = dsp3dModel[2 + b * 6 + 0];
		vertex_b[1] = dsp3dModel[2 + b * 6 + 1];
		vertex_b[2] = dsp3dModel[2 + b * 6 + 2];
		vertex_norm_b[0] = dsp3dModel[2 + b * 6 + 3];
		vertex_norm_b[1] = dsp3dModel[2 + b * 6 + 4];
		vertex_norm_b[2] = dsp3dModel[2 + b * 6 + 5];

		vertex_c[0] = dsp3dModel[2 + c * 6 + 0];
		vertex_c[1] = dsp3dModel[2 + c * 6 + 1];
		vertex_c[2] = dsp3dModel[2 + c * 6 + 2];
		vertex_norm_c[0] = dsp3dModel[2 + c * 6 + 3];
		vertex_norm_c[1] = dsp3dModel[2 + c * 6 + 4];
		vertex_norm_c[2] = dsp3dModel[2 + c * 6 + 5];

		if(culling != 0)
		{
			dsp3D_calculateFaceNormal(vertex_norm_a, vertex_norm_b, vertex_norm_c, matrix_worldView, face_norm);
			arm_sub_f32(cameraPosition, vertex_a, camToPointVector, 3);
			dsp3D_vectorNorm(face_norm, faceNormalNormalized);
			dsp3D_vectorNorm(camToPointVector, camToPointVectorNormalized);
			arm_dot_prod_f32(faceNormalNormalized, camToPointVectorNormalized, 3, &cullingAngle);
		}

		if((culling == 0) || (cullingAngle < 0))
		{
			dsp3D_projectVertexComplete(vertex_a, vertex_norm_a, vertex_transform_a);
			dsp3D_projectVertexComplete(vertex_b, vertex_norm_b, vertex_transform_b);
			dsp3D_projectVertexComplete(vertex_c, vertex_norm_c, vertex_transform_c);

			dsp3D_drawFaceGouraud(vertex_transform_a, vertex_transform_b, vertex_transform_c, ASSEMBLE_ARGB(0xFF, RGBr, RGBg, RGBb));
		}
	}

	if(lastRenderingType < 2)
		lastRenderingType = 2;
}

void dsp3D_renderFlat(const float32_t * dsp3dModel)
{
	uint32_t i;
	uint32_t numVert, numFaces;
	
	float32_t vertex_transform_a[9];
	float32_t vertex_transform_b[9];
	float32_t vertex_transform_c[9];
	float32_t vertex_a[3];
	float32_t vertex_b[3];
	float32_t vertex_c[3];
	float32_t vertex_norm_a[3];
	float32_t vertex_norm_b[3];
	float32_t vertex_norm_c[3];

	float32_t camToPointVector[3];
	float32_t faceNormalNormalized[3];
	float32_t camToPointVectorNormalized[3];
	float32_t cullingAngle;

	dsp3D_generateMatrices();

	numVert = dsp3dModel[0];
	numFaces = dsp3dModel[1];

	for(i = 0; i < numFaces; i++)
	{
		uint32_t a, b, c;
		uint8_t RGBr, RGBg, RGBb;

		a = dsp3dModel[2 + numVert * 6 + i * 6 + 0];
		b = dsp3dModel[2 + numVert * 6 + i * 6 + 1];
		c = dsp3dModel[2 + numVert * 6 + i * 6 + 2];

		RGBr = dsp3dModel[2 + numVert * 6 + i * 6 + 3];
		RGBg = dsp3dModel[2 + numVert * 6 + i * 6 + 4];
		RGBb = dsp3dModel[2 + numVert * 6 + i * 6 + 5];

		vertex_a[0] = dsp3dModel[2 + a * 6 + 0];
		vertex_a[1] = dsp3dModel[2 + a * 6 + 1];
		vertex_a[2] = dsp3dModel[2 + a * 6 + 2];
		vertex_norm_a[0] = dsp3dModel[2 + a * 6 + 3];
		vertex_norm_a[1] = dsp3dModel[2 + a * 6 + 4];
		vertex_norm_a[2] = dsp3dModel[2 + a * 6 + 5];

		vertex_b[0] = dsp3dModel[2 + b * 6 + 0];
		vertex_b[1] = dsp3dModel[2 + b * 6 + 1];
		vertex_b[2] = dsp3dModel[2 + b * 6 + 2];
		vertex_norm_b[0] = dsp3dModel[2 + b * 6 + 3];
		vertex_norm_b[1] = dsp3dModel[2 + b * 6 + 4];
		vertex_norm_b[2] = dsp3dModel[2 + b * 6 + 5];

		vertex_c[0] = dsp3dModel[2 + c * 6 + 0];
		vertex_c[1] = dsp3dModel[2 + c * 6 + 1];
		vertex_c[2] = dsp3dModel[2 + c * 6 + 2];
		vertex_norm_c[0] = dsp3dModel[2 + c * 6 + 3];
		vertex_norm_c[1] = dsp3dModel[2 + c * 6 + 4];
		vertex_norm_c[2] = dsp3dModel[2 + c * 6 + 5];

		if (culling != 0)
		{
			float32_t face_norm[3];

			dsp3D_calculateFaceNormal(vertex_norm_a, vertex_norm_b, vertex_norm_c, matrix_worldView, face_norm);
			arm_sub_f32(cameraPosition, vertex_a, camToPointVector, 3);
			dsp3D_vectorNorm(face_norm, faceNormalNormalized);
			dsp3D_vectorNorm(camToPointVector, camToPointVectorNormalized);
			arm_dot_prod_f32(faceNormalNormalized, camToPointVectorNormalized, 3, &cullingAngle);
		}

		if((culling == 0) || (cullingAngle < 0))
		{
			dsp3D_projectVertexComplete(vertex_a, vertex_norm_a, vertex_transform_a);
			dsp3D_projectVertexComplete(vertex_b, vertex_norm_b, vertex_transform_b);
			dsp3D_projectVertexComplete(vertex_c, vertex_norm_c, vertex_transform_c);

			dsp3D_drawFaceFlat(vertex_transform_a, vertex_transform_b, vertex_transform_c, ASSEMBLE_ARGB(0xFF, RGBr, RGBg, RGBb));
		}
	}

	if(lastRenderingType < 2)
		lastRenderingType = 2;
}

void dsp3D_renderWireframe(const float32_t * dsp3dModel)
{
	uint32_t i;
	uint32_t numVert, numFaces;
	

	dsp3D_generateMatrices();

	numVert = dsp3dModel[0];
	numFaces = dsp3dModel[1];

	for(i = 0; i < numFaces; i++)
	{
		uint32_t a, b, c;
		uint8_t RGBr, RGBg, RGBb;
		float32_t coord_a[4];
		float32_t coord_b[4];
		float32_t coord_c[4];
		float32_t vertex_a[4];
		float32_t vertex_b[4];
		float32_t vertex_c[4];

		a = dsp3dModel[2 + numVert * 6 + i * 6 + 0];
		b = dsp3dModel[2 + numVert * 6 + i * 6 + 1];
		c = dsp3dModel[2 + numVert * 6 + i * 6 + 2];

		RGBr = dsp3dModel[2 + numVert * 6 + i * 6 + 3];
		RGBg = dsp3dModel[2 + numVert * 6 + i * 6 + 4];
		RGBb = dsp3dModel[2 + numVert * 6 + i * 6 + 5];

		vertex_a[0] = dsp3dModel[2 + a * 6 + 0];
		vertex_a[1] = dsp3dModel[2 + a * 6 + 1];
		vertex_a[2] = dsp3dModel[2 + a * 6 + 2];

		vertex_b[0] = dsp3dModel[2 + b * 6 + 0];
		vertex_b[1] = dsp3dModel[2 + b * 6 + 1];
		vertex_b[2] = dsp3dModel[2 + b * 6 + 2];

		vertex_c[0] = dsp3dModel[2 + c * 6 + 0];
		vertex_c[1] = dsp3dModel[2 + c * 6 + 1];
		vertex_c[2] = dsp3dModel[2 + c * 6 + 2];
		vertex_c[3] = 0;

		dsp3D_projectVertex(vertex_a, coord_a);
		dsp3D_projectVertex(vertex_b, coord_b);
		dsp3D_projectVertex(vertex_c, coord_c);

		dsp3D_drawLine(coord_a[0], coord_a[1], coord_b[0], coord_b[1], ASSEMBLE_ARGB(0xFF, RGBr, RGBg, RGBb));
		dsp3D_drawLine(coord_b[0], coord_b[1], coord_c[0], coord_c[1], ASSEMBLE_ARGB(0xFF, RGBr, RGBg, RGBb));
		dsp3D_drawLine(coord_c[0], coord_c[1], coord_a[0], coord_a[1], ASSEMBLE_ARGB(0xFF, RGBr, RGBg, RGBb));
	}

	if(lastRenderingType < 1)
		lastRenderingType = 1;
}

void dsp3D_renderPoints(const float32_t * dsp3dModel)
{
	unsigned i;
	unsigned numVert;
	
	float32_t coord[3];
	float32_t vertex[3];

	dsp3D_generateMatrices();
	
	numVert = dsp3dModel[0];

	for(i = 0; i < numVert; i++)
	{
		vertex[0] = dsp3dModel[2 + i * 6 + 0];
		vertex[1] = dsp3dModel[2 + i * 6 + 1];
		vertex[2] = dsp3dModel[2 + i * 6 + 2];

		dsp3D_projectVertex(vertex, coord);

		dsp3D_drawPoint((int32_t)coord[0], (int32_t)coord[1], LCD_COLOR_WHITE);
	}

	if(lastRenderingType < 1)
		lastRenderingType = 1;
}

void dsp3D_renderPoint(float32_t x, float32_t y, float32_t z)
{
	float32_t vector[3];
	float32_t coord[3];	// 2 -> 3

	dsp3D_generateMatrices();

	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	dsp3D_projectVertex(vector, coord);
	dsp3D_drawPoint(coord[0], coord[1], LCD_COLOR_WHITE);
	
	if(lastRenderingType < 1)
		lastRenderingType = 1;
}

void dsp3D_setBackFaceCulling(uint32_t state)
{
	culling = state;
}

__inline void dsp3D_present(void)
{
	dsp3D_LL_switchScreen();
	dsp3D_LL_clearScreen(LCD_COLOR_BLACK);
	
	if(lastRenderingType == 2)
		dsp3D_LL_clearDepthBuffer();

	lastRenderingType = 0;
}

void dsp3D_generateMatrices(void)
{
	dsp3D_generateLookAtMatrixLH(cameraPosition, cameraTarget, unitY, matrix_view);
	dsp3D_generatePerspectiveFovMatrixLH(0.78, SCREEN_ASPECT_RATIO, 0.01, 1.0, matrix_projection);
	dsp3D_generateRotationMatrix(meshRotation[0], meshRotation[1], meshRotation[2], matrix_rotation);
	dsp3D_generateTranslationMatrix(meshPosition[0], meshPosition[1], meshPosition[2], matrix_translation);

	arm_mat_mult_f32(&instance_matrix_rotation, &instance_matrix_translation, &instance_matrix_world);
	arm_mat_mult_f32(&instance_matrix_world, &instance_matrix_view, &instance_matrix_transformhelper);
	arm_mat_mult_f32(&instance_matrix_transformhelper, &instance_matrix_projection, &instance_matrix_transform);
	arm_mat_mult_f32(&instance_matrix_world, &instance_matrix_view, &instance_matrix_worldView);
}

