//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CIRCLE2_H__
#define CIRCLE2_H__

#include <stdio.h>

#include <Math/Point2.h>
#include <Math/Segment2.h>
#include <Math/Polygon2.h>
#include <Math/Side.h>


/*
2D Circle
*/




class GS_DllExport Circle2
{
private:
	Point2 centre;
	double radius, radiusSquared;


public:
	inline Circle2() : radius( -1.0 ), radiusSquared( -1.0 )
	{
	}

	inline Circle2(const Point2 &centre, double radius)
				: centre( centre ), radius( radius ), radiusSquared( radius * radius )
	{
	}





	inline void read(FILE *f)
	{
		centre.read( f );
		fread( &radius, sizeof(double), 1, f );
		radiusSquared = radius * radius;
	}

	inline void write(FILE *f) const
	{
		centre.write( f );
		fwrite( &radius, sizeof(double), 1, f );
	}



	//check if the circle is empty
	inline bool isEmpty() const
	{
		return radius < 0.0;
	}


	// Get the centre
	inline const Point2 & getCentre() const
	{
		return centre;
	}

	// Get the radius
	inline double getRadius() const
	{
		return radius;
	}

	// Get the square of the radius
	inline double getRadiusSquared() const
	{
		return radiusSquared;
	}



	// Check to see if the line segment intersects @this
	inline bool intersects(const Segment2 &seg) const
	{
		double sqrDist = seg.sqrDistanceTo( centre );

		return sqrDist <= radiusSquared;
	}




	//check if p is within the circle
	inline bool contains(const Point2 &p) const
	{
		return p.sqrDistanceTo( centre )  <=  radiusSquared;
	}


	//check if @seg is wholly within the circle
	inline bool containsAllOf(const Segment2 &seg) const
	{
		//if the end points are contained within this, then so is all of the line
		return contains( seg.a )  &&  contains( seg.b );
	}

	inline bool py_containsAllOfSegment(const Segment2 &seg) const
	{
		return containsAllOf( seg );
	}

	//check if @seg is partially within the circle
	inline bool containsPartOf(const Segment2 &seg) const
	{
		//if either end point is within this, then @seg is partially
		//within @this
		if ( contains( seg.a )  ||  contains( seg.b ) )
		{
			return true;
		}

		//neither end point is within this, but, the segment may pass through
		//this: use intersects() to detect this
		return intersects( seg );
	}

	inline bool py_containsPartOfSegment(const Segment2 &seg) const
	{
		return containsPartOf( seg );
	}


	// Check if @polygon is wholly within the circle
	inline bool containsAllOf(const Polygon2 &polygon) const
	{
		// If any vertex of @polygon is outside @this, then @polygon is not wholly inside @this
		for (int polyI = 0; polyI < polygon.size(); polyI++)
		{
			if ( !contains( polygon[polyI] ) )
			{
				return false;
			}
		}

		// All vertices of @polygon are inside @this
		return true;
	}

	inline bool py_containsAllOfPolygon(const Polygon2 &polygon) const
	{
		return containsAllOf( polygon );
	}

	// Check if @polygon is partially within the circle
	inline bool containsPartOf(const Polygon2 &polygon) const
	{
		// If any vertex of @polygon is inside @this, then @polygon is partially inside @this
		for (int polyI = 0; polyI < polygon.size(); polyI++)
		{
			if ( contains( polygon[polyI] ) )
			{
				return true;
			}
		}

		// If the centre of @this is inside @polygon, then @polygon is partially inside @this
		if ( polygon.contains( centre ) )
		{
			return true;
		}

		// If any edge of @polygon is inside @this, then @polygon is partially inside @this
		int prevEdgeI = polygon.size() - 1;
		for (int edgeI = 0; edgeI < polygon.size(); edgeI++)
		{
			Segment2 edge( polygon[prevEdgeI], polygon[edgeI] );
			if ( intersects( edge ) )
			{
				return true;
			}
		}

		// No part of @polygon is inside @this
		return false;
	}

	inline bool py_containsPartOfPolygon(const Polygon2 &polygon) const
	{
		return containsPartOf( polygon );
	}
};



#endif
