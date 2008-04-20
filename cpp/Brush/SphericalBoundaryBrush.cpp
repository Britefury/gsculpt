//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SPHERICALBOUNDARYBRUSH_CPP__
#define SPHERICALBOUNDARYBRUSH_CPP__

#include <Brush/SphericalBoundaryBrush.h>




RT_IMPLEMENT_ABSTRACT_TYPE( SphericalBoundaryBrush, Brush );


SphericalBoundaryBrush::SphericalBoundaryBrush() : Brush(),
						radius( 0.0 ), sqrRadius( 0.0 )
{
}

SphericalBoundaryBrush::SphericalBoundaryBrush(double radius)
						: Brush()
{
	setRadius( radius );
}

SphericalBoundaryBrush::SphericalBoundaryBrush(const SphericalBoundaryBrush &b)
						: Brush( b ), radius( b.radius ), sqrRadius( b.sqrRadius )
{
}

SphericalBoundaryBrush::~SphericalBoundaryBrush()
{
}


double SphericalBoundaryBrush::getRadius() const
{
	return radius;
}

void SphericalBoundaryBrush::setRadius(double radius)
{
	this->radius = radius;
	sqrRadius = radius * radius;
}



bool SphericalBoundaryBrush::intersectsBBox(const Point3 &brushPosition, const BBox3 &box) const
{
	return box.sqrDistanceTo( brushPosition )  <=  sqrRadius;
}



#endif // SPHERICALBOUNDARYBRUSH_H__
