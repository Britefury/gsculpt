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


#include <Math/BBox3.h>

void export_BBox3()
{
	class_<BBox3>( "BBox3", init<>() )
		.def( init<Point3>() )
		.def( init<Point3, Point3>() )
		.def( init<const BBox3 &>() )
		.def_readwrite( "_lower", &BBox3::l )
		.def_readwrite( "_upper", &BBox3::u )
		.def( "isEmpty", &BBox3::isEmpty )
		.def( "addPoint", &BBox3::addPoint )
		.def( "addBox", &BBox3::addBox )
		.def( "side", &BBox3::side )
		.def( "enlargeForClipping", &BBox3::enlargeForClipping )
		.def( "enlargeForIntersection", &BBox3::enlargeForIntersection )
		.def( "intersects", &BBox3::py_intersectsSegment )
		.def( "intersects", &BBox3::py_intersectsBox )
		.def( "intersection", &BBox3::intersection )
		.def( "closestPointTo", &BBox3::closestPointTo )
		.def( "sqrDistanceTo", &BBox3::sqrDistanceTo )
		.def( "distanceTo", &BBox3::distanceTo )
		.def( "contains", &BBox3::py_containsPoint )
		.def( "containsAllOf", &BBox3::py_containsAllOfSegment )
		.def( "containsPartOf", &BBox3::py_containsPartOfSegment )
		.def( "containsAllOf", &BBox3::py_containsAllOfTriangle )
		.def( "containsPartOf", &BBox3::py_containsPartOfTriangle )
		.def( "contains", &BBox3::py_containsBBox )
		.def( "getLower", &BBox3::getLower )
		.def( "getUpper", &BBox3::getUpper )
		.def( "getCentre", &BBox3::getCentre )
		.def( "getPoint", &BBox3::getPoint )
		.def( "getPlane", &BBox3::getPlane )
		.def( "getBounds", &BBox3::getBounds )
		.def( "getLeadingVertex", &BBox3::getLeadingVertex )
		.def( "getTrailingVertex", &BBox3::getTrailingVertex )
		.def( "getWidth", &BBox3::getWidth )
		.def( "getHeight", &BBox3::getHeight )
		.def( "getDepth", &BBox3::getDepth )
		.def( "getSize", &BBox3::py_getSizeVector )
		.def( "getVolume", &BBox3::getVolume )
		.def( "getSurfaceArea", &BBox3::getSurfaceArea )
		.def( "getSizeInAxis", &BBox3::py_getSizeInAxis )
		.def( "splitX", &BBox3::py_splitX )
		.def( "splitY", &BBox3::py_splitY )
		.def( "splitZ", &BBox3::py_splitZ )
		.def( "clipSegment", &BBox3::clipSegment )
		.def( "getMajorAxis", &BBox3::getMajorAxis )
		.def( "getMinorAxis", &BBox3::getMinorAxis )
		.def( "getMajorSize", &BBox3::getMajorAxis )
		.def( "getMinorSize", &BBox3::getMinorAxis )
		.def( "toBBox2", &BBox3::toBBox2 );
}


#endif
