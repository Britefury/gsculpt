//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TRIANGLE2_H__
#define TRIANGLE2_H__

#include <Math/Point2.h>
#include <Math/Segment2.h>


class GS_DllExport Triangle2
{
public:
	Point2 a, b, c;


	inline Triangle2()
	{
	}

	inline Triangle2(const Point2 &va, const Point2 &vb, const Point2 &vc)
					: a( va ), b( vb ), c( vc )
	{
	}



	inline Segment2 getEdge(int edge) const
	{
		switch( edge )
		{
		case 0:
			return Segment2( a, b );
		case 1:
			return Segment2( b, c );
		case 2:
			return Segment2( c, a );
		default:
			return Segment2( a, b );
		}
	}


	inline double areaX2() const
	{
		return Point2::areaOfTrianglex2( a, b, c );
	}

	inline double area() const
	{
		return areaX2() * 0.5;
	}


	inline bool contains(const Point2 &p) const
	{
		if ( areaX2() > 0.0 )
		{
			return getEdge( 0 ).onOrLeft( p )  &&  getEdge( 1 ).onOrLeft( p )  &&  getEdge( 2 ).onOrLeft( p );
		}
		else
		{
			return getEdge( 0 ).onOrRight( p )  &&  getEdge( 1 ).onOrRight( p )  &&  getEdge( 2 ).onOrRight( p );
		}
	}
};


#endif

