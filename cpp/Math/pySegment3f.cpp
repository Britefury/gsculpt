//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYSEGMENT3F_CPP
#define PYSEGMENT3F_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Segment3f.h>

void export_Segment3f()
{
	class_<Segment3f>( "Segment3f", init<>() )
		.def( init<Point3f, Point3f>() )
		.def( init<Point3f, Vector3f>() )
		.def( init<const Segment3 &>() )
		.def( init<const Segment3f &>() )
		.def_readwrite( "a", &Segment3f::a )
		.def_readwrite( "b", &Segment3f::b )
		.def( self + Vector3f() )
		.def( self += Vector3f() )
		.def( self - Vector3f() )
		.def( self -= Vector3f() )
		.def( "getDirection", &Segment3f::getDirection )
		.def( "sqrLength", &Segment3f::sqrLength )
		.def( "length", &Segment3f::length )
		.def( "getMidPoint", &Segment3f::getMidPoint )
		.def( "getPoint", &Segment3f::getPoint )
		.def( "closestPointTo", &Segment3f::py_closestPointTo )
		.def( "sqrDistanceTo", &Segment3f::sqrDistanceTo )
		.def( "distanceTo", &Segment3f::distanceTo )
		.def( "translate", &Segment3f::translate )
		.def( "scale", &Segment3f::scale );
}


#endif
