#include "hardware.h"
#include "src/display/display.h"

#if WITHOPENVG

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

	static Drawable * d0 [LCDMODE_MAIN_PAGES];
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

void openvg_init(PACKEDCOLORPIP_T * const * frames)
{

#if LCDMODE_MAIN_RGB565
	VGImageFormat f = VG_sRGB_565;
	if (isBigEndian())
		f = VG_sBGR_565;
#elif LCDMODE_MAIN_ARGB888 && (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! WITHTFT_OVER_LVDS
	VGImageFormat f = VG_sXRGB_8888;	// 1+3 byte format
	 if(isBigEndian())
		 f = VG_sBGRX_8888;
#elif LCDMODE_MAIN_ARGB888 && (CPUSTYLE_XC7Z || CPUSTYLE_XCZU)
	VGImageFormat f = VG_sXRGB_8888;	// 1+3 byte format
	 if(isBigEndian())
		 f = VG_sBGRX_8888;
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
		const OpenVGRI::Color::Descriptor cds(OpenVGRI::Color::formatToDescriptor(f));

		for (unsigned i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
		{
			int maskBits = 0;
			OpenVGRI::Drawable * d = RI_NEW(OpenVGRI::Drawable, (cds, DIM_X, DIM_Y, GXADJ(DIM_X) * sizeof (PACKEDCOLORPIP_T), (OpenVGRI::RIuint8 *) frames [i], maskBits));	//throws bad_alloc
			RI_ASSERT(d);
			d->addReference();
			OpenVGRI::d0 [i] = d;
		}
	}
	catch(const std::bad_alloc &)
	{
	}

	OpenVGRI::ctx.setDefaultDrawable(OpenVGRI::d0 [0]);
}

void openvg_deinit(void)
{
	for (unsigned i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		RI_DELETE(OpenVGRI::d0 [i]);
	}
}

void openvg_next(unsigned page)
{
	RI_ASSERT(OpenVGRI::d0 [page]);
	OpenVGRI::ctx.setDefaultDrawable(OpenVGRI::d0 [page]);

}
#else /* WITHOPENVG */

void openvg_init(PACKEDCOLORPIP_T * const * frames)
{
}

void openvg_deinit(void)
{
}

void openvg_next(unsigned page)
{
}

#endif /* WITHOPENVG */
