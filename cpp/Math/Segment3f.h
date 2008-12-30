//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SEGMENT3F_H__
#define SEGMENT3F_H__

#include <boost/python.hpp>


#include <stdio.h>

#include <Math/clamp.h>

#include <Math/Point3f.h>
#include <Math/Vector3f.h>
#include <Math/Segment3.h>

/*
3D LINE SEGMENT
*/



class GS_DllExport Segment3f
{
public:
	Point3f a, b;


	inline Segment3f()
	{
		a = b = Point3f();
	}

	inline Segment3f(const Point3f &p1, const Point3f &p2)
	{
		a = p1;
		b = p2;
	}

	inline Segment3f(const Point3f &p, const Vector3f &direction)
	{
		a = p;
		b = p + direction;
	}

	inline Segment3f(const Segment3 &seg)
				: a( Point3f( seg.a ) ), b( Point3f( seg.b ) )
	{
	}



public:
	//read and write methods
	void read(FILE *f);
	void write(FILE *f) const;



	//get direction of line
	inline Vector3f getDirection() const
	{
		return b - a;
	}


	//get length of line
	inline float length() const
	{
		return a.distanceTo( b );
	}

	inline float sqrLength() const
	{
		return a.sqrDistanceTo( b );
	}


	//translation
	inline Segment3f operator+(const Vector3f &v) const
	{
		return Segment3f( a + v, b + v );
	}

	inline void operator+=(const Vector3f &v)
	{
		a += v;
		b += v;
	}

	inline Segment3f operator-(const Vector3f &v) const
	{
		return Segment3f( a - v, b - v );
	}

	inline void operator-=(const Vector3f &v)
	{
		a -= v;
		b -= v;
	}


	//get the closest point on the line to p
	inline Point3f closestPointTo(const Point3f &p, float &t) const
	{
		Vector3f ab = getDirection();
		Vector3f ap = p - a;

		//t is the position of p along ab
		//if t == 0: closest point is at a
		//if t == 1: closest point is at b
		t = ap.dot( ab )  /  ab.dot( ab );

		t = clamp( t, 0.0f, 1.0f );

		return a  +  ab * t;
	}

	inline boost::python::tuple py_closestPointTo(const Point3f &p) const
	{
		float t = 0.0;
		Point3f point = closestPointTo( p, t );
		return boost::python::make_tuple( point, t );
	}

	inline Point3f closestPointTo(const Point3f &p) const
	{
		float t;
		return closestPointTo( p, t );
	}

	//distance from the line to p
	inline float distanceTo(const Point3f &p) const
	{
		return p.distanceTo( closestPointTo( p ) );
	}

	//distance squared from the line to p
	inline float sqrDistanceTo(const Point3f &p) const
	{
		return p.sqrDistanceTo( closestPointTo( p ) );
	}


	//compute the intersection between the plane p and this
	//returns:
	//		true if intersection found

	//translate the segment by @v
	inline void translate(const Vector3f &v)
	{
		a += v;
		b += v;
	}

	//scale the line by @s
	inline void scale(float s)
	{
		//scale the start and end points
		a.scale( s );
		b.scale( s );
	}



	//get the mid-point
	inline Point3f getMidPoint() const
	{
		return Point3f::average( a, b );
	}


	//get a point on the line
	inline Point3f getPoint(float t) const
	{
		return Point3f::lerp( a, b, t );
	}
};



#endif
