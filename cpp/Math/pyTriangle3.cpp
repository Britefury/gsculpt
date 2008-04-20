//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYTRIANGLE3_CPP
#define PYTRIANGLE3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Triangle3.h>

void export_Triangle3()
{
	class_<Triangle3>( "Triangle3", init<>() )
		.def( init<Point3, Point3, Point3>() )
		.def( init<Triangle2>() )
		.def( init<const Triangle3 &>() )
		.def_readwrite( "a", &Triangle3::a )
		.def_readwrite( "b", &Triangle3::b )
		.def_readwrite( "c", &Triangle3::c )
		.def( "getEdge", &Triangle3::getEdge )
		.def( "getFastEdgePlane", &Triangle3::getFastEdgePlane )
		.def( "getPlane", &Triangle3::getPlane )
		.def( "getFastPlane", &Triangle3::getFastPlane )
		.def( "raytrace", &Triangle3::py_raytraceSegment3 )
		.def( "raytrace", &Triangle3::py_raytraceLine3 )
		.def( "uvToPoint", &Triangle3::uvToPoint )
		.def( "side", &Triangle3::side );
}


#endif
