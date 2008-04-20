//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TRANSFORMATION_H__
#define TRANSFORMATION_H__

#include <Math/Vector3.h>
#include <Math/Point3.h>


/*
*******************************************************************************
										Transformation
*******************************************************************************
*/

class GS_DllExport Transformation
{
private:
	Transformation *child;

public:
	Transformation();
	Transformation(const Transformation &c);
	virtual ~Transformation();

	//methods to transform a point or a vector
	Point3 transformPoint(const Point3 &p) const;
	Vector3 transformVector(const Vector3 &v, const Point3 &at) const;

	void chainTo(const Transformation *x);


	virtual Transformation* clone() const;

protected:
	virtual Point3 xfPoint(const Point3 &p) const;
	virtual Vector3 xfVector(const Vector3 &v, const Point3 &at) const;
};


#endif
