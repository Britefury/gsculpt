//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYQUATERNION_CPP
#define PYQUATERNION_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Quaternion.h>

void export_Quaternion()
{
	class_<Quaternion>( "Quaternion", init<>() )
		.def( init<double, double, double, double>() )
		.def( init<const Quaternion &>() )
		.def_readwrite( "x", &Quaternion::x )
		.def_readwrite( "y", &Quaternion::y )
		.def_readwrite( "z", &Quaternion::z )
		.def_readwrite( "w", &Quaternion::w )
		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( self * self )
		.def( self *= self )
		.def( self / self )
		.def( self /= self )
		.def( "norm", &Quaternion::norm )
		.def( "modulus", &Quaternion::modulus )
		.def( "conjugate", &Quaternion::conjugate )
		.def( "inverse", &Quaternion::inverse )
		.def( "toEuler", &Quaternion::toEuler )
		.def( "rotateXSinCos", &Quaternion::py_rotateXSinCos ).staticmethod( "rotateXSinCos" )
		.def( "rotateYSinCos", &Quaternion::py_rotateYSinCos ).staticmethod( "rotateYSinCos" )
		.def( "rotateZSinCos", &Quaternion::py_rotateZSinCos ).staticmethod( "rotateZSinCos" )
		.def( "rotateX", &Quaternion::py_rotateX ).staticmethod( "rotateX" )
		.def( "rotateY", &Quaternion::py_rotateY ).staticmethod( "rotateY" )
		.def( "rotateZ", &Quaternion::py_rotateZ ).staticmethod( "rotateZ" )
		.def( "rotateAxisSinCos", &Quaternion::py_rotateAxisSinCos ).staticmethod( "rotateAxisSinCos" )
		.def( "rotateAxis", &Quaternion::py_rotateAxis ).staticmethod( "rotateAxis" )
		.def( "rotateEuler", &Quaternion::py_rotateEuler ).staticmethod( "rotateEuler" )
		.def( "rotateEulerInAxes", &Quaternion::py_rotateEulerInAxes ).staticmethod( "rotateEulerInAxes" )
		.def( "axisToAxis", &Quaternion::axisToAxis ).staticmethod( "axisToAxis" );
}


#endif
