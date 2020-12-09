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
#include "src/display/display.h"

#if WITHOPENVG

#if 0

#include "egl.h"
#include "riImage.h"

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



static EGLDisplay			egldisplay;
static EGLConfig			eglconfig;
static EGLSurface			eglsurface;
static EGLContext			eglcontext;


/*

EGLClientBuffer * getClientImage(void)
{
//	Image
	return NULL;

}
*/
#if _BYTE_ORDER == _LITTLE_ENDIAN
static int isBigEndian(void) { return 0; }
#else
static int isBigEndian(void) { return 1; }
#endif

static EGLNativePixmapType getClientPixmap(void)
{
	static NativePixmap pixmap;

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

	pixmap.data = colmain_fb_draw();
	pixmap.format = f;
	pixmap.height = DIM_Y;
	pixmap.width = DIM_X;
	pixmap.stride = GXADJ(DIM_X) * sizeof (PACKEDCOLORMAIN_T);

	return & pixmap;

}

void openvg_init(void * window)
{
	static const EGLint s_configAttribs[] =
	{
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_LUMINANCE_SIZE, EGL_DONT_CARE,			//EGL_DONT_CARE
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_SAMPLES,		1,
		EGL_NONE
	};
	EGLint numconfigs;

	egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(egldisplay, NULL, NULL);
	ASSERT(eglGetError() == EGL_SUCCESS);
	eglBindAPI(EGL_OPENVG_API);

	eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
	ASSERT(eglGetError() == EGL_SUCCESS);
	ASSERT(numconfigs == 1);
	EGLNativePixmapType pixmap;
	//eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, window, NULL);
	//eglsurface = eglCreatePbufferFromClientBuffer(egldisplay, EGL_OPENVG_IMAGE, (EGLClientBuffer) getClientImage(), eglconfig, s_configAttribs);
	eglsurface = eglCreatePixmapSurface(egldisplay, eglconfig, getClientPixmap(), s_configAttribs);
	ASSERT(eglGetError() == EGL_SUCCESS);
	eglcontext = eglCreateContext(egldisplay, eglconfig, NULL, NULL);
	ASSERT(eglGetError() == EGL_SUCCESS);
	eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
	ASSERT(eglGetError() == EGL_SUCCESS);

}

/*--------------------------------------------------------------*/

void openvg_deinit(void)
{
	eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	ASSERT(eglGetError() == EGL_SUCCESS);
	eglTerminate(egldisplay);
	ASSERT(eglGetError() == EGL_SUCCESS);
	eglReleaseThread();
}

#else

#include "riContext.h"


namespace OpenVGRI
{
	void OSDeinitMutex(void)
	{
	}

	void OSAcquireMutex(void)
	{
	}

	void OSReleaseMutex(void)
	{
	}

	void* OSGetCurrentThreadID(void)
	{
		return NULL;
	}

	static Drawable * d0;
	static VGContext ctx(NULL);

	void* eglvgGetCurrentVGContext(void)
	{
		return & ctx;
	}

	bool eglvgIsInUse(void*)
	{
		return false;
	}
}   //namespace OpenVGRI

#if _BYTE_ORDER == _LITTLE_ENDIAN
static int isBigEndian(void) { return 0; }
#else
static int isBigEndian(void) { return 1; }
#endif

void openvg_init(void * window)
{

#if LCDMODE_MAIN_RGB565
	VGImageFormat f = VG_sRGB_565;
	if (isBigEndian())
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

	try
	{
		int maskBits = 0;
		OpenVGRI::d0 = RI_NEW(OpenVGRI::Drawable, (OpenVGRI::Color::formatToDescriptor(f), DIM_X, DIM_Y, GXADJ(DIM_X) * sizeof (PACKEDCOLORMAIN_T), (OpenVGRI::RIuint8 *) colmain_fb_draw(), maskBits));	//throws bad_alloc
	}
	catch(const std::bad_alloc &)
	{
	}
	RI_ASSERT(d0);
	OpenVGRI::ctx.setDefaultDrawable(OpenVGRI::d0);
}

void openvg_deinit(void)
{
	RI_DELETE(OpenVGRI::d0);
}

#endif

#endif /* WITHOPENVG */
