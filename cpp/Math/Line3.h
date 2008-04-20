//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINE3_H__
#define LINE3_H__

#include <boost/python.hpp>




#include <map>

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Vector3.h>

/*
3D LINE
*/



class GS_DllExport Line3
{
public:
	Point3 origin;
	Vector3 direction;


	inline Line3()
	{
	}

	inline Line3(const Point3 &p1, const Point3 &p2)
			: origin( p1 ), direction( p2 - p1 )
	{
	}

	inline Line3(const Point3 &p, const Vector3 &direction)
			: origin( p ), direction( direction )
	{
	}



	//translation
	inline Line3 operator+(const Vector3 &v) const
	{
		return Line3( origin + v, direction );
	}

	inline void operator+=(const Vector3 &v)
	{
		origin += v;
	}

	inline Line3 operator-(const Vector3 &v) const
	{
		return Line3( origin - v, direction );
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
		return direction.dot( p - origin )  /  direction.sqrLength();
	}

	inline Point3 closestPointTo(const Point3 &p, double &t) const
	{
		t = closestPointParam( p );
		return getPoint( t );
	}

	inline Point3 closestPointTo(const Point3 &p) const
	{
		return getPoint( closestPointParam( p ) );
	}

	inline boost::python::tuple py_closestPointTo(const Point3 &p) const
	{
		double t = closestPointParam( p );
		return boost::python::make_tuple( t, getPoint( t ) );
	}

	//distance from the line to p
	inline double distanceTo(const Point3 &p) const
	{
		return p.distanceTo( closestPointTo( p ) );
	}

	inline double py_distanceToPoint(const Point3 &p) const
	{
		return distanceTo( p );
	}

	//distance squared from the line to p
	inline double sqrDistanceTo(const Point3 &p) const
	{
		return p.sqrDistanceTo( closestPointTo( p ) );
	}

	inline double py_sqrDistanceToPoint(const Point3 &p) const
	{
		return distanceTo( p );
	}


	//get the closest points between two lines, @this and @l1
	//Returns:
	//	true - if lines are non-parallel
	//	false - if lines are parallel
	inline bool closestPointsParams(const Line3 &l1, double &t0, double &t1) const
	{
		/*
		L = A + sB		(@this)
		M = C + tD		(@l1)

		sqrDist = |M-L|^2  =  | C + tD  - A - sB |^2

		Q = C - A

		sqrDist = ( Q + tD - sB ) . ( Q + tD - sB )
				= Q.Q + t^2D.D + s^2B.B + 2tQ.D - 2sQ.b - 2stD.B

		sqrDist is minimum when the derivative of the above is 0:

		with respect to s:   2sB.B - 2tD.B - 2Q.B = 0
									sB.B - tD.B = Q.B

		with respect to t:  -2sD.B + 2tD.D + 2Q.D = 0
								-sD.B + tD.D = -Q.D

		As a matix equation:

		[  B.B	-B.D ] x  [ s ] = [  Q.B ]
		[ -B.D	 D.D ]    [ t ]   [ -Q.D ]

		*/

		Vector3 q = l1.origin - origin;

		double bDotB = direction.dot( l1.direction );
		double bDotD = direction.dot( l1.direction );
		double dDotD = l1.direction.dot( l1.direction );
		double qDotB = q.dot( direction );

		double sqrLengthProduct = bDotB * dDotD;

		double determinant = sqrLengthProduct  -  bDotD * bDotD;

		if ( fabs( determinant )  <  ( sqrLengthProduct * EPSILON ) )
		{
			//lines are parallel, use any points
			t0 = 0.0;
			t1 = -qDotB / bDotB;
			return false;
		}
		else
		{
			double invDet = 1.0 / determinant;

			double qDotD = q.dot( l1.direction );

			t0 = ( ( dDotD * qDotB )  -  ( bDotD * qDotD ) )  *  invDet;
			t1 = ( ( bDotD * qDotB )  -  ( bDotB * qDotD ) )  *  invDet;
			return true;
		}
	}

	inline bool closestPoints(const Line3 &l1, Point3 &p0, Point3 &p1, double &t0, double &t1) const
	{
		bool nonParallel = closestPointsParams( l1, t0, t1 );
		p0 = getPoint( t0 );
		p1 = l1.getPoint( t1 );
		return nonParallel;
	}

	inline bool closestPoints(const Line3 &l1, Point3 &p0, Point3 &p1) const
	{
		double t0, t1;
		return closestPoints( l1, p0, p1, t0, t1 );
	}

	inline boost::python::tuple py_closestPointsParams(const Line3 &l1) const
	{
		double t0 = 0.0, t1 = 0.0;
		bool nonParallel = closestPointsParams( l1, t0, t1 );
		return boost::python::make_tuple( nonParallel, t0, t1 );
	}

	inline boost::python::tuple py_closestPointsWithParams(const Line3 &l1) const
	{
		double t0 = 0.0, t1 = 0.0;
		Point3 p0, p1;
		bool nonParallel = closestPoints( l1, p0, p1, t0, t1 );
		return boost::python::make_tuple( nonParallel, p0, p1, t0, t1 );
	}

	inline boost::python::tuple py_closestPoints(const Line3 &l1) const
	{
		Point3 p0, p1;
		bool nonParallel = closestPoints( l1, p0, p1 );
		return boost::python::make_tuple( nonParallel, p0, p1 );
	}

	//distance from the line to p
	inline double distanceTo(const Line3 &l1) const
	{
		Point3 p0, p1;
		closestPoints( l1, p0, p1 );
		return p0.distanceTo( p1 );
	}

	inline double py_distanceToLine(const Line3 &l1) const
	{
		return distanceTo( l1 );
	}

	//distance squared from the line to p
	inline double sqrDistanceTo(const Line3 &l1) const
	{
		Point3 p0, p1;
		closestPoints( l1, p0, p1 );
		return p0.sqrDistanceTo( p1 );
	}

	inline double py_sqrDistanceToLine(const Line3 &l1) const
	{
		return distanceTo( l1 );
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

		intersection = getPoint( t );
		return true;
	}

	inline boost::python::tuple py_intersect(const Plane &p) const
	{
		double t = 0.0;
		Point3 intersection;
		bool success = intersect( p, t, intersection );
		return boost::python::make_tuple( success, t, intersection );
	}
};



#endif
