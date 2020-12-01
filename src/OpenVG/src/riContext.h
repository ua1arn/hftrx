#ifndef __RICONTEXT_H
#define __RICONTEXT_H

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
 * \brief	VGContext class. Used for storing OpenVG state.
 * \note	
 *//*-------------------------------------------------------------------*/

#ifndef OPENVG_H
#include "openvg.h"
#endif

#ifndef __RIDEFS_H
#include "riDefs.h"
#endif

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIIMAGE_H
#include "riImage.h"
#endif

#ifndef __RIPATH_H
#include "riPath.h"
#endif

#ifndef __RIFONT_H
#include "riFont.h"
#endif

#ifndef __RIARRAY_H
#include "riArray.h"
#endif

//==============================================================================================

namespace OpenVGRI
{

class VGContext;

/*-------------------------------------------------------------------*//*!
* \brief	A list of resources (Images, Paths, or Paints) shared by a
*			set of contexts.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

template <class Resource> class ResourceManager
{
public:
	ResourceManager() :
		m_referenceCount(0),
		m_resources()
	{
	}

	~ResourceManager()
	{
		RI_ASSERT(m_referenceCount == 0);
		RI_ASSERT(m_resources.size() == 0);
	}

	void			addReference()
	{
		m_referenceCount++;
	}

	int				removeReference()
	{
		m_referenceCount--;
		RI_ASSERT(m_referenceCount >= 0);
		return m_referenceCount;
	}

	void			addResource(Resource* resource, VGContext* context)
	{
		Entry r;
		r.resource = resource;
		r.context = context;
		m_resources.push_back(r);	//throws bad_alloc
		resource->addReference();
	}

	void			removeResource(Resource* resource)
	{
		if(!resource->removeReference())
			RI_DELETE(resource);

		int i=0;
		bool found = false;
		for(;i<m_resources.size();i++)
		{
			if(m_resources[i].resource == resource)
			{
				found = true;
				break;
			}
		}
		RI_ASSERT(found);

		for(;i<m_resources.size()-1;i++)
		{
			m_resources[i] = m_resources[i+1];
		}
		m_resources.resize(m_resources.size()-1);
	}

	bool			isValid(Resource* resource)
	{
		for(int i=0;i<m_resources.size();i++)
		{
			if(m_resources[i].resource == resource)
				return true;
		}
		return false;
	}

	Resource*		getFirstResource(VGContext* context)
	{
		for(int i=0;i<m_resources.size();i++)
		{
			if(m_resources[i].context == context)
				return m_resources[i].resource;
		}
		return NULL;
	}

private:
	ResourceManager(const ResourceManager&);
	ResourceManager operator=(const ResourceManager&);

	struct Entry
	{
		Resource*	resource;
		VGContext*	context;
	};

	int				m_referenceCount;
	Array<Entry>	m_resources;
};

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class VGContext
{
public:
	VGContext(VGContext* shareContext);	//throws bad_alloc
	~VGContext();

    void            setDefaultDrawable(Drawable* drawable); //called from EGL
    Drawable*       getCurrentDrawable()        { return m_eglDrawable; }

	bool			isValidImage(VGImage image);
	bool			isValidPath(VGPath path);
	bool			isValidPaint(VGPaint paint);
	bool			isValidFont(VGFont font);
	bool			isValidMaskLayer(VGMaskLayer layer);

	void			releasePaint(VGbitfield paintModes);

	void			setError(VGErrorCode error)		{ if(m_error == VG_NO_ERROR) m_error = error; }

	// Mode settings
	VGMatrixMode					m_matrixMode;
	VGFillRule						m_fillRule;
	VGImageQuality					m_imageQuality;
	VGRenderingQuality				m_renderingQuality;
	VGBlendMode						m_blendMode;
	VGImageMode						m_imageMode;
	
	// Scissor rectangles
	Array<Rectangle>				m_scissor;

	// Stroke parameters
	RIfloat							m_strokeLineWidth;
	RIfloat							m_inputStrokeLineWidth;
	VGCapStyle						m_strokeCapStyle;
	VGJoinStyle						m_strokeJoinStyle;
	RIfloat							m_strokeMiterLimit;
	RIfloat							m_inputStrokeMiterLimit;
	Array<RIfloat>					m_strokeDashPattern;
	Array<RIfloat>					m_inputStrokeDashPattern;
	RIfloat							m_strokeDashPhase;
	RIfloat							m_inputStrokeDashPhase;
	VGboolean						m_strokeDashPhaseReset;

	// Edge fill color for vgConvolve and pattern paint
	Color							m_tileFillColor;
	Color							m_inputTileFillColor;

	// Color for vgClear
	Color							m_clearColor;
	Color							m_inputClearColor;

    Vector2                         m_glyphOrigin;
    Vector2                         m_inputGlyphOrigin;

	VGboolean						m_masking;
	VGboolean						m_scissoring;

	VGPixelLayout					m_pixelLayout;

	VGboolean						m_filterFormatLinear;
	VGboolean						m_filterFormatPremultiplied;
	VGbitfield						m_filterChannelMask;

	// Matrices
	Matrix3x3						m_pathUserToSurface;
	Matrix3x3						m_imageUserToSurface;
	Matrix3x3						m_glyphUserToSurface;
	Matrix3x3						m_fillPaintToUser;
	Matrix3x3						m_strokePaintToUser;

	VGPaint							m_fillPaint;
	VGPaint							m_strokePaint;

    VGboolean                       m_colorTransform;
    RIfloat                         m_colorTransformValues[8];
    RIfloat                         m_inputColorTransformValues[8];

	VGErrorCode						m_error;

	ResourceManager<Image>*			m_imageManager;
	ResourceManager<Path>*			m_pathManager;
	ResourceManager<Paint>*			m_paintManager;
	ResourceManager<Font>*			m_fontManager;
	ResourceManager<Surface>*		m_maskLayerManager;
private:
	Drawable*                       m_eglDrawable;

	VGContext(const VGContext&);			//!< Not allowed.
	void operator=(const VGContext&);		//!< Not allowed.
};

//==============================================================================================

}	//namespace OpenVGRI

//==============================================================================================

#endif /* __RICONTEXT_H */
