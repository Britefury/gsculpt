//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef INPLACEALLOCATOR_H__
#define INPLACEALLOCATOR_H__

#include <memory>

/*
*******************************************************************************
											InPlaceAllocator
*******************************************************************************
*/

template <typename T, unsigned int size, typename Alloc = std::allocator<T> > class InPlaceAllocator
{
private:
	char inPlaceData[sizeof(T) * size];
	bool allocatedFlag;
	Alloc alloc;


	inline T * getInPlaceData()
	{
		return reinterpret_cast<T*>( inPlaceData );
	}

	inline void deallocateInPlace(T *data, unsigned int n)
	{
	}


public:
	inline InPlaceAllocator()
	{
		allocatedFlag = false;
	}

	inline T * allocate(unsigned int n)
	{
		if ( n <= size  &&  !allocatedFlag )
		{
			allocatedFlag = true;
			return getInPlaceData();
		}
		else
		{
			return alloc.allocate( n );
		}
	}

	inline void deallocate(T *data, unsigned int n)
	{
		if ( n <= size  &&  data == getInPlaceData()  &&  allocatedFlag )
		{
			allocatedFlag = false;
		}
		else
		{
			alloc.deallocate( data, n );
		}
	}

	inline void construct(T *element, const T &value)
	{
		alloc.construct( element, value );
	}

	inline void destroy(T *element)
	{
		alloc.destroy( element );
	}
};



#endif
