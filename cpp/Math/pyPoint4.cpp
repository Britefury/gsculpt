//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOINT4_CPP
#define PYPOINT4_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Point4.h>

void export_Point4()
{
	class_<Point4>( "Point4", init<>() )
		.def( init<double, double, double>() )
		.def( init<double, double, double, double>() )
		.def( init<Point3>() )
		.def( init<Point3, double>() )
		.def( init<const Point4 &>() )
		.def_readwrite( "x", &Point4::x )
		.def_readwrite( "y", &Point4::y )
		.def_readwrite( "z", &Point4::z )
		.def_readwrite( "w", &Point4::w )
		.def( "convertToPoint3", &Point4::convertToPoint3 )
		.def( "inverseConvertToPoint3", &Point4::inverseConvertToPoint3 )
		.def( "toPoint3", &Point4::toPoint3 );
}


#endif
