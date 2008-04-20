//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYAXES2_CPP
#define PYAXES2_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Axes2.h>

void export_Axes2()
{
	class_<Axes2>( "Axes2", init<>() )
		.def( init<Vector2, Vector2>() )
		.def( init<const Axes2 &>() )
		.def_readwrite( "i", &Axes2::i )
		.def_readwrite( "j", &Axes2::j )
		.def( self + self )
		.def( self += self )
		.def( self * double() )
		.def( self *= double() )
		.def( "normalise", &Axes2::normalise );
}


#endif
