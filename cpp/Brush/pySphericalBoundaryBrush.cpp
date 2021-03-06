//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYSPHERICALBRUSH_CPP__
#define PYSPHERICALBRUSH_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Brush/SphericalBoundaryBrush.h>


void export_SphericalBoundaryBrush()
{
	class_<SphericalBoundaryBrush, bases<Brush>, boost::noncopyable>( "SphericalBoundaryBrush", no_init )
		.add_property( "radius", &SphericalBoundaryBrush::getRadius, &SphericalBoundaryBrush::setRadius );
}


#endif
