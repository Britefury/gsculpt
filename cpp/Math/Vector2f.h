//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VECTOR2F_H__
#define VECTOR2F_H__

#include <math.h>

#include <stdio.h>

#include <Math/Vector2.h>





class GS_DllExport Vector2f
{
public:
	float x, y;


	inline Vector2f() : x( 0.0 ), y( 0.0 )
	{
	}

	inline Vector2f(float ix, float iy) : x( ix ), y( iy )
	{
	}

	inline Vector2f(const Vector2 &v) : x( v.x ), y( v.y )
	{
	}



	inline Vector2f operator+(const Vector2f &v) const
	{
		return Vector2f( x + v.x,  y + v.y );
	}

	inline void operator+=(const Vector2f &v)
	{
		x += v.x;
		y += v.y;
	}

	inline Vector2f operator-(const Vector2f &v) const
	{
		return Vector2f( x - v.x,  y - v.y );
	}

	inline void operator-=(const Vector2f &v)
	{
		x -= v.x;
		y -= v.y;
	}

	inline Vector2f operator*(float s) const
	{
		return Vector2f( x * s,  y * s );
	}

	inline void operator*=(float s)
	{
		x *= s;
		y *= s;
	}

	inline Vector2f operator-() const
	{
		return Vector2f( -x, -y );
	}


	inline float dot(const Vector2f &v) const
	{
		return ( x * v.x )  +  ( y * v.y );
	}

	inline float cross(const Vector2f &v) const
	{
		return x * v.y  -  y * v.x;
	}

	inline float sqrLength() const
	{
		return dot( *this );
	}

	inline float length() const
	{
		return sqrt( sqrLength() );
	}

	inline void normalise()
	{
		float oneOverLength = 1.0 / length();
		x *= oneOverLength;
		y *= oneOverLength;
	}

	inline void normaliseToLength(float l)
	{
		float lOverLength = l / length();
		x *= lOverLength;
		y *= lOverLength;
	}

	inline Vector2f getNormalised() const
	{
		float oneOverLength = 1.0 / length();
		return Vector2f( x * oneOverLength, y * oneOverLength );
	}

	inline Vector2f getNormalisedToLength(float l) const
	{
		float lOverLength = l / length();
		return Vector2f( x * lOverLength, y * lOverLength );
	}


	inline Vector2 toVector2() const
	{
		return Vector2( x, y );
	}


	inline Vector2f getRotated90CCW() const
	{
		return Vector2f( -y, x );
	}

	inline Vector2f getRotated90CW() const
	{
		return Vector2f( y, -x );
	}



	inline void read(FILE *f)
	{
		fread( &x, sizeof(float), 1, f );
		fread( &y, sizeof(float), 1, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( &x, sizeof(float), 1, f );
		fwrite( &y, sizeof(float), 1, f );
	}
};



#endif
