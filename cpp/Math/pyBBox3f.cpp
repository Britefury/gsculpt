//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYBBOX3_CPP
#define PYBBOX3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/BBox3f.h>

void export_BBox3f()
{
	class_<BBox3f>( "BBox3f", init<>() )
		.def( init<Point3f>() )
		.def( init<Point3f, Point3f>() )
		.def( init<const BBox3f &>() )
		.def_readwrite( "_lower", &BBox3f::l )
		.def_readwrite( "_upper", &BBox3f::u )
		.def( "isEmpty", &BBox3f::isEmpty )
		.def( "addPoint", &BBox3f::addPoint )
		.def( "addBox", &BBox3f::addBox )
		.def( "enlargeForClipping", &BBox3f::enlargeForClipping )
		.def( "enlargeForIntersection", &BBox3f::enlargeForIntersection )
		.def( "intersects", &BBox3f::py_intersectsBox )
		.def( "intersection", &BBox3f::intersection )
		.def( "closestPointTo", &BBox3f::closestPointTo )
		.def( "sqrDistanceTo", &BBox3f::sqrDistanceTo )
		.def( "distanceTo", &BBox3f::distanceTo )
		.def( "contains", &BBox3f::py_containsPoint )
		.def( "contains", &BBox3f::py_containsBBox )
		.def( "getLower", &BBox3f::getLower )
		.def( "getUpper", &BBox3f::getUpper )
		.def( "getCentre", &BBox3f::getCentre )
		.def( "getPoint", &BBox3f::getPoint )
		.def( "getBounds", &BBox3f::getBounds )
		.def( "getLeadingVertex", &BBox3f::getLeadingVertex )
		.def( "getTrailingVertex", &BBox3f::getTrailingVertex )
		.def( "getWidth", &BBox3f::getWidth )
		.def( "getHeight", &BBox3f::getHeight )
		.def( "getDepth", &BBox3f::getDepth )
		.def( "getSize", &BBox3f::py_getSizeVector )
		.def( "getVolume", &BBox3f::getVolume )
		.def( "getSurfaceArea", &BBox3f::getSurfaceArea )
		.def( "getSizeInAxis", &BBox3f::py_getSizeInAxis )
		.def( "splitX", &BBox3f::py_splitX )
		.def( "splitY", &BBox3f::py_splitY )
		.def( "splitZ", &BBox3f::py_splitZ )
		.def( "getMajorAxis", &BBox3f::getMajorAxis )
		.def( "getMinorAxis", &BBox3f::getMinorAxis )
		.def( "getMajorSize", &BBox3f::getMajorAxis )
		.def( "getMinorSize", &BBox3f::getMinorAxis );
}


#endif
