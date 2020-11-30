#ifndef __RIFONT_H
#define __RIFONT_H

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

#ifndef _OPENVG_H
#include "openvg.h"
#endif

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIARRAY_H
#include "riArray.h"
#endif

#ifndef __RIPATH_H
#include "riPath.h"
#endif

#ifndef __RIIMAGE_H
#include "riImage.h"
#endif

//==============================================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Storage and operations for VGFont.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

class Font
{
public:
	struct Glyph
	{
        enum State
        {
            GLYPH_UNINITIALIZED     = 0,
            GLYPH_PATH              = 1,
            GLYPH_IMAGE             = 2
        };
		Glyph()				{ m_state = GLYPH_UNINITIALIZED; m_path = m_image = VG_INVALID_HANDLE; m_isHinted = false; m_origin.set(0.0f, 0.0f); m_escapement.set(0.0f, 0.0f); }
        unsigned int m_index;
        State        m_state;
		VGPath		 m_path;
		VGImage		 m_image;
		bool		 m_isHinted;
		Vector2		 m_origin;
		Vector2		 m_escapement;
	};

	Font(int capacityHint);	//throws bad_alloc
	~Font();

	int				getNumGlyphs() const					{ int n=0; for(int i=0;i<m_glyphs.size();i++) { if(m_glyphs[i].m_state != Glyph::GLYPH_UNINITIALIZED) n++; } return n; }
	void			addReference()							{ m_referenceCount++; }
	int				removeReference()						{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }

	void			setGlyphToPath(unsigned int index, VGPath path, bool isHinted, const Vector2& origin, const Vector2& escapement);    //throws bad_alloc
	void			setGlyphToImage(unsigned int index, VGImage image, const Vector2& origin, const Vector2& escapement);    //throws bad_alloc
    Glyph*          findGlyph(unsigned int index);
    void            clearGlyph(Glyph* g);
private:
	Font(const Font&);						//!< Not allowed.
	void operator=(const Font&);			//!< Not allowed.

    Glyph*          newGlyph();    //throws bad_alloc

	int				m_referenceCount;
	Array<Glyph>	m_glyphs;
};

//=======================================================================

}	//namespace OpenVGRI

//=======================================================================

#endif /* __RIFONT_H */
