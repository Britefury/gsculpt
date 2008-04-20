//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MATRIXTRANSFORMATION_H__
#define MATRIXTRANSFORMATION_H__

#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Math/Matrix4.h>
#include <Transformation/Transformation.h>


/*
*******************************************************************************
										MatrixTransformation
*******************************************************************************
*/

class GS_DllExport MatrixTransformation : public Transformation
{
protected:
	Matrix4 matrix;				//the matrix

public:
	MatrixTransformation();
	MatrixTransformation(const Matrix4 &m);
	MatrixTransformation(const MatrixTransformation &x);


	virtual Transformation* clone() const;

protected:
	virtual Point3 xfPoint(const Point3 &p) const;
	virtual Vector3 xfVector(const Vector3 &v, const Point3 &at) const;
};


#endif
