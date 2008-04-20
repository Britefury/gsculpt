//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVECTOR3_CPP
#define PYVECTOR3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Vector3.h>


void export_Vector3()
{
	class_<Vector3>( "Vector3", init<>() )
		.def( init<double, double, double>() )
		.def( init<Vector2>() )
		.def( init<Vector2, double>() )
		.def( init<Axis>() )
		.def( init<const Vector3 &>() )
		.def_readwrite( "x", &Vector3::x )
		.def_readwrite( "y", &Vector3::y )
		.def_readwrite( "z", &Vector3::z )
		.def( self == self )
		.def( self != self )
		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( self * double() )
		.def( self *= double() )
		.def( -self )
		.def( "dot", &Vector3::dot )
		.def( "cross", &Vector3::cross )
		.def( "mul", &Vector3::mul )
		.def( "sqrLength", &Vector3::sqrLength )
		.def( "length", &Vector3::length )
		.def( "normalise", &Vector3::normalise )
		.def( "normaliseToLength", &Vector3::normaliseToLength )
		.def( "getNormalised", &Vector3::getNormalised )
		.def( "getNormalisedToLength", &Vector3::getNormalisedToLength )
		.def( "projectOntoUnitVector", &Vector3::projectOntoUnitVector )
		.def( "projectOntoPlane", &Vector3::projectOntoPlane )
		.def( "getMajorAxis", &Vector3::getMajorAxis )
		.def( "getMajorSignedAxis", &Vector3::getMajorSignedAxis )
		.def( "getMinorAxis", &Vector3::getMinorAxis )
		.def( "abs", &Vector3::abs )
		.def( "perpendicularAxis", &Vector3::perpendicularAxis )
		.def( "perpendicularVector", &Vector3::perpendicularVector )
		.def( "get", &Vector3::get )
		.def( "getAbs", &Vector3::getAbs )
		.def( "set", &Vector3::set )
		.def( "setAbs", &Vector3::setAbs )
		.def( "flatten", &Vector3::flatten )
		.def( "argPolar", &Vector3::argPolar )
		.def( "toVector2", &Vector3::toVector2 )
		.def( "toVector2Axis", &Vector3::toVector2Axis )
		.def( "min", &Vector3::min ).staticmethod( "min" )
		.def( "max", &Vector3::max ).staticmethod( "max" );
}


#endif
