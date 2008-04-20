//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPROJECTION_CPP__
#define PYPROJECTION_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Projection.h>

void export_Projection()
{
	class_<Projection>( "Projection", init<>() )
		.def( init<Matrix4, Matrix4, Plane, Plane>() )
		.def( init<const Projection &>() )
		.def( "getWorldToScreenMatrix", &Projection::getWorldToScreenMatrix, return_internal_reference<>() )
		.def( "getScreenToWorldMatrix", &Projection::getScreenToWorldMatrix, return_internal_reference<>() )
		.def( "getNearClipPlane", &Projection::getNearClipPlane, return_internal_reference<>() )
		.def( "getFarClipPlane", &Projection::getFarClipPlane, return_internal_reference<>() )
		.def( "setWorldToScreenMatrix", &Projection::py_setWorldToScreenMatrix )
		.def( "setScreenToWorldMatrix", &Projection::py_setScreenToWorldMatrix )
		.def( "setNearClipPlane", &Projection::py_setNearClipPlane )
		.def( "setFarClipPlane", &Projection::py_setFarClipPlane )
		.def( "preTransformed", &Projection::preTransformed )
		.def( "transformWorldToScreen", &Projection::transformWorldToScreen )
		.def( "transformScreenToWorldScreenZ", &Projection::transformScreenToWorldScreenZ )
		.def( "computeRay", &Projection::computeRay );
}


#endif
