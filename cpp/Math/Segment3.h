//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SEGMENT3_H__
#define SEGMENT3_H__

#include <boost/python.hpp>




#include <stdio.h>

#include <Math/clamp.h>

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Vector3.h>
#include <Math/Segment2.h>
#include <Math/Side.h>

/*
3D LINE SEGMENT
*/



class GS_DllExport Segment3
{
public:
	Point3 a, b;


	inline Segment3()
	{
		a = b = Point3();
	}

	inline Segment3(const Point3 &p1, const Point3 &p2)
	{
		a = p1;
		b = p2;
	}

	inline Segment3(const Point3 &p, const Vector3 &direction)
	{
		a = p;
		b = p + direction;
	}



public:
	//read and write methods
	void read(FILE *f);
	void write(FILE *f) const;



	Segment2 toSegment2() const
	{
		return Segment2( a.toPoint2(), b.toPoint2() );
	}




	//get direction of line
	inline Vector3 getDirection() const
	{
		return b - a;
	}


	//get length of line
	inline double length() const
	{
		return a.distanceTo( b );
	}

	inline double sqrLength() const
	{
		return a.sqrDistanceTo( b );
	}


	//translation
	inline Segment3 operator+(const Vector3 &v) const
	{
		return Segment3( a + v, b + v );
	}

	inline void operator+=(const Vector3 &v)
	{
		a += v;
		b += v;
	}

	inline Segment3 operator-(const Vector3 &v) const
	{
		return Segment3( a - v, b - v );
	}

	inline void operator-=(const Vector3 &v)
	{
		a -= v;
		b -= v;
	}


	//get the closest point on the line to p
	inline Point3 closestPointTo(const Point3 &p, double &t) const
	{
		Vector3 ab = getDirection();
		Vector3 ap = p - a;

		//t is the position of p along ab
		//if t == 0: closest point is at a
		//if t == 1: closest point is at b
		t = ap.dot( ab )  /  ab.dot( ab );

		t = clamp( t, 0.0, 1.0 );

		return a  +  ab * t;
	}

	inline boost::python::tuple py_closestPointTo(const Point3 &p) const
	{
		double t = 0.0;
		Point3 point = closestPointTo( p, t );
		return boost::python::make_tuple( point, t );
	}

	inline Point3 closestPointTo(const Point3 &p) const
	{
		double t;
		return closestPointTo( p, t );
	}

	//distance from the line to p
	inline double distanceTo(const Point3 &p) const
	{
		return p.distanceTo( closestPointTo( p ) );
	}

	//distance squared from the line to p
	inline double sqrDistanceTo(const Point3 &p) const
	{
		return p.sqrDistanceTo( closestPointTo( p ) );
	}


	//compute the intersection between the plane p and this
	//returns:
	//		true if intersection found

	//	t will contain the distance along the line of the intersection
	// intersection will contain the intersection point
	inline bool intersect(const Plane &p, double &t, Point3 &intersection) const
	{
		//raytrace the line against the plane
		double nDotDirection = p.n.dot( getDirection() );

		if ( nDotDirection == 0.0 )
		{
			return false;
		}

		t = ( p.d - a.dot( p.n ) ) / nDotDirection;

		if ( t >= 0.0  &&  t <= 1.0 )
		{
			intersection = a + ( getDirection() * t );
			return true;
		}
		else
		{
			return false;
		}
	}

	inline boost::python::tuple py_intersect(const Plane &p) const
	{
		double t = 0.0;
		Point3 intersection;
		bool success = intersect( p, t, intersection );
		return boost::python::make_tuple( success, t, intersection );
	}


	//clip this so that it is wholly on the positive side of p
	bool clip(const Plane &p);


	//translate the segment by @v
	inline void translate(const Vector3 &v)
	{
		a += v;
		b += v;
	}

	//scale the line by @s
	inline void scale(double s)
	{
		//scale the start and end points
		a.scale( s );
		b.scale( s );
	}



	//get the mid-point
	inline Point3 getMidPoint() const
	{
		return Point3::average( a, b );
	}


	//get a point on the line
	inline Point3 getPoint(double t) const
	{
		return Point3::lerp( a, b, t );
	}


	//compute which side of the plane *this is on
	inline Side side(const Plane &p) const
	{
		if ( a == p  &&  b == p )
		{
			return SIDE_ON;
		}
		else if ( a >= p  &&  b >= p )
		{
			return SIDE_POSITIVE;
		}
		else if ( a <= p  &&  b <= p )
		{
			return SIDE_NEGATIVE;
		}
		else
		{
			return SIDE_BOTH;
		}
	}
};



#endif
