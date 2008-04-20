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
#ifndef RADIUS2TRANSFORMATION_CPP__
#define RADIUS2TRANSFORMATION_CPP__

#include <complex>

#include <Transformation/Radius2Transformation.h>



Radius2Transformation::Radius2Transformation() : Transformation()
{
}


Radius2Transformation::Radius2Transformation(const Radius2Transformation &x)
									: Transformation( x )
{
}



Transformation * Radius2Transformation::clone() const
{
	return new Radius2Transformation( *this );
}


Point3 Radius2Transformation::xfPoint(const Point3 &p) const
{
	return Point3( p.x, p.y, Vector2( p.x, p.y ).length() );
}

Vector3 Radius2Transformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	//no need to subtract the result of transforming the origin, because
	//the origin remains unchanged
	return Vector3( v.x, v.y, 0.0 );
}



#endif

