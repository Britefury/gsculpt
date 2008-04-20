//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BENDTRANSFORMATION_CPP__
#define BENDTRANSFORMATION_CPP__

#include <math.h>

#include <Math/clamp.h>
#include <Math/Matrix4.h>


#include <Transformation/BendTransformation.h>




BendTransformation::BendTransformation() : Transformation()
{
	grad = 1.0;
	lowerBound = 0.0;
	upperBound = 1.0e16;
	lowerAngle = lowerBound * grad;
	upperAngle = upperBound * grad;
}

BendTransformation::BendTransformation(double dAdZ, double lower, double upper)
								: Transformation()
{
	grad = dAdZ;
	lowerBound = lower;
	upperBound = upper;
	lowerAngle = lowerBound * grad;
	upperAngle = upperBound * grad;
}



BendTransformation::BendTransformation(const BendTransformation &x)
								: Transformation( x )
{
	grad = x.grad;
	lowerBound = x.lowerBound;
	upperBound = x.upperBound;
	lowerAngle = x.lowerAngle;
	upperAngle = x.upperAngle;
}



Transformation* BendTransformation::clone() const
{
	return new BendTransformation( *this );
}


Point3 BendTransformation::xfPoint(const Point3 &p) const
{
	if ( p.z < lowerBound )
	{
		return p  *  Matrix4::translate( Vector3( 0.0, 0.0, -lowerBound ) ) *
						 Matrix4::rotateY( -lowerAngle );
	}
	else if ( p.z > upperBound )
	{
		return p  *  Matrix4::translate( Vector3( 0.0, 0.0, -upperBound ) ) *
						 Matrix4::rotateY( -upperAngle );
	}
	else
	{
		double angle = p.z * grad;
		double radius = p.x;

		return Point3( cos( angle )  *  radius,  p.y,  sin( angle )  *  radius );
	}
}

Vector3 BendTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	if ( at.z < lowerBound )
	{
		return v  *  Matrix4::rotateY( -lowerAngle );
	}
	else if ( at.z > upperBound )
	{
		return v  *  Matrix4::rotateY( -upperAngle );
	}
	else
	{
		double angle = at.z * grad;

		Matrix4 rotation = Matrix4::rotateY( -angle );

		return v * rotation;
	}
}



#endif
