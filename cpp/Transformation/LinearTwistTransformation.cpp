//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINEARTWISTTRANSFORMATION_CPP__
#define LINEARTWISTTRANSFORMATION_CPP__

#include <math.h>

#include <complex>

#include <Math/clamp.h>
#include <Math/Matrix4.h>

#include <Transformation/LinearTwistTransformation.h>






/*****************************************************************************
      LinearTwistTransformation
 *****************************************************************************/

LinearTwistTransformation::LinearTwistTransformation() : Transformation()
{
	zeroR = 1.0;
	dRdD = 0.0;
	lowerBound = -1e8;
	upperBound = 1e8;
}

LinearTwistTransformation::LinearTwistTransformation(double d0, double r0, double d1, double r1, double lower, double upper)
									: Transformation()
{
	double dD = d1 - d0;

	lowerBound = d0  +  dD * lower;
	upperBound = d0  +  dD * upper;

	double range = dD  *  ( upper - lower );

	if ( fabs( dD )  >=  EPSILON )
	{
		dRdD = ( r1 - r0 )  /  range;
		zeroR = r0  -  ( lowerBound * dRdD );
	}
	else
	{
		dRdD = 0.0;
		zeroR = 0.0;
	}
}

LinearTwistTransformation::LinearTwistTransformation(const LinearTwistTransformation &x)
									: Transformation( x )
{
	zeroR = x.zeroR;
	dRdD = x.dRdD;
	lowerBound = x.lowerBound;
	upperBound = x.upperBound;
}



Transformation* LinearTwistTransformation::clone() const
{
	return new LinearTwistTransformation( *this );
}


Point3 LinearTwistTransformation::xfPoint(const Point3 &p) const
{
	double param = clamp( p.z, lowerBound, upperBound );

	return p  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
}

Vector3 LinearTwistTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	double param = at.z;

	if ( param < lowerBound  ||  param > upperBound )
	{
		param = clamp( param, lowerBound, upperBound );
		return v  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
	}
	else
	{
		Vector3 toAt = at.toVector3();
		toAt.flatten( AXIS_Z );
		Vector3 shearVector = Vector3( 0.0, 0.0, 1.0 ).cross( toAt ) * dRdD;

		return v  *  Matrix4::shearZ( shearVector.x, shearVector.y )  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
	}
}



#endif
