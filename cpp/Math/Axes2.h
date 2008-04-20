//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef AXES2_H__
#define AXES2_H__

#include <Math/Vector2.h>



class GS_DllExport Axes2
{
public:
	Vector2 i, j;


public:
	inline Axes2()
	{
		i = Vector2( 1.0, 0.0 );
		j = Vector2( 0.0, 1.0 );
	}

	inline Axes2(const Vector2 &ai, const Vector2 &aj)
	{
		i = ai;
		j = aj;
	}



	inline Axes2 operator+(const Axes2 &a) const
	{
		return Axes2( i + a.i, j + a.j );
	}

	inline Axes2 &operator+=(const Axes2 &a)
	{
		i += a.i;
		j += a.j;
		return *this;
	}

	inline Axes2 operator*(double t) const
	{
		return Axes2( i * t, j * t );
	}

	inline Axes2 &operator*=(double t)
	{
		i *= t;
		j *= t;
		return *this;
	}
	



	inline void normalise()
	{
		i.normalise();
		j.normalise();
	}
};



#endif
