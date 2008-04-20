//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMATRIX4_CPP
#define PYMATRIX4_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Matrix4.h>

void export_Matrix4()
{
	class_<Matrix4>( "Matrix4", init<>() )
		.def( init<Vector3, Vector3, Vector3>() )
		.def( init<Vector3, Vector3, Vector3, Vector3>() )
		.def( init<Point3, Vector3, Vector3, Vector3>() )
		.def( init<Point3, Point3, Point3, Point3>() )
		.def( init<Vector2, Vector2>() )
		.def( init<Vector2, Vector2, Vector2>() )
		.def( init<Point2, Vector2, Vector2>() )
		.def( init<Point2, Point2, Point2>() )
		.def( init<const Matrix4 &>() )

		.def( "__getitem__", &Matrix4::py__getitem__ )
		.def( "__setitem__", &Matrix4::py__setitem__ )

		.def( self + self )
		.def( self - self )
		.def( self * double() )
		.def( self * self )
		.def( -self )

		.def( "determinant", &Matrix4::determinant )
		.def( "inverse", &Matrix4::inverse )
		.def( "rigidBodyInverse", &Matrix4::rigidBodyInverse )

		.def( "weightedAverage", &Matrix4::weightedAverage ).staticmethod( "weightedAverage" )
		.def( "lerp", &Matrix4::lerp ).staticmethod( "lerp" )

		.def( "translate", &Matrix4::py_translateVector3 )
		.def( "translate", &Matrix4::py_translateVector2 )
		.def( "translate", &Matrix4::py_translate3 )
		.def( "translate", &Matrix4::py_translate2 ).staticmethod( "translate" )

		.def( "scale", &Matrix4::py_scaleVector3 )
		.def( "scale", &Matrix4::py_scaleVector2 )
		.def( "scale", &Matrix4::py_scale )
		.def( "scale", &Matrix4::py_scale2 )
		.def( "scale", &Matrix4::py_scale3 )
		.def( "scale", &Matrix4::py_scaleAxis3 )
		.def( "scale", &Matrix4::py_scaleAxis2 ).staticmethod( "scale" )

		.def( "rotateX", &Matrix4::py_rotateXSinCos )
		.def( "rotateX", &Matrix4::py_rotateX ).staticmethod( "rotateX" )
		.def( "rotateX90", &Matrix4::rotateX90 ).staticmethod( "rotateX90" )
		.def( "rotateX180", &Matrix4::rotateX180 ).staticmethod( "rotateX180" )
		.def( "rotateX270", &Matrix4::rotateX270 ).staticmethod( "rotateX270" )
		.def( "rotateY", &Matrix4::py_rotateYSinCos )
		.def( "rotateY", &Matrix4::py_rotateY ).staticmethod( "rotateY" )
		.def( "rotateY90", &Matrix4::rotateY90 ).staticmethod( "rotateY90" )
		.def( "rotateY180", &Matrix4::rotateY180 ).staticmethod( "rotateY180" )
		.def( "rotateY270", &Matrix4::rotateY270 ).staticmethod( "rotateY270" )
		.def( "rotateZ", &Matrix4::py_rotateZSinCos )
		.def( "rotateZ", &Matrix4::py_rotateZ ).staticmethod( "rotateZ" )
		.def( "rotateZ90", &Matrix4::rotateZ90 ).staticmethod( "rotateZ90" )
		.def( "rotateZ180", &Matrix4::rotateZ180 ).staticmethod( "rotateZ180" )
		.def( "rotateZ270", &Matrix4::rotateZ270 ).staticmethod( "rotateZ270" )
		.def( "rotate", &Matrix4::py_rotateAxis )
		.def( "rotate", &Matrix4::py_rotateAxisVector )
		.def( "rotate", &Matrix4::py_rotateEuler )
		.def( "rotate", &Matrix4::py_rotateEulerInAxes )
		.def( "rotate", &Matrix4::py_rotateQuaternion ).staticmethod( "rotate" )
		.def( "rotate90", &Matrix4::rotate90 ).staticmethod( "rotate90" )
		.def( "rotate180", &Matrix4::rotate180 ).staticmethod( "rotate180" )
		.def( "rotate270", &Matrix4::rotate270 ).staticmethod( "rotate270" )

		.def( "shearX", &Matrix4::shearX ).staticmethod( "shearX" )
		.def( "shearY", &Matrix4::shearY ).staticmethod( "shearY" )
		.def( "shearZ", &Matrix4::shearZ ).staticmethod( "shearZ" )
		.def( "shear", &Matrix4::py_shear2 )
		.def( "shear", &Matrix4::py_shear3 ).staticmethod( "shear" )

		.def( "reflect", &Matrix4::py_reflectNormal )
		.def( "reflect", &Matrix4::py_reflectPlane ).staticmethod( "reflect" )

		.def( "spaceToSpace", &Matrix4::py_spaceToSpace3 )
		.def( "spaceToSpace", &Matrix4::py_spaceToSpace3 )
		.def( "spaceToSpace", &Matrix4::py_spaceToSpaceTri3 )
		.def( "spaceToSpace", &Matrix4::py_spaceToSpaceTri3 ).staticmethod( "spaceToSpace" )

		.def( "axesMatrix", Matrix4::axesMatrix ).staticmethod( "axesMatrix" )
		.def( "axesToAxesMatrix", Matrix4::axesToAxesMatrix ).staticmethod( "axesToAxesMatrix" )

		.def( "axisToAxis", &Matrix4::py_axisToAxis )
		.def( "axisToAxis", &Matrix4::py_axisToAxisVector ).staticmethod( "axisToAxis" )

		.def( "lookat", &Matrix4::py_lookatPoints )
		.def( "lookat", &Matrix4::py_lookatDirections ).staticmethod( "lookat" )

		.def( "perspective", &Matrix4::perspective ).staticmethod( "perspective" )
		.def( "orthographic", &Matrix4::orthographic ).staticmethod( "orthographic" )
		.def( "camera", &Matrix4::camera ).staticmethod( "camera" )
		.def( "screen", &Matrix4::screen ).staticmethod( "screen" )

		.def( "transformUnit", &Matrix4::transformUnit )
		.def( "transformUnit", &Matrix4::transformUnit )

		.def( "transformHomogeneous", &Matrix4::py_transformHomogeneousPoint3 )
		.def( "transformHomogeneous", &Matrix4::py_transformHomogeneousPoint2_z )
		.def( "transformHomogeneous", &Matrix4::py_transformHomogeneousSegment )
		.def( "transformHomogeneous", &Matrix4::py_transformHomogeneousTriangle )
		.def( "transformHomogeneous2d", &Matrix4::py_transformHomogeneous2dPoint3 )
		.def( "transformHomogeneous2d", &Matrix4::py_transformHomogeneous2dSegment )
		.def( "transformHomogeneous2d", &Matrix4::py_transformHomogeneous2dTriangle )

		.def( "transformZ", &Matrix4::transformZ )
		.def( "transformZHomogeneous", &Matrix4::transformZHomogeneous )

//		.def( Point3() *= self )
//		.def( Vector3() *= self )

		.def( Point2() * self )
		.def( Point2f() * self )
		.def( Point3() * self )
		.def( Point3f() * self )
		.def( Point4() * self )
		.def( Vector2f() * self )
		.def( Vector2() * self )
		.def( Vector3f() * self )
		.def( Vector3() * self )
		.def( Plane() * self )
		.def( Segment3() * self )
		.def( Triangle3() * self )
		.def( Axes3() * self );
}


#endif
