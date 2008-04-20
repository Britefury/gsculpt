//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
/************************
 This program is free software; you can use it, redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as published by the
 Free Software Foundation. The full text of the GNU General Public License
 version 2 can be found in the file named 'COPYING' that accompanies this
 program. This source code is (C)copyright Geoffrey French 1999-2005.
 ************************/
#ifndef RADIUS2TRANSFORMATION_H__
#define RADIUS2TRANSFORMATION_H__


#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Transformation/Transformation.h>



class GS_DllExport Radius2Transformation : public Transformation
{
public:
	Radius2Transformation();
	Radius2Transformation(const Radius2Transformation &x);


	virtual Transformation * clone() const;

protected:
	virtual Point3 xfPoint(const Point3 &p) const;
	virtual Vector3 xfVector(const Vector3 &v, const Point3 &at) const;
};



#endif
