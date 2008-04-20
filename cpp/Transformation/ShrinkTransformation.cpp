//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SHRINKTRANSFORMATION_CPP__
#define SHRINKTRANSFORMATION_CPP__

#include <algorithm>


#include <Math/Matrix4.h>

#include <Transformation/ShrinkTransformation.h>






ShrinkTransformation::ShrinkTransformation() : Transformation()
{
	offsetU = 1.0;
	offsetV = 1.0;
	dUdQ2 = 0.0;
	dVdQ2 = 0.0;
	centreZ = 0.0;
	lowerZ = 0.0;
	upperZ = 1.0e8;
}

ShrinkTransformation::ShrinkTransformation(double lowerZ, double upperZ, double shrinkCentreZ, double shrinkU, double shrinkV)
									: Transformation()
{
	this->lowerZ = lowerZ;
	this->upperZ = upperZ;
	centreZ = shrinkCentreZ;

	//get maximum depth from centre
	double depthToCentre = std::max( fabs( lowerZ - centreZ ),  fabs( upperZ - centreZ ) );
	if ( depthToCentre >= EPSILON )
	{
		double oneOverQSquaredAtEnd = 1.0  /  ( depthToCentre * depthToCentre );
		dUdQ2 = ( 1.0 - shrinkU ) * oneOverQSquaredAtEnd;
		dVdQ2 = ( 1.0 - shrinkV ) * oneOverQSquaredAtEnd;

		offsetU = shrinkU;
		offsetV = shrinkV;
	}
	else
	{
		dUdQ2 = dVdQ2 = 0.0;
		offsetU = offsetV = 1.0;
	}
}

ShrinkTransformation::ShrinkTransformation(const ShrinkTransformation &x)
									: Transformation( x )
{
	offsetU = x.offsetU;
	offsetV = x.offsetV;
	dUdQ2 = x.dUdQ2;
	dVdQ2 = x.dVdQ2;
	centreZ = x.centreZ;
	lowerZ = x.lowerZ;
	upperZ = x.upperZ;
}



Transformation* ShrinkTransformation::clone() const
{
	return new ShrinkTransformation( *this );
}


Point3 ShrinkTransformation::xfPoint(const Point3 &p) const
{
	double z = clamp( p.z, lowerZ, upperZ );

	double q = z - centreZ;

	double su = ( q * q ) * dUdQ2  +  offsetU;
	double sv = ( q * q ) * dVdQ2  +  offsetV;
	Matrix4 sc = Matrix4::scale( su, sv, 1.0 );

	return p * sc;
}

Vector3 ShrinkTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	double z = at.z;

	if ( z < lowerZ  ||  z > upperZ )
	{
		z = clamp( z, lowerZ, upperZ );
		double q = z - centreZ;

		double su = ( q * q ) * dUdQ2  +  offsetU;
		double sv = ( q * q ) * dVdQ2  +  offsetV;
		Matrix4 sc = Matrix4::scale( su, sv, 1.0 );

		return v * sc;
	}
	else
	{
		double q = z - centreZ;

		double su = ( q * q ) * dUdQ2  +  offsetU;
		double sv = ( q * q ) * dVdQ2  +  offsetV;
		Matrix4 sc = Matrix4::scale( su, sv, 1.0 );
		Matrix4 sh = Matrix4::shearZ( q * dUdQ2 * at.x * 2.0,  q * dVdQ2 * at.y * 2.0 );

		return v * sc * sh;
	}
}






#endif

