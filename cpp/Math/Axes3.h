//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef AXES3_H__
#define AXES3_H__

#include <Math/Vector3.h>



class GS_DllExport Axes3
{
public:
	Vector3 i, j, k;


public:
	inline Axes3()
	{
		i = Vector3( 1.0, 0.0, 0.0 );
		j = Vector3( 0.0, 1.0, 0.0 );
		k = Vector3( 0.0, 0.0, 1.0 );
	}

	inline Axes3(Axis ai, Axis aj, Axis ak)
	{
		i = Vector3( ai );
		j = Vector3( aj );
		k = Vector3( ak );
	}

	inline Axes3(const Vector3 &ai, const Vector3 &aj, const Vector3 &ak)
	{
		i = ai;
		j = aj;
		k = ak;
	}

	inline Axes3(Axis axesIdentifier)
	{
		switch ( axesIdentifier )
		{
		case AXIS_X:
			i = Vector3( 0.0, 0.0, -1.0 );
			j = Vector3( 0.0, 1.0, 0.0 );
			k = Vector3( 1.0, 0.0, 0.0 );
			break;
		case AXIS_NEGATIVE_X:
			i = Vector3( 0.0, 0.0, 1.0 );
			j = Vector3( 0.0, 1.0, 0.0 );
			k = Vector3( -1.0, 0.0, 0.0 );
			break;
		case AXIS_Y:
			i = Vector3( 1.0, 0.0, 0.0 );
			j = Vector3( 0.0, 0.0, -1.0 );
			k = Vector3( 0.0, 1.0, 0.0 );
			break;
		case AXIS_NEGATIVE_Y:
			i = Vector3( 1.0, 0.0, 0.0 );
			j = Vector3( 0.0, 0.0, 1.0 );
			k = Vector3( 0.0, -1.0, 0.0 );
			break;
		case AXIS_Z:
			i = Vector3( 1.0, 0.0, 0.0 );
			j = Vector3( 0.0, 1.0, 0.0 );
			k = Vector3( 0.0, 0.0, 1.0 );
			break;
		case AXIS_NEGATIVE_Z:
			i = Vector3( -1.0, 0.0, 0.0 );
			j = Vector3( 0.0, 1.0, 0.0 );
			k = Vector3( 0.0, 0.0, -1.0 );
			break;
		default:
			i = Vector3( 1.0, 0.0, 0.0 );
			j = Vector3( 0.0, 1.0, 0.0 );
			k = Vector3( 0.0, 0.0, 1.0 );
			break;
		}		
	}




	inline Axes3 operator+(const Axes3 &a) const
	{
		return Axes3( i + a.i, j + a.j, k + a.k );
	}

	inline Axes3 &operator+=(const Axes3 &a)
	{
		i += a.i;
		j += a.j;
		k += a.k;
		return *this;
	}

	inline Axes3 operator*(double t) const
	{
		return Axes3( i * t, j * t, k * t );
	}

	inline Axes3 &operator*=(double t)
	{
		i *= t;
		j *= t;
		k *= t;
		return *this;
	}
	



	inline void normalise()
	{
		i.normalise();
		j.normalise();
		k.normalise();
	}


	inline Axes3 getToX() const
	{
		return Axes3( -k, j, i );
	}

	inline Axes3 getToY() const
	{
		return Axes3( i, -k, j );
	}

	inline Axes3 getToZ() const
	{
		return *this;
	}
};



#endif
