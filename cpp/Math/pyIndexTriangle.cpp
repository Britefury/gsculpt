//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYINDEXTRIANGLE_CPP
#define PYINDEXTRIANGLE_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/IndexTriangle.h>


void export_IndexTriangle()
{
	class_<IndexTriangle>( "IndexTriangle", init<>() )
		.def( init<int, int, int>() )
		.def_readwrite( "a", &IndexTriangle::a )
		.def_readwrite( "b", &IndexTriangle::b )
		.def_readwrite( "c", &IndexTriangle::c );
}


#endif
