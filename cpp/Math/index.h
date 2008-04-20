//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef INDEX_H__
#define INDEX_H__

#include <Math/clamp.h>


GS_DllExport inline int prevIndex(int n, int size)
{
	return n == 0  ?  size - 1  :  n - 1;
}

GS_DllExport inline int nextIndex(int n, int size)
{
	return n == size - 1  ?  0  :  n + 1;
}


GS_DllExport inline int clampPrevIndex(int n, int size, bool closed)
{
	return ( closed  ?  prevIndex( n, size )  :  clampLower( n - 1, 0 ) );
}

GS_DllExport inline int clampNextIndex(int n, int size, bool closed)
{
	return ( closed  ?  nextIndex( n, size )  :  clampUpper( n + 1, size - 1 ) );
}


GS_DllExport inline int wrapSingleUnderflow(int n, int size)
{
	return n < 0  ?  n + size  :  n;
}

GS_DllExport inline int wrapSingleOverflow(int n, int size)
{
	return n >= size  ?  n - size  :  n;
}

GS_DllExport inline int wrapSingle(int n, int size)
{
	return wrapSingleOverflow( wrapSingleUnderflow( n, size ), size );
}


GS_DllExport inline int wrappedDifference(int from, int to, int size)
{
	if ( to < from )
	{
		return to + size - from;
	}
	else
	{
		return to - from;
	}
}



#endif

