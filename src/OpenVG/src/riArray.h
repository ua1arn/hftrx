#ifndef __RIARRAY_H
#define __RIARRAY_H

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
 * \brief	Array class.
 * \note	
 *//*-------------------------------------------------------------------*/

#ifndef __RIDEFS_H
#include "riDefs.h"
#endif

#include <string.h>	//for memcpy

namespace OpenVGRI
{

//=======================================================================

/*-------------------------------------------------------------------*//*!
* \brief	An array class similar to std::vector.
* \param	
* \return	
* \note		Follows std::vector's naming convention (except resizeAndReallocate).
*//*-------------------------------------------------------------------*/

template <class Item> class Array
{
public:
	Array() : m_array(NULL), m_size(0), m_allocated(0) {}	//throws bad_alloc
	~Array()
	{
		RI_DELETE_ARRAY(m_array);
	}

	void		swap(Array& s)
	{
		Item* tarray = m_array;
		m_array = s.m_array;
		s.m_array = tarray;

		int tsize = m_size;
		m_size = s.m_size;
		s.m_size = tsize;

		int tallocated = m_allocated;
		m_allocated = s.m_allocated;
		s.m_allocated = tallocated;
	}

	//if more room is needed, reallocate, otherwise return
	void		reserve( int items )	//throws bad_alloc
	{
		RI_ASSERT( items >= 0 );
		if( items <= m_allocated )
			return;	//if there is room already, return

		RI_ASSERT( items > m_allocated );

		Item* newa = RI_NEW_ARRAY(Item, items);	//throws bad_alloc if runs out of memory
		for(int i=0;i<m_size;i++)
			newa[i] = m_array[i];
		RI_DELETE_ARRAY(m_array);
		m_array = newa;
		m_allocated = items;
		//doesn't change size
	}

	//reserve and change size
	void		resize( int items )	//throws bad_alloc
	{
		reserve( items );	//throws bad_alloc if runs out of memory
		m_size = items;
	}

	//resize and allocate exactly the correct amount of memory
	void		resizeAndReallocate( int items )	//throws bad_alloc
	{
		RI_ASSERT( items >= 0 );
		if( items == m_allocated )
		{
			m_size = items;
			return;
		}

		if( items == 0 )
		{
			RI_DELETE_ARRAY(m_array);
			m_size = 0;
			m_allocated = 0;
			return;
		}

		Item* newa = RI_NEW_ARRAY(Item, items);	//throws bad_alloc if runs out of memory
		int copySize = (m_size < items) ? m_size : items;	//min(m_size,items)
		for(int i=0;i<copySize;i++)
			newa[i] = m_array[i];
		RI_DELETE_ARRAY(m_array);
		m_array = newa;
		m_allocated = items;
		m_size = items;		//changes also size
	}
	void		clear()
	{
		m_size = 0;
	}

    //sort array (needs operator< defined for Item. Define it with < for increasing order and > for decreasing.)
	void		sort()
	{
		if(m_size <= 1)
			return;
		quicksort(0, m_size - 1);
	}

    //remove the first occurrence of an item from the array
    bool        remove(const Item& item)
    {
        int i=0;
        for(;i<m_size;i++)
        {
            if(m_array[i] == item)
                break;
        }
        if(i >= m_size)
            return false;   //not found
        for(;i<m_size-1;i++)
        {
            m_array[i] = m_array[i+1];
        }
        m_size--;
        return true;
    }

	RI_INLINE void			push_back( const Item& item )	//throws bad_alloc
	{
		if( m_size >= m_allocated )
			reserve( (!m_allocated) ? 8 : m_allocated * 2 );	//by default, reserve 8. throws bad_alloc if runs out of memory
		m_array[m_size++] = item;
	}
	RI_INLINE int			size() const				{ return m_size; }
	RI_INLINE Item&			operator[](int i)			{ RI_ASSERT(i >= 0 && i < m_size); return m_array[i]; }
	RI_INLINE const Item&	operator[](int i) const		{ RI_ASSERT(i >= 0 && i < m_size); return m_array[i]; }

private:
	Array(const Array& s);				//!< Not allowed.
	void operator=(const Array& s);		//!< Not allowed.

	void quicksort(int left, int right)
	{
		int i = left, j = right;
		Item x = m_array[(left+right)>>1];

		do
		{    
			while (m_array[i] < x)
				i++;
			while (x < m_array[j])
				j--;
			if (i<=j)
			{
				Item tmp = m_array[i];
				m_array[i] = m_array[j];
				m_array[j] = tmp;
				i++;
				j--;
			}
		} while (i<=j);

		if(left < j) quicksort(left, j);
		if(i < right) quicksort(i, right);
	}


	Item*		m_array;
	int			m_size;
	int			m_allocated;
};

//=======================================================================

}	//namespace OpenVGRI

#endif /* __RIARRAY_H */
