//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GAUSSIANBRUSH_CPP__
#define GAUSSIANBRUSH_CPP__

#include <math.h>

#include <Math/Gaussian.h>

#include <Brush/GaussianBrush.h>



RT_IMPLEMENT_TYPE( GaussianBrush, SphericalBoundaryBrush );


GaussianBrush::GaussianBrush() : SphericalBoundaryBrush(), yOffset( 0.0 ), yScale( 1.0 ), xScale( 1.0 )
{
}

GaussianBrush::GaussianBrush(double radius, double gaussianMinimum)
						: SphericalBoundaryBrush( radius )
{
	setGaussianMinimum( gaussianMinimum );
}

GaussianBrush::GaussianBrush(const GaussianBrush &b)
						: SphericalBoundaryBrush( b ), gaussianMinimum( b.gaussianMinimum ), yOffset( b.yOffset ), yScale( b.yScale ), xScale( b.xScale )
{
}

GaussianBrush::~GaussianBrush()
{
}


void GaussianBrush::setRadius(double radius)
{
	SphericalBoundaryBrush::setRadius( radius );
	refresh();
}



double GaussianBrush::getGaussianMinimum() const
{
	return gaussianMinimum;
}

void GaussianBrush::setGaussianMinimum(double gaussianMinimum)
{
	this->gaussianMinimum = gaussianMinimum;
	refresh();
}




double GaussianBrush::computePointWeight(const Point3 &brushPosition, const Point3 &point) const
{
	double sqrDist = point.sqrDistanceTo( brushPosition );
	if ( sqrDist <= sqrRadius )
	{
		double x = sqrt( sqrDist ) * xScale;
		double y = ( gaussian( x, 1.0, 0.0, 1.0 )  +  yOffset )  *  yScale;
		return y;
	}
	else
	{
		return 0.0;
	}
}



void GaussianBrush::refresh()
{
	double xForMinimum = invGaussian( gaussianMinimum, 1.0, 0.0, 1.0 );
	xScale = xForMinimum  /  radius;
	yOffset = -gaussianMinimum;
	yScale = 1.0  /  ( 1.0 - gaussianMinimum );
}


#endif // GAUSSIANBRUSH_CPP__
