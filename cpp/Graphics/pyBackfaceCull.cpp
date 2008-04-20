//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYBACKFACECULL_CPP__
#define PYBACKFACECULL_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/BackfaceCull.h>


void export_BackfaceCull()
{
	class_<BackfaceCull, bases<RTObject> >( "BackfaceCull", init<>() );

	class_<OrthographicBackfaceCull, bases<BackfaceCull> >( "OrthographicBackfaceCull", init<>() )
		.def( init<const Vector3 &>() )
		.def_readwrite( "_forwardVector", &OrthographicBackfaceCull::forwardVector );

	class_<PerspectiveBackfaceCull, bases<BackfaceCull> >( "PerspectiveBackfaceCull", init<>() )
		.def( init<const Point3 &>() )
		.def_readwrite( "_camPosition", &PerspectiveBackfaceCull::camPosition );
}


#endif
