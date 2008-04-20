//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VECTOR2I_H__
#define VECTOR2I_H__

#include <Math/Vector2.h>



class GS_DllExport Vector2i
{
public:
	int x, y;


	inline Vector2i() : x( 0 ), y( 0 )
	{
	}

	inline Vector2i(int ix, int iy) : x( ix ), y( iy )
	{
	}


	inline Vector2i operator+(const Vector2i &v) const
	{
		return Vector2i( x + v.x,  y + v.y );
	}

	inline void operator+=(const Vector2i &v)
	{
		x += v.x;
		y += v.y;
	}


	inline Vector2i operator-(const Vector2i &v) const
	{
		return Vector2i( x - v.x,  y - v.y );
	}

	inline void operator-=(const Vector2i &v)
	{
		x -= v.x;
		y -= v.y;
	}


	inline Vector2 toVector2() const
	{
		return Vector2( x, y );
	}
};


#endif

