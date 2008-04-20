//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVECTOR2I_CPP
#define PYVECTOR2I_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Vector2i.h>

void export_Vector2i()
{
	class_<Vector2i>( "Vector2i", init<>() )
		.def( init<int, int>() )
		.def( init<const Vector2i &>() )
		.def_readwrite( "x", &Vector2i::x )
		.def_readwrite( "y", &Vector2i::y )
		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( "toVector2", &Vector2i::toVector2 );
}


#endif
