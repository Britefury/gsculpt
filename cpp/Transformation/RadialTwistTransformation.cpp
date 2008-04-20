//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RADIALTWISTTRANSFORMATION_CPP__
#define RADIALTWISTTRANSFORMATION_CPP__

#include <math.h>

#include <complex>

#include <Math/clamp.h>
#include <Math/Matrix4.h>

#include <Transformation/RadialTwistTransformation.h>






/*****************************************************************************
      RadialTwistTransformation
 *****************************************************************************/

RadialTwistTransformation::RadialTwistTransformation() : Transformation()
{
	zeroR = 1.0;
	dRdD = 0.0;
	lowerBound = -1e8;
	upperBound = 1e8;
}

RadialTwistTransformation::RadialTwistTransformation(double d0, double r0, double d1, double r1, double lower, double upper)
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

RadialTwistTransformation::RadialTwistTransformation(const RadialTwistTransformation &x)
									: Transformation( x )
{
	zeroR = x.zeroR;
	dRdD = x.dRdD;
	lowerBound = x.lowerBound;
	upperBound = x.upperBound;
}



Transformation* RadialTwistTransformation::clone() const
{
	return new RadialTwistTransformation( *this );
}


Point3 RadialTwistTransformation::xfPoint(const Point3 &p) const
{
	double param = Vector2( p.x, p.y ).length();

	param = clamp( param, lowerBound, upperBound );

	return p  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
}

Vector3 RadialTwistTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	double param = Vector2( at.x, at.y ).length();

	if ( param < lowerBound  ||  param > upperBound )
	{
		param = clamp( param, lowerBound, upperBound );
		return v  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
	}
	else
	{
		Vector3 toAt = at.toVector3();
		toAt.flatten( AXIS_Z );

		if ( param < EPSILON )
		{
			return v  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
		}
		else
		{
			Vector3 shearVector = Vector3( 0.0, 0.0, 1.0 ).cross( toAt ) * dRdD;
			toAt.normalise();

			return v  *  Matrix4::shear( toAt, shearVector )  *  Matrix4::rotateZ( param * dRdD  +  zeroR );
		}
	}
}



#endif
