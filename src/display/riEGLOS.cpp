/*------------------------------------------------------------------------
 *
 * EGL 1.3
 * -------
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
 * \brief	Generic OS EGL functionality (not thread safe, no window rendering)
 * \note
  *//*-------------------------------------------------------------------*/

#include "hardware.h"

#if 1//WITHOPENVG

#include "egl.h"
#include "riImage.h"

#include "src/display/display.h"
#include "display2.h"

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void* OSGetCurrentThreadID(void)
{
	return NULL;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static int mutexRefCount = 0;
static bool mutexInitialized = false;
//acquired mutex cannot be deinited
void OSDeinitMutex(void)
{
	RI_ASSERT(mutexInitialized);
	RI_ASSERT(mutexRefCount == 0);
}
void OSAcquireMutex(void)
{
	if(!mutexInitialized)
    {
        mutexInitialized = true;
    }
	mutexRefCount++;
}
void OSReleaseMutex(void)
{
	RI_ASSERT(mutexInitialized);
	mutexRefCount--;
	RI_ASSERT(mutexRefCount >= 0);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

struct OSWindowContext
{
    int                 tmpWidth;
    int                 tmpHeight;
};

void* OSCreateWindowContext(EGLNativeWindowType window)
{
    OSWindowContext* ctx = NULL;
    try
    {
        ctx = RI_NEW(OSWindowContext, ());
    }
	catch(const std::bad_alloc &)
	{
		return NULL;
	}
    ctx->tmpWidth = DIM_X;
    ctx->tmpHeight = DIM_Y;
    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        RI_DELETE(ctx);
    }
}

bool OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        return true;
    }
    return false;
}

void OSGetWindowSize(const void* context, int& width, int& height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        width = ctx->tmpWidth;
        height = ctx->tmpHeight;
    }
    else
    {
        width = 0;
        height = 0;
    }
}
//
//static bool isBigEndian()
//{
//	static const RIuint32 v = 0x12345678u;
//	const RIuint8* p = (const RIuint8*)&v;
//	RI_ASSERT (*p == (RIuint8)0x12u || *p == (RIuint8)0x78u);
//	return (*p == (RIuint8)(0x12)) ? true : false;
//}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
#if 0
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
		ctx->tmpWidth = drawable->getWidth();
		ctx->tmpHeight = drawable->getHeight();
		int w = drawable->getWidth();
		int h = drawable->getHeight();
	#if LCDMODE_MAIN_RGB565
		VGImageFormat f = VG_sRGB_565;
		if(isBigEndian())
			f = VG_sBGR_565;
	#elif LCDMODE_MAIN_ARGB888
		VGImageFormat f = VG_sARGB_8888;	// 4-th byte alpha value
		 if(isBigEndian())
			 f = VG_sBGRA_8888;
	#elif LCDMODE_MAIN_L8
		VGImageFormat f = VG_sL_8;
	#else
		#error Unsupported video format
	#endif
		vgReadPixels(fr, w * sizeof (* fr), f, 0, 0, w, h);
#endif
		display_flush();
		//PRINTF("OSBlitToWindow: tmpWidth=%d, tmpHeight=%d\n", ctx->tmpWidth, ctx->tmpHeight);
		//display_fillrect(x, y, x2 - x, y2 - y, color);
    }
}

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    RI_UNREF(display_id);
    return (EGLDisplay)1;    //support only a single display
}

}   //namespace OpenVGRI

#endif
