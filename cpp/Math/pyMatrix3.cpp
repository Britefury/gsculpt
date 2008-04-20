//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMATRIX3_CPP
#define PYMATRIX3_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Matrix3.h>


void export_Matrix3()
{
	class_<Matrix3>( "Matrix3", init<>() )
		.def( init<double, double, double, double, double, double, double, double, double>() )
		.def( init<const Vector3 &, const Vector3 &, const Vector3 &>() )
		.def( init<const Matrix3 &>() )

		.def( "__getitem__", &Matrix3::py__getitem__ )
		.def( "__setitem__", &Matrix3::py__setitem__ )

		.def( self + self )
		.def( self += self )
		.def( self - self )
		.def( self -= self )
		.def( self * double() )
		.def( self * self )
		.def( -self )

		.def( "transpose", &Matrix3::transpose )
		.def( "invert", &Matrix3::invert )
		.def( "inverse", &Matrix3::inverse )
		.def( "rigidBodyInverse", &Matrix3::rigidBodyInverse )

		.def( "scale", &Matrix3::py_scaleVector3 )
		.def( "scale", &Matrix3::py_scale )
		.def( "scale", &Matrix3::py_scale2 )
		.def( "scale", &Matrix3::py_scale3 )
		.def( "scale", &Matrix3::py_scaleAxis ).staticmethod( "scale" )

		.def( "rotateX", &Matrix3::py_rotateXSinCos )
		.def( "rotateX", &Matrix3::py_rotateX ).staticmethod( "rotateX" )
		.def( "rotateY", &Matrix3::py_rotateYSinCos )
		.def( "rotateY", &Matrix3::py_rotateY ).staticmethod( "rotateY" )
		.def( "rotateZ", &Matrix3::py_rotateZSinCos )
		.def( "rotateZ", &Matrix3::py_rotateZ ).staticmethod( "rotateZ" )
		.def( "rotate", &Matrix3::py_rotateAxis )
		.def( "rotate", &Matrix3::py_rotateAxisVector )
		.def( "rotate", &Matrix3::py_rotateQuaternion ).staticmethod( "rotate" )

		.def( "shearX", &Matrix3::shearX ).staticmethod( "shearX" )
		.def( "shearY", &Matrix3::shearY ).staticmethod( "shearY" )
		.def( "shearZ", &Matrix3::shearZ ).staticmethod( "shearZ" )
		.def( "shear", &Matrix3::shear ).staticmethod( "shear" )

		.def( "reflect", &Matrix3::reflect ).staticmethod( "reflect" )

		.def( "cross", &Matrix3::cross ).staticmethod( "cross" )

		.def( "spaceToSpace", &Matrix3::spaceToSpace ).staticmethod( "spaceToSpace" )

		.def( "axisToAxis", &Matrix3::axisToAxis ).staticmethod( "axisToAxis" )

		.def( "lookat", &Matrix3::lookat ).staticmethod( "lookat" )

		.def( "transformUnit", &Matrix3::transformUnit )

//		.def( Point3() *= self )
//		.def( Vector3() *= self )

		.def( Point3() * self )
		.def( Vector3() * self );
}


#endif
