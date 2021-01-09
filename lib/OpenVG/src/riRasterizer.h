#ifndef __RIRASTERIZER_H
#define __RIRASTERIZER_H

/*------------------------------------------------------------------------
 *
 * OpenVG 1.1 Reference Implementation
 * -----------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Materials. 
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
 * \file
 * \brief	Rasterizer class.
 * \note	
 *//*-------------------------------------------------------------------*/

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIARRAY_H
#include "riArray.h"
#endif

#ifndef __RIPIXELPIPE_H
#include "riPixelPipe.h"
#endif

//=======================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Scalar and vector data types used by the rasterizer.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

typedef RIfloat RScalar;	//change this if you want to have different precision for rasterizer scalars and RIfloat

struct RVector2
{
	RI_INLINE RVector2()							{ }
	RI_INLINE RVector2(const Vector2& v)			{ x = v.x; y = v.y; }
	RI_INLINE RVector2(RIfloat vx, RIfloat vy)		{ x = vx; y = vy; }
	RI_INLINE void set(RIfloat vx, RIfloat vy)		{ x = vx; y = vy; }
	RScalar		x;
	RScalar		y;
};

/*-------------------------------------------------------------------*//*!
* \brief	Converts a set of edges to coverage values for each pixel and
*			calls PixelPipe::pixelPipe for painting a pixel.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class Rasterizer
{
public:
	Rasterizer();	//throws bad_alloc
	~Rasterizer();

    void        setup(int vpx, int vpy, int vpwidth, int vpheight, VGFillRule fillRule, const PixelPipe* pixelPipe, RIuint32* covBuffer);
	void		setScissor(const Array<Rectangle>& scissors);	//throws bad_alloc

	void		clear();
	void		addEdge(const Vector2& v0, const Vector2& v1);	//throws bad_alloc

	int         setupSamplingPattern(VGRenderingQuality renderingQuality, int numFSAASamples);
	void		fill();	//throws bad_alloc

    void        getBBox(int& sx, int& sy, int& ex, int& ey) const       { sx = m_covMinx; sy = m_covMiny; ex = m_covMaxx; ey = m_covMaxy; }
private:
	Rasterizer(const Rasterizer&);						//!< Not allowed.
	const Rasterizer& operator=(const Rasterizer&);		//!< Not allowed.

	struct ScissorEdge
	{
		ScissorEdge() : x(0), miny(0), maxy(0), direction(0) {}
		bool operator<(const ScissorEdge& e) const	{ return x < e.x; }
		int			x;
		int			miny;
		int			maxy;
		int			direction;		//1 start, -1 end
	};

	struct Edge
	{
		Edge() : v0(), v1(), direction(1) {}
		bool operator<(const Edge& e) const	{ return v0.y < e.v0.y; }
		RVector2	v0;
		RVector2	v1;
		int			direction;
	};

	struct ActiveEdge
	{
		ActiveEdge() : v0(), v1(), direction(0), minx(0.0f), maxx(0.0f), n(), cnst(0.0f) {}
		bool operator<(const ActiveEdge& e) const	{ return minx < e.minx; }
		RVector2	v0;
		RVector2	v1;
		int			direction;		//-1 down, 1 up
		RScalar		minx;			//for the current scanline
		RScalar		maxx;			//for the current scanline
		RVector2	n;
		RScalar		cnst;
	};

	struct Sample
	{
		Sample() : x(0.0f), y(0.0f), weight(0.0f) {}
		RScalar		x;
		RScalar		y;
		RScalar		weight;
	};

    void                addBBox(const Vector2& v);

	Array<Edge>				m_edges;
	Array<ScissorEdge>		m_scissorEdges;
	bool					m_scissor;

	Sample				m_samples[RI_MAX_SAMPLES];
	int					m_numSamples;
	int					m_numFSAASamples;
	RScalar				m_sumWeights;
	RScalar				m_sampleRadius;

    Vector2             m_edgeMin;
    Vector2             m_edgeMax;
    int                 m_covMinx;
    int                 m_covMiny;
    int                 m_covMaxx;
    int                 m_covMaxy;
    int                 m_vpx;
    int                 m_vpy;
    int                 m_vpwidth;
    int                 m_vpheight;
    VGFillRule          m_fillRule;
    const PixelPipe*    m_pixelPipe;
    RIuint32*           m_covBuffer;
};

//=======================================================================

}	//namespace OpenVGRI

//=======================================================================

#endif /* __RIRASTERIZER_H */
