//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYSEGMENT3_CPP
#define PYSEGMENT3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Segment3.h>

void export_Segment3()
{
	class_<Segment3>( "Segment3", init<>() )
		.def( init<Point3, Point3>() )
		.def( init<Point3, Vector3>() )
		.def( init<const Segment3 &>() )
		.def_readwrite( "a", &Segment3::a )
		.def_readwrite( "b", &Segment3::b )
		.def( self + Vector3() )
		.def( self += Vector3() )
		.def( self - Vector3() )
		.def( self -= Vector3() )
		.def( "toSegment2", &Segment3::toSegment2 )
		.def( "getDirection", &Segment3::getDirection )
		.def( "sqrLength", &Segment3::sqrLength )
		.def( "length", &Segment3::length )
		.def( "getMidPoint", &Segment3::getMidPoint )
		.def( "getPoint", &Segment3::getPoint )
		.def( "closestPointTo", &Segment3::py_closestPointTo )
		.def( "sqrDistanceTo", &Segment3::sqrDistanceTo )
		.def( "distanceTo", &Segment3::distanceTo )
		.def( "intersect", &Segment3::py_intersect )
		.def( "clip", &Segment3::clip )
		.def( "translate", &Segment3::translate )
		.def( "scale", &Segment3::scale )
		.def( "side", &Segment3::side );
}


#endif
