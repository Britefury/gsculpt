//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOLYGON3_CPP
#define PYPOLYGON3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Polygon3.h>

void export_Polygon3()
{
	class_<Polygon3>( "Polygon3", init<>() )
		.def( init<const Polygon3 &>() )
		.def( "__len__", &Polygon3::size )
		.def( "__getitem__", &Polygon3::py__getitem__, return_internal_reference<>() )
		.def( "__setitem__", &Polygon3::py__setitem__ )
		.def( "append", &Polygon3::addVertex )
		.def( "__delitem__", &Polygon3::removeVertex )
		.def( "clear", &Polygon3::clear )
		.def( "toPolygon2", &Polygon3::py_toPolygon2 )
		.def( "side", &Polygon3::side )
		.def( "sutherlandHodgmanClip", &Polygon3::py_sutherlandHodgmanClip ).staticmethod( "sutherlandHodgmanClip" );
}


#endif
