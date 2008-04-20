//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MATRIXTRANSFORMATION_CPP__
#define MATRIXTRANSFORMATION_CPP__

#include <Transformation/MatrixTransformation.h>



/*
*******************************************************************************
										MatrixTransformation
*******************************************************************************
*/

MatrixTransformation::MatrixTransformation() : Transformation()
{
	matrix = Matrix4();
}

MatrixTransformation::MatrixTransformation(const Matrix4 &m) : Transformation()
{
	matrix = m;
}

MatrixTransformation::MatrixTransformation(const MatrixTransformation &x)
														 : Transformation( x )
{
	matrix = x.matrix;
}



Transformation* MatrixTransformation::clone() const
{
	return new MatrixTransformation( *this );
}


Point3 MatrixTransformation::xfPoint(const Point3 &p) const
{
	return p * matrix;
}

Vector3 MatrixTransformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	return v * matrix;
}


#endif
