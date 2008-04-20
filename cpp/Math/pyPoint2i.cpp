//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOINT2I_CPP
#define PYPOINT2I_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Point2i.h>

void export_Point2i()
{
	class_<Point2i>( "Point2i", init<>() )
		.def( init<int, int>() )
		.def( init<Vector2i>() )
		.def( init<const Point2i &>() )
		.def_readwrite( "x", &Point2i::x )
		.def_readwrite( "y", &Point2i::y )
		.def( self + Vector2i() )
		.def( self += Vector2i() )
		.def( self - self )
		.def( self - Vector2i() )
		.def( self -= Vector2i() )
		.def( "toVector2i", &Point2i::toVector2i )
		.def( "toPoint2", &Point2i::toPoint2 );
}


#endif
