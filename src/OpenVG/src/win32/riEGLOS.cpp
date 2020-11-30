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
 * \brief	Simple implementation of EGL 1.3
 * \note	caveats:
			- always renders into the backbuffer and blits it to window (no single buffered rendering)
			- no native Windows or Mac OS X pixmap support
			- no power management events
			- no support for swap interval
 * \todo	what happens in egl functions when eglTerminate has been called but the context and surface are still in use?
 * \todo	OSDeinitMutex should be called in case getEGL fails.
 * \todo	clarify getThread and getCurrentThread distinction.
 *//*-------------------------------------------------------------------*/

#if defined(WIN32)

#include "egl.h"
#include "openvg.h"
#include "riArray.h"
#include "riMath.h"
#include "riContext.h"
#include "riImage.h"

namespace OpenVGRI
{

#include <windows.h>

void* OSGetCurrentThreadID(void)
{
	return (void*)GetCurrentThreadId();	//TODO this is not safe
}

static HANDLE mutex = NULL;
static int mutexRefCount = 0;
//acquired mutex cannot be deinited.
void OSDeinitMutex(void)
{
	RI_ASSERT(mutex);
	RI_ASSERT(mutexRefCount == 0);
	BOOL ret = CloseHandle(mutex);
	RI_ASSERT(ret);
	RI_UNREF(ret);
}
void OSAcquireMutex(void)
{
	if(!mutex)
    {
        mutex = CreateMutex(NULL, FALSE, NULL);	//initially not locked
        mutexRefCount = 0;
    }
	RI_ASSERT(mutex);
	DWORD ret = WaitForSingleObject(mutex, INFINITE);
	RI_ASSERT(ret != WAIT_FAILED);
	RI_UNREF(ret);
	mutexRefCount++;
}
void OSReleaseMutex(void)
{
	RI_ASSERT(mutex);
	mutexRefCount--;
	RI_ASSERT(mutexRefCount >= 0);
	BOOL ret = ReleaseMutex(mutex);
	RI_ASSERT(ret);
	RI_UNREF(ret);
}

static bool isBigEndian()
{
	static const RIuint32 v = 0x12345678u;
	const RIuint8* p = (const RIuint8*)&v;
	RI_ASSERT (*p == (RIuint8)0x12u || *p == (RIuint8)0x78u);
	return (*p == (RIuint8)(0x12)) ? true : false;
}


#ifdef RI_USE_GLUT
#	include <GL/gl.h>
#	define GLUT_DISABLE_ATEXIT_HACK
#	include "glut.h"

struct OSWindowContext
{
    int                 window;
    unsigned int*       tmp;
    int                 tmpWidth;
    int                 tmpHeight;
};

void* OSCreateWindowContext(EGLNativeWindowType window)
{
    try
    {
        OSWindowContext* ctx = RI_NEW(OSWindowContext, ());
        ctx->window = (int)window;
        ctx->tmp = NULL;
        ctx->tmpWidth = 0;
        ctx->tmpHeight = 0;
        return ctx;
    }
	catch(std::bad_alloc)
	{
		return NULL;
	}
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        RI_DELETE_ARRAY(ctx->tmp);
        RI_DELETE(ctx);
    }
}

bool OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
		//TODO implement
        return true;
    }
    return false;
}

void OSGetWindowSize(const void* context, int& width, int& height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        int currWin = glutGetWindow();
        glutSetWindow(ctx->window);
        width = glutGet(GLUT_WINDOW_WIDTH);
        height = glutGet(GLUT_WINDOW_HEIGHT);
        glutSetWindow(currWin);
    }
    else
    {
        width = 0;
        height = 0;
    }
}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        int w = drawable->getWidth();
        int h = drawable->getHeight();

        int currWin = glutGetWindow();
        glutSetWindow(ctx->window);

        if(!ctx->tmp || ctx->tmpWidth != w || ctx->tmpHeight != h)
        {
            RI_DELETE_ARRAY(ctx->tmp);
            ctx->tmp = NULL;
            try
            {
                ctx->tmp = RI_NEW_ARRAY(unsigned int, w*h);	//throws bad_alloc
                ctx->tmpWidth = w;
                ctx->tmpHeight = h;
            }
            catch(std::bad_alloc)
            {
                //do nothing
            }
        }

        if(ctx->tmp)
        {
            glViewport(0, 0, w, h);
            glDisable(GL_DEPTH_TEST);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            //NOTE: we assume here that the display is always in sRGB color space
            VGImageFormat f = VG_sXBGR_8888;
            if(isBigEndian())
                f = VG_sRGBX_8888;
            vgReadPixels(ctx->tmp, w*sizeof(unsigned int), f, 0, 0, w, h);
            glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, ctx->tmp);
        }

        glutSwapBuffers();	//shows the OpenGL frame buffer
        glutSetWindow(currWin);		//restore the current window
    }
}

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    return (EGLDisplay)display_id;  //just casting to (EGLDisplay) should be enough to make a unique identifier out of this
}

#else
//Windows native
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct OSWindowContext
{
    HWND                window;
	HDC					bufDC;
	HBITMAP				bufDIB;
    unsigned int*       tmp;
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
	catch(std::bad_alloc)
	{
		return NULL;
	}

    ctx->window = (HWND)window;
    HDC winDC = GetDC(ctx->window);
    ctx->bufDC = CreateCompatibleDC(winDC);
    ReleaseDC(ctx->window, winDC);
    if(!ctx->bufDC)
    {
        RI_DELETE(ctx);
        return NULL;
    }

    ctx->bufDIB = NULL;
    ctx->tmp = NULL;
    ctx->tmpWidth = 0;
    ctx->tmpHeight = 0;
    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        if(ctx->bufDC)
        {
            SelectObject(ctx->bufDC, NULL);
            DeleteDC(ctx->bufDC);
        }
        if(ctx->bufDIB)
            DeleteObject(ctx->bufDIB);
        RI_DELETE(ctx);
    }
}

bool OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
		if(IsWindow(ctx->window))
			return true;
    }
    return false;
}

void OSGetWindowSize(const void* context, int& width, int& height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
		RECT rect;
		GetClientRect(ctx->window, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
    }
    else
    {
        width = 0;
        height = 0;
    }
}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        int w = drawable->getWidth();
        int h = drawable->getHeight();

        if(!ctx->tmp || !ctx->bufDIB || ctx->tmpWidth != w || ctx->tmpHeight != h)
        {
            if(ctx->bufDIB)
                DeleteObject(ctx->bufDIB);
            ctx->tmp = NULL;
            ctx->bufDIB = NULL;

            ctx->tmpWidth = w;
            ctx->tmpHeight = h;

            struct
            {
                BITMAPINFOHEADER	header;
                DWORD				rMask;
                DWORD				gMask;
                DWORD				bMask;
            } bmi;
            bmi.header.biSize			= sizeof(BITMAPINFOHEADER);
            bmi.header.biWidth			= w;
            bmi.header.biHeight			= h;
            bmi.header.biPlanes			= 1;
            bmi.header.biBitCount		= (WORD)32;
            bmi.header.biCompression	= BI_BITFIELDS;
            bmi.rMask = 0x000000ff;
            bmi.gMask = 0x0000ff00;
            bmi.bMask = 0x00ff0000;
            ctx->bufDIB = CreateDIBSection(ctx->bufDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, (void**)&ctx->tmp, NULL, 0);
            if(!ctx->bufDIB)
            {
                ctx->tmp = NULL;
                return;
            }
        }

        if(ctx->tmp)
        {
            //NOTE: we assume here that the display is always in sRGB color space
			GdiFlush();
            VGImageFormat f = VG_sXBGR_8888;
            if(isBigEndian())
                f = VG_sRGBX_8888;
            vgReadPixels(ctx->tmp, w*sizeof(unsigned int), f, 0, 0, w, h);

            SelectObject(ctx->bufDC, ctx->bufDIB);
            HDC winDC = GetDC(ctx->window);
            BitBlt(winDC, 0, 0, w, h, ctx->bufDC, 0, 0, SRCCOPY);
            ReleaseDC(ctx->window, winDC);
            SelectObject(ctx->bufDC, NULL);
        }
    }
}

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    RI_UNREF(display_id);
    return (EGLDisplay)1;    //support only a single display
}

#endif

}   //namespace OpenVGRI

#endif /* defined(WIN32) */

