//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYRAY3_CPP
#define PYRAY3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Ray3.h>

void export_Ray3()
{
	class_<Ray3>( "Ray3", init<>() )
		.def( init<Point3, Point3>() )
		.def( init<Point3, Vector3>() )
		.def( init<const Ray3 &>() )
		.def_readwrite( "origin", &Ray3::origin )
		.def_readwrite( "direction", &Ray3::direction )
		.def( self + Vector3() )
		.def( self += Vector3() )
		.def( self - Vector3() )
		.def( self -= Vector3() )
		.def( "getPoint", &Ray3::getPoint )
		.def( "closestPointParam", &Ray3::closestPointParam )
		.def( "closestPointTo", &Ray3::py_closestPointTo )
		.def( "sqrDistanceTo", &Ray3::sqrDistanceTo )
		.def( "distanceTo", &Ray3::distanceTo )
		.def( "intersect", &Ray3::py_intersect );
}


#endif
