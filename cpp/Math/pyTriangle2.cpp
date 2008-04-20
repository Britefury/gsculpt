//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYTRIANGLE2_CPP
#define PYTRIANGLE2_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Triangle2.h>

void export_Triangle2()
{
	class_<Triangle2>( "Triangle2", init<>() )
		.def( init<Point2, Point2, Point2>() )
		.def( init<const Triangle2 &>() )
		.def_readwrite( "a", &Triangle2::a )
		.def_readwrite( "b", &Triangle2::b )
		.def_readwrite( "c", &Triangle2::c )
		.def( "getEdge", &Triangle2::getEdge )
		.def( "areaX2", &Triangle2::areaX2 )
		.def( "area", &Triangle2::area )
		.def( "contains", &Triangle2::contains );
}


#endif
