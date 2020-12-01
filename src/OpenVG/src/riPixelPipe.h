#ifndef __RIPIXELPIPE_H
#define __RIPIXELPIPE_H

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
 * \brief	Paint and PixelPipe classes.
 * \note	
 *//*-------------------------------------------------------------------*/

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIIMAGE_H
#include "riImage.h"
#endif

//=======================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Storage and operations for VGPaint.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class Paint
{
public:
	Paint();
	~Paint();
	void					addReference()							{ m_referenceCount++; }
	int						removeReference()						{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }

	struct GradientStop
	{
		GradientStop() : offset(0.0f), color(0.0f, 0.0f, 0.0f, 0.0f, Color::sRGBA) {}
		RIfloat		offset;
		Color		color;
	};

	VGPaintType				m_paintType;
	Color					m_paintColor;
	Color					m_inputPaintColor;
	VGColorRampSpreadMode	m_colorRampSpreadMode;
	Array<GradientStop>		m_colorRampStops;
	Array<GradientStop>		m_inputColorRampStops;
	VGboolean				m_colorRampPremultiplied;
	Vector2					m_inputLinearGradientPoint0;
	Vector2					m_inputLinearGradientPoint1;
	Vector2					m_inputRadialGradientCenter;
	Vector2					m_inputRadialGradientFocalPoint;
	RIfloat					m_inputRadialGradientRadius;
	Vector2					m_linearGradientPoint0;
	Vector2					m_linearGradientPoint1;
	Vector2					m_radialGradientCenter;
	Vector2					m_radialGradientFocalPoint;
	RIfloat					m_radialGradientRadius;
	VGTilingMode			m_patternTilingMode;
	Image*					m_pattern;
private:
	Paint(const Paint&);						//!< Not allowed.
	const Paint& operator=(const Paint&);		//!< Not allowed.

	int						m_referenceCount;
};

/*-------------------------------------------------------------------*//*!
* \brief	Encapsulates all information needed for painting a pixel.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/
	
class PixelPipe
{
public:
	PixelPipe();	//throws bad_alloc
	~PixelPipe();

	void	pixelPipe(int x, int y, RIfloat coverage, unsigned int sampleMask) const;	//rasterizer calls this function for each pixel

	void	setDrawable(Drawable* drawable);
	void	setBlendMode(VGBlendMode blendMode);
	void	setMask(bool masking);
	void	setImage(Image* image, VGImageMode imageMode);	//image = NULL disables drawImage functionality
	void	setSurfaceToPaintMatrix(const Matrix3x3& surfaceToPaintMatrix);
	void	setSurfaceToImageMatrix(const Matrix3x3& surfaceToImageMatrix);
	void	setImageQuality(VGImageQuality imageQuality);
	void	setTileFillColor(const Color& c);
	void	setPaint(const Paint* paint);
    void    setColorTransform(bool enable, RIfloat values[8]);

private:
	void	linearGradient(RIfloat& g, RIfloat& rho, RIfloat x, RIfloat y) const;
	void	radialGradient(RIfloat& g, RIfloat& rho, RIfloat x, RIfloat y) const;
	Color	integrateColorRamp(RIfloat gmin, RIfloat gmax) const;
	Color	colorRamp(RIfloat gradient, RIfloat rho) const;
	Color	blend(const Color& s, RIfloat ar, RIfloat ag, RIfloat ab, const Color& d, VGBlendMode blendMode) const;
    void    colorTransform(Color& c) const;

	PixelPipe(const PixelPipe&);						//!< Not allowed.
	const PixelPipe& operator=(const PixelPipe&);		//!< Not allowed.

	Drawable*               m_drawable;
	bool					m_masking;
	Image*					m_image;
	const Paint*			m_paint;
	Paint					m_defaultPaint;
	VGBlendMode				m_blendMode;
	VGImageMode				m_imageMode;
	VGImageQuality			m_imageQuality;
	Color					m_tileFillColor;
    bool                    m_colorTransform;
    RIfloat                 m_colorTransformValues[8];
	Matrix3x3				m_surfaceToPaintMatrix;
	Matrix3x3				m_surfaceToImageMatrix;
};

//=======================================================================

}	//namespace OpenVGRI

//=======================================================================

#endif /* __RIPIXELPIPE_H */
