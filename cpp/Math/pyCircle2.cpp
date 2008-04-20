//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCIRCLE2_CPP
#define PYCIRCLE2_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Circle2.h>

void export_Circle2()
{
	class_<Circle2>( "Circle2", init<>() )
		.def( init<Point2, double>() )
		.def( init<const Circle2 &>() )
		.def( "isEmpty", &Circle2::isEmpty )
		.def( "getCentre", &Circle2::getCentre, return_internal_reference<>() )
		.def( "getRadius", &Circle2::getRadius )
		.def( "getSqrRadius", &Circle2::getRadiusSquared )
		.def( "intersects", &Circle2::intersects )
		.def( "isEmpty", &Circle2::isEmpty )
		.def( "containsAllOf", &Circle2::py_containsAllOfSegment )
		.def( "containsPartOf", &Circle2::py_containsPartOfSegment )
		.def( "containsAllOf", &Circle2::py_containsAllOfPolygon )
		.def( "containsPartOf", &Circle2::py_containsPartOfPolygon );
}


#endif
