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
#include <Math/Point3f.h>
#include <Math/Vector3f.h>

#include <boost/python.hpp>




/*
3D BOUNDING BOX
*/




class GS_DllExport BBox3f
{
public:
	//lower and upper corners
	Point3f l, u;

public:
	inline BBox3f() : l( 1.0f, 1.0f, 1.0f ), u( -1.0f, -1.0f, -1.0f )
	{
	}

	inline BBox3f(const Point3f &p) : l( p ), u( p )
	{
	}

	inline BBox3f(const Point3f &lower, const Point3f &upper)
	{
		l = Point3f::min( lower, upper );
		u = Point3f::max( lower, upper );
	}

	inline BBox3f(const Array<Point3f> &pts)
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
	inline void addPoint(const Point3f &p)
	{
		if ( isEmpty() )
		{
			l = u = p;
		}
		else
		{
			l = Point3f::min( l, p );
			u = Point3f::max( u, p );
		}
	}

	//if necessary, expand the box to include b
	inline void addBox(const BBox3f &b)
	{
		if ( !b.isEmpty() )
		{
			if ( isEmpty() )
			{
				*this = b;
			}
			else
			{
				l = Point3f::min( l, b.l );
				u = Point3f::max( u, b.u );
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
			float maxSize = std::max( getWidth(), getHeight() );
			maxSize = std::max( maxSize, getDepth() );

			//compute an amount to enlarge the box by
			float enlargement = maxSize * 5e-9;

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
			Vector3f enlargement = getSize() * 5e-6;

			l -= enlargement;
			u += enlargement;
		}
	}


	//check if @this intersects @b
	inline bool intersects(const BBox3f &b) const
	{
		return l.x <= b.u.x  &&  u.x >= b.l.x   &&   l.y <= b.u.y  &&  u.y >= b.l.y   &&   l.z <= b.u.z  &&  u.z >= b.l.z;
	}

	inline bool py_intersectsBox(const BBox3f &box) const
	{
		return intersects( box );
	}



	//compute the intersection of @this and @b
	inline BBox3f intersection(const BBox3f &b) const
	{
		BBox3f result;
		result.l = Point3f::max( l, b.l );
		result.u = Point3f::min( u, b.u );
		return result;
	}


	//compute the closest point in the box to @p
	inline Point3f closestPointTo(const Point3f &p) const
	{
		return Point3f::min( Point3f::max( p, l ), u );
	}

	inline float sqrDistanceTo(const Point3f &p) const
	{
		return closestPointTo( p ).sqrDistanceTo( p );
	}

	inline float distanceTo(const Point3f &p) const
	{
		return closestPointTo( p ).distanceTo( p );
	}


	//check if p is within the bounding box
	inline bool contains(const Point3f &p) const
	{
		return ( p.x >= l.x )  &&  ( p.x <= u.x )  &&
				 ( p.y >= l.y )  &&  ( p.y <= u.y )  &&
				 ( p.z >= l.z )  &&  ( p.z <= u.z );
	}

	inline bool py_containsPoint(const Point3f &p) const
	{
		return contains( p );
	}


	//check if @box is wholly within the bounding box
	inline bool contains(const BBox3f box) const
	{
		//if the end points are contained within this, then so is all of the boxment
		return contains( box.l )  &&  contains( box.u );
	}

	inline bool py_containsBBox(const BBox3f &box) const
	{
		return contains( box );
	}




	//get lower and upper corners, and centre
	inline Point3f getLower() const
	{
		return l;
	}

	inline Point3f getUpper() const
	{
		return u;
	}

	inline Point3f getCentre() const
	{
		return Point3f( ( l.x + u.x )  *  0.5f,
					( l.y + u.y )  *  0.5f,
					( l.z + u.z )  *  0.5f );
	}


	//get one of the (eight) corners
	inline Point3f getPoint(int i) const
	{
		switch( i )
		{
		case 0:
			return l;
		case 1:
			return Point3f( u.x, l.y, l.z );
		case 2:
			return Point3f( l.x, u.y, l.z );
		case 3:
			return Point3f( u.x, u.y, l.z );
		case 4:
			return Point3f( l.x, l.y, u.z );
		case 5:
			return Point3f( u.x, l.y, u.z );
		case 6:
			return Point3f( l.x, u.y, u.z );
		case 7:
			return u;
		default:
			return l;
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
	inline Point3f getLeadingVertex(const Vector3f &direction) const
	{
		return Point3f( direction.x < 0.0f  ?  l.x : u.x,
					direction.y < 0.0f  ?  l.y : u.y,
					direction.z < 0.0f  ?  l.z : u.z );
	}

	//TRAILING VERTEX: the opposite corner to the LEADING VERTEX
	inline Point3f getTrailingVertex(const Vector3f &direction) const
	{
		return Point3f( direction.x < 0.0f  ?  u.x : l.x,
					direction.y < 0.0f  ?  u.y : l.y,
					direction.z < 0.0f  ?  u.z : l.z );
	}



	//get dimensions
	inline float getWidth() const
	{
		return u.x - l.x;
	}

	inline float getHeight() const
	{
		return u.y - l.y;
	}

	inline float getDepth() const
	{
		return u.z - l.z;
	}

	inline Vector3f getSize() const
	{
		return u - l;
	}

	inline float getVolume() const
	{
		Vector3f size = Vector3f::max( getSize(), Vector3f( 0.0f, 0.0f, 0.0f ) );
		return size.x * size.y * size.z;
	}

	inline float getSurfaceArea() const
	{
		Vector3f size = Vector3f::max( getSize(), Vector3f( 0.0f, 0.0f, 0.0f ) );
		return ( size.x * size.y  +  size.x * size.z  +  size.y * size.z )  *  2.0f;
	}

	inline Vector3f py_getSizeVector() const
	{
		return getSize();
	}

	inline float getSize(Axis axis) const
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

	inline float py_getSizeInAxis(Axis axis) const
	{
		return getSize( axis );
	}


	//split the box at x = position, place result boxes in lowerBox and
	//upper Box
	inline void splitX(float position, BBox3f &lowerBox, BBox3f &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.x = position;
		upperBox.l.x = position;
	}

	inline boost::python::tuple py_splitX(float position) const
	{
		BBox3f lowerBox, upperBox;
		splitX( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}

	//split the box at y = position
	inline void splitY(float position, BBox3f &lowerBox, BBox3f &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.y = position;
		upperBox.l.y = position;
	}

	inline boost::python::tuple py_splitY(float position) const
	{
		BBox3f lowerBox, upperBox;
		splitY( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}

	//split the box at z = position
	inline void splitZ(float position, BBox3f &lowerBox, BBox3f &upperBox) const
	{
		lowerBox = *this;
		upperBox = *this;
		lowerBox.u.z = position;
		upperBox.l.z = position;
	}

	inline boost::python::tuple py_splitZ(float position) const
	{
		BBox3f lowerBox, upperBox;
		splitZ( position, lowerBox, upperBox );
		return boost::python::make_tuple( lowerBox, upperBox );
	}


	//get major and minor axis
	inline Axis getMajorAxis() const
	{
		Axis result = AXIS_X;
		float size = getWidth();

		float height = getHeight();
		float depth = getDepth();

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
		float size = getWidth();

		float height = getHeight();
		float depth = getDepth();

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
	inline float getMajorSize() const
	{
		return std::max( getWidth(), std::max( getHeight(), getDepth() ) );
	}

	inline float getMinorSize() const
	{
		return std::min( getWidth(), std::min( getHeight(), getDepth() ) );
	}
};



#endif
