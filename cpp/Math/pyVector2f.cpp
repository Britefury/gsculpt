//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVECTOR2F_CPP
#define PYVECTOR2F_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Vector2f.h>

void export_Vector2f()
{
	class_<Vector2f>( "Vector2f", init<>() )
		.def( init<float, float>() )
		.def( init<Vector2>() )
		.def( init<const Vector2f &>() )
		.def_readwrite( "x", &Vector2f::x )
		.def_readwrite( "y", &Vector2f::y )
		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( self * float() )
		.def( self *= float() )
		.def( -self )
		.def( "dot", &Vector2f::dot )
		.def( "cross", &Vector2f::cross )
		.def( "sqrLength", &Vector2f::sqrLength )
		.def( "length", &Vector2f::length )
		.def( "normalise", &Vector2f::normalise )
		.def( "normalise", &Vector2f::normaliseToLength )
		.def( "getNormalised", &Vector2f::getNormalised )
		.def( "getNormalised", &Vector2f::getNormalisedToLength )
		.def( "toVector2", &Vector2f::toVector2 )
		.def( "getRotated90CCW", &Vector2f::getRotated90CCW )
		.def( "getRotated90CW", &Vector2f::getRotated90CW );
}


#endif
