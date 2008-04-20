//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYFRUSTUM_CPP
#define PYFRUSTUM_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Frustum.h>


void export_Frustum()
{
	class_<Frustum>( "Frustum", init<>() )
		.def( init<boost::python::list>() )
		.def( init<const Frustum &>() )
		.def( self * Matrix4() )
		.def( self *= Matrix4() )
		.def( "contains", &Frustum::py_containsPoint )
		.def( "contains", &Frustum::py_containsBox )
		.def( "clip", &Frustum::clip );

	class_<PerspectiveFrustum, bases<Frustum> >( "PerspectiveFrustum", init<double, double, double, double, bool>() );

	class_<OrthographicFrustum, bases<Frustum> >( "OrthographicFrustum", init<double, double, double, double, bool>() );
}


#endif
