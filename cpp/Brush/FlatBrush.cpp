//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FLATBRUSH_CPP__
#define FLATBRUSH_CPP__

#include <math.h>

#include <Brush/FlatBrush.h>



RT_IMPLEMENT_TYPE( FlatBrush, SphericalBoundaryBrush );


FlatBrush::FlatBrush() : SphericalBoundaryBrush()
{
}

FlatBrush::FlatBrush(double radius)
				: SphericalBoundaryBrush( radius )
{
}

FlatBrush::FlatBrush(const FlatBrush &b)
				: SphericalBoundaryBrush( b )
{
}

FlatBrush::~FlatBrush()
{
}


double FlatBrush::computePointWeight(const Point3 &brushPosition, const Point3 &point) const
{
	double sqrDist = point.sqrDistanceTo( brushPosition );
	if ( sqrDist <= sqrRadius )
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}



#endif // FLATBRUSH_CPP__
