//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POINT3F_H__
#define POINT3F_H__

#include <stdio.h>

#include <Math/Vector3f.h>
#include <Math/Point2f.h>
#include <Math/Point3.h>



class GS_DllExport Point3f
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


	inline Point3f() : x( 0.0 ), y( 0.0 ), z( 0.0 )
	{
	}

	inline Point3f(float ix, float iy, float iz) : x( ix ), y( iy ), z( iz )
	{
	}

	inline Point3f(const Point2f &p2) : x( p2.x ), y( p2.y ), z( 0.0 )
	{
	}

	inline Point3f(const Point2f &p2, float iz) : x( p2.x ), y( p2.y ), z( iz )
	{
	}

	inline Point3f(const Vector3f &v) : x( v.x ), y( v.y ), z( v.z )
	{
	}

	inline Point3f(const Point3 &p) : x( p.x ), y( p.y ), z( p.z )
	{
	}



	inline bool operator==(const Point3f &p) const
	{
		return ( x == p.x )  &&  ( y == p.y )  &&  ( z == p.z );
	}

	inline bool operator!=(const Point3f &p) const
	{
		return ( x != p.x )  ||  ( y != p.y )  ||  ( z != p.z );
	}

	inline Point3f operator+(const Vector3f &v) const
	{
		return Point3f( x + v.x,  y + v.y,  z + v.z );
	}

	inline void operator+=(const Vector3f &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline Point3f operator-(const Vector3f &v) const
	{
		return Point3f( x - v.x,  y - v.y,  z - v.z );
	}

	inline void operator-=(const Vector3f &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline Vector3f operator-(const Point3f &v) const
	{
		return Vector3f( x - v.x,  y - v.y,  z - v.z );
	}

	inline Vector3f toVector3f() const
	{
		return Vector3f( x, y, z );
	}

	inline Point2f toPoint2f() const
	{
		return Point2f( x, y );
	}

	inline Point2f toPoint2fAxis(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_X:
			return Point2f( -z, y );
		case AXIS_NEGATIVE_X:
			return Point2f( z, y );
		case AXIS_Y:
			return Point2f( x, -z );
		case AXIS_NEGATIVE_Y:
			return Point2f( x, z );
		case AXIS_Z:
			return Point2f( x, y );
		case AXIS_NEGATIVE_Z:
			return Point2f( -x, y );
		default:
			return Point2f( x, y );
		}
	}

	inline Point3 toPoint3() const
	{
		return Point3( x, y, z );
	}






	inline float dot(const Vector3f &v) const
	{
		return ( x * v.x ) + ( y * v.y ) + ( z * v.z );
	}

	inline float sqrDistanceTo(const Point3f &p) const
	{
		return ( *this - p ).sqrLength();
	}

	inline float distanceTo(const Point3f &p) const
	{
		return ( *this - p ).length();
	}




	inline float get(Axis axis) const
	{
		switch ( axis )
		{
		case AXIS_X:
			return x;
		case AXIS_NEGATIVE_X:
			return -x;
		case AXIS_Y:
			return y;
		case AXIS_NEGATIVE_Y:
			return -y;
		case AXIS_Z:
			return z;
		case AXIS_NEGATIVE_Z:
			return -z;
		default:
			return 0.0;
		}
	}

	inline float getAbs(Axis axis) const
	{
		switch ( axis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			return x;
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			return y;
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			return z;
		default:
			return 0.0;
		}
	}


	inline void set(Axis axis, float v)
	{
		switch ( axis )
		{
		case AXIS_X:
			x = v;
			break;
		case AXIS_NEGATIVE_X:
			x = -v;
			break;
		case AXIS_Y:
			y = v;
			break;
		case AXIS_NEGATIVE_Y:
			y = -v;
			break;
		case AXIS_Z:
			z = v;
			break;
		case AXIS_NEGATIVE_Z:
			z = -v;
			break;
		default:
			break;
		}
	}

	inline void setAbs(Axis axis, float v)
	{
		switch ( axis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			x = v;
			break;
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			y = v;
			break;
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			z = v;
			break;
		default:
			break;
		}
	}


	inline void flatten(Axis axis)
	{
		set( axis, 0.0 );
	}



	inline void cumulativeAdd(const Point3f &p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
	}

	inline void sub(const Point3f &p)
	{
		x -= p.x;
		y -= p.y;
		z -= p.z;
	}

	inline void scale(float s)
	{
		x *= s;
		y *= s;
		z *= s;
	}


	inline static Point3f mul(const Point3f &p, float s)
	{
		return Point3f( p.x * s,  p.y * s,  p.z * s );
	}


	inline static Point3f sum(const Point3f &a, const Point3f &b)
	{
		return Point3f( a.x + b.x,  a.y + b.y,  a.z + b.z );
	}

	inline static Point3f sum(const Point3f &a, const Point3f &b, const Point3f &c)
	{
		return Point3f( a.x + b.x + c.x,  a.y + b.y + c.y,  a.z + b.z + c.z );
	}

	inline static Point3f sum(const Point3f &a, const Point3f &b, const Point3f &c, const Point3f &d)
	{
		return Point3f( a.x + b.x + c.x + d.x,  a.y + b.y + c.y + d.y,  a.z + b.z + c.z + d.z );
	}


	inline static Point3f py_sum2(const Point3f &a, const Point3f &b)
	{
		return sum( a, b );
	}

	inline static Point3f py_sum3(const Point3f &a, const Point3f &b, const Point3f &c)
	{
		return sum( a, b, c );
	}

	inline static Point3f py_sum4(const Point3f &a, const Point3f &b, const Point3f &c, const Point3f &d)
	{
		return sum( a, b, c, d );
	}



	inline static Point3f min(const Point3f &a, const Point3f &b)
	{
		return Point3f( std::min( a.x, b.x ), std::min( a.y, b.y ), std::min( a.z, b.z ) );
	}

	inline static Point3f max(const Point3f &a, const Point3f &b)
	{
		return Point3f( std::max( a.x, b.x ), std::max( a.y, b.y ), std::max( a.z, b.z ) );
	}


	inline static Point3f average(const Point3f &a, const Point3f &b)
	{
		return mul( sum( a, b ), 0.5 );
	}

	inline static Point3f average(const Point3f &a, const Point3f &b, const Point3f &c)
	{
		return mul( sum( a, b, c ), 1.0 / 3.0 );
	}

	inline static Point3f average(const Point3f &a, const Point3f &b, const Point3f &c, const Point3f &d)
	{
		return mul( sum( a, b, c, d ), 0.25 );
	}


	inline static Point3f py_average2(const Point3f &a, const Point3f &b)
	{
		return average( a, b );
	}

	inline static Point3f py_average3(const Point3f &a, const Point3f &b, const Point3f &c)
	{
		return average( a, b, c );
	}

	inline static Point3f py_average4(const Point3f &a, const Point3f &b, const Point3f &c, const Point3f &d)
	{
		return average( a, b, c, d );
	}



	inline static Point3f weightedAverage(const Point3f &a, float wa, const Point3f &b, float wb)
	{
		return Point3f( a.x * wa  +  b.x * wb,
					a.y * wa  +  b.y * wb,
					a.z * wa  +  b.z * wb );
	}

	inline static Point3f weightedAverage(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc)
	{
		return Point3f( a.x * wa  +  b.x * wb  +  c.x * wc,
					a.y * wa  +  b.y * wb  +  c.y * wc,
					a.z * wa  +  b.z * wb  +  c.z * wc );
	}

	inline static Point3f weightedAverage(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc, const Point3f &d, float wd)
	{
		return Point3f( a.x * wa  +  b.x * wb  +  c.x * wc  +  d.x * wd,
					a.y * wa  +  b.y * wb  +  c.y * wc  +  d.y * wd,
					a.z * wa  +  b.z * wb  +  c.z * wc  +  d.z * wd );
	}


	inline static Point3f py_weightedAverage2(const Point3f &a, float wa, const Point3f &b, float wb)
	{
		return weightedAverage( a, wa, b, wb );
	}

	inline static Point3f py_weightedAverage3(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc)
	{
		return weightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point3f py_weightedAverage4(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc, const Point3f &d, float wd)
	{
		return weightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point3f normalisedWeightedAverage(const Point3f &a, float wa, const Point3f &b, float wb)
	{
		float oneOverWTotal = 1.0 / ( wa + wb );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal );
	}

	inline static Point3f normalisedWeightedAverage(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc)
	{
		float oneOverWTotal = 1.0 / ( wa + wb + wc );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal );
	}

	inline static Point3f normalisedWeightedAverage(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc, const Point3f &d, float wd)
	{
		float oneOverWTotal = 1.0 / ( wa + wb + wc + wd );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal,
							d, wd * oneOverWTotal );
	}

	inline static Point3f py_normalisedWeightedAverage2(const Point3f &a, float wa, const Point3f &b, float wb)
	{
		return normalisedWeightedAverage( a, wa, b, wb );
	}

	inline static Point3f py_normalisedWeightedAverage3(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point3f py_normalisedWeightedAverage4(const Point3f &a, float wa, const Point3f &b, float wb, const Point3f &c, float wc, const Point3f &d, float wd)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point3f lerp(const Point3f &a, const Point3f &b, float t)
	{
		return a  +  ( b - a ) * t;
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
};



#endif
