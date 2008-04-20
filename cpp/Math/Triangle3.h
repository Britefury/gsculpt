//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TRIANGLE3_H__
#define TRIANGLE3_H__

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Segment3.h>
#include <Math/Line3.h>
#include <Math/Side.h>
#include <Math/Triangle2.h>



class GS_DllExport Triangle3
{
public:
	Point3 a, b, c;


	Triangle3()
	{
	}

	Triangle3(const Point3 &va, const Point3 &vb, const Point3 &vc)
				: a( va ), b( vb ), c( vc )
	{
	}

	Triangle3(const Triangle2 &t) : a ( t.a ), b( t.b ), c( t.c )
	{
	}



	Segment3 getEdge(int edge) const
	{
		switch( edge )
		{
		case 0:
			return Segment3( a, b );
		case 1:
			return Segment3( b, c );
		case 2:
			return Segment3( c, a );
		default:
			return Segment3( a, b );
		}
	}

/*	void generatePointList(array<Point3> &pointList) const
	{
		pointList.clear();
		pointList.reserve( 3 );
		pointList.push_back( a );
		pointList.push_back( b );
		pointList.push_back( c );
	}*/

	Plane getFastEdgePlane(int edge) const
	{
		Plane plane = getFastPlane();
		Segment3 edgeLine = getEdge( edge );

		switch( edge )
		{
		case 0:
			return Plane::fastPlane( a, plane.n, edgeLine.getDirection() );
		case 1:
			return Plane::fastPlane( b, plane.n, edgeLine.getDirection() );
		case 2:
			return Plane::fastPlane( c, plane.n, edgeLine.getDirection() );
		default:
			return Plane::fastPlane( a, plane.n, edgeLine.getDirection() );
		}
	}

	Plane getPlane() const
	{
		return Plane( a, b, c );
	}

	Plane getFastPlane() const
	{
		return Plane::fastPlane( a, b, c );
	}


	bool raytrace(const Segment3 &ray, double &t, double &u, double &v) const
	{
		//Uses Moller and Trumbore's algorithm

		//edges
		Vector3 e1 = b - a;
		Vector3 e2 = c - a;
		Vector3 rayDir = ray.getDirection();
		Vector3 p = rayDir.cross( e2 );

		//compute determinant of matrix
		double determinant = p.dot( e1 );
		if ( determinant == 0.0 )
		{
			return false;
		}
		double inverseDeterminant = 1.0 / determinant;

		//compute u
		Vector3 s = ray.a - a;
		double ru = s.dot( p )  *  inverseDeterminant;
		if ( ru < 0.0  ||  ru > 1.0 )
		{
			return false;
		}

		//compute v
		Vector3 q = s.cross( e1 );
		double rv = rayDir.dot( q )  *  inverseDeterminant;
		if ( rv < 0.0  ||  ( ru + rv ) > 1.0 )
		{
			return false;
		}

		//compute t
		double rt = e2.dot( q )  *  inverseDeterminant;


		if ( rt >= 0.0  &&  rt <= 1.0 )
		{
			u = ru;
			v = rv;
			t = rt;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool raytrace(const Line3 &ray, double &t, double &u, double &v) const
	{
		//Uses Moller and Trumbore's algorithm

		//edges
		Vector3 e1 = b - a;
		Vector3 e2 = c - a;
		Vector3 p = ray.direction.cross( e2 );

		//compute determinant of matrix
		double determinant = p.dot( e1 );
		if ( determinant == 0.0 )
		{
			return false;
		}
		double inverseDeterminant = 1.0 / determinant;

		//compute u
		Vector3 s = ray.origin - a;
		double ru = s.dot( p )  *  inverseDeterminant;
		if ( ru < 0.0  ||  ru > 1.0 )
		{
			return false;
		}

		//compute v
		Vector3 q = s.cross( e1 );
		double rv = ray.direction.dot( q )  *  inverseDeterminant;
		if ( rv < 0.0  ||  ( ru + rv ) > 1.0 )
		{
			return false;
		}

		//compute t
		double rt = e2.dot( q )  *  inverseDeterminant;

		u = ru;
		v = rv;
		t = rt;
		return true;
	}


	inline boost::python::tuple py_raytraceSegment3(const Segment3 &ray) const
	{
		double t = 0.0, u = 0.0, v = 0.0;
		bool success = raytrace( ray, t, u, v );
		Point3 intersection = ray.getPoint( t );
		return boost::python::make_tuple( success, t, u, v, intersection );
	}

	inline boost::python::tuple py_raytraceLine3(const Line3 &ray) const
	{
		double t = 0.0, u = 0.0, v = 0.0;
		bool success = raytrace( ray, t, u, v );
		Point3 intersection = ray.getPoint( t );
		return boost::python::make_tuple( success, t, u, v, intersection );
	}



/*	Point3 computeUV(const Point3 &point) const
	{
		Plane abEdgePlane = getFastEdgePlane( 0 );
		Plane acEdgePlane = getFastEdgePlane( 2 );

		double u = acEdgePlane.distanceTo( point ) / acEdgePlane.distanceTo( b );
		double v = abEdgePlane.distanceTo( point ) / abEdgePlane.distanceTo( c );

		return Point3( u, v, 0.0 );
	}*/

	Point3 uvToPoint(const Point3 &uv) const
	{
		return a   +    ( b - a ) * uv.x   +   ( c - a ) * uv.y;
	}


	//compute which side of the plane *this is on
	inline Side side(const Plane &p) const
	{
		if ( a == p  &&  b == p  &&  c == p )
		{
			return SIDE_ON;
		}
		else if ( a >= p  &&  b >= p  &&  c >= p )
		{
			return SIDE_POSITIVE;
		}
		else if ( a <= p  &&  b <= p  &&  c <= p )
		{
			return SIDE_NEGATIVE;
		}
		else
		{
			return SIDE_BOTH;
		}
	}
};




/*inline void pointListToTriangleList(const array<Point3> &points, array<Triangle3> &triangles)
{
	triangles.clear();
	triangles.reserve( points.size() - 2 );
	for (int i = 2; i < points.size(); i++)
	{
		triangles.push_back( Triangle3( points[0], points[ i - 1 ], points[i] ) );
	}
}*/



#endif
