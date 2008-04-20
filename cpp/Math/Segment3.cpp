//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SEGMENT3_CPP__
#define SEGMENT3_CPP__

#include <Math/Segment3.h>



void Segment3::read(FILE *f)
{
//	a.read( f );
//	b.read( f );
}

void Segment3::write(FILE *f) const
{
//	a.write( f );
//	b.write( f );
}




bool Segment3::clip(const Plane &p)
{
	double aDotN, bDotN;
	double t;
	Point3 intersection;



	//find the distance along the plane normal, of a and b
	aDotN = a.dot( p.n );
	bDotN = b.dot( p.n );

	//if aDotN == bDotN, then the line lies on p, so return true
	//if one of a point on the line is on the positive side of p
	if ( aDotN == bDotN )
	{
		return a >= p;
	}

	//need to clip: compute t:
	//t is the distance of the intersection between the line and p,
	//along the line
	t = ( p.d - aDotN ) / ( bDotN - aDotN );

	if ( t <= 0.0 )		//intersection before a: all of line on one side of p
	{
		return b >= p;		//if b is on +ve side of p, then so is all of the line
	}
	else if ( t >= 1.0 )	//intersection after b: all of line on one side of p
	{
		return a >= p;		//if a is on +ve side of p, then so is all of the line
	}
	else
	{
		//line segment straddles plane
		//compute the intersection point
		intersection = a  +  ( getDirection() * t );

		if ( aDotN < p.d )		//a on -ve side of plane
		{
			a = intersection;		//replace a with intersection point
		}
		else if ( aDotN > p.d )	//a on +ve side of plane
		{
			b = intersection;		//replace b with intersection point
		}
		//else a is on plane, no need to replace a with intersection point
		return true;
	}
}


#endif
