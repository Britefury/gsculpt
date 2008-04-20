//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RAY3_H__
#define RAY3_H__

#include <boost/python.hpp>




#include <Math/clamp.h>

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Vector3.h>

/*
3D LINE
*/



class GS_DllExport Ray3
{
public:
	Point3 origin;
	Vector3 direction;


	inline Ray3()
	{
	}

	inline Ray3(const Point3 &p1, const Point3 &p2)
			: origin( p1 ), direction( p2 - p1 )
	{
	}

	inline Ray3(const Point3 &p, const Vector3 &direction)
			: origin( p ), direction( direction )
	{
	}



	//translation
	inline Ray3 operator+(const Vector3 &v) const
	{
		return Ray3( origin + v, direction );
	}

	inline void operator+=(const Vector3 &v)
	{
		origin += v;
	}

	inline Ray3 operator-(const Vector3 &v) const
	{
		return Ray3( origin - v, direction );
	}

	inline void operator-=(const Vector3 &v)
	{
		origin -= v;
	}



	//get a point on the line
	inline Point3 getPoint(double t) const
	{
		return origin  +  direction * t;
	}



	//get the closest point on the line to p
	inline double closestPointParam(const Point3 &p) const
	{
		double t = direction.dot( p - origin )  /  direction.sqrLength();
		return clampLower( t, 0.0 );
	}

	inline Point3 closestPointTo(const Point3 &p, double &t) const
	{
		t = closestPointParam( p );
		return getPoint( t );
	}

	inline boost::python::tuple py_closestPointTo(const Point3 &p) const
	{
		double t = closestPointParam( p );
		return boost::python::make_tuple( t, getPoint( t ) );
	}

	inline Point3 closestPointTo(const Point3 &p) const
	{
		return getPoint( closestPointParam( p ) );
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
		double nDotDirection = p.n.dot( direction );

		if ( nDotDirection == 0.0 )
		{
			return false;
		}

		t = ( p.d - origin.dot( p.n ) ) / nDotDirection;

		if ( t >= 0.0 )
		{
			intersection = getPoint( t );
			return true;
		}
		else
		{
			return false;
		}
	}

	inline boost::python::tuple py_intersect(const Plane &p) const
	{
		double t;
		Point3 intersection;
		bool success = intersect( p, t, intersection );
		return boost::python::make_tuple( success, t, intersection );
	}
};



#endif
