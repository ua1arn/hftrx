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
 * \brief	Implementation of polygon rasterizer.
 * \note	
 *//*-------------------------------------------------------------------*/

#include "riRasterizer.h"

//==============================================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Rasterizer constructor.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Rasterizer::Rasterizer() :
	m_edges(),
	m_scissorEdges(),
	m_scissor(false),
	m_samples(),
	m_numSamples(0),
	m_numFSAASamples(0),
	m_sumWeights(0.0f),
	m_sampleRadius(0.0f),
    m_vpx(0),
    m_vpy(0),
    m_vpwidth(0),
    m_vpheight(0),
    m_fillRule(VG_EVEN_ODD),
    m_pixelPipe(NULL),
    m_covBuffer(NULL)
{}

/*-------------------------------------------------------------------*//*!
* \brief	Rasterizer destructor.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Rasterizer::~Rasterizer()
{
}

/*-------------------------------------------------------------------*//*!
* \brief	Removes all appended edges.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Rasterizer::clear()
{
	m_edges.clear();
    m_edgeMin.set(RI_FLOAT_MAX, RI_FLOAT_MAX);
    m_edgeMax.set(-RI_FLOAT_MAX, -RI_FLOAT_MAX);
}

/*-------------------------------------------------------------------*//*!
* \brief	Appends an edge to the rasterizer.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Rasterizer::addBBox(const Vector2& v)
{
    if(v.x < m_edgeMin.x) m_edgeMin.x = v.x;
    if(v.y < m_edgeMin.y) m_edgeMin.y = v.y;
    if(v.x > m_edgeMax.x) m_edgeMax.x = v.x;
    if(v.y > m_edgeMax.y) m_edgeMax.y = v.y;
}

void Rasterizer::addEdge(const Vector2& v0, const Vector2& v1)
{
	if( m_edges.size() >= RI_MAX_EDGES )
		throw std::bad_alloc();	//throw an out of memory error if there are too many edges

	if(v0.y == v1.y)
		return;	//skip horizontal edges (they don't affect rasterization since we scan horizontally)

	Edge e;
	if(v0.y < v1.y)
	{	//edge is going upward
		e.v0 = v0;
		e.v1 = v1;
		e.direction = 1;
	}
	else
	{	//edge is going downward
		e.v0 = v1;
		e.v1 = v0;
		e.direction = -1;
	}

    addBBox(v0);
    addBBox(v1);

	m_edges.push_back(e);	//throws bad_alloc
}

/*-------------------------------------------------------------------*//*!
* \brief	Set up rasterizer
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Rasterizer::setup(int vpx, int vpy, int vpwidth, int vpheight, VGFillRule fillRule, const PixelPipe* pixelPipe, unsigned int* covBuffer)
{
	RI_ASSERT(vpwidth >= 0 && vpheight >= 0);
	RI_ASSERT(vpx + vpwidth >= vpx && vpy + vpheight >= vpy);
	RI_ASSERT(fillRule == VG_EVEN_ODD || fillRule == VG_NON_ZERO);
    RI_ASSERT(pixelPipe || covBuffer);
    m_vpx = vpx;
    m_vpy = vpy;
    m_vpwidth = vpwidth;
    m_vpheight = vpheight;
    m_fillRule = fillRule;
    m_pixelPipe = pixelPipe;
    m_covBuffer = covBuffer;
    m_covMinx = vpx+vpwidth;
    m_covMiny = vpy+vpheight;
    m_covMaxx = vpx;
    m_covMaxy = vpy;
}

/*-------------------------------------------------------------------*//*!
* \brief	Sets scissor rectangles.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Rasterizer::setScissor(const Array<Rectangle>& scissors)
{
	m_scissor = true;
	try
	{
		m_scissorEdges.clear();
		for(int i=0;i<scissors.size();i++)
		{
			if(scissors[i].width > 0 && scissors[i].height > 0)
			{
				ScissorEdge e;
				e.miny = scissors[i].y;
				e.maxy = RI_INT_ADDSATURATE(scissors[i].y, scissors[i].height);

				e.x = scissors[i].x;
				e.direction = 1;
				m_scissorEdges.push_back(e);	//throws bad_alloc
				e.x = RI_INT_ADDSATURATE(scissors[i].x, scissors[i].width);
				e.direction = -1;
				m_scissorEdges.push_back(e);	//throws bad_alloc
			}
		}
	}
	catch(std::bad_alloc)
	{
		m_scissorEdges.clear();
		throw;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	Returns a radical inverse of a given integer for Hammersley
*			point set.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static double radicalInverseBase2(unsigned int i)
{
	if( i == 0 )
		return 0.0;
	double p = 0.0;
	double f = 0.5;
	double ff = f;
	for(unsigned int j=0;j<32;j++)
	{
		if( i & (1<<j) )
			p += f;
		f *= ff;
	}
	return p;
}

/*-------------------------------------------------------------------*//*!
* \brief	Calls PixelPipe::pixelPipe for each pixel with coverage greater
*			than zero.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

int Rasterizer::setupSamplingPattern(VGRenderingQuality renderingQuality, int numFSAASamples)
{
	RI_ASSERT(renderingQuality == VG_RENDERING_QUALITY_NONANTIALIASED ||
			  renderingQuality == VG_RENDERING_QUALITY_FASTER ||
			  renderingQuality == VG_RENDERING_QUALITY_BETTER);
	RI_ASSERT(numFSAASamples > 0 && numFSAASamples <= RI_MAX_SAMPLES);

	//make a sampling pattern
	m_sumWeights = 0.0f;
	m_sampleRadius = 0.0f;		//max offset of the sampling points from a pixel center
	m_numFSAASamples = numFSAASamples;
	if(numFSAASamples == 1)
	{
		if(renderingQuality == VG_RENDERING_QUALITY_NONANTIALIASED)
		{
			m_numSamples = 1;
			m_samples[0].x = 0.0f;
			m_samples[0].y = 0.0f;
			m_samples[0].weight = 1.0f;
			m_sampleRadius = 0.0f;
			m_sumWeights = 1.0f;
		}
		else if(renderingQuality == VG_RENDERING_QUALITY_FASTER)
		{	//box filter of diameter 1.0f, 8-queen sampling pattern
			m_numSamples = 8;
			m_samples[0].x = 3;
			m_samples[1].x = 7;
			m_samples[2].x = 0;
			m_samples[3].x = 2;
			m_samples[4].x = 5;
			m_samples[5].x = 1;
			m_samples[6].x = 6;
			m_samples[7].x = 4;
			for(int i=0;i<m_numSamples;i++)
			{
				m_samples[i].x = (m_samples[i].x + 0.5f) / (RScalar)m_numSamples - 0.5f;
				m_samples[i].y = ((RScalar)i + 0.5f) / (RScalar)m_numSamples - 0.5f;
				m_samples[i].weight = 1.0f / (RScalar)m_numSamples;
				m_sumWeights += m_samples[i].weight;
			}
			m_sampleRadius = 0.5f;
		}
		else
		{
			RI_ASSERT(renderingQuality == VG_RENDERING_QUALITY_BETTER);
			m_numSamples = RI_MAX_SAMPLES;
			m_sampleRadius = 0.75f;
			for(int i=0;i<m_numSamples;i++)
			{	//Gaussian filter, implemented using Hammersley point set for sample point locations
				RScalar x = (RScalar)radicalInverseBase2(i);
				RScalar y = ((RScalar)i + 0.5f) / (RScalar)m_numSamples;
				RI_ASSERT(x >= 0.0f && x < 1.0f);
				RI_ASSERT(y >= 0.0f && y < 1.0f);

				//map unit square to unit circle
				RScalar r = (RScalar)sqrt(x) * m_sampleRadius;
				x = r * (RScalar)sin(y*2.0f*PI);
				y = r * (RScalar)cos(y*2.0f*PI);
				m_samples[i].weight = (RScalar)exp(-0.5f * RI_SQR(r/m_sampleRadius));

				RI_ASSERT(x >= -1.5f && x <= 1.5f && y >= -1.5f && y <= 1.5f);	//the specification restricts the filter radius to be less than or equal to 1.5
				
				m_samples[i].x = x;
				m_samples[i].y = y;
				m_sumWeights += m_samples[i].weight;
			}
		}
	}
	else
	{	//box filter
        m_numSamples = numFSAASamples;
        RI_ASSERT(numFSAASamples >= 1 && numFSAASamples <= 32);	//sample mask is a 32-bit uint => can't support more than 32 samples
		//use Hammersley point set as a sampling pattern
        for(int i=0;i<m_numSamples;i++)
        {
            m_samples[i].x = (RScalar)radicalInverseBase2(i) + 1.0f / (RScalar)(m_numSamples<<1) - 0.5f;
            m_samples[i].y = ((RScalar)i + 0.5f) / (RScalar)m_numSamples - 0.5f;
            m_samples[i].weight = 1.0f;
			RI_ASSERT(m_samples[i].x > -0.5f && m_samples[i].x < 0.5f);
			RI_ASSERT(m_samples[i].y > -0.5f && m_samples[i].y < 0.5f);
        }
        m_sumWeights = (RScalar)m_numSamples;
        m_sampleRadius = 0.5f;
	}
    return m_numSamples;
}

/*-------------------------------------------------------------------*//*!
* \brief	Calls PixelPipe::pixelPipe for each pixel with coverage greater
*			than zero.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Rasterizer::fill()
{
	if(m_scissor && !m_scissorEdges.size())
		return;	//scissoring is on, but there are no scissor rectangles => nothing is visible

	//proceed scanline by scanline
	//keep track of edges that can intersect the pixel filters of the current scanline (Active Edge Table)
	//until all pixels of the scanline have been processed
	//  for all sampling points of the current pixel
	//    determine the winding number using edge functions
	//    add filter weight to coverage
	//  divide coverage by the number of samples
	//  determine a run of pixels with constant coverage
	//  call fill callback for each pixel of the run

	int fillRuleMask = 1;
	if(m_fillRule == VG_NON_ZERO)
		fillRuleMask = -1;

    int bbminx = (int)floor(m_edgeMin.x);
    int bbminy = (int)floor(m_edgeMin.y);
    int bbmaxx = (int)floor(m_edgeMax.x)+1;
    int bbmaxy = (int)floor(m_edgeMax.y)+1;
    int sx = RI_INT_MAX(m_vpx, bbminx);
    int ex = RI_INT_MIN(m_vpx+m_vpwidth, bbmaxx);
    int sy = RI_INT_MAX(m_vpy, bbminy);
    int ey = RI_INT_MIN(m_vpy+m_vpheight, bbmaxy);
    if(sx < m_covMinx) m_covMinx = sx;
    if(sy < m_covMiny) m_covMiny = sy;
    if(ex > m_covMaxx) m_covMaxx = ex;
    if(ey > m_covMaxy) m_covMaxy = ey;

	//fill the screen
	Array<ActiveEdge> aet;
	Array<ScissorEdge> scissorAet;
	for(int j=sy;j<ey;j++)
	{
		//gather scissor edges intersecting this scanline
		scissorAet.clear();
		if( m_scissor )
		{
			for(int e=0;e<m_scissorEdges.size();e++)
			{
				const ScissorEdge& se = m_scissorEdges[e];
				if(j >= se.miny && j < se.maxy)
					scissorAet.push_back(m_scissorEdges[e]);	//throws bad_alloc
			}
			if(!scissorAet.size())
				continue;	//scissoring is on, but there are no scissor rectangles on this scanline
		}

		//simple AET: scan through all the edges and pick the ones intersecting this scanline
		aet.clear();
		for(int e=0;e<m_edges.size();e++)
		{
			RScalar cminy = (RScalar)j - m_sampleRadius + 0.5f;
			RScalar cmaxy = (RScalar)j + m_sampleRadius + 0.5f;
			const Edge& ed = m_edges[e];
			RI_ASSERT(ed.v0.y <= ed.v1.y);	//horizontal edges should have been dropped already

			ActiveEdge ae;
			ae.v0 = ed.v0;
			ae.v1 = ed.v1;
			ae.direction = ed.direction;

			if(cmaxy >= ae.v0.y && cminy < ae.v1.y)
			{
				ae.n.set(ae.v0.y - ae.v1.y, ae.v1.x - ae.v0.x);	//edge normal
				ae.cnst = ae.v0.x * ae.n.x + ae.v0.y * ae.n.y;	//distance of v0 from the origin along the edge normal
				
				//compute edge min and max x-coordinates for this scanline
				Vector2 vd(ae.v1.x - ae.v0.x, ae.v1.y - ae.v0.y);
				RScalar wl = 1.0f / vd.y;
				RScalar sx = ae.v0.x + vd.x * (cminy - ae.v0.y) * wl;
				RScalar ex = ae.v0.x + vd.x * (cmaxy - ae.v0.y) * wl;
				RScalar bminx = RI_MIN(ae.v0.x, ae.v1.x);
				RScalar bmaxx = RI_MAX(ae.v0.x, ae.v1.x);
				sx = RI_CLAMP(sx, bminx, bmaxx);
				ex = RI_CLAMP(ex, bminx, bmaxx);
				ae.minx = RI_MIN(sx,ex);
				ae.maxx = RI_MAX(sx,ex);
				aet.push_back(ae);	//throws bad_alloc
			}
		}
		if(!aet.size())
			continue;	//no edges on the whole scanline, skip it

		//sort AET by edge minx
		aet.sort();
		
		//sort scissor AET by edge x
		scissorAet.sort();

		//fill the scanline
		int scissorWinding = m_scissor ? 0 : 1;	//if scissoring is off, winding is always 1
		int scissorIndex = 0;
		int aes = 0;
		int aen = 0;
		for(int i=sx;i<ex;)
		{
			Vector2 pc(i + 0.5f, j + 0.5f);		//pixel center
			
			//find edges that intersect or are to the left of the pixel antialiasing filter
			while(aes < aet.size() && pc.x + m_sampleRadius >= aet[aes].minx)
				aes++;
			//edges [0,aes[ may have an effect on winding, and need to be evaluated while sampling

			//compute coverage
			RScalar coverage = 0.0f;
			unsigned int sampleMask = 0;
			for(int s=0;s<m_numSamples;s++)
			{
				Vector2 sp = pc;	//sampling point
				sp.x += m_samples[s].x;
				sp.y += m_samples[s].y;

				//compute winding number by evaluating the edge functions of edges to the left of the sampling point
				int winding = 0;
				for(int e=0;e<aes;e++)
				{
					if(sp.y >= aet[e].v0.y && sp.y < aet[e].v1.y)
					{	//evaluate edge function to determine on which side of the edge the sampling point lies
						RScalar side = sp.x * aet[e].n.x + sp.y * aet[e].n.y - aet[e].cnst;
						if(side <= 0.0f)	//implicit tie breaking: a sampling point on an opening edge is in, on a closing edge it's out
						{
                            winding += aet[e].direction;
						}
					}
				}
                if(winding & fillRuleMask)
				{
					coverage += m_samples[s].weight;
					sampleMask |= (unsigned int)(1<<s);
				}
			}

			//constant coverage optimization:
			//scan AET from left to right and skip all the edges that are completely to the left of the pixel filter.
			//since AET is sorted by minx, the edge we stop at is the leftmost of the edges we haven't passed yet.
			//if that edge is to the right of this pixel, coverage is constant between this pixel and the start of the edge.
			while(aen < aet.size() && aet[aen].maxx < pc.x - m_sampleRadius - 0.01f)	//0.01 is a safety region to prevent too aggressive optimization due to numerical inaccuracy
				aen++;

			int endSpan = m_vpx + m_vpwidth;	//endSpan is the first pixel NOT part of the span
			if(aen < aet.size())
				endSpan = RI_INT_MAX(i+1, RI_INT_MIN(endSpan, (int)ceil(aet[aen].minx - m_sampleRadius - 0.5f)));

			coverage /= m_sumWeights;
			RI_ASSERT(coverage >= 0.0f && coverage <= 1.0f);

			//fill a run of pixels with constant coverage
			if(sampleMask)
			{
				for(;i<endSpan;i++)
				{
					//update scissor winding number
					while(scissorIndex < scissorAet.size() && scissorAet[scissorIndex].x <= i)
						scissorWinding += scissorAet[scissorIndex++].direction;
					RI_ASSERT(scissorWinding >= 0);

					if(scissorWinding)
                    {
                        if(m_covBuffer)
                            m_covBuffer[j*m_vpwidth+i] |= (RIuint32)sampleMask;
                        else
                            m_pixelPipe->pixelPipe(i, j, coverage, sampleMask);
                    }
				}
			}
			i = endSpan;
		}
	}
}

//=======================================================================

}	//namespace OpenVGRI
