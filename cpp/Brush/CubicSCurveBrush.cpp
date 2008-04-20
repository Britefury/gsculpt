//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CUBICBRUSH_CPP__
#define CUBICBRUSH_CPP__

#include <math.h>

#include <Math/Spline.h>

#include <Brush/CubicSCurveBrush.h>



RT_IMPLEMENT_TYPE( CubicSCurveBrush, SphericalBoundaryBrush );


CubicSCurveBrush::CubicSCurveBrush() : SphericalBoundaryBrush(), xScale( 1.0 )
{
}

CubicSCurveBrush::CubicSCurveBrush(double radius)
						: SphericalBoundaryBrush( radius )
{
	refresh();
}

CubicSCurveBrush::CubicSCurveBrush(const CubicSCurveBrush &b)
						: SphericalBoundaryBrush( b ), xScale( b.xScale )
{
}

CubicSCurveBrush::~CubicSCurveBrush()
{
}


void CubicSCurveBrush::setRadius(double radius)
{
	SphericalBoundaryBrush::setRadius( radius );
	refresh();
}



double CubicSCurveBrush::computePointWeight(const Point3 &brushPosition, const Point3 &point) const
{
	double sqrDist = point.sqrDistanceTo( brushPosition );
	if ( sqrDist <= sqrRadius )
	{
		double x = sqrt( sqrDist ) * xScale;
		double y = cubicBezier( 0.0, 0.0, 1.0, 1.0, 1.0 - x, false );
		return y;
	}
	else
	{
		return 0.0;
	}
}



void CubicSCurveBrush::refresh()
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


#endif // CUBICBRUSH_CPP__
