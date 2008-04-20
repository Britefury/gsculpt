//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SPHERICALBRUSH_CPP__
#define SPHERICALBRUSH_CPP__

#include <math.h>

#include <algorithm>

#include <Brush/SphericalBrush.h>



RT_IMPLEMENT_TYPE( SphericalBrush, SphericalBoundaryBrush );


SphericalBrush::SphericalBrush() : SphericalBoundaryBrush(), sqrXScale( 1.0 )
{
}

SphericalBrush::SphericalBrush(double radius)
						: SphericalBoundaryBrush( radius )
{
	refresh();
}

SphericalBrush::SphericalBrush(const SphericalBrush &b)
						: SphericalBoundaryBrush( b ), sqrXScale( b.sqrXScale )
{
}

SphericalBrush::~SphericalBrush()
{
}


void SphericalBrush::setRadius(double radius)
{
	SphericalBoundaryBrush::setRadius( radius );
	refresh();
}



double SphericalBrush::computePointWeight(const Point3 &brushPosition, const Point3 &point) const
{
	double sqrDist = point.sqrDistanceTo( brushPosition );
	if ( sqrDist <= sqrRadius )
	{
		double sqrX = sqrDist * sqrXScale;
		double oneMinusSqrX = 1.0 - sqrX;
		double y = sqrt(  std::max( oneMinusSqrX, 0.0 ) );
		return y;
	}
	else
	{
		return 0.0;
	}
}



void SphericalBrush::refresh()
{
	if ( radius != 0.0 )
	{
		double xScale = 1.0 / radius;
		sqrXScale = xScale * xScale;
	}
	else
	{
		sqrXScale = 1.0;
	}
}


#endif // SPHERICALBRUSH_CPP__
