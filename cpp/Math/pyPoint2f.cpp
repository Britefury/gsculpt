//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOINT2F_CPP
#define PYPOINT2F_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Point2f.h>

void export_Point2f()
{
	class_<Point2f>( "Point2f", init<>() )
		.def( init<float, float>() )
		.def( init<Point2>() )
		.def( init<Vector2f>() )
		.def( init<const Point2f &>() )
		.def_readwrite( "x", &Point2f::x )
		.def_readwrite( "y", &Point2f::y )
		.def( self == self )
		.def( self != self )
		.def( self + Vector2f() )
		.def( self += Vector2f() )
		.def( self - self )
		.def( self - Vector2f() )
		.def( self -= Vector2f() )
		.def( "toVector2f", &Point2f::toVector2f )
		.def( "dot", &Point2f::dot )
		.def( "sqrDistanceTo", &Point2f::sqrDistanceTo )
		.def( "distanceTo", &Point2f::distanceTo )
		.def( "toPoint2", &Point2f::toPoint2 )
		.def( "cumulativeAdd", &Point2f::cumulativeAdd )
		.def( "sub", &Point2f::sub )
		.def( "scale", &Point2f::scale )
		.def( "mul", &Point2f::mul ).staticmethod( "mul" )
		.def( "sum", &Point2f::py_sum2 ).staticmethod( "sum" )
		.def( "sum3", &Point2f::py_sum3 ).staticmethod( "sum3" )
		.def( "sum4", &Point2f::py_sum4 ).staticmethod( "sum4" )
		.def( "min", &Point2f::min ).staticmethod( "min" )
		.def( "max", &Point2f::max ).staticmethod( "max" )
		.def( "average", &Point2f::py_average2 ).staticmethod( "average" )
		.def( "average3", &Point2f::py_average3 ).staticmethod( "average3" )
		.def( "average4", &Point2f::py_average4 ).staticmethod( "average4" )
		.def( "weightedAverage", &Point2f::py_weightedAverage2 ).staticmethod( "weightedAverage" )
		.def( "weightedAverage3", &Point2f::py_weightedAverage3 ).staticmethod( "weightedAverage3" )
		.def( "weightedAverage4", &Point2f::py_weightedAverage4 ).staticmethod( "weightedAverage4" )
		.def( "normalisedWeightedAverage", &Point2f::py_normalisedWeightedAverage2 ).staticmethod( "normalisedWeightedAverage" )
		.def( "normalisedWeightedAverage3", &Point2f::py_normalisedWeightedAverage3 ).staticmethod( "normalisedWeightedAverage3" )
		.def( "normalisedWeightedAverage4", &Point2f::py_normalisedWeightedAverage4 ).staticmethod( "normalisedWeightedAverage4" );
}


#endif
