//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOINT3_CPP
#define PYPOINT3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Point3.h>

void export_Point3()
{
	class_<Point3>( "Point3", init<>() )
		.def( init<double, double, double>() )
		.def( init<Point2>() )
		.def( init<Point2, double>() )
		.def( init<Vector3>() )
		.def( init<const Point3 &>() )
		.def_readwrite( "x", &Point3::x )
		.def_readwrite( "y", &Point3::y )
		.def_readwrite( "z", &Point3::z )
		.def( self == self )
		.def( self != self )
		.def( self + Vector3() )
		.def( self += Vector3() )
		.def( self - self )
		.def( self - Vector3() )
		.def( self -= Vector3() )
		.def( "toVector3", &Point3::toVector3 )
		.def( "toPoint2", &Point3::toPoint2 )
		.def( "toPoint2Axis", &Point3::toPoint2Axis )
		.def( "dot", &Point3::dot )
		.def( "sqrDistanceTo", &Point3::sqrDistanceTo )
		.def( "distanceTo", &Point3::distanceTo )
		.def( "get", &Point3::get )
		.def( "getAbs", &Point3::getAbs )
		.def( "set", &Point3::set )
		.def( "setAbs", &Point3::setAbs )
		.def( "flatten", &Point3::flatten )
		.def( "cumulativeAdd", &Point3::cumulativeAdd )
		.def( "sub", &Point3::sub )
		.def( "scale", &Point3::scale )
		.def( "mul", &Point3::mul ).staticmethod( "mul" )
		.def( "sum", &Point3::py_sum2 ).staticmethod( "sum" )
		.def( "sum3", &Point3::py_sum3 ).staticmethod( "sum3" )
		.def( "sum4", &Point3::py_sum4 ).staticmethod( "sum4" )
		.def( "min", &Point3::min ).staticmethod( "min" )
		.def( "max", &Point3::max ).staticmethod( "max" )
		.def( "average", &Point3::py_average2 ).staticmethod( "average" )
		.def( "average3", &Point3::py_average3 ).staticmethod( "average3" )
		.def( "average4", &Point3::py_average4 ).staticmethod( "average4" )
		.def( "weightedAverage", &Point3::py_weightedAverage2 ).staticmethod( "weightedAverage" )
		.def( "weightedAverage3", &Point3::py_weightedAverage3 ).staticmethod( "weightedAverage3" )
		.def( "weightedAverage4", &Point3::py_weightedAverage4 ).staticmethod( "weightedAverage4" )
		.def( "normalisedWeightedAverage", &Point3::py_normalisedWeightedAverage2 ).staticmethod( "normalisedWeightedAverage" )
		.def( "normalisedWeightedAverage3", &Point3::py_normalisedWeightedAverage3 ).staticmethod( "normalisedWeightedAverage3" )
		.def( "normalisedWeightedAverage4", &Point3::py_normalisedWeightedAverage4 ).staticmethod( "normalisedWeightedAverage4" );
}


#endif
