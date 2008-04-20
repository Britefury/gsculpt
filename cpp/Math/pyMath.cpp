//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMATH_CPP
#define PYMATH_CPP

#include <boost/python.hpp>
using namespace boost::python;



void export_Axes2();
void export_Axes3();
void export_Axis();
void export_BBox2();
void export_BBox3();
void export_Bezier2Util();
void export_Circle2();
void export_Colour3f();
void export_ConvexHull2();
void export_EulerRotation();
void export_Frustum();
void export_IndexTriangle();
void export_Line3();
void export_Matrix3();
void export_Matrix4();
void export_Plane();
void export_Point2();
void export_Point2f();
void export_Point2i();
void export_Point3();
void export_Point3f();
void export_Point4();
void export_Polygon2();
void export_Polygon3();
void export_Projection();
void export_Quadratic();
void export_Quaternion();
void export_Ray3();
void export_Segment2();
void export_Segment3();
void export_Side();
void export_Triangle2();
void export_Triangle3();
void export_Vector2();
void export_Vector2f();
void export_Vector2i();
void export_Vector3();
void export_Vector3f();



BOOST_PYTHON_MODULE(Math)
{
	export_Axes2();
	export_Axes3();
	export_Axis();
	export_BBox2();
	export_BBox3();
	export_Bezier2Util();
	export_Circle2();
	export_Colour3f();
	export_ConvexHull2();
	export_EulerRotation();
	export_Frustum();
	export_IndexTriangle();
	export_Line3();
	export_Matrix3();
	export_Matrix4();
	export_Plane();
	export_Point2();
	export_Point2f();
	export_Point2i();
	export_Point3();
	export_Point3f();
	export_Point4();
	export_Polygon2();
	export_Polygon3();
	export_Projection();
	export_Quadratic();
	export_Quaternion();
	export_Ray3();
	export_Segment2();
	export_Segment3();
	export_Side();
	export_Triangle2();
	export_Triangle3();
	export_Vector2();
	export_Vector2f();
	export_Vector2i();
	export_Vector3();
	export_Vector3f();
}



#endif
