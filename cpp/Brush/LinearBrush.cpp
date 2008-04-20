//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINEARBRUSH_CPP__
#define LINEARBRUSH_CPP__

#include <math.h>

#include <Brush/LinearBrush.h>



RT_IMPLEMENT_TYPE( LinearBrush, SphericalBoundaryBrush );


LinearBrush::LinearBrush() : SphericalBoundaryBrush(), xScale( 1.0 )
{
}

LinearBrush::LinearBrush(double radius)
						: SphericalBoundaryBrush( radius )
{
	refresh();
}

LinearBrush::LinearBrush(const LinearBrush &b)
						: SphericalBoundaryBrush( b ), xScale( b.xScale )
{
}

LinearBrush::~LinearBrush()
{
}


void LinearBrush::setRadius(double radius)
{
	SphericalBoundaryBrush::setRadius( radius );
	refresh();
}



double LinearBrush::computePointWeight(const Point3 &brushPosition, const Point3 &point) const
{
	double sqrDist = point.sqrDistanceTo( brushPosition );
	if ( sqrDist <= sqrRadius )
	{
		double x = sqrt( sqrDist ) * xScale;
		double y = 1.0 - x;
		return y;
	}
	else
	{
		return 0.0;
	}
}



void LinearBrush::refresh()
{
	if ( radius != 0.0 )
	{
		xScale = 1.0 / radius;
	}
	else
	{
		xScale = 1.0;
	}
}


#endif // LINEARBRUSH_CPP__
