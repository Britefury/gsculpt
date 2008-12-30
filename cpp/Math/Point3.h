//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POINT3_H__
#define POINT3_H__

#include <algorithm>
#include <Util/SETree.h>

#include <Math/Vector3.h>
#include <Math/Point2.h>



class GS_DllExport Point3
{
public:
	union
	{
		struct
		{
			double x, y, z;
		};

		double v[3];
	};


	inline Point3() : x( 0.0 ), y( 0.0 ), z( 0.0 )
	{
	}

	inline Point3(double ix, double iy, double iz) : x( ix ), y( iy ), z( iz )
	{
	}

	inline Point3(const Point2 &p2) : x( p2.x ), y( p2.y ), z( 0.0 )
	{
	}

	inline Point3(const Point2 &p2, double iz) : x( p2.x ), y( p2.y ), z( iz )
	{
	}

	inline Point3(const Vector3 &v) : x( v.x ), y( v.y ), z( v.z )
	{
	}



	inline bool operator==(const Point3 &p) const
	{
		return ( x == p.x )  &&  ( y == p.y )  &&  ( z == p.z );
	}

	inline bool operator!=(const Point3 &p) const
	{
		return ( x != p.x )  ||  ( y != p.y )  ||  ( z != p.z );
	}

	inline Point3 operator+(const Vector3 &v) const
	{
		return Point3( x + v.x,  y + v.y,  z + v.z );
	}

	inline void operator+=(const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline Point3 operator-(const Vector3 &v) const
	{
		return Point3( x - v.x,  y - v.y,  z - v.z );
	}

	inline void operator-=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline Vector3 operator-(const Point3 &v) const
	{
		return Vector3( x - v.x,  y - v.y,  z - v.z );
	}

	inline Vector3 toVector3() const
	{
		return Vector3( x, y, z );
	}

	inline Point2 toPoint2() const
	{
		return Point2( x, y );
	}

	inline Point2 toPoint2Axis(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_X:
			return Point2( -z, y );
		case AXIS_NEGATIVE_X:
			return Point2( z, y );
		case AXIS_Y:
			return Point2( x, -z );
		case AXIS_NEGATIVE_Y:
			return Point2( x, z );
		case AXIS_Z:
			return Point2( x, y );
		case AXIS_NEGATIVE_Z:
			return Point2( -x, y );
		default:
			return Point2( x, y );
		}
	}



	inline double dot(const Vector3 &v) const
	{
		return ( x * v.x ) + ( y * v.y ) + ( z * v.z );
	}

	inline double sqrDistanceTo(const Point3 &p) const
	{
		return ( *this - p ).sqrLength();
	}

	inline double distanceTo(const Point3 &p) const
	{
		return ( *this - p ).length();
	}




	inline double get(Axis axis) const
	{
		switch( axis )
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

	inline double getAbs(Axis axis) const
	{
		switch( axis )
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


	inline void set(Axis axis, double v)
	{
		switch( axis )
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

	inline void setAbs(Axis axis, double v)
	{
		switch( axis )
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




	inline void cumulativeAdd(const Point3 &p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
	}

	inline void sub(const Point3 &p)
	{
		x -= p.x;
		y -= p.y;
		z -= p.z;
	}

	inline void scale(double s)
	{
		x *= s;
		y *= s;
		z *= s;
	}


	inline static Point3 mul(const Point3 &p, double s)
	{
		return Point3( p.x * s,  p.y * s,  p.z * s );
	}


	inline static Point3 sum(const Point3 &a, const Point3 &b)
	{
		return Point3( a.x + b.x,  a.y + b.y,  a.z + b.z );
	}

	inline static Point3 sum(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return Point3( a.x + b.x + c.x,  a.y + b.y + c.y,  a.z + b.z + c.z );
	}

	inline static Point3 sum(const Point3 &a, const Point3 &b, const Point3 &c, const Point3 &d)
	{
		return Point3( a.x + b.x + c.x + d.x,  a.y + b.y + c.y + d.y,  a.z + b.z + c.z + d.z );
	}


	inline static Point3 py_sum2(const Point3 &a, const Point3 &b)
	{
		return sum( a, b );
	}

	inline static Point3 py_sum3(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return sum( a, b, c );
	}

	inline static Point3 py_sum4(const Point3 &a, const Point3 &b, const Point3 &c, const Point3 &d)
	{
		return sum( a, b, c, d );
	}



	inline static Point3 min(const Point3 &a, const Point3 &b)
	{
		return Point3( std::min( a.x, b.x ), std::min( a.y, b.y ), std::min( a.z, b.z ) );
	}

	inline static Point3 max(const Point3 &a, const Point3 &b)
	{
		return Point3( std::max( a.x, b.x ), std::max( a.y, b.y ), std::max( a.z, b.z ) );
	}


	inline static Point3 average(const Point3 &a, const Point3 &b)
	{
		return mul( sum( a, b ), 0.5 );
	}

	inline static Point3 average(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return mul( sum( a, b, c ), 1.0 / 3.0 );
	}

	inline static Point3 average(const Point3 &a, const Point3 &b, const Point3 &c, const Point3 &d)
	{
		return mul( sum( a, b, c, d ), 0.25 );
	}


	inline static Point3 py_average2(const Point3 &a, const Point3 &b)
	{
		return average( a, b );
	}

	inline static Point3 py_average3(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return average( a, b, c );
	}

	inline static Point3 py_average4(const Point3 &a, const Point3 &b, const Point3 &c, const Point3 &d)
	{
		return average( a, b, c, d );
	}



	inline static Point3 weightedAverage(const Point3 &a, double wa, const Point3 &b, double wb)
	{
		return Point3( a.x * wa  +  b.x * wb,
					a.y * wa  +  b.y * wb,
					a.z * wa  +  b.z * wb );
	}

	inline static Point3 weightedAverage(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc)
	{
		return Point3( a.x * wa  +  b.x * wb  +  c.x * wc,
					a.y * wa  +  b.y * wb  +  c.y * wc,
					a.z * wa  +  b.z * wb  +  c.z * wc );
	}

	inline static Point3 weightedAverage(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc, const Point3 &d, double wd)
	{
		return Point3( a.x * wa  +  b.x * wb  +  c.x * wc  +  d.x * wd,
					a.y * wa  +  b.y * wb  +  c.y * wc  +  d.y * wd,
					a.z * wa  +  b.z * wb  +  c.z * wc  +  d.z * wd );
	}


	inline static Point3 py_weightedAverage2(const Point3 &a, double wa, const Point3 &b, double wb)
	{
		return weightedAverage( a, wa, b, wb );
	}

	inline static Point3 py_weightedAverage3(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc)
	{
		return weightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point3 py_weightedAverage4(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc, const Point3 &d, double wd)
	{
		return weightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point3 normalisedWeightedAverage(const Point3 &a, double wa, const Point3 &b, double wb)
	{
		double oneOverWTotal = 1.0 / ( wa + wb );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal );
	}

	inline static Point3 normalisedWeightedAverage(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc)
	{
		double oneOverWTotal = 1.0 / ( wa + wb + wc );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal );
	}

	inline static Point3 normalisedWeightedAverage(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc, const Point3 &d, double wd)
	{
		double oneOverWTotal = 1.0 / ( wa + wb + wc + wd );
		return weightedAverage( a, wa * oneOverWTotal,
							b, wb * oneOverWTotal,
							c, wc * oneOverWTotal,
							d, wd * oneOverWTotal );
	}

	inline static Point3 py_normalisedWeightedAverage2(const Point3 &a, double wa, const Point3 &b, double wb)
	{
		return normalisedWeightedAverage( a, wa, b, wb );
	}

	inline static Point3 py_normalisedWeightedAverage3(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc );
	}

	inline static Point3 py_normalisedWeightedAverage4(const Point3 &a, double wa, const Point3 &b, double wb, const Point3 &c, double wc, const Point3 &d, double wd)
	{
		return normalisedWeightedAverage( a, wa, b, wb, c, wc, d, wd );
	}



	inline static Point3 lerp(const Point3 &a, const Point3 &b, double t)
	{
		return a  +  ( b - a ) * t;
	}



	inline void read(FILE *f)
	{
		fread( &x, sizeof(double), 1, f );
		fread( &y, sizeof(double), 1, f );
		fread( &z, sizeof(double), 1, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( &x, sizeof(double), 1, f );
		fwrite( &y, sizeof(double), 1, f );
		fwrite( &z, sizeof(double), 1, f );
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
