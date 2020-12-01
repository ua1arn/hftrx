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
 * \brief	Implementations of OpenVG API functions.
 * \note	The actual processing is done in Path, Image, Rasterizer and PixelPipe classes.
 *//*-------------------------------------------------------------------*/

#include "openvg.h"
#include "egl.h"
#include "riContext.h"
#include "riRasterizer.h"
#include "riPixelPipe.h"
#include "riPath.h"
#include <stdio.h>

//==============================================================================================

namespace OpenVGRI
{

/* EGL&OS functions for use in an OpenVG implementation */
void* eglvgGetCurrentVGContext(void);
bool  eglvgIsInUse(void* image);
void  OSAcquireMutex(void);
void  OSReleaseMutex(void);


#define RI_NO_RETVAL

//this must be the first line in an API function
#define RI_GET_CONTEXT(RETVAL) \
	OSAcquireMutex(); \
	VGContext* context = (VGContext*)eglvgGetCurrentVGContext(); \
	if(!context) \
	{ \
		OSReleaseMutex(); \
		return RETVAL;\
	}

#define RI_IF_ERROR(COND, ERRORCODE, RETVAL) \
	if(COND) { context->setError(ERRORCODE); OSReleaseMutex(); return RETVAL; }

//all API functions must call this as their last operation (also functions that don't return values)
//NOTE: don't evaluate anything or read state in RETVAL (it'll be executed after the mutex has been released)
#define RI_RETURN(RETVAL) \
	{ OSReleaseMutex(); \
	return RETVAL; }

static bool isAligned(const void* ptr, int alignment)
{
	RI_ASSERT(alignment == 1 || alignment == 2 || alignment == 4);
	if(((RIuintptr)ptr) & (alignment-1))
		return false;
	return true;
}

static bool isAligned(const void* ptr, VGImageFormat format)
{
	RI_ASSERT(isValidImageFormat(format));
	int alignment = Color::formatToDescriptor(format).bitsPerPixel >> 3;
	if(alignment <= 1)
		return true;	//one bit or byte per pixel
	return isAligned(ptr, alignment);
}

bool isValidImageFormat(int f)
{
	if(f < VG_sRGBX_8888 || f > VG_lABGR_8888_PRE)
		return false;
	return true;
}

}	//namespace OpenVGRI

using namespace OpenVGRI;

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgFlush(void)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	//the RI doesn't cache anything, so this is a no-op
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgFinish(void)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	//the RI doesn't cache anything, so this is a no-op
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGErrorCode RI_APIENTRY vgGetError(void)
{
	RI_GET_CONTEXT(VG_NO_CONTEXT_ERROR);
	VGErrorCode error = context->m_error;
	context->m_error = VG_NO_ERROR;
	RI_RETURN(error);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

namespace OpenVGRI
{

RIfloat inputFloat(VGfloat f)
{
	//this function is used for all floating point input values
	if(RI_ISNAN(f)) return 0.0f;	//convert NaN to zero
	return RI_CLAMP(f, -RI_FLOAT_MAX, RI_FLOAT_MAX);	//clamp +-inf to +-RIfloat max
}

Vector2 inputVector2(const Vector2& v)
{
    return Vector2(inputFloat(v.x), inputFloat(v.y));
}

Color inputColor(const Color& c)
{
    Color r = c;
    r.r = inputFloat(r.r);
    r.g = inputFloat(r.g);
    r.b = inputFloat(r.b);
    r.a = inputFloat(r.a);
    return r;
}

static int inputFloatToInt(VGfloat value)
{
	double v = (double)floor(value);
	v = v > (double)RI_INT32_MAX ? (double)RI_INT32_MAX : v;
	v = v < (double)RI_INT32_MIN ? (double)RI_INT32_MIN : v;
	return (int)v;
}

static int paramToInt(const void* values, bool floats, int count, int i)
{
	RI_ASSERT(i >= 0);
	if(i >= count || !values)
		return 0;
	if(floats)
		return inputFloatToInt(((const VGfloat*)values)[i]);
	return (int)((const VGint*)values)[i];
}

static RIfloat paramToFloat(const void* values, bool floats, int count, int i)
{
	RI_ASSERT(i >= 0);
	if(i >= count || !values)
		return 0.0f;
	if(floats)
		return ((const VGfloat*)values)[i];
	return (RIfloat)((const VGint*)values)[i];
}

static void floatToParam(void* output, bool outputFloats, int count, int i, VGfloat value)
{
	RI_ASSERT(i >= 0);
	RI_ASSERT(output);
	if(i >= count)
		return;
	if(outputFloats)
		((VGfloat*)output)[i] = value;
	else
		((VGint*)output)[i] = (VGint)inputFloatToInt(value);
}

static void intToParam(void* output, bool outputFloats, int count, int i, VGint value)
{
	RI_ASSERT(i >= 0);
	RI_ASSERT(output);
	if(i >= count)
		return;
	if(outputFloats)
		((VGfloat*)output)[i] = (VGfloat)value;
	else
		((VGint*)output)[i] = value;
}

}	//namespace OpenVGRI

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void setifv(VGContext* context, VGParamType type, VGint count, const void* values, bool floats)
{
	RI_ASSERT(context);
	RI_ASSERT(!count || (count && values));

	int ivalue = paramToInt(values, floats, count, 0);
	RIfloat fvalue = paramToFloat(values, floats, count, 0);

	switch(type)
	{
	case VG_MATRIX_MODE:
		if(count != 1 || ivalue < VG_MATRIX_PATH_USER_TO_SURFACE || ivalue > VG_MATRIX_GLYPH_USER_TO_SURFACE)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_matrixMode = (VGMatrixMode)ivalue;
		break;

	case VG_FILL_RULE:
		if(count != 1 || ivalue < VG_EVEN_ODD || ivalue > VG_NON_ZERO)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_fillRule = (VGFillRule)ivalue;
		break;

	case VG_IMAGE_QUALITY:
		if(count != 1 || ivalue < VG_IMAGE_QUALITY_NONANTIALIASED || ivalue > VG_IMAGE_QUALITY_BETTER)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_imageQuality = (VGImageQuality)ivalue;
		break;

	case VG_RENDERING_QUALITY:
		if(count != 1 || ivalue < VG_RENDERING_QUALITY_NONANTIALIASED || ivalue > VG_RENDERING_QUALITY_BETTER)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_renderingQuality = (VGRenderingQuality)ivalue;
		break;

	case VG_BLEND_MODE:
		if(count != 1 || ivalue < VG_BLEND_SRC || ivalue > VG_BLEND_ADDITIVE)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_blendMode = (VGBlendMode)ivalue;
		break;

	case VG_IMAGE_MODE:
		if(count != 1 || ivalue < VG_DRAW_IMAGE_NORMAL || ivalue > VG_DRAW_IMAGE_STENCIL)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_imageMode = (VGImageMode)ivalue;
		break;

	case VG_SCISSOR_RECTS:
	{
		if(count & 3)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }	//count must be a multiple of four
		try
		{
			Array<Rectangle> scissor;
			for(int i=0;i<RI_INT_MIN(count, RI_MAX_SCISSOR_RECTANGLES*4);i+=4)
			{
				Rectangle s;
				s.x = paramToInt(values, floats, count, i+0);
				s.y = paramToInt(values, floats, count, i+1);
				s.width = paramToInt(values, floats, count, i+2);
				s.height = paramToInt(values, floats, count, i+3);
				scissor.push_back(s);	//throws bad_alloc
			}
			context->m_scissor.swap(scissor);	//replace context data
		}
		catch(std::bad_alloc)
		{
			context->setError(VG_OUT_OF_MEMORY_ERROR);
		}
		break;
	}

	case VG_COLOR_TRANSFORM:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_colorTransform = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_COLOR_TRANSFORM_VALUES:
		if(count != 8 || !values) { context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
        {
            for(int i=0;i<8;i++)
            {
                context->m_inputColorTransformValues[i] = paramToFloat(values, floats, count, i);
                context->m_colorTransformValues[i] = inputFloat(context->m_inputColorTransformValues[i]);
            }
        }
		break;

	case VG_STROKE_LINE_WIDTH:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputStrokeLineWidth = fvalue;
        context->m_strokeLineWidth = inputFloat(fvalue);
		break;

	case VG_STROKE_CAP_STYLE:
		if(count != 1 || ivalue < VG_CAP_BUTT || ivalue > VG_CAP_SQUARE)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_strokeCapStyle = (VGCapStyle)ivalue;
		break;

	case VG_STROKE_JOIN_STYLE:
		if(count != 1 || ivalue < VG_JOIN_MITER || ivalue > VG_JOIN_BEVEL)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_strokeJoinStyle = (VGJoinStyle)ivalue;
		break;

	case VG_STROKE_MITER_LIMIT:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputStrokeMiterLimit = fvalue;
		context->m_strokeMiterLimit = inputFloat(fvalue);
		break;

	case VG_STROKE_DASH_PATTERN:
	{
		try
		{
			Array<RIfloat> inputStrokeDashPattern;
			Array<RIfloat> strokeDashPattern;
			for(int i=0;i<RI_INT_MIN(count, RI_MAX_DASH_COUNT);i++)
            {
                RIfloat v = paramToFloat(values, floats, count, i);
				inputStrokeDashPattern.push_back(v);	//throws bad_alloc
				strokeDashPattern.push_back(inputFloat(v));	//throws bad_alloc
            }
			context->m_inputStrokeDashPattern.swap(inputStrokeDashPattern);	//replace context data
			context->m_strokeDashPattern.swap(strokeDashPattern);	//replace context data
		}
		catch(std::bad_alloc)
		{
			context->setError(VG_OUT_OF_MEMORY_ERROR);
		}
		break;
	}

	case VG_STROKE_DASH_PHASE:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputStrokeDashPhase = fvalue;
		context->m_strokeDashPhase = inputFloat(fvalue);
		break;

	case VG_STROKE_DASH_PHASE_RESET:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_strokeDashPhaseReset = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_TILE_FILL_COLOR:
		if(count != 4 || !values) { context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputTileFillColor.set(paramToFloat(values, floats, count, 0),
									 paramToFloat(values, floats, count, 1),
									 paramToFloat(values, floats, count, 2),
									 paramToFloat(values, floats, count, 3),
									 Color::sRGBA);
        context->m_tileFillColor = inputColor(context->m_inputTileFillColor);
		break;

	case VG_GLYPH_ORIGIN:
		if(count != 2 || !values) { context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputGlyphOrigin.x = paramToFloat(values, floats, count, 0);
		context->m_inputGlyphOrigin.y = paramToFloat(values, floats, count, 1);
		context->m_glyphOrigin = inputVector2(context->m_inputGlyphOrigin);
		break;

	case VG_CLEAR_COLOR:
		if(count != 4 || !values) { context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_inputClearColor.set(paramToFloat(values, floats, count, 0),
								  paramToFloat(values, floats, count, 1),
								  paramToFloat(values, floats, count, 2),
								  paramToFloat(values, floats, count, 3),
								  Color::sRGBA);
        context->m_clearColor = inputColor(context->m_inputClearColor);
		break;

	case VG_MASKING:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_masking = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_SCISSORING:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_scissoring = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_PIXEL_LAYOUT:
		if(count != 1 || ivalue < VG_PIXEL_LAYOUT_UNKNOWN || ivalue > VG_PIXEL_LAYOUT_BGR_HORIZONTAL)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_pixelLayout = (VGPixelLayout)ivalue;
		break;

	case VG_SCREEN_LAYOUT:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		break;	//setting read-only values has no effect

	case VG_FILTER_FORMAT_LINEAR:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_filterFormatLinear = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_FILTER_FORMAT_PREMULTIPLIED:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		context->m_filterFormatPremultiplied = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_FILTER_CHANNEL_MASK:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		//undefined bits are ignored
		context->m_filterChannelMask = (VGbitfield)ivalue;
		break;

	case VG_MAX_SCISSOR_RECTS:
	case VG_MAX_DASH_COUNT:
	case VG_MAX_KERNEL_SIZE:
	case VG_MAX_SEPARABLE_KERNEL_SIZE:
	case VG_MAX_COLOR_RAMP_STOPS:
	case VG_MAX_IMAGE_WIDTH:
	case VG_MAX_IMAGE_HEIGHT:
	case VG_MAX_IMAGE_PIXELS:
	case VG_MAX_IMAGE_BYTES:
	case VG_MAX_FLOAT:
	case VG_MAX_GAUSSIAN_STD_DEVIATION:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		break;	//setting read-only values has no effect

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetf(VGParamType type, VGfloat value)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(type == VG_SCISSOR_RECTS || type == VG_STROKE_DASH_PATTERN || type == VG_TILE_FILL_COLOR ||
				type == VG_CLEAR_COLOR, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//vector type value
	VGfloat values[1] = {value};
	setifv(context, type, 1, values, true);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSeti(VGParamType type, VGint value)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(type == VG_SCISSOR_RECTS || type == VG_STROKE_DASH_PATTERN || type == VG_TILE_FILL_COLOR ||
				type == VG_CLEAR_COLOR, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//vector type value
	VGint values[1] = {value};
	setifv(context, type, 1, values, false);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetiv(VGParamType type, VGint count, const VGint * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count < 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR((!values && count > 0) || (values && !isAligned(values,4)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	setifv(context, type, count, values, false);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetfv(VGParamType type, VGint count, const VGfloat * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count < 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR((!values && count > 0) || (values && !isAligned(values,4)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	setifv(context, type, count, values, true);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void getifv(VGContext* context, VGParamType type, VGint count, void* values, bool floats)
{
	switch(type)
	{
	case VG_MATRIX_MODE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_matrixMode);
		break;

	case VG_FILL_RULE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_fillRule);
		break;

	case VG_IMAGE_QUALITY:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_imageQuality);
		break;

	case VG_RENDERING_QUALITY:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_renderingQuality);
		break;

	case VG_BLEND_MODE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_blendMode);
		break;

	case VG_IMAGE_MODE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_imageMode);
		break;

	case VG_SCISSOR_RECTS:
	{
		if(count > context->m_scissor.size()*4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		for(int i=0;i<context->m_scissor.size();i++)
		{
			intToParam(values, floats, count, i*4+0, context->m_scissor[i].x);
			intToParam(values, floats, count, i*4+1, context->m_scissor[i].y);
			intToParam(values, floats, count, i*4+2, context->m_scissor[i].width);
			intToParam(values, floats, count, i*4+3, context->m_scissor[i].height);
		}
		break;
	}

	case VG_COLOR_TRANSFORM:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_colorTransform);
		break;

	case VG_COLOR_TRANSFORM_VALUES:
		if(count > 8) { context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
        {
            for(int i=0;i<count;i++)
            {
                floatToParam(values, floats, count, i, context->m_inputColorTransformValues[i]);
            }
        }
		break;

	case VG_STROKE_LINE_WIDTH:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputStrokeLineWidth);
		break;

	case VG_STROKE_CAP_STYLE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_strokeCapStyle);
		break;

	case VG_STROKE_JOIN_STYLE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_strokeJoinStyle);
		break;

	case VG_STROKE_MITER_LIMIT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputStrokeMiterLimit);
		break;

	case VG_STROKE_DASH_PATTERN:
	{
		if(count > context->m_inputStrokeDashPattern.size())	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		for(int i=0;i<context->m_inputStrokeDashPattern.size();i++)
			floatToParam(values, floats, count, i, context->m_inputStrokeDashPattern[i]);
		break;
	}

	case VG_STROKE_DASH_PHASE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputStrokeDashPhase);
		break;

	case VG_STROKE_DASH_PHASE_RESET:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_strokeDashPhaseReset);
		break;

	case VG_TILE_FILL_COLOR:
		if(count > 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputTileFillColor.r);
		floatToParam(values, floats, count, 1, context->m_inputTileFillColor.g);
		floatToParam(values, floats, count, 2, context->m_inputTileFillColor.b);
		floatToParam(values, floats, count, 3, context->m_inputTileFillColor.a);
		break;

	case VG_CLEAR_COLOR:
		if(count > 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputClearColor.r);
		floatToParam(values, floats, count, 1, context->m_inputClearColor.g);
		floatToParam(values, floats, count, 2, context->m_inputClearColor.b);
		floatToParam(values, floats, count, 3, context->m_inputClearColor.a);
		break;

	case VG_GLYPH_ORIGIN:
		if(count > 2)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, context->m_inputGlyphOrigin.x);
		floatToParam(values, floats, count, 1, context->m_inputGlyphOrigin.y);
		break;

	case VG_MASKING:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_masking);
		break;

	case VG_SCISSORING:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_scissoring);
		break;

	case VG_PIXEL_LAYOUT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_pixelLayout);
		break;

	case VG_SCREEN_LAYOUT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, VG_PIXEL_LAYOUT_UNKNOWN);
		break;

	case VG_FILTER_FORMAT_LINEAR:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_filterFormatLinear);
		break;

	case VG_FILTER_FORMAT_PREMULTIPLIED:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_filterFormatPremultiplied);
		break;

	case VG_FILTER_CHANNEL_MASK:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, context->m_filterChannelMask);
		break;

	case VG_MAX_SCISSOR_RECTS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_SCISSOR_RECTANGLES);
		break;

	case VG_MAX_DASH_COUNT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_DASH_COUNT);
		break;

	case VG_MAX_KERNEL_SIZE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_KERNEL_SIZE);
		break;

	case VG_MAX_SEPARABLE_KERNEL_SIZE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_SEPARABLE_KERNEL_SIZE);
		break;

	case VG_MAX_COLOR_RAMP_STOPS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_COLOR_RAMP_STOPS);
		break;

	case VG_MAX_IMAGE_WIDTH:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_IMAGE_WIDTH);
		break;

	case VG_MAX_IMAGE_HEIGHT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_IMAGE_HEIGHT);
		break;

	case VG_MAX_IMAGE_PIXELS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_IMAGE_PIXELS);
		break;

	case VG_MAX_IMAGE_BYTES:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, RI_MAX_IMAGE_BYTES);
		break;

	case VG_MAX_FLOAT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, RI_FLOAT_MAX);
		break;

	case VG_MAX_GAUSSIAN_STD_DEVIATION:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, RI_MAX_GAUSSIAN_STD_DEVIATION);
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGfloat RI_APIENTRY vgGetf(VGParamType type)
{
	RI_GET_CONTEXT(0.0f);
	RI_IF_ERROR(type == VG_SCISSOR_RECTS || type == VG_STROKE_DASH_PATTERN || type == VG_TILE_FILL_COLOR ||
				type == VG_CLEAR_COLOR, VG_ILLEGAL_ARGUMENT_ERROR, 0.0f);	//vector type value
	RIfloat ret = 0.0f;
	getifv(context, type, 1, &ret, true);
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGint RI_APIENTRY vgGeti(VGParamType type)
{
	RI_GET_CONTEXT(0);
	RI_IF_ERROR(type == VG_SCISSOR_RECTS || type == VG_STROKE_DASH_PATTERN || type == VG_TILE_FILL_COLOR ||
				type == VG_CLEAR_COLOR, VG_ILLEGAL_ARGUMENT_ERROR, 0);	//vector type value
	VGint ret = 0;
	getifv(context, type, 1, &ret, false);
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetiv(VGParamType type, VGint count, VGint * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!values || !isAligned(values,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	getifv(context, type, count, values, false);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetfv(VGParamType type, VGint count, VGfloat * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!values || !isAligned(values,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	getifv(context, type, count, values, true);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGint RI_APIENTRY vgGetVectorSize(VGParamType type)
{
	RI_GET_CONTEXT(0);
	VGint ret = 0;
	switch(type)
	{
	case VG_MATRIX_MODE:
	case VG_FILL_RULE:
	case VG_IMAGE_QUALITY:
	case VG_RENDERING_QUALITY:
	case VG_BLEND_MODE:
	case VG_IMAGE_MODE:
		ret = 1;
		break;

	case VG_SCISSOR_RECTS:
		ret = 4*context->m_scissor.size();
		break;

    case VG_COLOR_TRANSFORM:
        ret = 1;
        break;

    case VG_COLOR_TRANSFORM_VALUES:
        ret = 8;
        break;

	case VG_STROKE_LINE_WIDTH:
	case VG_STROKE_CAP_STYLE:
	case VG_STROKE_JOIN_STYLE:
	case VG_STROKE_MITER_LIMIT:
		ret = 1;
		break;

	case VG_STROKE_DASH_PATTERN:
		ret = context->m_inputStrokeDashPattern.size();
		break;

	case VG_STROKE_DASH_PHASE:
	case VG_STROKE_DASH_PHASE_RESET:
		ret = 1;
		break;

	case VG_TILE_FILL_COLOR:
	case VG_CLEAR_COLOR:
		ret = 4;
		break;

	case VG_GLYPH_ORIGIN:
		ret = 2;
		break;

	case VG_MASKING:
	case VG_SCISSORING:
	case VG_PIXEL_LAYOUT:
	case VG_SCREEN_LAYOUT:
	case VG_FILTER_FORMAT_LINEAR:
	case VG_FILTER_FORMAT_PREMULTIPLIED:
	case VG_FILTER_CHANNEL_MASK:
	case VG_MAX_SCISSOR_RECTS:
	case VG_MAX_DASH_COUNT:
	case VG_MAX_KERNEL_SIZE:
	case VG_MAX_SEPARABLE_KERNEL_SIZE:
	case VG_MAX_COLOR_RAMP_STOPS:
	case VG_MAX_IMAGE_WIDTH:
	case VG_MAX_IMAGE_HEIGHT:
	case VG_MAX_IMAGE_PIXELS:
	case VG_MAX_IMAGE_BYTES:
	case VG_MAX_FLOAT:
	case VG_MAX_GAUSSIAN_STD_DEVIATION:
		ret = 1;
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void setPaintParameterifv(VGContext* context, Paint* paint, VGPaintParamType paramType, VGint count, const void* values, bool floats)
{
	RI_ASSERT(context);
	RI_ASSERT(paint);

	int ivalue = paramToInt(values, floats, count, 0);

	switch(paramType)
	{
	case VG_PAINT_TYPE:
		if(count != 1 || ivalue < VG_PAINT_TYPE_COLOR || ivalue > VG_PAINT_TYPE_PATTERN)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_paintType = (VGPaintType)ivalue;
		break;

	case VG_PAINT_COLOR:
		if(count != 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_inputPaintColor.set(paramToFloat(values, floats, count, 0),
									 paramToFloat(values, floats, count, 1),
									 paramToFloat(values, floats, count, 2),
									 paramToFloat(values, floats, count, 3),
									 Color::sRGBA);
		paint->m_paintColor = inputColor(paint->m_inputPaintColor);
		paint->m_paintColor.clamp();
		paint->m_paintColor.premultiply();
		break;

	case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
		if(count != 1 || ivalue < VG_COLOR_RAMP_SPREAD_PAD || ivalue > VG_COLOR_RAMP_SPREAD_REFLECT)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_colorRampSpreadMode = (VGColorRampSpreadMode)ivalue;
		break;

	case VG_PAINT_COLOR_RAMP_STOPS:
	{
		int numStops = count/5;
		if(count != numStops*5)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }	//count must be a multiple of five
		try
		{
			Array<Paint::GradientStop> colorRampStops;
			Array<Paint::GradientStop> inputColorRampStops;
			RIfloat prevOffset = -RI_FLOAT_MAX;
			bool valid = true;
			for(int i=0;i<RI_INT_MIN(numStops, RI_MAX_COLOR_RAMP_STOPS);i++)	//NOTE: ignores the final stop if there is not enough parameters
			{
				Paint::GradientStop gs;
				gs.offset = paramToFloat(values, floats, count, i*5);
				gs.color.set(paramToFloat(values, floats, count, i*5+1),
							 paramToFloat(values, floats, count, i*5+2),
							 paramToFloat(values, floats, count, i*5+3),
							 paramToFloat(values, floats, count, i*5+4),
							 Color::sRGBA);
				inputColorRampStops.push_back(gs);

				if(gs.offset < prevOffset)
					valid = false;	//decreasing sequence, ignore it

				if(gs.offset >= 0.0f && gs.offset <= 1.0f)
				{
					gs.color.clamp();

					if(!colorRampStops.size() && gs.offset > 0.0f)
					{	//the first valid stop is not at 0, replicate the first one
						RIfloat tmp = gs.offset;
						gs.offset = 0.0f;
						colorRampStops.push_back(gs);	//throws bad_alloc
						gs.offset = tmp;
					}
					colorRampStops.push_back(gs);	//throws bad_alloc
				}
				prevOffset = gs.offset;
			}
			if(valid && colorRampStops.size() && colorRampStops[colorRampStops.size()-1].offset < 1.0f)
			{	//there is at least one stop, but the last one is not at 1, replicate the last one
				Paint::GradientStop gs = colorRampStops[colorRampStops.size()-1];
				gs.offset = 1.0f;
				colorRampStops.push_back(gs);	//throws bad_alloc
			}
			if(!valid || !colorRampStops.size())
			{	//there are no valid stops, add implicit stops
				colorRampStops.clear();
				Paint::GradientStop gs;
				gs.offset = 0.0f;
				gs.color.set(0,0,0,1,Color::sRGBA);
				colorRampStops.push_back(gs);	//throws bad_alloc
				gs.offset = 1.0f;
				gs.color.set(1,1,1,1,Color::sRGBA);
				colorRampStops.push_back(gs);	//throws bad_alloc
			}
			RI_ASSERT(colorRampStops.size() >= 2 && colorRampStops.size() <= RI_MAX_COLOR_RAMP_STOPS);
			paint->m_colorRampStops.swap(colorRampStops);	//set paint array
			paint->m_inputColorRampStops.swap(inputColorRampStops);	//set paint array
		}
		catch(std::bad_alloc)
		{
			context->setError(VG_OUT_OF_MEMORY_ERROR);
		}
		break;
	}

	case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
		if(count != 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_colorRampPremultiplied = ivalue ? VG_TRUE : VG_FALSE;
		break;

	case VG_PAINT_LINEAR_GRADIENT:
		if(count != 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_inputLinearGradientPoint0.set(paramToFloat(values, floats, count, 0),
										  paramToFloat(values, floats, count, 1));
		paint->m_inputLinearGradientPoint1.set(paramToFloat(values, floats, count, 2),
										  paramToFloat(values, floats, count, 3));
        paint->m_linearGradientPoint0 = inputVector2(paint->m_inputLinearGradientPoint0);
        paint->m_linearGradientPoint1 = inputVector2(paint->m_inputLinearGradientPoint1);
		break;

	case VG_PAINT_RADIAL_GRADIENT:
		if(count != 5)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_inputRadialGradientCenter.set(paramToFloat(values, floats, count, 0),
										  paramToFloat(values, floats, count, 1));
		paint->m_inputRadialGradientFocalPoint.set(paramToFloat(values, floats, count, 2),
											  paramToFloat(values, floats, count, 3));
		paint->m_inputRadialGradientRadius = paramToFloat(values, floats, count, 4);
        paint->m_radialGradientCenter = inputVector2(paint->m_inputRadialGradientCenter);
        paint->m_radialGradientFocalPoint = inputVector2(paint->m_inputRadialGradientFocalPoint);
        paint->m_radialGradientRadius = inputFloat(paint->m_inputRadialGradientRadius);
		break;

	case VG_PAINT_PATTERN_TILING_MODE:
		if(count != 1 || ivalue < VG_TILE_FILL || ivalue > VG_TILE_REFLECT)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		paint->m_patternTilingMode = (VGTilingMode)ivalue;
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetParameterf(VGHandle object, VGint paramType, VGfloat value)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isMaskLayer = context->isValidMaskLayer(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isMaskLayer && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL); //invalid object handle
	RI_IF_ERROR(paramType == VG_PAINT_COLOR || paramType == VG_PAINT_COLOR_RAMP_STOPS || paramType == VG_PAINT_LINEAR_GRADIENT ||
				paramType == VG_PAINT_RADIAL_GRADIENT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL); //vector valued parameter
	VGfloat values[1] = {value};
	if(isImage)
	{	//read only, the function does nothing
		RI_ASSERT(!isPath && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_IMAGE_FORMAT || paramType > VG_IMAGE_HEIGHT)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPath)
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_PATH_FORMAT || paramType > VG_PATH_NUM_COORDS)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isMaskLayer && !isFont);
		setPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, 1, values, true);
	}
	else if(isMaskLayer)
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && !isFont);
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
    else
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPath && !isPaint && !isMaskLayer && isFont);
        if (paramType != VG_FONT_NUM_GLYPHS)
    		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
    }
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetParameteri(VGHandle object, VGint paramType, VGint value)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isMaskLayer = context->isValidMaskLayer(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isMaskLayer && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL); //invalid object handle
	RI_IF_ERROR(paramType == VG_PAINT_COLOR || paramType == VG_PAINT_COLOR_RAMP_STOPS || paramType == VG_PAINT_LINEAR_GRADIENT ||
				paramType == VG_PAINT_RADIAL_GRADIENT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//vector valued parameter
	VGint values[1] = {value};
	if(isImage)
	{	//read only, the function does nothing
		RI_ASSERT(!isPath && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_IMAGE_FORMAT || paramType > VG_IMAGE_HEIGHT)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPath)
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_PATH_FORMAT || paramType > VG_PATH_NUM_COORDS)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isMaskLayer && !isFont);
		setPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, 1, values, false);
	}
	else if(isMaskLayer)
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && !isFont);
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
    else
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPath && !isPaint && !isMaskLayer && isFont);
        if (paramType != VG_FONT_NUM_GLYPHS)
    		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
    }
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetParameterfv(VGHandle object, VGint paramType, VGint count, const VGfloat * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count < 0 || (!values && count > 0) || (values && !isAligned(values,4)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isMaskLayer = context->isValidMaskLayer(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isMaskLayer && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL); //invalid object handle
	if(isImage)
	{	//read only, the function does nothing
		RI_ASSERT(!isPath && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_IMAGE_FORMAT || paramType > VG_IMAGE_HEIGHT)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPath)
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_PATH_FORMAT || paramType > VG_PATH_NUM_COORDS)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isMaskLayer && !isFont);
		setPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, count, values, true);
	}
	else if(isMaskLayer)
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && !isFont);
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
    else
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPath && !isPaint && !isMaskLayer && isFont);
        if (paramType != VG_FONT_NUM_GLYPHS)
    		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
    }
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetParameteriv(VGHandle object, VGint paramType, VGint count, const VGint * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count < 0 || (!values && count > 0) || (values && !isAligned(values,4)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isMaskLayer = context->isValidMaskLayer(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isMaskLayer && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL); //invalid object handle
	if(isImage)
	{	//read only, the function does nothing
		RI_ASSERT(!isPath && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_IMAGE_FORMAT || paramType > VG_IMAGE_HEIGHT)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPath)
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPaint && !isMaskLayer && !isFont);
		if(paramType < VG_PATH_FORMAT || paramType > VG_PATH_NUM_COORDS)
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isMaskLayer && !isFont);
		setPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, count, values, false);
	}
	else if(isMaskLayer)
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && !isFont);
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
	}
    else
	{	//read only, the function does nothing
		RI_ASSERT(!isImage && !isPath && !isPaint && !isMaskLayer && isFont);
        if (paramType != VG_FONT_NUM_GLYPHS)
    		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
    }
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void getPaintParameterifv(VGContext* context, Paint* paint, VGPaintParamType type, VGint count, void* values, bool floats)
{
	switch(type)
	{
	case VG_PAINT_TYPE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, paint->m_paintType);
		break;

	case VG_PAINT_COLOR:
		if(count > 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, paint->m_inputPaintColor.r);
		floatToParam(values, floats, count, 1, paint->m_inputPaintColor.g);
		floatToParam(values, floats, count, 2, paint->m_inputPaintColor.b);
		floatToParam(values, floats, count, 3, paint->m_inputPaintColor.a);
		break;

	case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, paint->m_colorRampSpreadMode);
		break;

	case VG_PAINT_COLOR_RAMP_STOPS:
		{
			if(count > paint->m_inputColorRampStops.size()*5)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
			int j = 0;
			for(int i=0;i<paint->m_inputColorRampStops.size();i++)
			{
				floatToParam(values, floats, count, j++, paint->m_inputColorRampStops[i].offset);
				floatToParam(values, floats, count, j++, paint->m_inputColorRampStops[i].color.r);
				floatToParam(values, floats, count, j++, paint->m_inputColorRampStops[i].color.g);
				floatToParam(values, floats, count, j++, paint->m_inputColorRampStops[i].color.b);
				floatToParam(values, floats, count, j++, paint->m_inputColorRampStops[i].color.a);
			}
			break;
		}

	case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, paint->m_colorRampPremultiplied);
		break;

	case VG_PAINT_LINEAR_GRADIENT:
		if(count > 4)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, paint->m_inputLinearGradientPoint0.x);
		floatToParam(values, floats, count, 1, paint->m_inputLinearGradientPoint0.y);
		floatToParam(values, floats, count, 2, paint->m_inputLinearGradientPoint1.x);
		floatToParam(values, floats, count, 3, paint->m_inputLinearGradientPoint1.y);
		break;

	case VG_PAINT_RADIAL_GRADIENT:
		if(count > 5)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, paint->m_inputRadialGradientCenter.x);
		floatToParam(values, floats, count, 1, paint->m_inputRadialGradientCenter.y);
		floatToParam(values, floats, count, 2, paint->m_inputRadialGradientFocalPoint.x);
		floatToParam(values, floats, count, 3, paint->m_inputRadialGradientFocalPoint.y);
		floatToParam(values, floats, count, 4, paint->m_inputRadialGradientRadius);
		break;

	case VG_PAINT_PATTERN_TILING_MODE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, paint->m_patternTilingMode);
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void getPathParameterifv(VGContext* context, Path* path, VGPathParamType type, VGint count, void* values, bool floats)
{
	switch(type)
	{
	case VG_PATH_FORMAT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, path->getFormat());
		break;

	case VG_PATH_DATATYPE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, path->getDatatype());
		break;

	case VG_PATH_SCALE:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, path->getScale());
		break;

	case VG_PATH_BIAS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		floatToParam(values, floats, count, 0, path->getBias());
		break;

	case VG_PATH_NUM_SEGMENTS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, path->getNumSegments());
		break;

	case VG_PATH_NUM_COORDS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, path->getNumCoordinates());
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void getImageParameterifv(VGContext* context, Image* image, VGImageParamType type, VGint count, void* values, bool floats)
{
	switch(type)
	{
	case VG_IMAGE_FORMAT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		RI_ASSERT(isValidImageFormat(image->getDescriptor().format));
		intToParam(values, floats, count, 0, image->getDescriptor().format);
		break;

	case VG_IMAGE_WIDTH:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, image->getWidth());
		break;

	case VG_IMAGE_HEIGHT:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, image->getHeight());
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void getFontParameterifv(VGContext* context, Font* font, VGFontParamType type, VGint count, void* values, bool floats)
{
	switch(type)
	{
	case VG_FONT_NUM_GLYPHS:
		if(count > 1)	{ context->setError(VG_ILLEGAL_ARGUMENT_ERROR); return; }
		intToParam(values, floats, count, 0, font->getNumGlyphs());
		break;

	default:
		context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid VGParamType
		break;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGfloat RI_APIENTRY vgGetParameterf(VGHandle object, VGint paramType)
{
	RI_GET_CONTEXT(0.0f);
	RI_IF_ERROR(paramType == VG_PAINT_COLOR || paramType == VG_PAINT_COLOR_RAMP_STOPS || paramType == VG_PAINT_LINEAR_GRADIENT ||
				paramType == VG_PAINT_RADIAL_GRADIENT, VG_ILLEGAL_ARGUMENT_ERROR, 0.0f);	//vector valued parameter
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isFont, VG_BAD_HANDLE_ERROR, 0.0f);	//invalid object handle
	VGfloat ret = 0.0f;
	if(isImage)
	{
		RI_ASSERT(!isPath && !isPaint && !isFont);
		getImageParameterifv(context, (Image*)object, (VGImageParamType)paramType, 1, &ret, true);
	}
	else if(isPath)
	{
		RI_ASSERT(!isImage && !isPaint && !isFont);
		getPathParameterifv(context, (Path*)object, (VGPathParamType)paramType, 1, &ret, true);
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isFont);
		getPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, 1, &ret, true);
	}
	else
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && isFont);
		getFontParameterifv(context, (Font*)object, (VGFontParamType)paramType, 1, &ret, true);
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGint RI_APIENTRY vgGetParameteri(VGHandle object, VGint paramType)
{
	RI_GET_CONTEXT(0);
	RI_IF_ERROR(paramType == VG_PAINT_COLOR || paramType == VG_PAINT_COLOR_RAMP_STOPS || paramType == VG_PAINT_LINEAR_GRADIENT ||
				paramType == VG_PAINT_RADIAL_GRADIENT, VG_ILLEGAL_ARGUMENT_ERROR, 0);	//vector valued parameter
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isFont, VG_BAD_HANDLE_ERROR, 0);	//invalid object handle
	VGint ret = 0;
	if(isImage)
	{
		RI_ASSERT(!isPath && !isPaint && !isFont);
		getImageParameterifv(context, (Image*)object, (VGImageParamType)paramType, 1, &ret, false);
	}
	else if(isPath)
	{
		RI_ASSERT(!isImage && !isPaint && !isFont);
		getPathParameterifv(context, (Path*)object, (VGPathParamType)paramType, 1, &ret, false);
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isFont);
		getPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, 1, &ret, false);
	}
	else
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && isFont);
		getFontParameterifv(context, (Font*)object, (VGFontParamType)paramType, 1, &ret, false);
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetParameterfv(VGHandle object, VGint paramType, VGint count, VGfloat * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count <= 0 || !values || !isAligned(values,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid object handle
	if(isImage)
	{
		RI_ASSERT(!isPath && !isPaint && !isFont);
		getImageParameterifv(context, (Image*)object, (VGImageParamType)paramType, count, values, true);
	}
	else if(isPath)
	{
		RI_ASSERT(!isImage && !isPaint && !isFont);
		getPathParameterifv(context, (Path*)object, (VGPathParamType)paramType, count, values, true);
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isFont);
		getPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, count, values, true);
	}
	else
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && isFont);
		getFontParameterifv(context, (Font*)object, (VGFontParamType)paramType, count, values, true);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetParameteriv(VGHandle object, VGint paramType, VGint count, VGint * values)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(count <= 0 || !values || !isAligned(values,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isFont, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid object handle
	if(isImage)
	{
		RI_ASSERT(!isPath && !isPaint && !isFont);
		getImageParameterifv(context, (Image*)object, (VGImageParamType)paramType, count, values, false);
	}
	else if(isPath)
	{
		RI_ASSERT(!isImage && !isPaint && !isFont);
		getPathParameterifv(context, (Path*)object, (VGPathParamType)paramType, count, values, false);
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isFont);
		getPaintParameterifv(context, (Paint*)object, (VGPaintParamType)paramType, count, values, false);
	}
	else
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && isFont);
		getFontParameterifv(context, (Font*)object, (VGFontParamType)paramType, count, values, false);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGint RI_APIENTRY vgGetParameterVectorSize(VGHandle object, VGint paramType)
{
	RI_GET_CONTEXT(0);
	bool isImage = context->isValidImage(object);
	bool isPath = context->isValidPath(object);
	bool isPaint = context->isValidPaint(object);
	bool isFont = context->isValidFont(object);
	RI_IF_ERROR(!isImage && !isPath && !isPaint && !isFont, VG_BAD_HANDLE_ERROR, 0);	//invalid object handle
	int ret = 0;
	if(isImage)
	{
		RI_ASSERT(!isPath && !isPaint && !isFont);
		switch(paramType)
		{
		case VG_IMAGE_FORMAT:
		case VG_IMAGE_WIDTH:
		case VG_IMAGE_HEIGHT:
			ret = 1;
			break;

		default:
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid paramType
			break;
		}
	}
	else if(isPath)
	{
		RI_ASSERT(!isImage && !isPaint && !isFont);
		switch(paramType)
		{
		case VG_PATH_FORMAT:
		case VG_PATH_DATATYPE:
		case VG_PATH_SCALE:
		case VG_PATH_BIAS:
		case VG_PATH_NUM_SEGMENTS:
		case VG_PATH_NUM_COORDS:
			ret = 1;
			break;

		default:
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid paramType
			break;
		}
	}
	else if(isPaint)
	{
		RI_ASSERT(!isImage && !isPath && !isFont);
		switch(paramType)
		{
		case VG_PAINT_TYPE:
		case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
		case VG_PAINT_PATTERN_TILING_MODE:
			ret = 1;
			break;

		case VG_PAINT_COLOR:
		case VG_PAINT_LINEAR_GRADIENT:
			ret = 4;
			break;

		case VG_PAINT_COLOR_RAMP_STOPS:
			ret = ((Paint*)object)->m_inputColorRampStops.size() * 5;
			break;

		case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
			ret = 1;
			break;

		case VG_PAINT_RADIAL_GRADIENT:
			ret = 5;
			break;
			
		default:
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid paramType
			break;
		}
	}
	else
	{
		RI_ASSERT(!isImage && !isPath && !isPaint && isFont);
		switch(paramType)
		{
		case VG_FONT_NUM_GLYPHS:
			ret = 1;
			break;

		default:
			context->setError(VG_ILLEGAL_ARGUMENT_ERROR);	//invalid paramType
			break;
		}
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static Matrix3x3* getCurrentMatrix(VGContext* context)
{
	RI_ASSERT(context);
	switch(context->m_matrixMode)
	{
	case VG_MATRIX_PATH_USER_TO_SURFACE:
		return &context->m_pathUserToSurface;

	case VG_MATRIX_IMAGE_USER_TO_SURFACE:
		return &context->m_imageUserToSurface;

	case VG_MATRIX_FILL_PAINT_TO_USER:
		return &context->m_fillPaintToUser;

	case VG_MATRIX_STROKE_PAINT_TO_USER:
		return &context->m_strokePaintToUser;

	default:
		RI_ASSERT(context->m_matrixMode == VG_MATRIX_GLYPH_USER_TO_SURFACE);
		return &context->m_glyphUserToSurface;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgLoadIdentity(void)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	Matrix3x3* d = getCurrentMatrix(context);
	d->identity();
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgLoadMatrix(const VGfloat * m)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!m || !isAligned(m,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	Matrix3x3* d = getCurrentMatrix(context);
	d->set(inputFloat(m[0]), inputFloat(m[3]), inputFloat(m[6]),
		   inputFloat(m[1]), inputFloat(m[4]), inputFloat(m[7]),
		   inputFloat(m[2]), inputFloat(m[5]), inputFloat(m[8]));
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetMatrix(VGfloat * m)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!m || !isAligned(m,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	Matrix3x3* d = getCurrentMatrix(context);
	m[0] = (*d)[0][0];
	m[1] = (*d)[1][0];
	m[2] = (*d)[2][0];
	m[3] = (*d)[0][1];
	m[4] = (*d)[1][1];
	m[5] = (*d)[2][1];
	m[6] = (*d)[0][2];
	m[7] = (*d)[1][2];
	m[8] = (*d)[2][2];
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgMultMatrix(const VGfloat * m)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!m || !isAligned(m,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	Matrix3x3 n(inputFloat(m[0]), inputFloat(m[3]), inputFloat(m[6]),
				inputFloat(m[1]), inputFloat(m[4]), inputFloat(m[7]),
				inputFloat(m[2]), inputFloat(m[5]), inputFloat(m[8]));
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
		n[2].set(0,0,1);

	Matrix3x3* d = getCurrentMatrix(context);
	*d *= n;
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);	//force affinity
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgTranslate(VGfloat tx, VGfloat ty)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	Matrix3x3 n(1, 0, inputFloat(tx),
				0, 1, inputFloat(ty),
				0, 0, 1 );
	Matrix3x3* d = getCurrentMatrix(context);
	*d *= n;
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);	//force affinity
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgScale(VGfloat sx, VGfloat sy)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	Matrix3x3 n(inputFloat(sx), 0,              0,
				0,              inputFloat(sy), 0,
				0,              0,              1 );
	Matrix3x3* d = getCurrentMatrix(context);
	*d *= n;
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);	//force affinity
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgShear(VGfloat shx, VGfloat shy)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	Matrix3x3 n(1,               inputFloat(shx), 0,
				inputFloat(shy), 1,               0,
				0,               0,               1);
	Matrix3x3* d = getCurrentMatrix(context);
	*d *= n;
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);	//force affinity
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgRotate(VGfloat angle)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RIfloat a = RI_DEG_TO_RAD(inputFloat(angle));
	Matrix3x3 n((RIfloat)cos(a), -(RIfloat)sin(a), 0,
				(RIfloat)sin(a),  (RIfloat)cos(a), 0,
				0,              0,             1 );
	Matrix3x3* d = getCurrentMatrix(context);
	*d *= n;
	if(context->m_matrixMode != VG_MATRIX_IMAGE_USER_TO_SURFACE)
	{
		(*d)[2].set(0,0,1);	//force affinity
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgMask(VGHandle mask, VGMaskOperation operation, VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
    bool isImage = context->isValidImage(mask);
    bool isMaskLayer = context->isValidMaskLayer(mask);
	RI_IF_ERROR(operation != VG_CLEAR_MASK && operation != VG_FILL_MASK && !isImage && !isMaskLayer, VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(operation != VG_CLEAR_MASK && operation != VG_FILL_MASK && isImage && eglvgIsInUse((Image*)mask), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(operation < VG_CLEAR_MASK || operation > VG_SUBTRACT_MASK, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
	RI_IF_ERROR(isMaskLayer && drawable->getNumSamples() != ((Surface*)mask)->getNumSamples(), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    if(!drawable || !drawable->getMaskBuffer())
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment or context has no mask buffer
    }
    if(isImage)
        drawable->getMaskBuffer()->mask((Image*)mask, operation, x, y, width, height);
    else
        drawable->getMaskBuffer()->mask((Surface*)mask, operation, x, y, width, height);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static void renderStroke(const VGContext* context, int w, int h, int numSamples, Path* path, Rasterizer& rasterizer, const PixelPipe* pixelPipe, const Matrix3x3& userToSurface)
{
    RI_ASSERT(context);
    RI_ASSERT(w > 0 && h > 0 && numSamples >= 1 && numSamples <= 32);

    RIuint32* covBuffer = RI_NEW_ARRAY(RIuint32, w*h);
    memset(covBuffer, 0, w*h*sizeof(RIuint32));

    rasterizer.setup(0, 0, w, h, VG_NON_ZERO, NULL, covBuffer);
    path->stroke(userToSurface, rasterizer, context->m_strokeDashPattern, context->m_strokeDashPhase, context->m_strokeDashPhaseReset ? true : false,
                 context->m_strokeLineWidth, context->m_strokeCapStyle, context->m_strokeJoinStyle, RI_MAX(context->m_strokeMiterLimit, 1.0f));	//throws bad_alloc

    int sx,sy,ex,ey;
    rasterizer.getBBox(sx,sy,ex,ey);
    RI_ASSERT(sx >= 0 && sx <= w);
    RI_ASSERT(sy >= 0 && sy <= h);
    RI_ASSERT(ex >= 0 && ex <= w);
    RI_ASSERT(ey >= 0 && ey <= h);

    for(int j=sy;j<ey;j++)
    {
        for(int i=sx;i<ex;i++)
        {
            unsigned int c = covBuffer[j*w+i];
            if(c)
            {
                int coverage = 0;
                for(int k=0;k<numSamples;k++)
                {
                    if(c & (1<<k))
                        coverage++;
                }
                pixelPipe->pixelPipe(i, j, (RIfloat)coverage/(RIfloat)numSamples, c);
            }
        }
    }
    RI_DELETE_ARRAY(covBuffer);
}

void RI_APIENTRY vgRenderToMask(VGPath path, VGbitfield paintModes, VGMaskOperation operation)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!paintModes || (paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid paint mode
	RI_IF_ERROR(operation < VG_CLEAR_MASK || operation > VG_SUBTRACT_MASK, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* curr = context->getCurrentDrawable();
    if(!curr || !curr->getMaskBuffer())
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment or context has no mask buffer
    }

    if(operation == VG_CLEAR_MASK || operation == VG_FILL_MASK)
    {
        Surface* dummy = NULL;
        curr->getMaskBuffer()->mask(dummy, operation, 0, 0, curr->getWidth(), curr->getHeight());
        RI_RETURN(RI_NO_RETVAL);
    }

	try
	{
        Drawable drawable(Color::formatToDescriptor(VG_A_8), curr->getWidth(), curr->getHeight(), curr->getNumSamples(), 1);    //TODO 0 mask bits (mask buffer is not used)

        Rasterizer rasterizer;
        if(context->m_scissoring)
            rasterizer.setScissor(context->m_scissor);	//throws bad_alloc
        int numSamples = rasterizer.setupSamplingPattern(context->m_renderingQuality, drawable.getNumSamples());

        PixelPipe pixelPipe;
        pixelPipe.setDrawable(&drawable);
        pixelPipe.setMask(false);
        pixelPipe.setPaint(NULL);   //use default paint (solid color alpha = 1)
        pixelPipe.setBlendMode(VG_BLEND_SRC);   //write solid color * coverage to dest

        Matrix3x3 userToSurface = context->m_pathUserToSurface;
        userToSurface[2].set(0,0,1);	//force affinity

        if(paintModes & VG_FILL_PATH)
        {
            drawable.getColorBuffer()->clear(Color(0,0,0,0,drawable.getColorBuffer()->getDescriptor().internalFormat), 0, 0, drawable.getWidth(), drawable.getHeight());
            ((Path*)path)->fill(userToSurface, rasterizer);	//throws bad_alloc
            rasterizer.setup(0, 0, drawable.getWidth(), drawable.getHeight(), context->m_fillRule, &pixelPipe, NULL);
            rasterizer.fill();	//throws bad_alloc
            curr->getMaskBuffer()->mask(drawable.getColorBuffer(), operation, 0, 0, drawable.getWidth(), drawable.getHeight());
        }

        if(paintModes & VG_STROKE_PATH && context->m_strokeLineWidth > 0.0f)
        {
            drawable.getColorBuffer()->clear(Color(0,0,0,0,drawable.getColorBuffer()->getDescriptor().internalFormat), 0, 0, drawable.getWidth(), drawable.getHeight());
            renderStroke(context, drawable.getWidth(), drawable.getHeight(), numSamples, (Path*)path, rasterizer, &pixelPipe, userToSurface);
            curr->getMaskBuffer()->mask(drawable.getColorBuffer(), operation, 0, 0, drawable.getWidth(), drawable.getHeight());
        }
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGMaskLayer RI_APIENTRY vgCreateMaskLayer(VGint width, VGint height)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
	RI_IF_ERROR(width > RI_MAX_IMAGE_WIDTH || height > RI_MAX_IMAGE_HEIGHT || width*height > RI_MAX_IMAGE_PIXELS ||
				width*height > RI_MAX_IMAGE_BYTES, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
    Drawable* curr = context->getCurrentDrawable();
    if(!curr || !curr->getMaskBuffer())
        RI_RETURN(VG_INVALID_HANDLE);   //no current drawing surface

	Surface* layer = NULL;
	try
	{
		layer = RI_NEW(Surface, (Color::formatToDescriptor(VG_A_8), width, height, curr->getNumSamples()));	//throws bad_alloc
		RI_ASSERT(layer);
		context->m_maskLayerManager->addResource(layer, context);	//throws bad_alloc
        layer->clear(Color(1,1,1,1,Color::sRGBA), 0, 0, width, height);
		RI_RETURN((VGMaskLayer)layer);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(layer);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDestroyMaskLayer(VGMaskLayer maskLayer)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidMaskLayer(maskLayer), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid handle

	context->m_maskLayerManager->removeResource((Surface*)maskLayer);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgFillMaskLayer(VGMaskLayer maskLayer, VGint x, VGint y, VGint width, VGint height, VGfloat value)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidMaskLayer(maskLayer), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid handle
    RI_IF_ERROR(value < 0.0f || value > 1.0f, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Surface* layer = (Surface*)maskLayer;
    RI_IF_ERROR(width <= 0 || height <= 0 || x < 0 || y < 0 || x > layer->getWidth()-width || y > layer->getHeight()-height, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    layer->clear(Color(1,1,1,value,Color::sRGBA), x, y, width, height);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgCopyMask(VGMaskLayer maskLayer, VGint dx, VGint dy, VGint sx, VGint sy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidMaskLayer(maskLayer), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid handle
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable || !drawable->getMaskBuffer())
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment or context has no mask buffer
    }
    Surface* layer = (Surface*)maskLayer;
    RI_IF_ERROR(width <= 0 || height <= 0 || drawable->getNumSamples() != layer->getNumSamples(), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    try
    {   //copy drawing surface mask to mask layer
        layer->blit(drawable->getMaskBuffer(), sx, sy, dx, dy, width, height);	//throws bad_alloc
    }
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
    RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgClear(VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }

	try
	{
		if(context->m_scissoring)
			drawable->getColorBuffer()->clear(context->m_clearColor, x, y, width, height, context->m_scissor);	//throws bad_alloc
		else
			drawable->getColorBuffer()->clear(context->m_clearColor, x, y, width, height);
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGPath RI_APIENTRY vgCreatePath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(pathFormat != VG_PATH_FORMAT_STANDARD, VG_UNSUPPORTED_PATH_FORMAT_ERROR, VG_INVALID_HANDLE);
	RI_IF_ERROR(datatype < VG_PATH_DATATYPE_S_8 || datatype > VG_PATH_DATATYPE_F, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
	RIfloat s = inputFloat(scale);
	RIfloat b = inputFloat(bias);
	RI_IF_ERROR(s == 0.0f, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
	capabilities &= VG_PATH_CAPABILITY_ALL;	//undefined bits are ignored

	Path* path = NULL;
	try
	{
		path = RI_NEW(Path, (pathFormat, datatype, s, b, segmentCapacityHint, coordCapacityHint, capabilities));	//throws bad_alloc
		RI_ASSERT(path);
		context->m_pathManager->addResource(path, context);	//throws bad_alloc
		RI_RETURN((VGPath)path);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(path);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgClearPath(VGPath path, VGbitfield capabilities)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	capabilities &= VG_PATH_CAPABILITY_ALL;	//undefined bits are ignored
	((Path*)path)->clear(capabilities);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDestroyPath(VGPath path)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle

	context->m_pathManager->removeResource((Path*)path);

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgRemovePathCapabilities(VGPath path, VGbitfield capabilities)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	capabilities &= VG_PATH_CAPABILITY_ALL;	//undefined bits are ignored

	VGbitfield caps = ((Path*)path)->getCapabilities();
	caps &= ~capabilities;
	((Path*)path)->setCapabilities(caps);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGbitfield RI_APIENTRY vgGetPathCapabilities(VGPath path)
{
	RI_GET_CONTEXT(0);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, 0);	//invalid path handle
	VGbitfield ret = ((Path*)path)->getCapabilities();
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgAppendPathData(VGPath dstPath, VGint numSegments, const VGubyte * pathSegments, const void * pathData)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(dstPath), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	Path* p = (Path*)dstPath;
	RI_IF_ERROR(!(p->getCapabilities() & VG_PATH_CAPABILITY_APPEND_TO), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//no append cap
	RI_IF_ERROR(numSegments <= 0 || !pathSegments || !pathData, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//no segments or data
	RI_IF_ERROR((p->getDatatype() == VG_PATH_DATATYPE_S_16 && !isAligned(pathData,2)) ||
				((p->getDatatype() == VG_PATH_DATATYPE_S_32 || p->getDatatype() == VG_PATH_DATATYPE_F) && !isAligned(pathData,4)),
				VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid alignment
	for(int i=0;i<numSegments;i++)
	{
		VGPathSegment c = (VGPathSegment)(pathSegments[i] & 0x1e);
		RI_IF_ERROR(c < VG_CLOSE_PATH || c > VG_LCWARC_TO, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid segment
		RI_IF_ERROR(c & ~0x1f, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//reserved bits are nonzero
	}

	try
	{
		p->appendData((const RIuint8*)pathSegments, numSegments, (const RIuint8*)pathData);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgModifyPathCoords(VGPath dstPath, VGint startIndex, VGint numSegments, const void * pathData)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(dstPath), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	Path* p = (Path*)dstPath;
	RI_IF_ERROR(!(p->getCapabilities() & VG_PATH_CAPABILITY_MODIFY), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//no modify cap
	RI_IF_ERROR(!pathData || startIndex < 0 || numSegments <= 0 || RI_INT_ADDSATURATE(startIndex, numSegments) > p->getNumSegments(), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//no segments
	RI_IF_ERROR((p->getDatatype() == VG_PATH_DATATYPE_S_16 && !isAligned(pathData,2)) ||
				((p->getDatatype() == VG_PATH_DATATYPE_S_32 || p->getDatatype() == VG_PATH_DATATYPE_F) && !isAligned(pathData,4)),
				VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid alignment
	p->modifyCoords(startIndex, numSegments, (const RIuint8*)pathData);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgAppendPath(VGPath dstPath, VGPath srcPath)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(dstPath) || !context->isValidPath(srcPath), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!(((Path*)dstPath)->getCapabilities() & VG_PATH_CAPABILITY_APPEND_TO) ||
				!(((Path*)srcPath)->getCapabilities() & VG_PATH_CAPABILITY_APPEND_FROM), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//invalid caps

	try
	{
		((Path*)dstPath)->append((Path*)srcPath);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgTransformPath(VGPath dstPath, VGPath srcPath)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(dstPath) || !context->isValidPath(srcPath), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!(((Path*)dstPath)->getCapabilities() & VG_PATH_CAPABILITY_TRANSFORM_TO) ||
				!(((Path*)srcPath)->getCapabilities() & VG_PATH_CAPABILITY_TRANSFORM_FROM), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//invalid caps
	try
	{
		((Path*)dstPath)->transform((Path*)srcPath, context->m_pathUserToSurface);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static bool drawPath(VGContext* context, VGPath path, const Matrix3x3& userToSurfaceMatrix, VGbitfield paintModes)
{
	//set up rendering surface and mask buffer
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
        return false;   //no EGL surface is current at the moment

	Rasterizer rasterizer;
	if(context->m_scissoring)
		rasterizer.setScissor(context->m_scissor);	//throws bad_alloc
	int numSamples = rasterizer.setupSamplingPattern(context->m_renderingQuality, drawable->getNumSamples());

	PixelPipe pixelPipe;
	pixelPipe.setDrawable(drawable);
	pixelPipe.setMask(context->m_masking ? true : false);
	pixelPipe.setBlendMode(context->m_blendMode);
	pixelPipe.setTileFillColor(context->m_tileFillColor);
	pixelPipe.setImageQuality(context->m_imageQuality);
    pixelPipe.setColorTransform(context->m_colorTransform ? true : false, context->m_colorTransformValues);

	Matrix3x3 userToSurface = userToSurfaceMatrix;
	userToSurface[2].set(0,0,1);	//force affinity

	if(paintModes & VG_FILL_PATH)
	{
		pixelPipe.setPaint((Paint*)context->m_fillPaint);

		Matrix3x3 surfaceToPaintMatrix = userToSurface * context->m_fillPaintToUser;
		if(surfaceToPaintMatrix.invert())
		{
			surfaceToPaintMatrix[2].set(0,0,1);		//force affinity
			pixelPipe.setSurfaceToPaintMatrix(surfaceToPaintMatrix);

            rasterizer.setup(0, 0, drawable->getWidth(), drawable->getHeight(), context->m_fillRule, &pixelPipe, NULL);
			((Path*)path)->fill(userToSurface, rasterizer);	//throws bad_alloc
			rasterizer.fill();	//throws bad_alloc
		}
	}

	if(paintModes & VG_STROKE_PATH && context->m_strokeLineWidth > 0.0f)
	{
		pixelPipe.setPaint((Paint*)context->m_strokePaint);

		Matrix3x3 surfaceToPaintMatrix = userToSurface * context->m_strokePaintToUser;
		if(surfaceToPaintMatrix.invert())
		{
			surfaceToPaintMatrix[2].set(0,0,1);		//force affinity
			pixelPipe.setSurfaceToPaintMatrix(surfaceToPaintMatrix);

            renderStroke(context, drawable->getWidth(), drawable->getHeight(), numSamples, (Path*)path, rasterizer, &pixelPipe, userToSurface);
		}
	}
	return true;
}

void RI_APIENTRY vgDrawPath(VGPath path, VGbitfield paintModes)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!paintModes || (paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid paint mode

	try
	{
		if(!drawPath(context, path, context->m_pathUserToSurface, paintModes))
		{
			RI_RETURN(RI_NO_RETVAL);
		}
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGfloat RI_APIENTRY vgPathLength(VGPath path, VGint startSegment, VGint numSegments)
{
	RI_GET_CONTEXT(-1.0f);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, -1.0f);	//invalid path handle
	Path* p = (Path*)path;
	RI_IF_ERROR(!(p->getCapabilities() & VG_PATH_CAPABILITY_PATH_LENGTH), VG_PATH_CAPABILITY_ERROR, -1.0f);	//invalid caps
	RI_IF_ERROR(startSegment < 0 || numSegments <= 0 || RI_INT_ADDSATURATE(startSegment, numSegments) > p->getNumSegments(), VG_ILLEGAL_ARGUMENT_ERROR, -1.0f);
	RIfloat pathLength = -1.0f;
	try
	{
		pathLength = p->getPathLength(startSegment, numSegments);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(pathLength);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgPointAlongPath(VGPath path, VGint startSegment, VGint numSegments, VGfloat distance, VGfloat * x, VGfloat * y, VGfloat * tangentX, VGfloat * tangentY)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	Path* p = (Path*)path;
	RI_IF_ERROR((x && y && !(p->getCapabilities() & VG_PATH_CAPABILITY_POINT_ALONG_PATH)) ||
				(tangentX && tangentY && !(p->getCapabilities() & VG_PATH_CAPABILITY_TANGENT_ALONG_PATH)), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//invalid caps
	RI_IF_ERROR(startSegment < 0 || numSegments <= 0 || RI_INT_ADDSATURATE(startSegment, numSegments) > p->getNumSegments(), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!isAligned(x,4) || !isAligned(y,4) || !isAligned(tangentX,4) || !isAligned(tangentY,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	try
	{
		Vector2 point, tangent;
		p->getPointAlong(startSegment, numSegments, distance, point, tangent);	//throws bad_alloc
		if(x && y)
		{
			*x = point.x;
			*y = point.y;
		}
		if(tangentX && tangentY)
		{
			tangent.normalize();
			*tangentX = tangent.x;
			*tangentY = tangent.y;
		}
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgPathBounds(VGPath path, VGfloat * minx, VGfloat * miny, VGfloat * width, VGfloat * height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!(((Path*)path)->getCapabilities() & VG_PATH_CAPABILITY_PATH_BOUNDS), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//invalid caps
	RI_IF_ERROR(!minx || !miny || !width || !height, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!isAligned(minx,4) || !isAligned(miny,4) || !isAligned(width,4) || !isAligned(height,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	try
	{
		RIfloat pminx,pminy,pmaxx,pmaxy;
		((Path*)path)->getPathBounds(pminx, pminy, pmaxx, pmaxy);	//throws bad_alloc
		*minx = pminx;
		*miny = pminy;
		*width = pmaxx - pminx;
		*height = pmaxy - pminy;
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgPathTransformedBounds(VGPath path, VGfloat * minx, VGfloat * miny, VGfloat * width, VGfloat * height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
	RI_IF_ERROR(!(((Path*)path)->getCapabilities() & VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS), VG_PATH_CAPABILITY_ERROR, RI_NO_RETVAL);	//invalid caps
	RI_IF_ERROR(!minx || !miny || !width || !height, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!isAligned(minx,4) || !isAligned(miny,4) || !isAligned(width,4) || !isAligned(height,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	try
	{
		RIfloat pminx, pminy, pmaxx, pmaxy;
		((Path*)path)->getPathTransformedBounds(context->m_pathUserToSurface, pminx, pminy, pmaxx, pmaxy);	//throws bad_alloc
		*minx = pminx;
		*miny = pminy;
		*width = pmaxx - pminx;
		*height = pmaxy - pminy;
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGboolean RI_APIENTRY vgInterpolatePath(VGPath dstPath, VGPath startPath, VGPath endPath, VGfloat amount)
{
	RI_GET_CONTEXT(VG_FALSE);
	RI_IF_ERROR(!context->isValidPath(dstPath) || !context->isValidPath(startPath) || !context->isValidPath(endPath), VG_BAD_HANDLE_ERROR, VG_FALSE);	//invalid path handle
	RI_IF_ERROR(!(((Path*)dstPath)->getCapabilities() & VG_PATH_CAPABILITY_INTERPOLATE_TO) ||
				!(((Path*)startPath)->getCapabilities() & VG_PATH_CAPABILITY_INTERPOLATE_FROM) ||
				!(((Path*)endPath)->getCapabilities() & VG_PATH_CAPABILITY_INTERPOLATE_FROM), VG_PATH_CAPABILITY_ERROR, VG_FALSE);	//invalid caps
	VGboolean ret = VG_FALSE;
	try
	{
		if(((Path*)dstPath)->interpolate((const Path*)startPath, (const Path*)endPath, inputFloat(amount)))	//throws bad_alloc
			ret = VG_TRUE;
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGPaint RI_APIENTRY vgCreatePaint(void)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	Paint* paint = NULL;
	try
	{
		paint = RI_NEW(Paint, ());	//throws bad_alloc
		RI_ASSERT(paint);
		context->m_paintManager->addResource(paint, context);	//throws bad_alloc
		RI_RETURN((VGPaint)paint);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(paint);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDestroyPaint(VGPaint paint)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPaint(paint), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid paint handle

	context->m_paintManager->removeResource((Paint*)paint);

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetPaint(VGPaint paint, VGbitfield paintModes)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(paint && !context->isValidPaint(paint), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid paint handle
	RI_IF_ERROR(!paintModes || paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid paint mode

	context->releasePaint(paintModes);

	if(paintModes & VG_FILL_PATH)
	{
		if(paint)
			((Paint*)paint)->addReference();
		context->m_fillPaint = paint;
	}
	if(paintModes & VG_STROKE_PATH)
	{
		if(paint)
			((Paint*)paint)->addReference();
		context->m_strokePaint = paint;
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetColor(VGPaint paint, VGuint rgba)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPaint(paint), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid paint handle
	Paint* p = (Paint*)paint;
	p->m_inputPaintColor.unpack(rgba, Color::formatToDescriptor(VG_sRGBA_8888));
	p->m_paintColor = inputColor(p->m_inputPaintColor);
	p->m_paintColor.clamp();
	p->m_paintColor.premultiply();
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGuint RI_APIENTRY vgGetColor(VGPaint paint)
{
	RI_GET_CONTEXT(0);
	RI_IF_ERROR(!context->isValidPaint(paint), VG_BAD_HANDLE_ERROR, 0);	//invalid paint handle
	unsigned int ret = ((Paint*)paint)->m_inputPaintColor.pack(Color::formatToDescriptor(VG_sRGBA_8888));
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGPaint RI_APIENTRY vgGetPaint(VGPaintMode paintMode)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(paintMode != VG_FILL_PATH && paintMode != VG_STROKE_PATH, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);	//invalid paint mode

	if(paintMode == VG_FILL_PATH)
	{
		RI_RETURN(context->m_fillPaint);
	}
	RI_RETURN(context->m_strokePaint);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgPaintPattern(VGPaint paint, VGImage image)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidPaint(paint) || (image != VG_INVALID_HANDLE && !context->isValidImage(image)), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid handle
	Image* img = (Image*)image;
	Paint* pnt = (Paint*)paint;
	RI_IF_ERROR(image != VG_INVALID_HANDLE && eglvgIsInUse(img), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	Image* pattern = pnt->m_pattern;
	if(pattern)
	{
		pattern->removeInUse();
		if(!pattern->removeReference())
			RI_DELETE(pattern);
	}
	pnt->m_pattern = img;
	if(img)
	{
		img->addReference();
		img->addInUse();
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGImage RI_APIENTRY vgCreateImage(VGImageFormat format, VGint width, VGint height, VGbitfield allowedQuality)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(!isValidImageFormat(format), VG_UNSUPPORTED_IMAGE_FORMAT_ERROR, VG_INVALID_HANDLE);
	RI_IF_ERROR(width <= 0 || height <= 0 || !allowedQuality ||
				(allowedQuality & ~(VG_IMAGE_QUALITY_NONANTIALIASED | VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_BETTER)), VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
	RI_IF_ERROR(width > RI_MAX_IMAGE_WIDTH || height > RI_MAX_IMAGE_HEIGHT || width*height > RI_MAX_IMAGE_PIXELS ||
				((width*Color::formatToDescriptor(format).bitsPerPixel+7)/8)*height > RI_MAX_IMAGE_BYTES, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);

	Image* image = NULL;
	try
	{
		image = RI_NEW(Image, (Color::formatToDescriptor(format), width, height, allowedQuality));	//throws bad_alloc
		RI_ASSERT(image);
		context->m_imageManager->addResource(image, context);	//throws bad_alloc
		RI_RETURN((VGImage)image);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(image);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDestroyImage(VGImage image)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid image handle

	context->m_imageManager->removeResource((Image*)image);

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgClearImage(VGImage image, VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* img = (Image*)image;
	RI_IF_ERROR(eglvgIsInUse(img), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	img->clear(context->m_clearColor, x, y, width, height);
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgImageSubData(VGImage image, const void * data, VGint dataStride, VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* img = (Image*)image;
	RI_IF_ERROR(eglvgIsInUse(img), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!isValidImageFormat(dataFormat), VG_UNSUPPORTED_IMAGE_FORMAT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!data || !isAligned(data, dataFormat) || width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	{
		Image input(Color::formatToDescriptor(dataFormat), width, height, dataStride, const_cast<RIuint8*>((const RIuint8*)data));
		input.addReference();
		try
		{
			img->blit(input, 0, 0, x, y, width, height, false);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
		}
		input.removeReference();
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetImageSubData(VGImage image, void * data, VGint dataStride, VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* img = (Image*)image;
	RI_IF_ERROR(eglvgIsInUse(img), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!isValidImageFormat(dataFormat), VG_UNSUPPORTED_IMAGE_FORMAT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!data || !isAligned(data, dataFormat) || width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	{
		Image output(Color::formatToDescriptor(dataFormat), width, height, dataStride, (RIuint8*)data);
		output.addReference();
		try
		{
			output.blit(*img, x, y, 0, 0, width, height, false);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
		}
		output.removeReference();
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGImage RI_APIENTRY vgChildImage(VGImage parent, VGint x, VGint y, VGint width, VGint height)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(!context->isValidImage(parent), VG_BAD_HANDLE_ERROR, VG_INVALID_HANDLE);
	Image* p = (Image*)parent;
	RI_IF_ERROR(eglvgIsInUse((Image*)parent), VG_IMAGE_IN_USE_ERROR, VG_INVALID_HANDLE);
	RI_IF_ERROR(x < 0 || x >= p->getWidth() || y < 0 || y >= p->getHeight() ||
				width <= 0 || height <= 0 || RI_INT_ADDSATURATE(x, width) > p->getWidth() || RI_INT_ADDSATURATE(y, height) > p->getHeight(), VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);

	Image* child = NULL;
	try
	{
		child = RI_NEW(Image, (p, x, y, width, height));	//throws bad_alloc
		RI_ASSERT(child);
		context->m_imageManager->addResource(child, context);	//throws bad_alloc
		RI_RETURN((VGImage)child);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(child);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGImage RI_APIENTRY vgGetParent(VGImage image)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, VG_INVALID_HANDLE);
	VGImage ret = image;	//if image has no ancestors, image is returned.

    //The vgGetParent function returns the closest valid ancestor (i.e., one that has not been the target of a vgDestroyImage call)
    // of the given image.
	Image* im = ((Image*)image)->getParent();
    for(;im;im = im->getParent())
    {
		if(context->isValidImage((VGImage)im))
		{	//the parent is valid and alive
			ret = (VGImage)im;
            break;
		}
	}
	RI_RETURN(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgCopyImage(VGImage dst, VGint dx, VGint dy, VGImage src, VGint sx, VGint sy, VGint width, VGint height, VGboolean dither)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(eglvgIsInUse((Image*)dst) || eglvgIsInUse((Image*)src), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	try
	{
		((Image*)dst)->blit(*(Image*)src, sx, sy, dx, dy, width, height, dither ? true : false);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static bool drawImage(VGContext* context, VGImage image, const Matrix3x3& userToSurfaceMatrix)
{
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
        return false;   //no EGL surface is current at the moment

	Image* img = (Image*)image;
	//transform image corners into the surface space
	Vector3 p0(0, 0, 1);
	Vector3 p1(0, (RIfloat)img->getHeight(), 1);
	Vector3 p2((RIfloat)img->getWidth(), (RIfloat)img->getHeight(), 1);
	Vector3 p3((RIfloat)img->getWidth(), 0, 1);
	p0 = userToSurfaceMatrix * p0;
	p1 = userToSurfaceMatrix * p1;
	p2 = userToSurfaceMatrix * p2;
	p3 = userToSurfaceMatrix * p3;
	if(p0.z <= 0.0f || p1.z <= 0.0f || p2.z <= 0.0f || p3.z <= 0.0f)
		return false;

	//projection
	p0 *= 1.0f/p0.z;
	p1 *= 1.0f/p1.z;
	p2 *= 1.0f/p2.z;
	p3 *= 1.0f/p3.z;

	Rasterizer rasterizer;
	if(context->m_scissoring)
		rasterizer.setScissor(context->m_scissor);	//throws bad_alloc
	rasterizer.setupSamplingPattern(context->m_renderingQuality, drawable->getNumSamples());

	PixelPipe pixelPipe;
	pixelPipe.setTileFillColor(context->m_tileFillColor);
	pixelPipe.setPaint((Paint*)context->m_fillPaint);
	pixelPipe.setImageQuality(context->m_imageQuality);
	pixelPipe.setBlendMode(context->m_blendMode);
	pixelPipe.setDrawable(drawable);
	pixelPipe.setMask(context->m_masking ? true : false);
    pixelPipe.setColorTransform(context->m_colorTransform ? true : false, context->m_colorTransformValues);

	Matrix3x3 surfaceToImageMatrix = userToSurfaceMatrix;
	Matrix3x3 surfaceToPaintMatrix = userToSurfaceMatrix * context->m_fillPaintToUser;
	if(surfaceToImageMatrix.invert() && surfaceToPaintMatrix.invert())
	{
		VGImageMode imode = context->m_imageMode;
		if(!surfaceToPaintMatrix.isAffine())
			imode = VG_DRAW_IMAGE_NORMAL;	//if paint matrix is not affine, always use normal image mode
		surfaceToPaintMatrix[2].set(0,0,1);	//force affine

		pixelPipe.setImage(img, imode);
		pixelPipe.setSurfaceToPaintMatrix(surfaceToPaintMatrix);
		pixelPipe.setSurfaceToImageMatrix(surfaceToImageMatrix);

		rasterizer.addEdge(Vector2(p0.x,p0.y), Vector2(p1.x,p1.y));	//throws bad_alloc
		rasterizer.addEdge(Vector2(p1.x,p1.y), Vector2(p2.x,p2.y));	//throws bad_alloc
		rasterizer.addEdge(Vector2(p2.x,p2.y), Vector2(p3.x,p3.y));	//throws bad_alloc
		rasterizer.addEdge(Vector2(p3.x,p3.y), Vector2(p0.x,p0.y));	//throws bad_alloc
        rasterizer.setup(0, 0, drawable->getWidth(), drawable->getHeight(), VG_EVEN_ODD, &pixelPipe, NULL);
		rasterizer.fill();	//throws bad_alloc
	}
	return true;
}

void RI_APIENTRY vgDrawImage(VGImage image)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* img = (Image*)image;
	RI_IF_ERROR(eglvgIsInUse(img), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);

	try
	{
		if(!drawImage(context, image, context->m_imageUserToSurface))
		{
			RI_RETURN(RI_NO_RETVAL);
		}
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetPixels(VGint dx, VGint dy, VGImage src, VGint sx, VGint sy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(eglvgIsInUse((Image*)src), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }
	try
	{
		if(context->m_scissoring)
			drawable->getColorBuffer()->blit(*(Image*)src, sx, sy, dx, dy, width, height, context->m_scissor);	//throws bad_alloc
		else
			drawable->getColorBuffer()->blit(*(Image*)src, sx, sy, dx, dy, width, height);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgWritePixels(const void * data, VGint dataStride, VGImageFormat dataFormat, VGint dx, VGint dy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!isValidImageFormat(dataFormat), VG_UNSUPPORTED_IMAGE_FORMAT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!data || !isAligned(data, dataFormat) || width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }
	{
		Image input(Color::formatToDescriptor(dataFormat), width, height, dataStride, const_cast<RIuint8*>((const RIuint8*)data));
		input.addReference();
		try
		{
			if(context->m_scissoring)
				drawable->getColorBuffer()->blit(input, 0, 0, dx, dy, width, height, context->m_scissor);	//throws bad_alloc
			else
				drawable->getColorBuffer()->blit(input, 0, 0, dx, dy, width, height);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
		}
		input.removeReference();
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGetPixels(VGImage dst, VGint dx, VGint dy, VGint sx, VGint sy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(eglvgIsInUse((Image*)dst), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }
	try
	{
		((Image*)dst)->blit(drawable->getColorBuffer(), sx, sy, dx, dy, width, height);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgReadPixels(void* data, VGint dataStride, VGImageFormat dataFormat, VGint sx, VGint sy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!isValidImageFormat(dataFormat), VG_UNSUPPORTED_IMAGE_FORMAT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!data || !isAligned(data, dataFormat) || width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }
	{
		Image output(Color::formatToDescriptor(dataFormat), width, height, dataStride, (RIuint8*)data);
		output.addReference();
		try
		{
			output.blit(drawable->getColorBuffer(), sx, sy, 0, 0, width, height);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
		}
		output.removeReference();
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgCopyPixels(VGint dx, VGint dy, VGint sx, VGint sy, VGint width, VGint height)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(width <= 0 || height <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
    Drawable* drawable = context->getCurrentDrawable();
    if(!drawable)
    {
        RI_RETURN(RI_NO_RETVAL);	//no EGL surface is current at the moment
    }
	try
	{
		if(context->m_scissoring)
			drawable->getColorBuffer()->blit(drawable->getColorBuffer(), sx, sy, dx, dy, width, height, context->m_scissor);	//throws bad_alloc
		else
			drawable->getColorBuffer()->blit(drawable->getColorBuffer(), sx, sy, dx, dy, width, height);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgColorMatrix(VGImage dst, VGImage src, const VGfloat * matrix)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!matrix || !isAligned(matrix,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored

	RIfloat m[20];
	for(int i=0;i<20;i++)
	{
		m[i] = inputFloat(matrix[i]);
	}
	try
	{
		d->colorMatrix(*s, m, context->m_filterFormatLinear ? true : false, context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgConvolve(VGImage dst, VGImage src, VGint kernelWidth, VGint kernelHeight, VGint shiftX, VGint shiftY, const VGshort * kernel, VGfloat scale, VGfloat bias, VGTilingMode tilingMode)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!kernel || !isAligned(kernel,2) || kernelWidth <= 0 || kernelHeight <= 0 || kernelWidth > RI_MAX_KERNEL_SIZE || kernelHeight > RI_MAX_KERNEL_SIZE, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(tilingMode < VG_TILE_FILL || tilingMode > VG_TILE_REFLECT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored
	try
	{
		d->convolve(*s, kernelWidth, kernelHeight, shiftX, shiftY, (const RIint16*)kernel, inputFloat(scale), inputFloat(bias), tilingMode, context->m_tileFillColor, context->m_filterFormatLinear ? true : false, context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSeparableConvolve(VGImage dst, VGImage src, VGint kernelWidth, VGint kernelHeight, VGint shiftX, VGint shiftY, const VGshort * kernelX, const VGshort * kernelY, VGfloat scale, VGfloat bias, VGTilingMode tilingMode)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!kernelX || !kernelY || !isAligned(kernelX,2) || !isAligned(kernelY,2) || kernelWidth <= 0 || kernelHeight <= 0 || kernelWidth > RI_MAX_SEPARABLE_KERNEL_SIZE || kernelHeight > RI_MAX_SEPARABLE_KERNEL_SIZE, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(tilingMode < VG_TILE_FILL || tilingMode > VG_TILE_REFLECT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored
	try
	{
		d->separableConvolve(*s, kernelWidth, kernelHeight, shiftX, shiftY, (const RIint16*)kernelX, (const RIint16*)kernelY,
										 inputFloat(scale), inputFloat(bias), tilingMode, context->m_tileFillColor, context->m_filterFormatLinear ? true : false,
										 context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgGaussianBlur(VGImage dst, VGImage src, VGfloat stdDeviationX, VGfloat stdDeviationY, VGTilingMode tilingMode)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RIfloat sx = inputFloat(stdDeviationX);
	RIfloat sy = inputFloat(stdDeviationY);
	RI_IF_ERROR(sx <= 0.0f || sy <= 0.0f || sx > (RIfloat)RI_MAX_GAUSSIAN_STD_DEVIATION || sy > (RIfloat)RI_MAX_GAUSSIAN_STD_DEVIATION, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(tilingMode < VG_TILE_FILL || tilingMode > VG_TILE_REFLECT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored
	try
	{
		d->gaussianBlur(*s, sx, sy, tilingMode, context->m_tileFillColor, context->m_filterFormatLinear ? true : false,
						context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgLookup(VGImage dst, VGImage src, const VGubyte * redLUT, const VGubyte * greenLUT, const VGubyte * blueLUT, const VGubyte * alphaLUT, VGboolean outputLinear, VGboolean outputPremultiplied)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!redLUT || !greenLUT || !blueLUT || !alphaLUT, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored
	try
	{
		d->lookup(*s, (const RIuint8*)redLUT, (const RIuint8*)greenLUT, (const RIuint8*)blueLUT, (const RIuint8*)alphaLUT,
				  outputLinear ? true : false, outputPremultiplied ? true : false, context->m_filterFormatLinear ? true : false,
				  context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgLookupSingle(VGImage dst, VGImage src, const VGuint * lookupTable, VGImageChannel sourceChannel, VGboolean outputLinear, VGboolean outputPremultiplied)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidImage(dst) || !context->isValidImage(src), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);
	Image* d = (Image*)dst;
	Image* s = (Image*)src;
	RI_IF_ERROR(eglvgIsInUse(d) || eglvgIsInUse(s), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(d->overlaps(s), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(!lookupTable || !isAligned(lookupTable,4), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	const Color::Descriptor& desc = s->getDescriptor();
	RI_ASSERT(Color::isValidDescriptor(desc));
	//give an error if src is in rgb format and the source channel is not valid
	RI_IF_ERROR((!desc.isLuminance() && !desc.isAlphaOnly()) && (sourceChannel != VG_RED && sourceChannel != VG_GREEN && sourceChannel != VG_BLUE && sourceChannel != VG_ALPHA), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	unsigned int channelMask = context->m_filterChannelMask & (VG_RED|VG_GREEN|VG_BLUE|VG_ALPHA);	//undefined bits are ignored
	try
	{
		d->lookupSingle(*s, (const RIuint32*)lookupTable, sourceChannel, outputLinear ? true : false, outputPremultiplied ? true : false,
						context->m_filterFormatLinear ? true : false, context->m_filterFormatPremultiplied ? true : false, channelMask);
	}
	catch(std::bad_alloc)
	{
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGHardwareQueryResult RI_APIENTRY vgHardwareQuery(VGHardwareQueryType key, VGint setting)
{
	RI_GET_CONTEXT(VG_HARDWARE_UNACCELERATED);
	RI_IF_ERROR(key != VG_IMAGE_FORMAT_QUERY && key != VG_PATH_DATATYPE_QUERY, VG_ILLEGAL_ARGUMENT_ERROR, VG_HARDWARE_UNACCELERATED);
	RI_IF_ERROR(key == VG_IMAGE_FORMAT_QUERY && !isValidImageFormat(setting), VG_ILLEGAL_ARGUMENT_ERROR, VG_HARDWARE_UNACCELERATED);
	RI_IF_ERROR(key == VG_PATH_DATATYPE_QUERY && (setting < VG_PATH_DATATYPE_S_8 || setting > VG_PATH_DATATYPE_F), VG_ILLEGAL_ARGUMENT_ERROR, VG_HARDWARE_UNACCELERATED);
	RI_RETURN(VG_HARDWARE_UNACCELERATED);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

const VGubyte * RI_APIENTRY vgGetString(VGStringID name)
{
	static const VGubyte vendor[] = "Khronos Group";
	static const VGubyte renderer[] = {"OpenVG 1.1 Reference Implementation May 13 2008"};
	static const VGubyte version[] = "1.1";
	static const VGubyte extensions[] = "";
	const VGubyte* r = NULL;
	RI_GET_CONTEXT(NULL);
	switch(name)
	{
	case VG_VENDOR:
		r = vendor;
		break;
	case VG_RENDERER:
		r = renderer;
		break;
	case VG_VERSION:
		r = version;
		break;
	case VG_EXTENSIONS:
		r = extensions;
		break;
	default:
		break;
	}
	RI_RETURN(r);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

VGFont RI_APIENTRY vgCreateFont(VGint glyphCapacityHint)
{
	RI_GET_CONTEXT(VG_INVALID_HANDLE);
	RI_IF_ERROR(glyphCapacityHint < 0, VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);

	Font* font = NULL;
	try
	{
		font = RI_NEW(Font, (glyphCapacityHint));	//throws bad_alloc
		RI_ASSERT(font);
		context->m_fontManager->addResource(font, context);	//throws bad_alloc
		RI_RETURN((VGFont)font);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(font);
		context->setError(VG_OUT_OF_MEMORY_ERROR);
		RI_RETURN(VG_INVALID_HANDLE);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDestroyFont(VGFont font)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle

	context->m_fontManager->removeResource((Font*)font);

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetGlyphToPath(VGFont font, VGuint glyphIndex, VGPath path, VGboolean isHinted, VGfloat glyphOrigin[2], VGfloat escapement[2])
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle
	RI_IF_ERROR(path != VG_INVALID_HANDLE && !context->isValidPath(path), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid path handle
    RI_IF_ERROR(!glyphOrigin || !escapement || !isAligned(glyphOrigin,sizeof(VGfloat)) || !isAligned(escapement,sizeof(VGfloat)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	Font* f = (Font*)font;

	try
	{
        f->setGlyphToPath(glyphIndex, path, isHinted ? true : false, Vector2(inputFloat(glyphOrigin[0]), inputFloat(glyphOrigin[1])), Vector2(inputFloat(escapement[0]), inputFloat(escapement[1])));
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgSetGlyphToImage(VGFont font, VGuint glyphIndex, VGImage image, VGfloat glyphOrigin[2], VGfloat escapement[2])
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle
    if(image != VG_INVALID_HANDLE)
    {
        RI_IF_ERROR(!context->isValidImage(image), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid image handle
        RI_IF_ERROR(eglvgIsInUse((Image*)image), VG_IMAGE_IN_USE_ERROR, RI_NO_RETVAL); //image in use
    }
    RI_IF_ERROR(!glyphOrigin || !escapement || !isAligned(glyphOrigin,sizeof(VGfloat)) || !isAligned(escapement,sizeof(VGfloat)), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	Font* f = (Font*)font;

	try
	{
        f->setGlyphToImage(glyphIndex, image, Vector2(inputFloat(glyphOrigin[0]), inputFloat(glyphOrigin[1])), Vector2(inputFloat(escapement[0]), inputFloat(escapement[1])));
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}
	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgClearGlyph(VGFont font, VGuint glyphIndex)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle
	Font* f = (Font*)font;
    Font::Glyph* g = f->findGlyph(glyphIndex);
    RI_IF_ERROR(!g, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);   //glyphIndex not defined

	f->clearGlyph(g);

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDrawGlyph(VGFont font, VGuint glyphIndex, VGbitfield paintModes, VGboolean allowAutoHinting)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle
	RI_IF_ERROR(paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid paint mode
	Font* f = (Font*)font;
    Font::Glyph* g = f->findGlyph(glyphIndex);
    RI_IF_ERROR(!g, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);   //glyphIndex not defined
    RI_UNREF(allowAutoHinting); //RI doesn't implement autohinting

	try
	{
        if(paintModes)
        {
            Matrix3x3 userToSurfaceMatrix = context->m_glyphUserToSurface;
            Vector2 t = context->m_glyphOrigin - g->m_origin;
            Matrix3x3 n(1, 0, t.x,
                        0, 1, t.y,
                        0, 0, 1 );
            userToSurfaceMatrix *= n;
            userToSurfaceMatrix[2].set(0,0,1);		//force affinity

            bool ret = true;
            if(g->m_image != VG_INVALID_HANDLE)
                ret = drawImage(context, g->m_image, userToSurfaceMatrix);
            else if(g->m_path != VG_INVALID_HANDLE)
                ret = drawPath(context, g->m_path, userToSurfaceMatrix, paintModes);
            if(!ret)
            {
                RI_RETURN(RI_NO_RETVAL);
            }
        }

        context->m_glyphOrigin += g->m_escapement;
        context->m_inputGlyphOrigin = context->m_glyphOrigin;
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}

	RI_RETURN(RI_NO_RETVAL);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void RI_APIENTRY vgDrawGlyphs(VGFont font, VGint glyphCount, VGuint *glyphIndices, VGfloat *adjustments_x, VGfloat *adjustments_y, VGbitfield paintModes, VGboolean allowAutoHinting)
{
	RI_GET_CONTEXT(RI_NO_RETVAL);
	RI_IF_ERROR(!context->isValidFont(font), VG_BAD_HANDLE_ERROR, RI_NO_RETVAL);	//invalid font handle
	RI_IF_ERROR(!glyphIndices || !isAligned(glyphIndices, sizeof(VGuint)) || glyphCount <= 0, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR((adjustments_x && !isAligned(adjustments_x, sizeof(VGfloat))) || (adjustments_y && !isAligned(adjustments_y, sizeof(VGfloat))), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);
	RI_IF_ERROR(paintModes & ~(VG_FILL_PATH | VG_STROKE_PATH), VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);	//invalid paint mode
	Font* f = (Font*)font;
	for(int i=0;i<glyphCount;i++)
	{
        Font::Glyph* g = f->findGlyph(glyphIndices[i]);
        RI_IF_ERROR(!g, VG_ILLEGAL_ARGUMENT_ERROR, RI_NO_RETVAL);   //glyphIndex not defined
    }
    RI_UNREF(allowAutoHinting); //RI doesn't implement autohinting

	try
	{
		for(int i=0;i<glyphCount;i++)
		{
            Font::Glyph* g = f->findGlyph(glyphIndices[i]);

            if(paintModes)
            {
                Matrix3x3 userToSurfaceMatrix = context->m_glyphUserToSurface;
                Vector2 t = context->m_glyphOrigin - g->m_origin;
                Matrix3x3 n(1, 0, t.x,
                            0, 1, t.y,
                            0, 0, 1 );
                userToSurfaceMatrix *= n;
                userToSurfaceMatrix[2].set(0,0,1);		//force affinity

                bool ret = true;
                if(g->m_image != VG_INVALID_HANDLE)
                    ret = drawImage(context, g->m_image, userToSurfaceMatrix);
                else if(g->m_path != VG_INVALID_HANDLE)
                    ret = drawPath(context, g->m_path, userToSurfaceMatrix, paintModes);
                if(!ret)
                {
                    RI_RETURN(RI_NO_RETVAL);
                }
            }

            context->m_glyphOrigin += g->m_escapement;
            if(adjustments_x)
                context->m_glyphOrigin.x += inputFloat(adjustments_x[i]);
            if(adjustments_y)
                context->m_glyphOrigin.y += inputFloat(adjustments_y[i]);
            context->m_inputGlyphOrigin = context->m_glyphOrigin;
		}
	}
	catch(std::bad_alloc)
	{
		context->setError(VG_OUT_OF_MEMORY_ERROR);
	}

	RI_RETURN(RI_NO_RETVAL);
}
