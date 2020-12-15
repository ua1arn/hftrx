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
 * \brief	Implementation of Font class.
 * \note	
 *//*-------------------------------------------------------------------*/

#include "riFont.h"

//==============================================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Font constructor.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Font::Font(int capacityHint) :
	m_referenceCount(0),
	m_glyphs()
{
	RI_ASSERT(capacityHint >= 0);
	m_glyphs.reserve(capacityHint);
}

/*-------------------------------------------------------------------*//*!
* \brief	Font destructor.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Font::~Font()
{
	//remove references to paths and images
	for(int i=0;i<m_glyphs.size();i++)
		clearGlyph(&m_glyphs[i]);
	RI_ASSERT(m_referenceCount == 0);
}

/*-------------------------------------------------------------------*//*!
* \brief	Find a glyph based on glyphIndex.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Font::Glyph* Font::findGlyph(unsigned int index)
{
    for(int i=0;i<m_glyphs.size();i++)
    {
        if(m_glyphs[i].m_state != Glyph::GLYPH_UNINITIALIZED && m_glyphs[i].m_index == index)
            return &m_glyphs[i];
    }
    return NULL;
}

/*-------------------------------------------------------------------*//*!
* \brief	Find a free glyph or allocate a new one.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

Font::Glyph* Font::newGlyph()
{
    for(int i=0;i<m_glyphs.size();i++)
    {
        if(m_glyphs[i].m_state == Glyph::GLYPH_UNINITIALIZED)
            return &m_glyphs[i];
    }
    m_glyphs.resize(m_glyphs.size()+1);
    return &m_glyphs[m_glyphs.size()-1];
}

/*-------------------------------------------------------------------*//*!
* \brief	Free glyph and its data.
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Font::clearGlyph(Glyph* g)
{
    RI_ASSERT(g);
	if(g->m_path != VG_INVALID_HANDLE)
	{
		Path* p = (Path*)g->m_path;
		if(!p->removeReference())
			RI_DELETE(p);
	}
	if(g->m_image != VG_INVALID_HANDLE)
	{
		Image* p = (Image*)g->m_image;
		p->removeInUse();
		if(!p->removeReference())
			RI_DELETE(p);
	}
	Glyph a;
	*g = a;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Font::setGlyphToPath(unsigned int index, VGPath path, bool isHinted, const Vector2& origin, const Vector2& escapement)
{
    Glyph* g = findGlyph(index);
    if(g)
    {   //glyph exists, replace
        clearGlyph(g);
    }
    else
    {   //glyph doesn't exist, allocate a new one
        g = newGlyph();
    }

    g->m_index = index;
    g->m_state = Glyph::GLYPH_PATH;
	g->m_path = path;
    g->m_image = VG_INVALID_HANDLE;
	g->m_isHinted = isHinted;
	g->m_origin = origin;
	g->m_escapement = escapement;

    if(path != VG_INVALID_HANDLE)
    {
        Path* p = (Path*)path;
        p->addReference();
    }
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void Font::setGlyphToImage(unsigned int index, VGImage image, const Vector2& origin, const Vector2& escapement)
{
    Glyph* g = findGlyph(index);
    if(g)
    {   //glyph exists, replace
        clearGlyph(g);
    }
    else
    {   //glyph doesn't exist, allocate a new one
        g = newGlyph();
    }

    g->m_index = index;
    g->m_state = Glyph::GLYPH_IMAGE;
	g->m_path = VG_INVALID_HANDLE;
    g->m_image = image;
	g->m_isHinted = false;
	g->m_origin = origin;
	g->m_escapement = escapement;

    if(image != VG_INVALID_HANDLE)
    {
        Image* p = (Image*)image;
        p->addReference();
        p->addInUse();
    }
}

//=======================================================================

}	//namespace OpenVGRI
