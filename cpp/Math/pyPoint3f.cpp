//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOINT3F_CPP
#define PYPOINT3F_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Point3f.h>

void export_Point3f()
{
	class_<Point3f>( "Point3f", init<>() )
		.def( init<float, float, float>() )
		.def( init<Point2f>() )
		.def( init<Point2f, float>() )
		.def( init<Vector3f>() )
		.def( init<Point3>() )
		.def( init<const Point3f &>() )
		.def_readwrite( "x", &Point3f::x )
		.def_readwrite( "y", &Point3f::y )
		.def_readwrite( "z", &Point3f::z )
		.def( self == self )
		.def( self != self )
		.def( self + Vector3() )
		.def( self += Vector3() )
		.def( self - self )
		.def( self - Vector3() )
		.def( self -= Vector3() )
		.def( "toVector3f", &Point3f::toVector3f )
		.def( "toPoint2f", &Point3f::toPoint2f )
		.def( "toPoint2fAxis", &Point3f::toPoint2fAxis )
		.def( "toPoint3", &Point3f::toPoint3 )
		.def( "dot", &Point3f::dot )
		.def( "sqrDistanceTo", &Point3f::sqrDistanceTo )
		.def( "distanceTo", &Point3f::distanceTo )
		.def( "get", &Point3f::get )
		.def( "getAbs", &Point3f::getAbs )
		.def( "set", &Point3f::set )
		.def( "setAbs", &Point3f::setAbs )
		.def( "flatten", &Point3f::flatten )
		.def( "cumulativeAdd", &Point3f::cumulativeAdd )
		.def( "sub", &Point3f::sub )
		.def( "scale", &Point3f::scale )
		.def( "mul", &Point3f::mul ).staticmethod( "mul" )
		.def( "sum", &Point3f::py_sum2 ).staticmethod( "sum" )
		.def( "sum3", &Point3f::py_sum3 ).staticmethod( "sum3" )
		.def( "sum4", &Point3f::py_sum4 ).staticmethod( "sum4" )
		.def( "min", &Point3f::min ).staticmethod( "min" )
		.def( "max", &Point3f::max ).staticmethod( "max" )
		.def( "average", &Point3f::py_average2 ).staticmethod( "average" )
		.def( "average3", &Point3f::py_average3 ).staticmethod( "average3" )
		.def( "average4", &Point3f::py_average4 ).staticmethod( "average4" )
		.def( "weightedAverage", &Point3f::py_weightedAverage2 ).staticmethod( "weightedAverage" )
		.def( "weightedAverage3", &Point3f::py_weightedAverage3 ).staticmethod( "weightedAverage3" )
		.def( "weightedAverage4", &Point3f::py_weightedAverage4 ).staticmethod( "weightedAverage4" )
		.def( "normalisedWeightedAverage", &Point3f::py_normalisedWeightedAverage2 ).staticmethod( "normalisedWeightedAverage" )
		.def( "normalisedWeightedAverage3", &Point3f::py_normalisedWeightedAverage3 ).staticmethod( "normalisedWeightedAverage3" )
		.def( "normalisedWeightedAverage4", &Point3f::py_normalisedWeightedAverage4 ).staticmethod( "normalisedWeightedAverage4" );
}



#endif
