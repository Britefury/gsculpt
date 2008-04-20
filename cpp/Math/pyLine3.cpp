//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYLINE3_CPP
#define PYLINE3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Line3.h>

void export_Line3()
{
	class_<Line3>( "Line3", init<>() )
		.def( init<Point3, Point3>() )
		.def( init<Point3, Vector3>() )
		.def( init<const Line3 &>() )
		.def_readwrite( "origin", &Line3::origin )
		.def_readwrite( "direction", &Line3::direction )
		.def( self + Vector3() )
		.def( self += Vector3() )
		.def( self - Vector3() )
		.def( self -= Vector3() )
		.def( "getPoint", &Line3::getPoint )
		.def( "closestPointParam", &Line3::closestPointParam )
		.def( "closestPointTo", &Line3::py_closestPointTo )
		.def( "sqrDistanceTo", &Line3::py_sqrDistanceToPoint )
		.def( "distanceTo", &Line3::py_distanceToPoint )
		.def( "closestPointsParams", &Line3::py_closestPointsParams )
		.def( "closestPointsWithParams", &Line3::py_closestPointsWithParams )
		.def( "closestPoints", &Line3::py_closestPoints )
		.def( "sqrDistanceTo", &Line3::py_sqrDistanceToLine )
		.def( "distanceTo", &Line3::py_distanceToLine )
		.def( "intersect", &Line3::py_intersect );
}


#endif
