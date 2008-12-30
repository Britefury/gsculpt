//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BBOX3_H__
#define BBOX3_H__

#include <stdio.h>

#include <algorithm>

#include <Util/Array.h>

#include <Math/epsilon.h>
#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Segment3.h>
#include <Math/Triangle3.h>
#include <Math/BBox2.h>




/*
3D BOUNDING BOX
*/




class GS_DllExport BBox3
{
public:
	//lower and upper corners
	Point3 l, u;

public:
	inline BBox3() : l( 1.0, 1.0, 1.0 ), u( -1.0, -1.0, -1.0 )
	{
	}

	inline BBox3(const Point3 &p) : l( p ), u( p )
	{
	}

	inline BBox3(const Point3 &lower, const Point3 &upper)
	{
		l = Point3::min( lower, upper );
		u = Point3::max( lower, upper );
	}

	inline BBox3(const Array<Point3> &pts)
	{
		if ( pts.size() == 0 )
		{
			return;
		}

		l = pts[0];
		u = pts[0];

		for (int i = 1; i < pts.size(); i++)
		{
			addPoint( pts[i] );
		}
	}






	inline void read(FILE *f)
	{
		l.read( f );
		u.read( f );
	}

	inline void write(FILE *f) const
	{
		l.write( f );
		u.write( f );
	}




	//check if the box is empty
	inline bool isEmpty() const
	{
		return l.x > u.x  ||  l.y > u.y  ||  l.z > u.z;
	}


	//if necessary, expand the box to include p
	inline void addPoint(const Point3 &p)
	{
		if ( isEmpty() )
		{
			l = u = p;
		}
		else
		{
			l = Point3::min( l, p );
			u = Point3::max( u, p );
		}
	}

	//if necessary, expand the box to include b
	inline void addBox(const BBox3 &b)
	{
		if ( !b.isEmpty() )
		{
			if ( isEmpty() )
			{
				*this = b;
			}
			else
			{
				l = Point3::min( l, b.l );
				u = Point3::max( u, b.u );
			}
		}
	}


	//detemine which side of the plane p, this is on
	inline Side side(const Plane &p) const
	{
		//get the normal of the plane, and use it to obtain the leading and
		//trailing vertices of the box.
		Point3 leading = getLeadingVertex( p.n );
		Point3 trailing = getTrailingVertex( p.n );

		//if the trailing vertex is on the positive side of p, so is
		//the whole box
		if ( trailing > p )
		{
			return SIDE_POSITIVE;
		}
		else
		{
			//if the leading vertex is on the negative side of p, so
			//is the whole box
			if ( leading < p )
			{
				return SIDE_NEGATIVE;
			}
			else
			{
				//the trailing and leading vertices of the box are on different
				//sides of the plane, so the box lies on both sides of the plane
				return SIDE_BOTH;
			}
		}
	}


	//If any of the width, height, or depth are 0.0, and should *this be
	//used to clip a line, the result would be a line of length 0.0.
	//to ensure that this does not happen, this method is used to enlarge
	//any near zero dimension
	//Also, should any faces in the box be flush with the sides of the
	//box, a line clipped with the box will have problems when raytracing
	//with that face.
	inline void enlargeForClipping()
	{
		if ( !isEmpty() )
		{
			double maxSize = std::max( getWidth(), getHeight() );
			maxSize = std::max( maxSize, getDepth() );

			//compute an amount to enlarge the box by
			double enlargement = maxSize * 5e-9;

			//enlarge by this amount in all three axes
			l.x -= enlargement;
			u.x += enlargement;
			l.y -= enlargement;
			u.y += enlargement;
			l.z -= enlargement;
			u.z += enlargement;
		}
	}


	// Enlarge box for intersection
	inline void enlargeForIntersection()
	{
		if ( !isEmpty() )
		{
			Vector3 enlargement = getSize() * 5e-6;

			l -= enlargement;
			u += enlargement;
		}
	}


	inline bool intersects(const Segment3 &seg) const
	{
		// Segment->box intersection code based on the algorithm used by David Eberly,
		// http://www.geometrictools.com

		double segExtents[3], diffExtents[3], absDirCrossDelta[3], rhs;

		Vector3 delta = seg.getMidPoint() - getCentre();
		Vector3 segDir = seg.getDirection() * 0.5;
		Vector3 boxExtents = getSize() * 0.5;


		segExtents[0] = fabs( segDir.x );
		diffExtents[0] = fabs( delta.x );
		rhs = boxExtents.x + segExtents[0];
		if ( diffExtents[0] > rhs )
		{
			return false;
		}

		segExtents[1] = fabs( segDir.y );
		diffExtents[1] = fabs( delta.y );
		rhs = boxExtents.y + segExtents[1];
		if ( diffExtents[1] > rhs )
		{
			return false;
		}

		segExtents[2] = fabs( segDir.z );
		diffExtents[2] = fabs( delta.z );
		rhs = boxExtents.z + segExtents[2];
		if ( diffExtents[2] > rhs )
		{
			return false;
		}


		Vector3 dirCrossDelta = segDir.cross( delta );

		absDirCrossDelta[0] = fabs( dirCrossDelta.x );
		rhs = boxExtents.y * segExtents[2]  +  boxExtents.z * segExtents[1];
		if ( absDirCrossDelta[0]  >  rhs )
		{
			return false;
		}

		absDirCrossDelta[1] = fabs( dirCrossDelta.y );
		rhs = boxExtents.x * segExtents[2]  +  boxExtents.z * segExtents[0];
		if ( absDirCrossDelta[1]  >  rhs )
		{
			return false;
		}

		absDirCrossDelta[2] = fabs( dirCrossDelta.z );
		rhs = boxExtents.x * segExtents[1]  +  boxExtents.y * segExtents[0];
		if ( absDirCrossDelta[2]  >  rhs )
		{
			return false;
		}

		return true;
	}

	//check if @this intersects @b
	inline bool intersects(const BBox3 &b) const
	{
		return l.x <= b.u.x  &&  u.x >= b.l.x   &&   l.y <= b.u.y  &&  u.y >= b.l.y   &&   l.z <= b.u.z  &&  u.z >= b.l.z;
	}

	inline bool py_intersectsSegment(const Segment3 &seg) const
	{
		return intersects( seg );
	}

	inline bool py_intersectsBox(const BBox3 &box) const
	{
		return intersects( box );
	}



	//compute the intersection of @this and @b
	inline BBox3 intersection(const BBox3 &b) const
	{
		BBox3 result;
		result.l = Point3::max( l, b.l );
		result.u = Point3::min( u, b.u );
		return result;
	}


	//compute the closest point in the box to @p
	inline Point3 closestPointTo(const Point3 &p) const
	{
		return Point3::min( Point3::max( p, l ), u );
	}

	inline double sqrDistanceTo(const Point3 &p) const
	{
		return closestPointTo( p ).sqrDistanceTo( p );
	}

	inline double distanceTo(const Point3 &p) const
	{
		return closestPointTo( p ).distanceTo( p );
	}


	//check if p is within the bounding box
	inline bool contains(const Point3 &p) const
	{
		return ( p.x >= l.x )  &&  ( p.x <= u.x )  &&
				 ( p.y >= l.y )  &&  ( p.y <= u.y )  &&
				 ( p.z >= l.z )  &&  ( p.z <= u.z );
	}

	inline bool py_containsPoint(const Point3 &p) const
	{
		return contains( p );
	}


	//check if @seg is wholly within the bounding box
	inline bool containsAllOf(const Segment3 seg) const
	{
		//if the end points are contained within this, then so is all of the segment
		return contains( seg.a )  &&  contains( seg.b );
	}

	inline bool py_containsAllOfSegment(const Segment3 &seg) const
	{
		return containsAllOf( seg );
	}


	//check if @seg is partially within the bounding box
	inline bool containsPartOf(const Segment3 &seg) const
	{
		//if either end point is within this, then line is partially
		//within this
		if ( contains( seg.a )  ||  contains( seg.b ) )
		{
			return true;
		}

		//neither end point is within this, but, the line may pass through
		//this: use intersects() to detect this
		return intersects( seg );
	}

	inline bool py_containsPartOfSegment(const Segment3 &seg) const
	{
		return containsPartOf( seg );
	}


	//check if t is wholly within the bounding box
	inline bool containsAllOf(const Triangle3 &t) const
	{
		//if the vertices are contained within this, then so is all of the triangle
		return contains( t.a )  &&  contains( t.b )  &&  contains( t.c );
	}

	inline bool py_containsAllOfTriangle(const Triangle3 &tri) const
	{
		return containsAllOf( tri );
	}

	//check if t is partially within the bounding box
	inline bool containsPartOf(const Triangle3 &t) const
	{
		//first check if any of the vertices t are inside this
		//if so, the triangle is partially within this
		if ( contains( t.a )  ||  contains( t.b )  ||  contains( t.c ) )
		{
			return true;
		}
		else
		{
			Plane plane = t.getFastPlane();

			//none of the vertices of t are within this

			//check to see if the plane of t bisects this
			//if so, there is a chance this may contain part of t
			if ( side( plane )  ==  SIDE_BOTH )
			{
				//check all 3 edges of t to see if they intersect this
				//if so, then this partially contains t
				if ( containsPartOf( t.getEdge( 0 ) ) )
				{
					return true;
				}
				if ( containsPartOf( t.getEdge( 1 ) ) )
				{
					return true;
				}
				if ( containsPartOf( t.getEdge( 2 ) ) )
				{
					return true;
				}

				//so far the following has been determined:
				// - the vertices of t are outside this
				// - the edges of t are outside this
				// - the plane of t bisects this
				//so:
				// any intersection between this and the plane of t
				// is either wholly inside or wholly outside of the bounds of t
				//
				//to check for the former (inside), trace a ray from the
				//the leading vertex to the trailing vertex of this (using plane
				//normal) through t, and check for intersection

				double raytraceT, raytraceU, raytraceV;

				const Vector3 &n = plane.n;

				return t.raytrace( Segment3( getTrailingVertex( n ), getLeadingVertex( n ) ), raytraceT, raytraceU, raytraceV );
			}
			else
			{
				return false;
			}
		}
	}

	inline bool py_containsPartOfTriangle(const Triangle3 &tri) const
	{
		return containsPartOf( tri );
	}



	//check if @box is wholly within the bounding box
	inline bool contains(const BBox3 box) const
	{
		//if the end points are contained within this, then so is all of the boxment
		return contains( box.l )  &&  contains( box.u );
	}

	inline bool py_containsBBox(const BBox3 &box) const
	{
		return contains( box );
	}




	//get lower and upper corners, and centre
	inline Point3 getLower() const
	{
		return l;
	}

	inline Point3 getUpper() const
	{
		return u;
	}

	inline Point3 getCentre() const
	{
		return Point3( ( l.x + u.x )  *  0.5,
					( l.y + u.y )  *  0.5,
					( l.z + u.z )  *  0.5 );
	}


	//get one of the (eight) corners
	inline Point3 getPoint(int i) const
	{
		switch( i )
		{
		case 0:
			return l;
		case 1:
			return Point3( u.x, l.y, l.z );
		case 2:
			return Point3( l.x, u.y, l.z );
		case 3:
			return Point3( u.x, u.y, l.z );
		case 4:
			return Point3( l.x, l.y, u.z );
		case 5:
			return Point3( u.x, l.y, u.z );
		case 6:
			return Point3( l.x, u.y, u.z );
		case 7:
			return u;
		default:
			return l;
		}
	}

	//get one of the six planes that define the box
	inline Plane getPlane(int i) const
	{
		switch( i )
		{
		case 0:
			return Plane( Vector3( 1.0, 0.0, 0.0 ), l );
		case 1:
			return Plane( Vector3( -1.0, 0.0, 0.0 ), u );
		case 2:
			return Plane( Vector3( 0.0, 1.0, 0.0 ), l );
		case 3:
			return Plane( Vector3( 0.0, -1.0, 0.0 ), u );
		case 4:
			return Plane( Vector3( 0.0, 0.0, 1.0 ), l );
		case 5:
			return Plane( Vector3( 0.0, 0.0, -1.0 ), u );
		default:
			return Plane( Vector3( 1.0, 0.0, 0.0 ), l );
		}
	}


	//get bounds
	inline double getBounds(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_NEGATIVE_X:
			return l.x;
		case AXIS_X:
			return u.x;
		case AXIS_NEGATIVE_Y:
			return l.y;
		case AXIS_Y:
			return u.y;
		case AXIS_NEGATIVE_Z:
			return l.z;
		case AXIS_Z:
			return u.z;
		default:
			return 0.0;
		}
	}

	//leading and trailing vertices
	//LEADING VERTEX: the vertex of the box further along the direction vector
	inline Point3 getLeadingVertex(const Vector3 &direction) const
	{
		return Point3( direction.x < 0.0  ?  l.x : u.x,
					direction.y < 0.0  ?  l.y : u.y,
					direction.z < 0.0  ?  l.z : u.z );
	}

	//TRAILING VERTEX: the opposite corner to the LEADING VERTEX
	inline Point3 getTrailingVertex(const Vector3 &direction) const
	{
		return Point3( direction.x < 0.0  ?  u.x : l.x,
					direction.y < 0.0  ?  u.y : l.y,
					direction.z < 0.0  ?  u.z : l.z );
	}



	//get dimensions
	inline double getWidth() const
	{
		return u.x - l.x;
	}

	inline double getHeight() const
	{
		return u.y - l.y;
	}

	inline double getDepth() const
	{
		return u.z - l.z;
	}

	inline Vector3 getSize() const
	{
		return u - l;
	}

	inline double getVolume() const
	{
		Vector3 size = Vector3::max( getSize(), Vector3( 0.0, 0.0, 0.0 ) );
		return size.x * size.y * size.z;
	}

	inline double getSurfaceArea() const
	{
		Vector3 size = Vector3::max( getSize(), Vector3( 0.0, 0.0, 0.0 ) );
		return ( size.x * size.y  +  size.x * size.z  +  size.y * size.z )  *  2.0;
	}

	inline Vector3 py_getSizeVector() const
	{
		return getSize();
	}

	inline double getSize(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			return getWidth();
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			return getHeight();
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			return getDepth();
		default:
			return 0.0;
		}
	}

	inline double py_getSizeInAxis(Axis axis) const
	{
		return getSize( axis );
	}


	//split the box at x = position, place result boxes in lowerBox and
	//upper Box
	inline void splitX(double position, BBox3 &lowerBox, BBox3 &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.x = position;
		upperBox.l.x = position;
	}

	inline boost::python::tuple py_splitX(double position) const
	{
		BBox3 lowerBox, upperBox;
		splitX( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}

	//split the box at y = position
	inline void splitY(double position, BBox3 &lowerBox, BBox3 &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.y = position;
		upperBox.l.y = position;
	}

	inline boost::python::tuple py_splitY(double position) const
	{
		BBox3 lowerBox, upperBox;
		splitY( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}

	//split the box at z = position
	inline void splitZ(double position, BBox3 &lowerBox, BBox3 &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.z = position;
		upperBox.l.z = position;
	}

	inline boost::python::tuple py_splitZ(double position) const
	{
		BBox3 lowerBox, upperBox;
		splitZ( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}


	//clip segment such that it lies within this
	inline bool clipSegment(Segment3 &seg) const
	{
		//use the six face places to clip seg
		for (int i = 0; i < 6; i++)
		{
			if ( !seg.clip( getPlane( i ) ) )
			{
				return false;
			}
		}

		return true;
	}


	//get major and minor axis
	inline Axis getMajorAxis() const
	{
		Axis result = AXIS_X;
		double size = getWidth();

		double height = getHeight();
		double depth = getDepth();

		if ( height > size )
		{
			size = height;
			result = AXIS_Y;
		}
		if ( depth > size )
		{
			result = AXIS_Z;
		}

		return result;
	}

	inline Axis getMinorAxis() const
	{
		Axis result = AXIS_X;
		double size = getWidth();

		double height = getHeight();
		double depth = getDepth();

		if ( height < size )
		{
			size = height;
			result = AXIS_Y;
		}
		if ( depth < size )
		{
			result = AXIS_Z;
		}

		return result;
	}

	//get major and minor size
	inline double getMajorSize() const
	{
		return std::max( getWidth(), std::max( getHeight(), getDepth() ) );
	}

	inline double getMinorSize() const
	{
		return std::min( getWidth(), std::min( getHeight(), getDepth() ) );
	}


	//convert to 2d bounding box
	inline BBox2 toBBox2(Axis axis) const
	{
		return BBox2( l.toPoint2Axis( axis ), u.toPoint2Axis( axis ) );
	}
};



#endif
