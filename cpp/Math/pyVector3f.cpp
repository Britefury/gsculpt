//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVECTOR3F_CPP
#define PYVECTOR3F_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Vector3f.h>

void export_Vector3f()
{
	class_<Vector3f>( "Vector3f", init<>() )
		.def( init<float, float, float>() )
		.def( init<Vector3>() )
		.def( init<const Vector3f &>() )
		.def_readwrite( "x", &Vector3f::x )
		.def_readwrite( "y", &Vector3f::y )
		.def_readwrite( "z", &Vector3f::z )
		.def( self == self )
		.def( self != self )
		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( self * double() )
		.def( self *= double() )
		.def( -self )
		.def( "dot", &Vector3f::dot )
		.def( "cross", &Vector3f::cross )
		.def( "sqrLength", &Vector3f::sqrLength )
		.def( "length", &Vector3f::length )
		.def( "normalise", &Vector3f::normalise )
		.def( "normaliseToLength", &Vector3f::normaliseToLength )
		.def( "getNormalised", &Vector3f::getNormalised )
		.def( "getNormalisedToLength", &Vector3f::getNormalisedToLength )
		.def( "toVector3", &Vector3f::toVector3 );
}


#endif
