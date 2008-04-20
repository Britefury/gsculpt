//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVISUALPLANE_CPP__
#define PYVISUALPLANE_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Product/GSProduct.h>

#include <Model/VisualPlane.h>



void export_VisualPlane()
{
	class_<VisualPlane>( "VisualPlane", init<>() )
		.def( init<const VisualPlane &>() )
		.def( init<const Point3 &, const Quaternion &, double, bool>() )
		.def( "_p_getPosition", &VisualPlane::getPosition, return_internal_reference<>() )
		.def( "_p_getRotation", &VisualPlane::getRotation, return_internal_reference<>() )
		.def( "_p_getSize", &VisualPlane::getSize )
		.def( "_p_getPlane", &VisualPlane::getPlane, return_internal_reference<>() )
		.def( "_p_isVisible", &VisualPlane::isVisible )
		.def( "_p_setPosition", &VisualPlane::setPosition )
		.def( "_p_setRotation", &VisualPlane::setRotation )
		.def( "_p_setSize", &VisualPlane::setSize )
		.def( "_p_setVisible", &VisualPlane::setVisible )
		.def( "draw", &VisualPlane::draw )
		.def( "paint", &VisualPlane::paint );
}


#endif
