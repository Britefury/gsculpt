//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POINT2F_H__
#define POINT2F_H__

#include <stdio.h>

#include <Util/SETree.h>

#include <Math/Vector2f.h>
#include <Math/Point2.h>


class GS_DllExport Point2f
{
public:
	union
	{
		struct
		{
			float x, y;
		};

		float v[2];
	};


	inline Point2f() : x( 0.0 ), y( 0.0 )
	{
	}

	inline Point2f(float ix, float iy) : x( ix ), y( iy )
	{
	}

	inline Point2f(const Point2 &p) : x( p.x ), y( p.y )
	{
	}

	inline Point2f(const Vector2f &v) : x( v.x ), y( v.y )
	{
	}



	inline bool operator==(const Point2f &p) const
	{
		return ( x == p.x )  &&  ( y == p.y );
	}

	inline bool operator!=(const Point2f &p) const
	{
		return ( x != p.x )  ||  ( y != p.y );
	}

	inline Point2f operator+(const Vector2f &v) const
	{
		return Point2f( x + v.x,  y + v.y );
	}

	inline void operator+=(const Vector2f &v)
	{
		x += v.x;
		y += v.y;
	}

	inline Point2f operator-(const Vector2f &v) const
	{
		return Point2f( x - v.x,  y - v.y );
	}

	inline void operator-=(const Vector2f &v)
	{
		x -= v.x;
		y -= v.y;
	}

	inline Vector2f operator-(const Point2f &v) const
	{
		return Vector2f( x - v.x,  y - v.y );
	}

	inline Vector2f toVector2f() const
	{
		return Vector2f( x, y );
	}

	inline float dot(const Vector2f &v) const
	{
		return ( x * v.x ) + ( y * v.y );
	}

	inline float sqrDistanceTo(const Point2f &p) const
	{
		return ( *this - p ).sqrLength();
	}

	inline float distanceTo(const Point2f &p) const
	{
		return ( *this - p ).length();
	}



	inline Point2 toPoint2() const
	{
		return Point2( x, y );
	}




	inline void cumulativeAdd(const Point2f &p)
	{
		x += p.x;
		y += p.y;
	}

	inline void sub(const Point2f &p)
	{
		x -= p.x;
		y -= p.y;
	}

	inline void scale(float s)
	{
		x *= s;
		y *= s;
	}


	inline static Point2f mul(const Point2f &p, float s)
	{
		return Point2f( p.x * s,  p.y * s );
	}



	inline static Point2f sum(const Point2f &a, const Point2f &b)
	{
		return Point2f( a.x + b.x,
					a.y + b.y );
	}

	inline static Point2f sum(const Point2f &a, const Point2f &b,
									 const Point2f &c)
	{
		return Point2f( a.x + b.x + c.x,
					a.y + b.y + c.y );
	}

	inline static Point2f sum(const Point2f &a, const Point2f &b,
									 const Point2f &c, const Point2f &d)
	{
		return Point2f( a.x + b.x + c.x + d.x,
					a.y + b.y + c.y + d.y );
	}


	inline static Point2f py_sum2(const Point2f &a, const Point2f &b)
	{
		return sum( a, b );
	}

	inline static Point2f py_sum3(const Point2f &a, const Point2f &b, const Point2f &c)
	{
		return sum( a, b, c );
	}

	inline static Point2f py_sum4(const Point2f &a, const Point2f &b, const Point2f &c, const Point2f &d)
	{
		return sum( a, b, c, d );
	}



	inline static Point2f min(const Point2f &a, const Point2f &b)
	{
		return Point2f( std::min( a.x, b.x ), std::min( a.y, b.y ) );
	}

	inline static Point2f max(const Point2f &a, const Point2f &b)
	{
		return Point2f( std::max( a.x, b.x ), std::max( a.y, b.y ) );
	}



	inline static Point2f average(const Point2f &a, const Point2f &b)
	{
		return mul( sum( a, b ), 0.5 );
	}

	inline static Point2f average(const Point2f &a, const Point2f &b, const Point2f &c)
	{
		return mul( sum( a, b, c ), 1.0 / 3.0 );
	}

	inline static Point2f average(const Point2f &a, const Point2f &b, const Point2f &c, const Point2f &d)
	{
		return mul( sum( a, b, c, d ), 0.25 );
	}


	inline static Point2f py_average2(const Point2f &a, const Point2f &b)
	{
		return average( a, b );
	}

	inline static Point2f py_average3(const Point2f &a, const Point2f &b, const Point2f &c)
	{
		return average( a, b, c );
	}

	inline static Point2f py_average4(const Point2f &a, const Point2f &b, const Point2f &c, const Point2f &d)
	{
		return average( a, b, c, d );
	}



	inline static Point2f weightedAverage(const Point2f &a, float wa, const Point2f &b, float wb)
	{
		return Point2f( a.x * wa  +  b.x * wb,
					a.y * wa  +  b.y * wb );
	}

	inline static Point2f weightedAverage(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc)
	{
		return Point2f( a.x * wa  +  b.x * wb  +  c.x * wc,
					a.y * wa  +  b.y * wb  +  c.y * wc );
	}

	inline static Point2f weightedAverage(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc, const Point2f &d, float wd)
	{
		return Point2f( a.x * wa  +  b.x * wb  +  c.x * wc  +  d.x * wd,
					a.y * wa  +  b.y * wb  +  c.y * wc  +  d.y * wd );
	}


	inline static Point2f py_weightedAverage2(const Point2f &a, float wa, const Point2f &b, float wb)
	{
		return weightedAverage( a, wa, b, wb );
	}

	inline static Point2f py_weightedAverage3(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc)
	{
		return weightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point2f py_weightedAverage4(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc, const Point2f &d, float wd)
	{
		return weightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point2f normalisedWeightedAverage(const Point2f &a, float wa, const Point2f &b, float wb)
	{
		float oneOverWTotal = 1.0 / ( wa + wb );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal );
	}

	inline static Point2f normalisedWeightedAverage(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc)
	{
		float oneOverWTotal = 1.0 / ( wa + wb + wc );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal );
	}

	inline static Point2f normalisedWeightedAverage(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc, const Point2f &d, float wd)
	{
		float oneOverWTotal = 1.0 / ( wa + wb + wc + wd );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal,
							d, wd * oneOverWTotal );
	}


	inline static Point2f py_normalisedWeightedAverage2(const Point2f &a, float wa, const Point2f &b, float wb)
	{
		return normalisedWeightedAverage( a, wa, b, wb );
	}

	inline static Point2f py_normalisedWeightedAverage3(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point2f py_normalisedWeightedAverage4(const Point2f &a, float wa, const Point2f &b, float wb, const Point2f &c, float wc, const Point2f &d, float wd)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point2f lerp(const Point2f &a, const Point2f &b, float t)
	{
		return a  +  ( b - a ) * t;
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

	inline void read(SENode &node)
	{
		node[0]  >>  x;
		node[1]  >>  y;
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  x;
		node.addItem()  <<  y;
	}
};



#endif
