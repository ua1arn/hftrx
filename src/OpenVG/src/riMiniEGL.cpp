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
 *//*-------------------------------------------------------------------*/

#include "egl.h"
#include "openvg.h"
#include "riArray.h"
#include "riMath.h"
#include "riContext.h"
#include "riImage.h"

//==============================================================================================

namespace OpenVGRI
{

void* OSGetCurrentThreadID(void);
void OSAcquireMutex(void);
void OSReleaseMutex(void);
void OSDeinitMutex(void);

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id);
void* OSCreateWindowContext(EGLNativeWindowType window);
void OSDestroyWindowContext(void* context);
bool OSIsWindow(const void* context);
void OSGetWindowSize(const void* context, int& width, int& height);
void OSBlitToWindow(void* context, const Drawable* drawable);

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class RIEGLConfig
{
public:
	RIEGLConfig() : m_desc(Color::formatToDescriptor(VG_sRGBA_8888)), m_configID(0)	{}
	~RIEGLConfig()							{}
	void		set(int r, int g, int b, int a, int l, int bpp, int samples, int maskBits, int ID)
	{
		m_desc.redBits = r;
		m_desc.greenBits = g;
		m_desc.blueBits = b;
		m_desc.alphaBits = a;
		m_desc.luminanceBits = l;
		m_desc.alphaShift = 0;
		m_desc.luminanceShift = 0;
		m_desc.blueShift = b ? a : 0;
		m_desc.greenShift = g ? a + b : 0;
		m_desc.redShift = r ? a + b + g : 0;
		m_desc.format = (VGImageFormat)-1;
		m_desc.internalFormat = l ? Color::sLA : Color::sRGBA;
		m_desc.bitsPerPixel = bpp;
		RI_ASSERT(Color::isValidDescriptor(m_desc));
		m_samples = samples;
        m_maskBits = maskBits;
		m_configID = ID;
        m_config = (EGLConfig)ID;
	}

    Color::Descriptor configToDescriptor(bool sRGB, bool premultiplied) const
    {
        Color::Descriptor desc = m_desc;
        unsigned int f = m_desc.luminanceBits ? Color::LUMINANCE : 0;
        f |= sRGB ? Color::NONLINEAR : 0;
        f |= premultiplied ? Color::PREMULTIPLIED : 0;
        desc.internalFormat = (Color::InternalFormat)f;
        return desc;
    }

	//EGL RED SIZE bits of Red in the color buffer
	//EGL GREEN SIZE bits of Green in the color buffer
	//EGL BLUE SIZE bits of Blue in the color buffer
	//EGL ALPHA SIZE bits of Alpha in the color buffer
	//EGL LUMINANCE SIZE bits of Luminance in the color buffer
	Color::Descriptor	m_desc;
	int					m_samples;
    int                 m_maskBits;
	EGLint				m_configID;			//EGL CONFIG ID unique EGLConfig identifier
    EGLConfig           m_config;
	//EGL BUFFER SIZE depth of the color buffer (sum of channel bits)
	//EGL ALPHA MASK SIZE number alpha mask bits (always 8)
	//EGL BIND TO TEXTURE RGB boolean True if bindable to RGB textures. (always EGL_FALSE)
	//EGL BIND TO TEXTURE RGBA boolean True if bindable to RGBA textures. (always EGL_FALSE)
	//EGL COLOR BUFFER TYPE enum color buffer type (EGL_RGB_BUFFER, EGL_LUMINANCE_BUFFER)
	//EGL CONFIG CAVEAT enum any caveats for the configuration (always EGL_NONE)
	//EGL DEPTH SIZE integer bits of Z in the depth buffer (always 0)
	//EGL LEVEL integer frame buffer level (always 0)
	//EGL MAX PBUFFER WIDTH integer maximum width of pbuffer (always INT_MAX)
	//EGL MAX PBUFFER HEIGHT integer maximum height of pbuffer (always INT_MAX)
	//EGL MAX PBUFFER PIXELS integer maximum size of pbuffer (always INT_MAX)
	//EGL MAX SWAP INTERVAL integer maximum swap interval (always 1)
	//EGL MIN SWAP INTERVAL integer minimum swap interval (always 1)
	//EGL NATIVE RENDERABLE boolean EGL TRUE if native rendering APIs can render to surface (always EGL_FALSE)
	//EGL NATIVE VISUAL ID integer handle of corresponding native visual (always 0)
	//EGL NATIVE VISUAL TYPE integer native visual type of the associated visual (always EGL_NONE)
	//EGL RENDERABLE TYPE bitmask which client rendering APIs are supported. (always EGL_OPENVG_BIT)
	//EGL SAMPLE BUFFERS integer number of multisample buffers (always 0)
	//EGL SAMPLES integer number of samples per pixel (always 0)
	//EGL STENCIL SIZE integer bits of Stencil in the stencil buffer (always 0)
	//EGL SURFACE TYPE bitmask which types of EGL surfaces are supported. (always EGL WINDOW BIT | EGL PIXMAP BIT | EGL PBUFFER BIT)
	//EGL TRANSPARENT TYPE enum type of transparency supported (always EGL_NONE)
	//EGL TRANSPARENT RED VALUE integer transparent red value (undefined)
	//EGL TRANSPARENT GREEN VALUE integer transparent green value (undefined)
	//EGL TRANSPARENT BLUE VALUE integer transparent blue value (undefined)
};

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class RIEGLContext
{
public:
	RIEGLContext(OpenVGRI::VGContext* vgctx, const EGLConfig config);
	~RIEGLContext();
	void	addReference()				{ m_referenceCount++; }
	int		removeReference()			{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }

    VGContext*      getVGContext() const      { return m_vgContext; }
    const EGLConfig getConfig() const         { return m_config; }
private:
	RIEGLContext(const RIEGLContext&);
	RIEGLContext& operator=(const RIEGLContext&);
	VGContext*		m_vgContext;
	const EGLConfig	m_config;
	int				m_referenceCount;
};

RIEGLContext::RIEGLContext(OpenVGRI::VGContext* vgctx, const EGLConfig config) :
	m_vgContext(vgctx),
	m_config(config),
	m_referenceCount(0)
{
}
RIEGLContext::~RIEGLContext()
{
	RI_ASSERT(m_referenceCount == 0);
	RI_DELETE(m_vgContext);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class RIEGLSurface
{
public:
    RIEGLSurface(void* OSWindowContext, const EGLConfig config, Drawable* drawable, bool largestPbuffer, int renderBuffer);
	~RIEGLSurface();
	void	addReference()				{ m_referenceCount++; }
	int		removeReference()			{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }

    void*           getOSWindowContext() const { return m_OSWindowContext; }
    const EGLConfig getConfig() const          { return m_config; }
    Drawable*       getDrawable() const        { return m_drawable; }
    bool            isLargestPbuffer() const   { return m_largestPbuffer; }
    int             getRenderBuffer() const    { return m_renderBuffer; }

private:
	RIEGLSurface(const RIEGLSurface&);
	RIEGLSurface& operator=(const RIEGLSurface&);
    void*            m_OSWindowContext;
	const EGLConfig	 m_config;
	Drawable*        m_drawable;
	bool			 m_largestPbuffer;
	int				 m_renderBuffer;		//EGL_BACK_BUFFER or EGL_SINGLE_BUFFER
	int				 m_referenceCount;
};

RIEGLSurface::RIEGLSurface(void* OSWindowContext, const EGLConfig config, Drawable* drawable, bool largestPbuffer, int renderBuffer) :
    m_OSWindowContext(OSWindowContext),
	m_config(config),
	m_drawable(drawable),
	m_largestPbuffer(largestPbuffer),
	m_renderBuffer(renderBuffer),
	m_referenceCount(0)
{
    RI_ASSERT(m_renderBuffer == EGL_BACK_BUFFER);   //only back buffer rendering is supported
    m_drawable->addReference();
}

RIEGLSurface::~RIEGLSurface()
{
	RI_ASSERT(m_referenceCount == 0);
    OSDestroyWindowContext(m_OSWindowContext);
	if(m_drawable)
	{
		if(!m_drawable->removeReference())
			RI_DELETE(m_drawable);
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

#define EGL_NUMCONFIGS		60

class RIEGLDisplay
{
public:
	RIEGLDisplay(EGLDisplay id);
	~RIEGLDisplay();

	int                getNumConfigs() const              { return EGL_NUMCONFIGS; }
    const RIEGLConfig& getConfig(int i) const             { RI_ASSERT(i >= 0 && i < EGL_NUMCONFIGS); return m_configs[i]; }
    const RIEGLConfig& getConfig(const EGLConfig config) const        { for(int i=0;i<EGL_NUMCONFIGS;i++) { if(m_configs[i].m_config == config) return m_configs[i]; } RI_ASSERT(0); return m_configs[0]; }

    const EGLDisplay  getID() const                       { return m_id; }

    void              addContext(RIEGLContext* ctx)       { RI_ASSERT(ctx); m_contexts.push_back(ctx); }  //throws bad_alloc
    void              removeContext(RIEGLContext* ctx)    { RI_ASSERT(ctx); bool res = m_contexts.remove(ctx); RI_ASSERT(res); RI_UNREF(res); }

    void              addSurface(RIEGLSurface* srf)       { RI_ASSERT(srf); m_surfaces.push_back(srf); }  //throws bad_alloc
    void              removeSurface(RIEGLSurface* srf)    { RI_ASSERT(srf); bool res = m_surfaces.remove(srf); RI_ASSERT(res); RI_UNREF(res); }

    EGLBoolean        contextExists(const EGLContext ctx) const;
    EGLBoolean        surfaceExists(const EGLSurface srf) const;
    EGLBoolean        configExists(const EGLConfig cfg) const;

private:
	RIEGLDisplay(const RIEGLDisplay& t);
	RIEGLDisplay& operator=(const RIEGLDisplay&t);

	EGLDisplay              m_id;

	Array<RIEGLContext*>	m_contexts;
	Array<RIEGLSurface*>	m_surfaces;

	RIEGLConfig             m_configs[EGL_NUMCONFIGS];
};

RIEGLDisplay::RIEGLDisplay(EGLDisplay id) :
	m_id(id),
	m_contexts(),
	m_surfaces()
{
	RI_ASSERT(EGL_NUMCONFIGS == 60);

	//sorted by RGB/LUMINANCE (exact), larger total number of color bits (at least), buffer size (at least), config ID (exact)
	//NOTE: 16 bit configs need to be sorted on the fly if the request ignores some channels
	//NOTE: config IDs start from 1
	//               R  B  G  A  L  bpp samples maskBits ID
	m_configs[0].set(8, 8, 8, 8, 0, 32, 1, 8, 1);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[1].set(8, 8, 8, 0, 0, 32, 1, 8, 2);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[2].set(5, 5, 5, 1, 0, 16, 1, 4, 3);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[3].set(5, 6, 5, 0, 0, 16, 1, 4, 4);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[4].set(4, 4, 4, 4, 0, 16, 1, 4, 5);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[5].set(0, 0, 0, 8, 0, 8,  1, 8, 6);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[6].set(0, 0, 0, 4, 0, 4,  1, 4, 7);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[7].set(0, 0, 0, 1, 0, 1,  1, 1, 8);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[8].set(0, 0, 0, 0, 8, 8,  1, 8, 9);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[9].set(0, 0, 0, 0, 1, 1,  1, 1, 10);	//EGL_LUMINANCE_BUFFER, buffer size = 1

	m_configs[10].set(8, 8, 8, 8, 0, 32, 4, 1, 11);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[11].set(8, 8, 8, 0, 0, 32, 4, 1, 12);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[12].set(5, 5, 5, 1, 0, 16, 4, 1, 13);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[13].set(5, 6, 5, 0, 0, 16, 4, 1, 14);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[14].set(4, 4, 4, 4, 0, 16, 4, 1, 15);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[15].set(0, 0, 0, 8, 0, 8,  4, 1, 16);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[16].set(0, 0, 0, 4, 0, 4,  4, 1, 17);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[17].set(0, 0, 0, 1, 0, 1,  4, 1, 18);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[18].set(0, 0, 0, 0, 8, 8,  4, 1, 19);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[19].set(0, 0, 0, 0, 1, 1,  4, 1, 20);	//EGL_LUMINANCE_BUFFER, buffer size = 1

	m_configs[20].set(8, 8, 8, 8, 0, 32, 32, 1, 21);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[21].set(8, 8, 8, 0, 0, 32, 32, 1, 22);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[22].set(5, 5, 5, 1, 0, 16, 32, 1, 23);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[23].set(5, 6, 5, 0, 0, 16, 32, 1, 24);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[24].set(4, 4, 4, 4, 0, 16, 32, 1, 25);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[25].set(0, 0, 0, 8, 0, 8,  32, 1, 26);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[26].set(0, 0, 0, 4, 0, 4,  32, 1, 27);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[27].set(0, 0, 0, 1, 0, 1,  32, 1, 28);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[28].set(0, 0, 0, 0, 8, 8,  32, 1, 29);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[29].set(0, 0, 0, 0, 1, 1,  32, 1, 30);	//EGL_LUMINANCE_BUFFER, buffer size = 1

    //configs without mask
	m_configs[30].set(8, 8, 8, 8, 0, 32, 1, 0, 31);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[31].set(8, 8, 8, 0, 0, 32, 1, 0, 32);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[32].set(5, 5, 5, 1, 0, 16, 1, 0, 33);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[33].set(5, 6, 5, 0, 0, 16, 1, 0, 34);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[34].set(4, 4, 4, 4, 0, 16, 1, 0, 35);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[35].set(0, 0, 0, 8, 0, 8,  1, 0, 36);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[36].set(0, 0, 0, 4, 0, 4,  1, 0, 37);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[37].set(0, 0, 0, 1, 0, 1,  1, 0, 38);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[38].set(0, 0, 0, 0, 8, 8,  1, 0, 39);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[39].set(0, 0, 0, 0, 1, 1,  1, 0, 40);	//EGL_LUMINANCE_BUFFER, buffer size = 1

	m_configs[40].set(8, 8, 8, 8, 0, 32, 4, 0, 41);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[41].set(8, 8, 8, 0, 0, 32, 4, 0, 42);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[42].set(5, 5, 5, 1, 0, 16, 4, 0, 43);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[43].set(5, 6, 5, 0, 0, 16, 4, 0, 44);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[44].set(4, 4, 4, 4, 0, 16, 4, 0, 45);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[45].set(0, 0, 0, 8, 0, 8,  4, 0, 46);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[46].set(0, 0, 0, 4, 0, 4,  4, 0, 47);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[47].set(0, 0, 0, 1, 0, 1,  4, 0, 48);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[48].set(0, 0, 0, 0, 8, 8,  4, 0, 49);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[49].set(0, 0, 0, 0, 1, 1,  4, 0, 50);	//EGL_LUMINANCE_BUFFER, buffer size = 1

	m_configs[50].set(8, 8, 8, 8, 0, 32, 32, 0, 51);	//EGL_RGB_BUFFER, buffer size = 32
	m_configs[51].set(8, 8, 8, 0, 0, 32, 32, 0, 52);	//EGL_RGB_BUFFER, buffer size = 24
	m_configs[52].set(5, 5, 5, 1, 0, 16, 32, 0, 53);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[53].set(5, 6, 5, 0, 0, 16, 32, 0, 54);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[54].set(4, 4, 4, 4, 0, 16, 32, 0, 55);	//EGL_RGB_BUFFER, buffer size = 16
	m_configs[55].set(0, 0, 0, 8, 0, 8,  32, 0, 56);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[56].set(0, 0, 0, 4, 0, 4,  32, 0, 57);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[57].set(0, 0, 0, 1, 0, 1,  32, 0, 58);	//EGL_RGB_BUFFER, buffer size = 8
	m_configs[58].set(0, 0, 0, 0, 8, 8,  32, 0, 59);	//EGL_LUMINANCE_BUFFER, buffer size = 8
	m_configs[59].set(0, 0, 0, 0, 1, 1,  32, 0, 60);	//EGL_LUMINANCE_BUFFER, buffer size = 1
/*
attrib                default        criteria order   priority
--------------------------------------------------------------
EGL_COLOR_BUFFER_TYPE EGL_RGB_BUFFER Exact    None    2 
EGL_RED_SIZE          0              AtLeast  Special 3 
EGL_GREEN_SIZE        0              AtLeast  Special 3 
EGL_BLUE_SIZE         0              AtLeast  Special 3 
EGL_LUMINANCE_SIZE    0              AtLeast  Special 3 
EGL_ALPHA_SIZE        0              AtLeast  Special 3 
EGL_BUFFER_SIZE       0              AtLeast  Smaller 4 
EGL_CONFIG_ID         EGL_DONT_CARE  Exact    Smaller 11
*/
}

RIEGLDisplay::~RIEGLDisplay()
{
	//mark everything for deletion, but don't delete the current context and surface
	for(int i=0;i<m_contexts.size();i++)
	{
		if(!m_contexts[i]->removeReference())
			RI_DELETE(m_contexts[i]);
	}
	m_contexts.clear();	//remove all contexts from the list (makes further references to the current contexts invalid)

	for(int i=0;i<m_surfaces.size();i++)
	{
		if(!m_surfaces[i]->removeReference())
			RI_DELETE(m_surfaces[i]);
	}
	m_surfaces.clear();	//remove all surfaces from the list (makes further references to the current surfaces invalid)
}

EGLBoolean RIEGLDisplay::contextExists(const EGLContext ctx) const
{
	for(int i=0;i<m_contexts.size();i++)
	{
		if(m_contexts[i] == ctx)
			return EGL_TRUE;
	}
	return EGL_FALSE;
}

EGLBoolean RIEGLDisplay::surfaceExists(const EGLSurface surf) const
{
	for(int i=0;i<m_surfaces.size();i++)
	{
		if(m_surfaces[i] == surf)
			return EGL_TRUE;
	}
	return EGL_FALSE;
}

EGLBoolean RIEGLDisplay::configExists(const EGLConfig config) const
{
    for(int i=0;i<EGL_NUMCONFIGS;i++)
    {
        if(m_configs[i].m_config == config)
		return EGL_TRUE;
    }
	return EGL_FALSE;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class RIEGLThread
{
public:
	RIEGLThread(void* currentThreadID);
	~RIEGLThread();

    void*           getThreadID() const       { return m_threadID; }

    void            makeCurrent(RIEGLContext* c, RIEGLSurface* s)       { m_context = c; m_surface = s; }
	RIEGLContext*	getCurrentContext() const   { return m_context; }
	RIEGLSurface*	getCurrentSurface() const   { return m_surface; }

    void            setError(EGLint error)      { m_error = error; }
    EGLint          getError() const            { return m_error; }

    void            bindAPI(EGLint api)         { m_boundAPI = api; }
    EGLint          getBoundAPI() const         { return m_boundAPI; }

private:
	RIEGLThread(const RIEGLThread&);
	RIEGLThread operator=(const RIEGLThread&);

	RIEGLContext*		m_context;
	RIEGLSurface*		m_surface;
	EGLint              m_error;
	void*               m_threadID;
	EGLint              m_boundAPI;
};

RIEGLThread::RIEGLThread(void* currentThreadID) :
	m_context(NULL),
	m_surface(NULL),
	m_error(EGL_SUCCESS),
	m_threadID(currentThreadID),
	m_boundAPI(EGL_NONE)
{
}

RIEGLThread::~RIEGLThread()
{
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class EGL
{
public:
	EGL();
	~EGL();

	void	addReference()				{ m_referenceCount++; }
	int		removeReference()			{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }

    void                addDisplay(RIEGLDisplay* display)           { RI_ASSERT(display); m_displays.push_back(display); }  //throws bad alloc
    void                removeDisplay(RIEGLDisplay* display)        { RI_ASSERT(display); bool res = m_displays.remove(display); RI_ASSERT(res); RI_UNREF(res); }
    RIEGLDisplay*       getDisplay(const EGLDisplay displayID) const;
    const EGLDisplay    findDisplay(const EGLContext ctx) const;

    void                addCurrentThread(RIEGLThread* thread)       { RI_ASSERT(thread); m_currentThreads.push_back(thread); }  //throws bad alloc
    void                removeCurrentThread(RIEGLThread* thread)    { RI_ASSERT(thread); bool res = m_currentThreads.remove(thread); RI_ASSERT(res); RI_UNREF(res); }
    RIEGLThread*        getCurrentThread() const;

    RIEGLThread*        getThread();
    void                destroyThread();

    bool                isInUse(const void* image) const;

private:
	EGL(const EGL&);						// Not allowed.
	const EGL& operator=(const EGL&);		// Not allowed.

	Array<RIEGLThread*>		m_threads;			//threads that have called EGL
	Array<RIEGLThread*>		m_currentThreads;	//threads that have a bound context
	Array<RIEGLDisplay*>	m_displays;

	int                     m_referenceCount;
};

EGL::EGL() :
	m_displays(),
	m_threads(),
	m_currentThreads(),
	m_referenceCount(0)
{
}
EGL::~EGL()
{
	for(int i=0;i<m_displays.size();i++)
	{
		RI_DELETE(m_displays[i]);
	}
	for(int i=0;i<m_threads.size();i++)
	{
		RI_DELETE(m_threads[i]);
	}
	//currentThreads contain just pointers to threads we just deleted
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static EGL* g_egl = NULL;	//never use this directly
static EGL* getEGL()
{
	if(!g_egl)
	{
		try
		{
			g_egl = RI_NEW(EGL, ());				//throws bad_alloc
			g_egl->addReference();
		}
		catch(std::bad_alloc)
		{
			g_egl = NULL;
		}
	}
	return g_egl;
}
static void releaseEGL()
{
	if(g_egl)
	{
		if(!g_egl->removeReference())
		{
			RI_DELETE(g_egl);
			g_egl = NULL;
		}
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	Given a display ID, return the corresponding object, or NULL
*			if the ID hasn't been initialized.
* \param	
* \return	
* \note		if egl has been initialized for this display, the display ID can
*			be found from egl->m_displays
*//*-------------------------------------------------------------------*/

RIEGLDisplay* EGL::getDisplay(EGLDisplay displayID) const
{
	for(int i=0;i<m_displays.size();i++)
	{
		if(displayID == m_displays[i]->getID())
			return m_displays[i];
	}
	return NULL;		//error: the display hasn't been eglInitialized
}

/*-------------------------------------------------------------------*//*!
* \brief	return EGLDisplay for the current context
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

const EGLDisplay EGL::findDisplay(EGLContext ctx) const
{
	for(int i=0;i<m_displays.size();i++)
	{
        if(m_displays[i]->contextExists(ctx))
            return m_displays[i]->getID();
	}
    return EGL_NO_DISPLAY;
}

/*-------------------------------------------------------------------*//*!
* \brief	return an EGL thread struct for the thread made current, or
*            NULL if there's no current context.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

RIEGLThread* EGL::getCurrentThread() const
{
	void* currentThreadID = OSGetCurrentThreadID();
	for(int i=0;i<m_currentThreads.size();i++)
	{
		if(currentThreadID == m_currentThreads[i]->getThreadID())
			return m_currentThreads[i];
	}
	return NULL;		//thread is not current
}

/*-------------------------------------------------------------------*//*!
* \brief	return an EGL thread struct corresponding to current OS thread.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

RIEGLThread* EGL::getThread()
{
	void* currentThreadID = OSGetCurrentThreadID();
	for(int i=0;i<m_threads.size();i++)
	{
		if(currentThreadID == m_threads[i]->getThreadID())
			return m_threads[i];
	}

	//EGL doesn't have a struct for the thread yet, add it to EGL's list
	RIEGLThread* newThread = NULL;
	try
	{
		newThread = RI_NEW(RIEGLThread, (OSGetCurrentThreadID()));	//throws bad_alloc
		m_threads.push_back(newThread);	//throws bad_alloc
		return newThread;
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(newThread);
		return NULL;
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	destroy an EGL thread struct
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void EGL::destroyThread()
{
	void* currentThreadID = OSGetCurrentThreadID();
	for(int i=0;i<m_threads.size();i++)
	{
		if(currentThreadID == m_threads[i]->getThreadID())
        {
            RIEGLThread* thread = m_threads[i];
            bool res = m_threads.remove(thread);
            RI_ASSERT(res);
            RI_UNREF(res);
            RI_DELETE(thread);
            break;
        }
	}
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

bool EGL::isInUse(const void* image) const
{
    for(int i=0;i<m_currentThreads.size();i++)
    {
        RIEGLSurface* s = m_currentThreads[i]->getCurrentSurface();
        if(s && s->getDrawable() && s->getDrawable()->isInUse((Image*)image))
            return true;
    }
    return false;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

#define EGL_GET_DISPLAY(DISPLAY, RETVAL) \
	OSAcquireMutex(); \
	EGL* egl = getEGL(); \
    if(!egl) \
    { \
		OSReleaseMutex(); \
		return RETVAL; \
    } \
	RIEGLDisplay* display = egl->getDisplay(DISPLAY); \

#define EGL_GET_EGL(RETVAL) \
	OSAcquireMutex(); \
	EGL* egl = getEGL(); \
    if(!egl) \
    { \
		OSReleaseMutex(); \
		return RETVAL; \
    } \

#define EGL_IF_ERROR(COND, ERRORCODE, RETVAL) \
	if(COND) { eglSetError(egl, ERRORCODE); OSReleaseMutex(); return RETVAL; } \

#define EGL_RETURN(ERRORCODE, RETVAL) \
	{ \
		eglSetError(egl, ERRORCODE); \
		OSReleaseMutex(); \
		return RETVAL; \
	}

// Note: egl error handling model differs from OpenVG. The latest error is stored instead of the oldest one.
static void eglSetError(EGL* egl, EGLint error)
{
	RIEGLThread* thread = egl->getThread();
	if(thread)
		thread->setError(error);
}

/*-------------------------------------------------------------------*//*!
* \brief	Returns the OpenVG context current to the calling thread.
* \param	
* \return	
* \note		This function is always called from a mutexed API function
*//*-------------------------------------------------------------------*/

void* eglvgGetCurrentVGContext(void)
{
	EGL* egl = getEGL();
    if(egl)
    {
        RIEGLThread* thread = egl->getCurrentThread();
        if(thread)
        {
            RI_ASSERT(thread->getCurrentContext() && thread->getCurrentSurface());
            return thread->getCurrentContext()->getVGContext();
        }
    }
	return NULL;	//not initialized or made current
}

/*-------------------------------------------------------------------*//*!
* \brief	Check if the image is current in any of the displays
* \param	
* \return	
* \note		This function is always called from a mutexed API function
*//*-------------------------------------------------------------------*/

bool eglvgIsInUse(void* image)
{
	EGL* egl = getEGL();
    if(egl)
    {
        return egl->isInUse(image);
    }
	return false;
}

//==============================================================================================

}	//namespace OpenVGRI

using namespace OpenVGRI;





/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLint eglGetError()
{
    OSAcquireMutex();
    EGLint ret = EGL_SUCCESS;
	EGL* egl = getEGL();
    if(egl)
    {
        RIEGLThread* thread = egl->getThread();
        if(thread)
            ret = thread->getError();	//initialized, return error code
    }
    else ret = EGL_NOT_INITIALIZED;
    OSReleaseMutex();
    return ret;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
    return OSGetDisplay(display_id);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(display, EGL_SUCCESS, EGL_TRUE);	//already initialized

	//create the current display
	//if a context and a surface are bound by the time of eglTerminate, they remain bound until eglMakeCurrent is called
	RIEGLDisplay* newDisplay = NULL;
	try
	{
		newDisplay = RI_NEW(RIEGLDisplay, (dpy));	//throws bad_alloc
		egl->addDisplay(newDisplay);	//throws bad_alloc
		display = newDisplay;
		RI_ASSERT(display);
	}
	catch(std::bad_alloc)
	{
		RI_DELETE(newDisplay);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_FALSE);
	}

	if(major) *major = 1;
	if(minor) *minor = 2;
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglTerminate(EGLDisplay dpy)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_SUCCESS, EGL_TRUE);
    egl->removeDisplay(display);
    RI_DELETE(display);
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

const char *eglQueryString(EGLDisplay dpy, EGLint name)
{
	EGL_GET_DISPLAY(dpy, NULL);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, NULL);

	static const char apis[] = "OpenVG";
	static const char extensions[] = "";
	static const char vendor[] = "Khronos Group";
	static const char version[] = "1.3";

	const char* ret = NULL;
	switch(name)
	{
	case EGL_CLIENT_APIS:
		ret = apis;
		break;

	case EGL_EXTENSIONS:
		ret = extensions;
		break;

	case EGL_VENDOR:
		ret = vendor;
		break;

	case EGL_VERSION:
		ret = version;
		break;

	default:
		EGL_RETURN(EGL_BAD_PARAMETER, NULL);
	}
	EGL_RETURN(EGL_SUCCESS, ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!num_config, EGL_BAD_PARAMETER, EGL_FALSE);
	if(!configs)
	{
		*num_config = display->getNumConfigs();
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
	}
	*num_config = RI_INT_MIN(config_size, display->getNumConfigs());
	for(int i=0;i<*num_config;i++)
		configs[i] = display->getConfig(i).m_config;

	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

static bool smaller(EGLint c, EGLint filter)
{
	return (filter != EGL_DONT_CARE) && (c < filter);
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!num_config, EGL_BAD_PARAMETER, EGL_FALSE);

	if(!configs)
	{
		*num_config = display->getNumConfigs();
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
	}
	*num_config = 0;
	if(!config_size)
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);

	int bufferSize = 0;
	int redSize = 0;
	int greenSize = 0;
	int blueSize = 0;
	int luminanceSize = 0;
	int alphaSize = 0;
	int colorBufferType = EGL_RGB_BUFFER;
	int configID = EGL_DONT_CARE;
	int sampleBuffers = 0;
	int samples = 0;
	if(attrib_list)
	{
		for(int i=0;attrib_list[i] != EGL_NONE;i+=2)
		{
			switch(attrib_list[i])
			{
			case EGL_BUFFER_SIZE:				//depth of the color buffer
				bufferSize = attrib_list[i+1];
				break;
			case EGL_RED_SIZE:					//bits of Red in the color buffer
				redSize = attrib_list[i+1];
				break;
			case EGL_GREEN_SIZE:				//bits of Green in the color buffer
				greenSize = attrib_list[i+1];
				break;
			case EGL_BLUE_SIZE:					//bits of Blue in the color buffer
				blueSize = attrib_list[i+1];
				break;
			case EGL_LUMINANCE_SIZE:			//bits of Luminance in the color buffer
				luminanceSize = attrib_list[i+1];
				break;
			case EGL_ALPHA_SIZE:				//bits of Alpha in the color buffer
				alphaSize = attrib_list[i+1];
				break;
			case EGL_ALPHA_MASK_SIZE:			//bits of Alpha in the alpha mask buffer
				if(attrib_list[i+1] > 8)
					EGL_RETURN(EGL_SUCCESS, EGL_TRUE);	//not supported
				break;
			case EGL_COLOR_BUFFER_TYPE:			//enum color buffer type (EGL_RGB_BUFFER, EGL_LUMINANCE_BUFFER)
				EGL_IF_ERROR(attrib_list[i+1] != EGL_RGB_BUFFER && attrib_list[i+1] != EGL_LUMINANCE_BUFFER && attrib_list[i+1] != EGL_DONT_CARE, EGL_BAD_ATTRIBUTE, EGL_FALSE);
				colorBufferType = attrib_list[i+1];
				break;
			case EGL_CONFIG_ID:					//unique EGLConfig identifier
				configID = attrib_list[i+1];
				break;

			case EGL_SAMPLE_BUFFERS:			//integer number of multisample buffers
				sampleBuffers = attrib_list[i+1];
				break;
			case EGL_SAMPLES:					//integer number of samples per pixel
				samples = attrib_list[i+1];
				break;

			case EGL_BIND_TO_TEXTURE_RGB:		//boolean True if bindable to RGB textures. (always EGL_FALSE)
			case EGL_BIND_TO_TEXTURE_RGBA:		//boolean True if bindable to RGBA textures. (always EGL_FALSE)
			case EGL_DEPTH_SIZE:				//integer bits of Z in the depth buffer (always 0)
			case EGL_LEVEL:						//integer frame buffer level (always 0)
			case EGL_NATIVE_RENDERABLE:			//boolean EGL TRUE if native rendering APIs can render to surface (always EGL_FALSE)
			case EGL_STENCIL_SIZE:				//integer bits of Stencil in the stencil buffer (always 0)
				if(attrib_list[i+1])
					EGL_RETURN(EGL_SUCCESS, EGL_TRUE);	//not supported
				break;

			case EGL_CONFIG_CAVEAT:				//enum any caveats for the configuration (always EGL_NONE)
			case EGL_NATIVE_VISUAL_TYPE:		//integer native visual type of the associated visual (always EGL_NONE)
				if(attrib_list[i+1] != EGL_NONE)
					EGL_RETURN(EGL_SUCCESS, EGL_TRUE);	//not supported
				break;

			case EGL_MAX_SWAP_INTERVAL:			//integer maximum swap interval (always 1)
			case EGL_MIN_SWAP_INTERVAL:			//integer minimum swap interval (always 1)
				if(attrib_list[i+1] != 1)
					EGL_RETURN(EGL_SUCCESS, EGL_TRUE);	//not supported
				break;

			case EGL_RENDERABLE_TYPE:			//bitmask which client rendering APIs are supported. (always EGL_OPENVG_BIT)
				if(!(attrib_list[i+1] & EGL_OPENVG_BIT))
					EGL_RETURN(EGL_SUCCESS, EGL_TRUE);	//not supported
				break;

			case EGL_SURFACE_TYPE:				//bitmask which types of EGL surfaces are supported. (always EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT)
				break;	//all types are always supported

			case EGL_TRANSPARENT_TYPE:			//enum type of transparency supported (always EGL_NONE)
			case EGL_NATIVE_VISUAL_ID:			//integer handle of corresponding native visual (always 0)
			case EGL_MAX_PBUFFER_WIDTH:			//integer maximum width of pbuffer (always INT_MAX)
			case EGL_MAX_PBUFFER_HEIGHT:		//integer maximum height of pbuffer (always INT_MAX)
			case EGL_MAX_PBUFFER_PIXELS:		//integer maximum size of pbuffer (always INT_MAX)
			case EGL_TRANSPARENT_RED_VALUE:		//integer transparent red value (undefined)
			case EGL_TRANSPARENT_GREEN_VALUE:	//integer transparent green value (undefined)
			case EGL_TRANSPARENT_BLUE_VALUE:	//integer transparent blue value (undefined)
				break;	//ignored

			default:
				EGL_RETURN(EGL_BAD_ATTRIBUTE, EGL_FALSE);	//unknown attribute
			}
		}
	}

	if(configID && configID != EGL_DONT_CARE)
	{	//if CONFIG_ID is defined, ignore the rest of the attribs
        for(int i=0;i<EGL_NUMCONFIGS;i++)
        {
            if(display->getConfig(i).m_configID == configID)
            {
                *num_config = 1;
                *configs = display->getConfig(i).m_config;
            }
        }
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
	}

	//go through all configs, add passed configs to return list
    //TODO take alpha mask size into account
	EGLConfig found[EGL_NUMCONFIGS];
    int keys[EGL_NUMCONFIGS];
	int numFound = 0;
	for(int i=0;i<display->getNumConfigs();i++)
	{
		const RIEGLConfig& c = display->getConfig(i);

		int colorBits = c.m_desc.redBits + c.m_desc.greenBits + c.m_desc.blueBits;
		int luminanceBits = c.m_desc.luminanceBits;
		int configBufferSize;
		if(colorBits)
		{
			RI_ASSERT(!luminanceBits);
			colorBits += c.m_desc.alphaBits;
			configBufferSize = colorBits;
		}
		else if(luminanceBits)
		{
			luminanceBits += c.m_desc.alphaBits;
			configBufferSize = luminanceBits;
		}
		else
		{	//alpha only surface
			colorBits = c.m_desc.alphaBits;
			luminanceBits = c.m_desc.alphaBits;
			configBufferSize = colorBits;
		}

		if (smaller(configBufferSize, bufferSize))
			continue;

		int configSampleBuffers = c.m_samples == 1 ? 0 : 1;
		if (smaller(configSampleBuffers, sampleBuffers))
			continue;
		if (smaller(c.m_samples, samples))
			continue;

		if (smaller(c.m_desc.redBits, redSize)
			|| smaller(c.m_desc.greenBits, greenSize)
			|| smaller(c.m_desc.blueBits, blueSize) 
			|| smaller(c.m_desc.alphaBits, alphaSize) )
			continue;

		if (smaller(c.m_desc.luminanceBits, luminanceSize))
			continue;

		if ((colorBufferType == EGL_RGB_BUFFER && !colorBits) ||
			(colorBufferType == EGL_LUMINANCE_BUFFER && !luminanceBits))
			continue;

		int sortKey = c.m_configID;	//sort from smaller to larger
		int sortBits = 0;
		if(redSize != 0 && redSize != EGL_DONT_CARE)
			sortBits += c.m_desc.redBits;
		if(greenSize != 0 && greenSize != EGL_DONT_CARE)
			sortBits += c.m_desc.greenBits;
		if(blueSize != 0 && blueSize != EGL_DONT_CARE)
			sortBits += c.m_desc.blueBits;
		if(alphaSize != 0 && alphaSize != EGL_DONT_CARE)
			sortBits += c.m_desc.alphaBits;
		if(luminanceSize != 0 && luminanceSize != EGL_DONT_CARE)
			sortBits += c.m_desc.luminanceBits;
		RI_ASSERT(c.m_configID <= EGL_NUMCONFIGS);	//if there are more configs, increase the shift value
		RI_ASSERT(sortBits <= 32);
		sortKey += (32-sortBits) << 4;	//sort from larger to smaller

		found[numFound] = c.m_config;
		keys[numFound++] = sortKey;
	}
	if(!numFound)
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);

	//sort return list into increasing order
	for(int e=0;e<numFound-1;e++)
	{
		for(int f=e+1;f<numFound;f++)
		{
			if(keys[e] > keys[f])
			{
                EGLConfig tmp = found[e];
                found[e] = found[f];
                found[f] = tmp;
				RI_INT_SWAP(keys[e], keys[f]);
			}
		}
	}

	//write configs into return array
	numFound = RI_INT_MIN(numFound, config_size);
	for(int i=0;i<numFound;i++)
	{
		configs[i] = found[i];
	}
	*num_config = numFound;

	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_FALSE);
	const RIEGLConfig& c = display->getConfig(config);
	switch(attribute)
	{
	case EGL_BUFFER_SIZE:
		*value = RI_INT_MAX(c.m_desc.redBits + c.m_desc.greenBits + c.m_desc.blueBits + c.m_desc.alphaBits, c.m_desc.luminanceBits + c.m_desc.alphaBits);
		break;

	case EGL_RED_SIZE:
		*value = c.m_desc.redBits;
		break;

	case EGL_GREEN_SIZE:
		*value = c.m_desc.greenBits;
		break;

	case EGL_BLUE_SIZE:
		*value = c.m_desc.blueBits;
		break;

	case EGL_LUMINANCE_SIZE:
		*value = c.m_desc.luminanceBits;
		break;

	case EGL_ALPHA_SIZE:
		*value = c.m_desc.alphaBits;
		break;

	case EGL_ALPHA_MASK_SIZE:
		*value = c.m_maskBits;
		break;

	case EGL_BIND_TO_TEXTURE_RGB:
	case EGL_BIND_TO_TEXTURE_RGBA:
		*value = EGL_FALSE;
		break;

	case EGL_COLOR_BUFFER_TYPE:
		if(c.m_desc.redBits)
			*value = EGL_RGB_BUFFER;
		else
			*value = EGL_LUMINANCE_BUFFER;
		break;

	case EGL_CONFIG_CAVEAT:
		*value = EGL_NONE;
		break;

	case EGL_CONFIG_ID:
		*value = c.m_configID;
		break;

	case EGL_DEPTH_SIZE:
		*value = 0;
		break;

	case EGL_LEVEL:
		*value = 0;
		break;

	case EGL_MAX_PBUFFER_WIDTH:
	case EGL_MAX_PBUFFER_HEIGHT:
		*value = 16384;			//NOTE arbitrary maximum
		break;
		
	case EGL_MAX_PBUFFER_PIXELS:
		*value = 16384*16384;	//NOTE arbitrary maximum
		break;

	case EGL_MAX_SWAP_INTERVAL:
	case EGL_MIN_SWAP_INTERVAL:
		*value = 1;
		break;

	case EGL_NATIVE_RENDERABLE:
		*value = EGL_FALSE;
		break;

	case EGL_NATIVE_VISUAL_ID:
		*value = 0;
		break;

	case EGL_NATIVE_VISUAL_TYPE:
		*value = EGL_NONE;
		break;

	case EGL_RENDERABLE_TYPE:
		*value = EGL_OPENVG_BIT;
		break;

	case EGL_SAMPLE_BUFFERS:
		*value = c.m_samples > 1 ? 1 : 0;
		break;

	case EGL_SAMPLES:
		*value = c.m_samples > 1 ? c.m_samples : 0;
		break;

	case EGL_STENCIL_SIZE:
		*value = 0;
		break;

	case EGL_SURFACE_TYPE:
		*value = EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT;
		break;

	case EGL_TRANSPARENT_TYPE:
		*value = EGL_NONE;
		break;

	case EGL_TRANSPARENT_RED_VALUE:
	case EGL_TRANSPARENT_GREEN_VALUE:
	case EGL_TRANSPARENT_BLUE_VALUE:
		*value = 0;
		break;

    case EGL_CONFORMANT:
        *value = EGL_OPENVG_BIT;  //TODO return proper value
        break;

	default:
		EGL_RETURN(EGL_BAD_ATTRIBUTE, EGL_FALSE);
	}
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
	EGL_GET_DISPLAY(dpy, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_NO_SURFACE);

	int renderBuffer = EGL_BACK_BUFFER;
	int colorSpace = EGL_VG_COLORSPACE_sRGB;
	int alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
	if(attrib_list)
	{
		for(int i=0;attrib_list[i] != EGL_NONE;i+=2)
		{
			switch(attrib_list[i])
			{
			case EGL_RENDER_BUFFER:
				renderBuffer = attrib_list[i+1];
				break;

			case EGL_VG_COLORSPACE:
				colorSpace = attrib_list[i+1];
				break;

			case EGL_VG_ALPHA_FORMAT:
				alphaFormat = attrib_list[i+1];
				break;

			default:
				EGL_RETURN(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
			}
		}
	}
	//we ignore the renderBuffer parameter since we can only render to double buffered surfaces

	//TODO If the attributes of win do not correspond to config, then an EGL BAD MATCH error is generated.
	//TODO If there is already an EGLConfig associated with win (as a result of a previous eglCreateWindowSurface call), then an EGL BAD ALLOC error is generated

    void* wc = NULL;
    Drawable* d = NULL;
	RIEGLSurface* s = NULL;
	try
	{
        wc = OSCreateWindowContext(win);
		RI_ASSERT(wc);
		//TODO what should happen if window width or height is zero?
		int windowWidth = 0, windowHeight = 0;
		OSGetWindowSize(wc, windowWidth, windowHeight);
        bool isWindow = OSIsWindow(wc);
		if(windowWidth <= 0 || windowHeight <= 0 || !isWindow)
		{
            OSDestroyWindowContext(wc);
            EGL_IF_ERROR(!isWindow, EGL_BAD_NATIVE_WINDOW, EGL_NO_SURFACE);
			EGL_IF_ERROR(windowWidth <= 0 || windowHeight <= 0, EGL_BAD_NATIVE_WINDOW, EGL_NO_SURFACE);
		}
		d = RI_NEW(Drawable, (display->getConfig(config).configToDescriptor((colorSpace == EGL_VG_COLORSPACE_LINEAR) ? false : true, (alphaFormat == EGL_VG_ALPHA_FORMAT_PRE) ? true : false), windowWidth, windowHeight, display->getConfig(config).m_samples, display->getConfig(config).m_maskBits));	//throws bad_alloc
		RI_ASSERT(d);
		s = RI_NEW(RIEGLSurface,(wc, config, d, false, renderBuffer));	//throws bad_alloc
		RI_ASSERT(s);
		s->addReference();
		display->addSurface(s);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
        OSDestroyWindowContext(wc);
        RI_DELETE(d);
        RI_DELETE(s);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);
	}
	EGL_RETURN(EGL_SUCCESS, (EGLSurface)s);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
	EGL_GET_DISPLAY(dpy, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_NO_SURFACE);

	int width = 0, height = 0;
	bool largestPbuffer = false;
	int colorSpace = EGL_VG_COLORSPACE_sRGB;
	int alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
	if(attrib_list)
	{
		for(int i=0;attrib_list[i] != EGL_NONE;i+=2)
		{
			switch(attrib_list[i])
			{
			case EGL_WIDTH:
				width = attrib_list[i+1];
				break;

			case EGL_HEIGHT:
				height = attrib_list[i+1];
				break;

			case EGL_LARGEST_PBUFFER:
				largestPbuffer = attrib_list[i+1] ? true : false;
				break;

			case EGL_VG_COLORSPACE:
				colorSpace = attrib_list[i+1];
				break;

			case EGL_VG_ALPHA_FORMAT:
				alphaFormat = attrib_list[i+1];
				break;

			case EGL_TEXTURE_FORMAT:	//config doesn't support OpenGL ES
			case EGL_TEXTURE_TARGET:	//config doesn't support OpenGL ES
			case EGL_MIPMAP_TEXTURE:	//config doesn't support OpenGL ES
			default:
				EGL_RETURN(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
			break;
			}
		}
	}
	EGL_IF_ERROR(width <= 0 || height <= 0, EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);

    Drawable* d = NULL;
	RIEGLSurface* s = NULL;
	try
	{
		d = RI_NEW(Drawable, (display->getConfig(config).configToDescriptor((colorSpace == EGL_VG_COLORSPACE_LINEAR) ? false : true, (alphaFormat == EGL_VG_ALPHA_FORMAT_PRE) ? true : false), width, height, display->getConfig(config).m_samples, display->getConfig(config).m_maskBits));	//throws bad_alloc
		RI_ASSERT(d);
		s = RI_NEW(RIEGLSurface,(NULL, config, d, largestPbuffer, EGL_BACK_BUFFER));	//throws bad_alloc
		RI_ASSERT(s);
		s->addReference();
		display->addSurface(s);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
        RI_DELETE(d);
        RI_DELETE(s);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);
	}
	EGL_RETURN(EGL_SUCCESS, (EGLSurface)s);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
	EGL_GET_DISPLAY(dpy, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
	EGL_IF_ERROR(buftype != EGL_OPENVG_IMAGE, EGL_BAD_PARAMETER, EGL_NO_SURFACE);
	EGL_IF_ERROR(!buffer, EGL_BAD_PARAMETER, EGL_NO_SURFACE);	//TODO should also check if buffer really is a valid VGImage object (needs VG context for that)
    Image* image = (Image*)buffer;
	EGL_IF_ERROR(image->isInUse(), EGL_BAD_ACCESS, EGL_NO_SURFACE);	//buffer is in use by OpenVG
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_NO_SURFACE);
	EGL_IF_ERROR(attrib_list && attrib_list[0] != EGL_NONE, EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);	//there are no valid attribs for OpenVG
	const Color::Descriptor& bc = ((Image*)buffer)->getDescriptor();
	const Color::Descriptor& cc = display->getConfig(config).m_desc;
	EGL_IF_ERROR(bc.redBits != cc.redBits || bc.greenBits != cc.greenBits || bc.blueBits != cc.blueBits ||
				 bc.alphaBits != cc.alphaBits || bc.luminanceBits != cc.luminanceBits, EGL_BAD_MATCH, EGL_NO_SURFACE);

	//TODO If buffer is already bound to another pbuffer, an EGL BAD ACCESS error is generated.

    Drawable* d = NULL;
	RIEGLSurface* s = NULL;
	try
	{
		d = RI_NEW(Drawable, (image, display->getConfig(config).m_maskBits));
		RI_ASSERT(d);
		s = RI_NEW(RIEGLSurface,(NULL, config, d, false, EGL_BACK_BUFFER));	//throws bad_alloc
		RI_ASSERT(s);
		s->addReference();
		display->addSurface(s);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
        RI_DELETE(d);
        RI_DELETE(s);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);
	}
	EGL_RETURN(EGL_SUCCESS, (EGLSurface)s);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
	EGL_GET_DISPLAY(dpy, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_NO_SURFACE);
	EGL_IF_ERROR(!pixmap || !isValidImageFormat(pixmap->format) || !pixmap->data || pixmap->width <= 0 || pixmap->height <= 0, EGL_BAD_NATIVE_PIXMAP, EGL_NO_SURFACE);
	RI_UNREF(attrib_list);
	EGL_IF_ERROR(display->getConfig(config).m_samples != 1, EGL_BAD_MATCH, EGL_NO_SURFACE);

	//TODO If there is already an EGLSurface associated with pixmap (as a result of a previous eglCreatePixmapSurface call), then a EGL BAD ALLOC error is generated.

    Drawable* d = NULL;
	RIEGLSurface* s = NULL;
	try
	{
		d = RI_NEW(Drawable, (Color::formatToDescriptor((VGImageFormat)pixmap->format), pixmap->width, pixmap->height, pixmap->stride, (RIuint8*)pixmap->data, display->getConfig(config).m_maskBits));	//throws bad_alloc
		RI_ASSERT(d);
		s = RI_NEW(RIEGLSurface,(NULL, config, d, false, EGL_BACK_BUFFER));	//throws bad_alloc
		RI_ASSERT(s);
		s->addReference();
		display->addSurface(s);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
        RI_DELETE(d);
        RI_DELETE(s);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);
	}
	EGL_RETURN(EGL_SUCCESS, (EGLSurface)s);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->surfaceExists(surface), EGL_BAD_SURFACE, EGL_FALSE);

    display->removeSurface((RIEGLSurface*)surface);
	if(!((RIEGLSurface*)surface)->removeReference())
		RI_DELETE((RIEGLSurface*)surface);

	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->surfaceExists(surface), EGL_BAD_SURFACE, EGL_FALSE);
	RI_UNREF(attribute);
	RI_UNREF(value);
	//do nothing
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->surfaceExists(surface), EGL_BAD_SURFACE, EGL_FALSE);
	//TODO give an error if value is NULL?

	RIEGLSurface* s = (RIEGLSurface*)surface;
	switch(attribute)
	{
	case EGL_VG_ALPHA_FORMAT:
		*value = (s->getDrawable()->getDescriptor().isPremultiplied()) ? EGL_VG_ALPHA_FORMAT_PRE : EGL_VG_ALPHA_FORMAT_NONPRE;
		break;

	case EGL_VG_COLORSPACE:
		*value = (s->getDrawable()->getDescriptor().isNonlinear()) ? EGL_VG_COLORSPACE_sRGB : EGL_VG_COLORSPACE_LINEAR;
		break;

	case EGL_CONFIG_ID:
		*value = display->getConfig(s->getConfig()).m_configID;
		break;

	case EGL_HEIGHT:
		*value = s->getDrawable()->getHeight();
		break;

	case EGL_HORIZONTAL_RESOLUTION:
		*value = EGL_UNKNOWN;			//TODO Horizontal dot pitch
		break;

	case EGL_LARGEST_PBUFFER:
		if(!s->getOSWindowContext())
			*value = s->isLargestPbuffer() ? EGL_TRUE : EGL_FALSE;
		break;

	case EGL_MIPMAP_TEXTURE:
		if(!s->getOSWindowContext())
			*value = EGL_FALSE;
		break;

	case EGL_MIPMAP_LEVEL:
		if(!s->getOSWindowContext())
			*value = 0;
		break;

	case EGL_PIXEL_ASPECT_RATIO:
		*value = EGL_UNKNOWN;			//TODO Display aspect ratio
		break;

	case EGL_RENDER_BUFFER:
		*value = s->getRenderBuffer();
		break;

	case EGL_SWAP_BEHAVIOR:
		*value = EGL_BUFFER_PRESERVED;
		break;

	case EGL_TEXTURE_FORMAT:
		if(!s->getOSWindowContext())
			*value = EGL_NO_TEXTURE;
		break;

	case EGL_TEXTURE_TARGET:
		if(!s->getOSWindowContext())
			*value = EGL_NO_TEXTURE;
		break;

	case EGL_VERTICAL_RESOLUTION:
		*value = EGL_UNKNOWN;			//TODO Vertical dot pitch
		break;

	case EGL_WIDTH:
		*value = s->getDrawable()->getWidth();
		break;

	default:
		EGL_RETURN(EGL_BAD_ATTRIBUTE, EGL_FALSE);
	}
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
	EGL_GET_DISPLAY(dpy, EGL_NO_CONTEXT);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_CONTEXT);
	EGL_IF_ERROR(!display->configExists(config), EGL_BAD_CONFIG, EGL_NO_CONTEXT);
	RI_UNREF(attrib_list);

	RIEGLThread* thread = egl->getThread();
	if(!thread)
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_CONTEXT);

	//creation of OpenGL ES contexts is not allowed in this implementation
	if(thread->getBoundAPI() != EGL_OPENVG_API)
		EGL_RETURN(EGL_BAD_MATCH, EGL_NO_CONTEXT);

    OpenVGRI::VGContext* vgctx = NULL;
	RIEGLContext* c = NULL;
	try
	{
		vgctx = RI_NEW(OpenVGRI::VGContext, (share_context ? ((RIEGLContext*)share_context)->getVGContext() : NULL));	//throws bad_alloc
		c = RI_NEW(RIEGLContext, (vgctx, config));	//throws bad_alloc
		c->addReference();
		display->addContext(c);	//throws bad_alloc
	}
	catch(std::bad_alloc)
	{
        RI_DELETE(vgctx);
        RI_DELETE(c);
		EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_CONTEXT);
	}

	EGL_RETURN(EGL_SUCCESS, (EGLContext)c);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->contextExists(ctx), EGL_BAD_CONTEXT, EGL_FALSE);

	RIEGLContext* context = (RIEGLContext*)ctx;
    display->removeContext(context);
	if(!context->removeReference() )
		RI_DELETE(context);

	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(ctx != EGL_NO_CONTEXT && !display->contextExists(ctx), EGL_BAD_CONTEXT, EGL_FALSE);
	EGL_IF_ERROR(draw != EGL_NO_SURFACE && !display->surfaceExists(draw), EGL_BAD_SURFACE, EGL_FALSE);
	EGL_IF_ERROR(read != EGL_NO_SURFACE && !display->surfaceExists(read), EGL_BAD_SURFACE, EGL_FALSE);
	EGL_IF_ERROR(draw != read, EGL_BAD_MATCH, EGL_FALSE);	//TODO what's the proper error code?
	EGL_IF_ERROR((draw != EGL_NO_SURFACE && ctx == EGL_NO_CONTEXT) || (draw == EGL_NO_SURFACE && ctx != EGL_NO_CONTEXT), EGL_BAD_MATCH, EGL_FALSE);

	RIEGLSurface* s = NULL;
	RIEGLContext* c = NULL;
	if(draw != EGL_NO_SURFACE && ctx != EGL_NO_CONTEXT)
	{
		EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);

		s = (RIEGLSurface*)draw;
		c = (RIEGLContext*)ctx;

        //If either draw or read are pbuffers created with eglCreatePbufferFromClientBuffer, and the underlying bound client API buffers
        //are in use by the client API that created them, an EGL BAD ACCESS error is generated.
		EGL_IF_ERROR(s->getDrawable()->isInUse(), EGL_BAD_ACCESS, EGL_FALSE);


		//TODO properly check compatibility of surface and context:
		//-both have RGB or LUMINANCE configs
		//-buffer bit depths match
		//-configs support OpenVG
		//-both have the same display
		EGL_IF_ERROR(s->getConfig() != c->getConfig(), EGL_BAD_MATCH, EGL_FALSE);
		//TODO check if context or surfaces are already bound to another thread

		//If a native window underlying either draw or read is no longer valid, an EGL BAD NATIVE WINDOW error is generated.
        EGL_IF_ERROR(s->getOSWindowContext() && !OSIsWindow(s->getOSWindowContext()), EGL_BAD_NATIVE_WINDOW, EGL_FALSE);

		//TODO If the previous context of the calling display has unflushed commands, and the previous surface is no longer valid, an EGL BAD CURRENT SURFACE error is generated. (can this happen?)
		//TODO If the ancillary buffers for draw and read cannot be allocated, an EGL BAD ALLOC error is generated. (mask buffer?)
	}

	//check if the thread is current
	RIEGLThread* thread = egl->getCurrentThread();
	if(thread)
	{	//thread is current, release the old bindinds and remove the thread from the current thread list
		RIEGLContext* pc = thread->getCurrentContext();
		RIEGLSurface* ps = thread->getCurrentSurface();
		if(pc)
		{
			vgFlush();
			pc->getVGContext()->setDefaultDrawable(NULL);
			if(!pc->removeReference())
				RI_DELETE(pc);
		}
		if(ps)
		{
			if(!ps->removeReference())
				RI_DELETE(ps);
		}

        egl->removeCurrentThread(thread);
	}

	if( c && s )
	{
		//bind context and surface to the current display
		RIEGLThread* newThread = egl->getThread();
		if(!newThread)
			EGL_RETURN(EGL_BAD_ALLOC, EGL_FALSE);
        newThread->makeCurrent(c, s);
		c->getVGContext()->setDefaultDrawable(s->getDrawable());

		try
		{
			egl->addCurrentThread(newThread);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
			EGL_RETURN(EGL_BAD_ALLOC, EGL_FALSE);
		}

		c->addReference();
		s->addReference();
	}
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLContext eglGetCurrentContext()
{
    EGL_GET_EGL(EGL_NO_CONTEXT);
	EGLContext ret = EGL_NO_CONTEXT;
	RIEGLThread* thread = egl->getCurrentThread();
	if(thread && thread->getBoundAPI() == EGL_OPENVG_API)
    {
        ret = thread->getCurrentContext();
        RI_ASSERT(ret);
    }
	EGL_RETURN(EGL_SUCCESS, ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
    EGL_GET_EGL(EGL_NO_SURFACE);
	EGL_IF_ERROR(readdraw != EGL_READ && readdraw != EGL_DRAW, EGL_BAD_PARAMETER, EGL_NO_SURFACE);
	EGLContext ret = EGL_NO_SURFACE;
	RIEGLThread* thread = egl->getCurrentThread();
	if(thread && thread->getBoundAPI() == EGL_OPENVG_API)
    {
        ret = thread->getCurrentSurface();
        RI_ASSERT(ret);
    }
	EGL_RETURN(EGL_SUCCESS, ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	Returns the current display
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLDisplay eglGetCurrentDisplay(void)
{
    EGL_GET_EGL(EGL_NO_DISPLAY);

	RIEGLThread* thread = egl->getCurrentThread();
	if(!thread || thread->getBoundAPI() != EGL_OPENVG_API)
		EGL_RETURN(EGL_SUCCESS, EGL_NO_DISPLAY);

	RIEGLContext* ctx = thread->getCurrentContext();
    RI_ASSERT(ctx);
    EGLDisplay ret = egl->findDisplay(ctx);
	EGL_RETURN(EGL_SUCCESS, ret);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->contextExists(ctx), EGL_BAD_CONTEXT, EGL_FALSE);
	EGL_IF_ERROR(attribute != EGL_CONFIG_ID && attribute != EGL_CONTEXT_CLIENT_TYPE, EGL_BAD_ATTRIBUTE, EGL_FALSE);
	if(attribute == EGL_CONFIG_ID)
		*value = display->getConfig(((RIEGLContext*)ctx)->getConfig()).m_configID;
	if(attribute == EGL_CONTEXT_CLIENT_TYPE)
		*value = EGL_OPENVG_API;
	// \todo [kalle 05/Jul/05] Handling of EGL_RENDER_BUFFER attribute is missing.
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglBindAPI(EGLenum api)
{
    EGL_GET_EGL(EGL_FALSE);
	EGL_IF_ERROR(api != EGL_OPENVG_API && api != EGL_OPENGL_ES_API, EGL_BAD_PARAMETER, EGL_FALSE);
	RIEGLThread* thread = egl->getThread();
	if(thread)
		thread->bindAPI(api);
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLenum eglQueryAPI(void)
{
    EGL_GET_EGL(EGL_NONE);
	RIEGLThread* thread = egl->getThread();
	if(thread)
		EGL_RETURN(EGL_SUCCESS, thread->getBoundAPI());
	EGL_RETURN(EGL_SUCCESS, EGL_NONE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglWaitClient()
{
    EGL_GET_EGL(EGL_FALSE);
	RIEGLThread* thread = egl->getCurrentThread();
	if(thread && thread->getBoundAPI() == EGL_OPENVG_API)
		vgFinish();
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	Waits for OpenGL ES
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglWaitGL(void)
{
	return EGL_TRUE;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		We don't support native rendering
*//*-------------------------------------------------------------------*/

EGLBoolean eglWaitNative(EGLint engine)
{
	RI_UNREF(engine);
	return EGL_TRUE;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->surfaceExists(surface), EGL_BAD_SURFACE, EGL_FALSE);

	RIEGLSurface* s = (RIEGLSurface*)surface;

	RIEGLThread* currentThread = egl->getCurrentThread();
	EGL_IF_ERROR(!currentThread || currentThread->getCurrentSurface() != s, EGL_BAD_SURFACE, EGL_FALSE);
	EGL_IF_ERROR(!OSIsWindow(s->getOSWindowContext()), EGL_BAD_NATIVE_WINDOW, EGL_FALSE);

	vgFlush();

	if(!s->getOSWindowContext())
	{	//do nothing for other than window surfaces (NOTE: single-buffered window surfaces should return immediately as well)
		EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
	}

	int windowWidth = 0, windowHeight = 0;
    OSGetWindowSize(s->getOSWindowContext(), windowWidth, windowHeight);

	if(windowWidth != s->getDrawable()->getWidth() || windowHeight != s->getDrawable()->getHeight())
	{	//resize the back buffer
		RIEGLContext* c = currentThread->getCurrentContext();
		RI_ASSERT(c);
		try
		{
			s->getDrawable()->resize(windowWidth, windowHeight);	//throws bad_alloc
		}
		catch(std::bad_alloc)
		{
			c->getVGContext()->setDefaultDrawable(NULL);
			EGL_RETURN(EGL_BAD_ALLOC, EGL_FALSE);
		}
	}

    OSBlitToWindow(s->getOSWindowContext(), s->getDrawable());

	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	EGL_IF_ERROR(!display->surfaceExists(surface), EGL_BAD_SURFACE, EGL_FALSE);
	EGL_IF_ERROR(!target || !isValidImageFormat(target->format) || !target->data || target->width <= 0 || target->height <= 0, EGL_BAD_NATIVE_PIXMAP, EGL_FALSE);
	try
	{
		Image output(Color::formatToDescriptor((VGImageFormat)target->format), target->width, target->height, target->stride, (RIuint8*)target->data);
        output.addReference();
		output.blit(((RIEGLSurface*)surface)->getDrawable()->getColorBuffer(), 0, 0, 0, 0, target->width, target->height);	//throws bad_alloc
        output.removeReference();
	}
	catch(std::bad_alloc)
	{
	}
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		We support only swap interval one
*//*-------------------------------------------------------------------*/

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	EGL_GET_DISPLAY(dpy, EGL_FALSE);
	EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
	RI_UNREF(interval);
	EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

typedef void RI_Proc();

void (*eglGetProcAddress(const char *procname))()
{
	if(!procname)
		return NULL;
	return NULL;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EGLBoolean eglReleaseThread(void)
{
    EGL_GET_EGL(EGL_FALSE);

	//check if the thread is current
	RIEGLThread* thread = egl->getCurrentThread();
	if(thread)
	{	//thread is current, release the old bindings and remove the thread from the current thread list
		RIEGLContext* pc = thread->getCurrentContext();
		RIEGLSurface* ps = thread->getCurrentSurface();
		if(pc)
		{
			vgFlush();
			pc->getVGContext()->setDefaultDrawable(NULL);
			if(!pc->removeReference())
				RI_DELETE(pc);
		}
		if(ps)
		{
			if(!ps->removeReference())
				RI_DELETE(ps);
		}

        egl->removeCurrentThread(thread);
	}

    //destroy EGL's thread struct
    egl->destroyThread();

	//destroy the EGL instance
	releaseEGL();

	OSReleaseMutex();
    OSDeinitMutex();

	return EGL_SUCCESS;
}

#undef EGL_NUMCONFIGS
