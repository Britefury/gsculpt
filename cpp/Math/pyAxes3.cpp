//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYAXES3_CPP
#define PYAXES3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Axes3.h>

void export_Axes3()
{
	class_<Axes3>( "Axes3", init<>() )
		.def( init<Axis, Axis, Axis>() )
		.def( init<Vector3, Vector3, Vector3>() )
		.def( init<Axis>() )
		.def( init<const Axes3 &>() )
		.def_readwrite( "i", &Axes3::i )
		.def_readwrite( "j", &Axes3::j )
		.def_readwrite( "k", &Axes3::k )
		.def( self + self )
		.def( self += self )
		.def( self * double() )
		.def( self *= double() )
		.def( "normalise", &Axes3::normalise )
		.def( "getToX", &Axes3::getToX )
		.def( "getToY", &Axes3::getToY )
		.def( "getToZ", &Axes3::getToZ );
}


#endif
