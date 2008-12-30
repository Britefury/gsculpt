//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VECTOR3F_H__
#define VECTOR3F_H__

#include <math.h>

#include <stdio.h>

#include <Util/SETree.h>

#include <Math/Vector3.h>



class GS_DllExport Vector3f
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};

		float v[3];
	};


	inline Vector3f() : x( 0.0 ), y( 0.0 ), z( 0.0 )
	{
	}

	inline Vector3f(float ix, float iy, float iz) : x( ix ), y( iy ), z( iz )
	{
	}

	inline Vector3f(const Vector3 &v3d) : x( v3d.x ), y( v3d.y ), z( v3d.z )
	{
	}



	inline bool operator==(const Vector3f &v) const
	{
		return ( x == v.x )  &&  ( y  ==  v.y )  &&  ( z == v.z );
	}

	inline bool operator!=(const Vector3f &v) const
	{
		return ( x != v.x )  ||  ( y != v.y )  ||  ( z != v.z );
	}


	inline Vector3f operator+(const Vector3f &v) const
	{
		return Vector3f( x + v.x,  y + v.y,  z + v.z );
	}

	inline void operator+=(const Vector3f &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline Vector3f operator-(const Vector3f &v) const
	{
		return Vector3f( x - v.x,  y - v.y,  z - v.z );
	}

	inline void operator-=(const Vector3f &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline Vector3f operator*(float s) const
	{
		return Vector3f( x * s,  y * s,  z * s );
	}

	inline void operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
	}

	inline Vector3f operator-() const
	{
		return Vector3f( -x, -y, -z );
	}


	inline float dot(const Vector3f &v) const
	{
		return ( x * v.x )  +  ( y * v.y )  +  ( z * v.z );
	}

	inline Vector3f cross(const Vector3f &v) const
	{
		return Vector3f( y * v.z  -  z * v.y,
					z * v.x  -  x * v.z,
					x * v.y  -  y * v.x );
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
		z *= oneOverLength;
	}

	inline void normaliseToLength(float l)
	{
		float lOverLength = l / length();
		x *= lOverLength;
		y *= lOverLength;
		z *= lOverLength;
	}

	inline Vector3f getNormalised() const
	{
		float oneOverLength = 1.0 / length();
		return Vector3f( x * oneOverLength, y * oneOverLength, z * oneOverLength );
	}

	inline Vector3f getNormalisedToLength(float l) const
	{
		float lOverLength = l / length();
		return Vector3f( x * lOverLength, y * lOverLength, z * lOverLength );
	}



	static Vector3f min(const Vector3f &a, const Vector3f &b)
	{
		return Vector3f( std::min( a.x, b.x ), std::min( a.y, b.y ), std::min( a.z, b.z ) );
	}

	static Vector3f max(const Vector3f &a, const Vector3f &b)
	{
		return Vector3f( std::max( a.x, b.x ), std::max( a.y, b.y ), std::max( a.z, b.z ) );
	}



	inline Vector3 toVector3() const
	{
		return Vector3( x, y, z );
	}



	inline void read(FILE *f)
	{
		fread( &x, sizeof(float), 1, f );
		fread( &y, sizeof(float), 1, f );
		fread( &z, sizeof(float), 1, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( &x, sizeof(float), 1, f );
		fwrite( &y, sizeof(float), 1, f );
		fwrite( &z, sizeof(float), 1, f );
	}

	inline void read(SENode &node)
	{
		node[0]  >>  x;
		node[1]  >>  y;
		node[2]  >>  z;
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  x;
		node.addItem()  <<  y;
		node.addItem()  <<  z;
	}
};



#endif
