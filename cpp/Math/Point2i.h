//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POINT2I_H__
#define POINT2I_H__

#include <Math/Point2.h>
#include <Math/Vector2i.h>


class GS_DllExport Point2i
{
public:
	union
	{
		struct
		{
			int x, y;
		};

		int v[2];
	};


	inline Point2i()
	{
		x = y = 0;
	}

	inline Point2i(int ix, int iy)
	{
		x = ix;
		y = iy;
	}

	inline Point2i(const Vector2i &v)
	{
		x = v.x;
		y = v.y;
	}


	inline Point2i operator+(const Vector2i &v) const
	{
		return Point2i( x + v.x,  y + v.y );
	}

	inline void operator+=(const Vector2i &v)
	{
		x += v.x;
		y += v.y;
	}


	inline Point2i operator-(const Vector2i &v) const
	{
		return Point2i( x - v.x,  y - v.y );
	}

	inline void operator-=(const Vector2i &v)
	{
		x -= v.x;
		y -= v.y;
	}

	inline Vector2i operator-(const Point2i &p) const
	{
		return Vector2i( x - p.x,  y - p.y );
	}


	inline Vector2i toVector2i() const
	{
		return Vector2i( x, y );
	}

	inline Point2 toPoint2() const
	{
		return Point2( x, y );
	}
};

#endif
