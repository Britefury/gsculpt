//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PLANE_H__
#define PLANE_H__

#include <boost/python.hpp>




#include <stdio.h>

#include <Math/epsilon.h>

#include <Util/Array.h>
#include <Util/SETree.h>

#include <Math/Side.h>
#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Math/Axis.h>

class GS_DllExport Plane
{
public:
	Vector3 n;
	double d;


	inline Plane() : n( Vector3( 0.0, 0.0, 1.0 ) ), d( 0.0 )
	{
	}

	inline Plane(const Vector3 &normal, double distance)
					 : n( normal ), d( distance )
	{
	}

	inline Plane(const Vector3 &normal, const Point3 &a)
	{
		n = normal;
		d = a.dot( n );
	}

	inline Plane(Axis normal, const Point3 &a)
	{
		n = Vector3( normal );
		d = a.dot( n );
	}

	inline Plane(const Point3 &a, const Vector3 &u, const Vector3 &v)
	{
		n = u.cross( v ).getNormalised();
		d = a.dot( n );
	}

	inline Plane(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		Vector3 u = b - a;
		Vector3 v = c - a;
		n = u.cross( v ).getNormalised();
		d = a.dot( n );
	}

	inline Plane(const Array<Point3> &polygon)
	{
		int b = 1;

		Point3 midPoint = Point3::sum( polygon[0], polygon[1] );

		//compute normal as weighted average of normals of triangles that make up
		//the polygon (weight is the area of the triangle)
		//also, compute the midpoint
		for (int c = 2; c < polygon.size(); c++)
		{
			Vector3 u = polygon[b] - polygon[0];
			Vector3 v = polygon[c] - polygon[0];

			n += u.cross( v );			// u.cross( v )  =  normal * area of triangle
			midPoint.cumulativeAdd( polygon[c] );

			b = c;
		}
		//normalise n
		n.normalise();

		//divide by n (mean)
		midPoint.scale( 1.0 / (double)polygon.size() );

		d = midPoint.dot( n );
	}




	inline bool operator!=(const Plane &cmp) const
	{
		return n != cmp.n  ||  d != cmp.d;
	}



	inline Plane operator-() const
	{
		return Plane( -n, -d );
	}



	inline Side side(const Point3 &p, bool strict) const
	{
		double pDotN = p.dot( n );

		if ( strict )
		{
			if ( pDotN < d )
			{
				return SIDE_NEGATIVE;
			}
			else if ( pDotN > d )
			{
				return SIDE_POSITIVE;
			}
			else
			{
				return SIDE_ON;
			}
		}
		else
		{
			double error = fabs( d )  *  EPSILON;

			if ( pDotN  <  ( d - error ) )
			{
				return SIDE_NEGATIVE;
			}
			if ( pDotN  >  ( d + error ) )
			{
				return SIDE_POSITIVE;
			}
			else
			{
				return SIDE_ON;
			}
		}
	}

	inline Side py_sidePoint(const Point3 &p, bool strict) const
	{
		return side( p, strict );
	}

	inline Side side(const Array<Point3> &poly, bool strict) const
	{
		Side s = side( poly[0], strict );

		for (int polyI = 1; polyI < poly.size(); polyI++)
		{
			Side s2 = side( poly[polyI], strict );

			if ( s2 != s )
			{
				return SIDE_BOTH;
			}
		}

		return s;
	}


	inline Side directionSide(const Vector3 &v) const
	{
		double vDotN = v.dot( n );

		if ( vDotN < 0.0 )
		{
			return SIDE_NEGATIVE;
		}
		else if ( vDotN > 0.0 )
		{
			return SIDE_POSITIVE;
		}
		else
		{
			return SIDE_ON;
		}
	}



	inline double distanceTo(const Point3 &p) const
	{
		return p.dot( n ) - d;
	}

	inline Point3 getPointOnPlane() const
	{
		return Point3( n * d );
	}

	inline Point3 projectPoint(const Point3 &p) const
	{
		double dist = distanceTo( p );

		return p  -  n * dist;
	}



	inline bool intersect(const Point3 &st, const Vector3 &dir, double &t) const
	{
		double dirDotN = dir.dot( n );

		if ( dirDotN != 0.0 )
		{
			t = ( d - st.dot( n ) ) / dirDotN;
			return true;
		}
		else
		{
			t = 0.0;
			return false;
		}
	}

	inline bool intersect(const Point3 &st, const Point3 &en, double &t) const
	{
		if ( intersect( st, en - st, t ) )
		{
			if ( t >= 0.0  &&  t <= 1.0 )
			{
				return true;
			}
		}
		return false;
	}

	inline bool intersectStrict(const Point3 &st, const Point3 &en, double &t) const
	{
		if ( intersect( st, en - st, t  ))
		{
			if ( t > 0.0  &&  t < 1.0 )
			{
				return true;
			}
		}
		return false;
	}


	inline boost::python::tuple py_intersect_ray(const Point3 &st, const Vector3 &dir) const
	{
		double t = 0.0;
		bool result = intersect( st, dir, t );
		return boost::python::make_tuple( result, t );
	}

	inline boost::python::tuple py_intersect_seg(const Point3 &st, const Point3 &en) const
	{
		double t = 0.0;
		bool result = intersect( st, en, t );
		return boost::python::make_tuple( result, t );
	}

	inline boost::python::tuple py_intersectStrict(const Point3 &st, const Point3 &en) const
	{
		double t = 0.0;
		bool result = intersectStrict( st, en, t );
		return boost::python::make_tuple( result, t );
	}



	inline bool separates(const Point3 &a, const Point3 &b) const
	{
		double aDotN = a.dot( n );
		double bDotN = b.dot( n );

		return ( aDotN > d  &&  bDotN < d )   ||   ( aDotN < d  &&  bDotN > d );
	}



	inline static Plane fastPlane(const Point3 &a, const Vector3 &u, const Vector3 &v)
	{
		Vector3 n = u.cross( v );
		return Plane( n, a.dot( n ) );
	}

	inline static Plane fastPlane(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return fastPlane( a, b - a, c - a );
	}


	inline static Plane py_fastPlaneAUV(const Point3 &a, const Vector3 &u, const Vector3 &v)
	{
		return Plane::fastPlane( a, u, v );
	}

	inline static Plane py_fastPlaneABC(const Point3 &a, const Point3 &b, const Point3 &c)
	{
		return Plane::fastPlane( a, b, c );
	}



	inline void read(FILE *f)
	{
		n.read( f );
		fread( &d, sizeof(double), 1, f );
	}

	inline void write(FILE *f) const
	{
		n.write( f );
		fwrite( &d, sizeof(double), 1, f );
	}


	inline void read(SENode &node)
	{
		n.read( node[0] );
		node[1]  >>  d;
	}

	inline void write(SENode &node) const
	{
		n.write( node.addList() );
		node.addItem()  <<  d;
	}
};



inline bool operator==(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) == pl.d;
}

inline bool operator==(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) == pl.d;
}

inline bool operator!=(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) != pl.d;
}

inline bool operator!=(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) != pl.d;
}

inline bool operator<(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) > pl.d;
}

inline bool operator<(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) < pl.d;
}

inline bool operator<=(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) >= pl.d;
}

inline bool operator<=(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) <= pl.d;
}

inline bool operator>(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) < pl.d;
}

inline bool operator>(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) > pl.d;
}

inline bool operator>=(const Plane &pl, const Point3 &pt)
{
	return pt.dot( pl.n ) <= pl.d;
}

inline bool operator>=(const Point3 &pt, const Plane &pl)
{
	return pt.dot( pl.n ) >= pl.d;
}



#endif
