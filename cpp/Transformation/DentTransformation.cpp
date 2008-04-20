//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef DENTTRANSFORMATION_CPP__
#define DENTTRANSFORMATION_CPP__

#include <math.h>

#include <complex>

#include <Math/Matrix4.h>

#include <Transformation/DentTransformation.h>






DentTransformation::DentTransformation() : Transformation()
{
	scale = offset = 0.0;
}

DentTransformation::DentTransformation(double radius, double dentScale)
								: Transformation()
{
	double sqrRadius = radius * radius;

	if ( sqrRadius >= SQR_EPSILON )
	{
		scale = ( 1.0 - dentScale )  /  sqrRadius;
		offset = dentScale;
	}
	else
	{
		scale = 0.0;
		offset = 1.0;
	}
}

DentTransformation::DentTransformation(const DentTransformation &x)
								: Transformation( x )
{
	scale = x.scale;
	offset = x.offset;
}



Transformation* DentTransformation::clone() const
{
	return new DentTransformation( *this );
}


Point3 DentTransformation::xfPoint(const Point3 &p) const
{
	double sqrR = Vector2( p.x, p.y ).sqrLength();
	double s = sqrR * scale  +  offset;

	Matrix4 sc = Matrix4::scale( Vector3( 1.0, 1.0, s ) );

	return p * sc;
}

Vector3 DentTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	double sqrR = Vector2( at.x, at.y ).sqrLength();
	double r = sqrt( sqrR );
	double s = sqrR * scale  +  offset;

	Vector3 shearDirection = at.toVector3();
	shearDirection.flatten( AXIS_Z );
	shearDirection.normalise();

	Matrix4 sc = Matrix4::scale( Vector3( 1.0, 1.0, s ) );

	Matrix4 sh = Matrix4::shear( shearDirection, Vector3( 0.0, 0.0, r * scale * at.z * 2.0 ) );

	return v * sc * sh;
}


#endif

