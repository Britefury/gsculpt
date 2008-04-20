//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPLANE_CPP
#define PYPLANE_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Plane.h>

void export_Plane()
{
	class_<Plane>( "Plane", init<>() )
		.def( init<Vector3, double>() )
		.def( init<Vector3, Point3>() )
		.def( init<Axis, Point3>() )
		.def( init<Point3, Vector3, Vector3>() )
		.def( init<Point3, Point3, Point3>() )
		.def( init<const Plane &>() )
		.def_readwrite( "n", &Plane::n )
		.def_readwrite( "d", &Plane::d )
		.def( self != self )
		.def( -self )
		.def( "side", &Plane::py_sidePoint )
		.def( "directionSide", &Plane::directionSide )
		.def( "distanceTo", &Plane::distanceTo )
		.def( "getPointOnPlane", &Plane::getPointOnPlane )
		.def( "projectPoint", &Plane::projectPoint )
		.def( "intersect", &Plane::py_intersect_ray )
		.def( "intersect", &Plane::py_intersect_seg )
		.def( "intersectStrict", &Plane::py_intersectStrict )
		.def( "separates", &Plane::separates )
		.def( "fastPlaneAUV", &Plane::py_fastPlaneAUV ).staticmethod( "fastPlaneAUV" )
		.def( "fastPlaneABC", &Plane::py_fastPlaneABC ).staticmethod( "fastPlaneABC" )
		.def( self == Point3() )
		.def( Point3() == self )
		.def( self != Point3() )
		.def( Point3() != self )
		.def( self < Point3() )
		.def( Point3() < self )
		.def( self <= Point3() )
		.def( Point3() <= self )
		.def( self >= Point3() )
		.def( Point3() >= self )
		.def( self > Point3() )
		.def( Point3() > self );
}


#endif
