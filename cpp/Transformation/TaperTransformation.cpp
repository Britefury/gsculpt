//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TAPERTRANSFORMATION_CPP__
#define TAPERTRANSFORMATION_CPP__

#include <Math/clamp.h>
#include <Math/Matrix4.h>

#include <Transformation/TaperTransformation.h>



TaperTransformation::TaperTransformation() : Transformation()
{
	dUdQ = 0.0;
	dVdQ = 0.0;
	lowerZ = 0.0;
	upperZ = 1.0e8;
}

TaperTransformation::TaperTransformation(double lowerZ, double upperZ, double taperU, double taperV)
								: Transformation()
{
	this->lowerZ = lowerZ;
	this->upperZ = upperZ;

	double depth = upperZ - lowerZ;
	if ( fabs( depth )  >=  EPSILON )
	{
		double oneOverDepth = 1.0 / depth;

		dUdQ = ( taperU - 1.0 )  *  oneOverDepth;
		dVdQ = ( taperV - 1.0 )  *  oneOverDepth;
	}
	else
	{
		dUdQ = dVdQ = 0.0;
	}
}


TaperTransformation::TaperTransformation(const TaperTransformation &x)
													  : Transformation( x )
{
	dUdQ = x.dUdQ;
	dVdQ = x.dVdQ;
	lowerZ = x.lowerZ;
	upperZ = x.upperZ;
}



Transformation* TaperTransformation::clone() const
{
	return new TaperTransformation( *this );
}


Point3 TaperTransformation::xfPoint(const Point3 &p) const
{
	double z = p.z;

	z = clamp( z, lowerZ, upperZ );
	double q = z - lowerZ;

	double su = q * dUdQ  +  1.0;
	double sv = q * dVdQ  +  1.0;
	Matrix4 sc = Matrix4::scale( su, sv, 1.0 );

	return p * sc;
}

Vector3 TaperTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	double z = at.z;

	if ( z < lowerZ  ||  z > upperZ )
	{
		z = clamp( z, lowerZ, upperZ );
		double q = z - lowerZ;

		double su = q * dUdQ  +  1.0;
		double sv = q * dVdQ  +  1.0;
		Matrix4 sc = Matrix4::scale( su, sv, 1.0 );

		return v * sc;
	}
	else
	{
		double q = z - lowerZ;

		double su = q * dUdQ  +  1.0;
		double sv = q * dVdQ  +  1.0;
		Matrix4 sc = Matrix4::scale( su, sv, 1.0 );
		Matrix4 sh = Matrix4::shearZ( dUdQ * at.x,  dVdQ * at.y );

		return v * sc * sh;
	}
}



#endif

