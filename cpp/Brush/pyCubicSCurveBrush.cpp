//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCUBICBRUSH_CPP__
#define PYCUBICBRUSH_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Brush/CubicSCurveBrush.h>


void export_CubicSCurveBrush()
{
	class_<CubicSCurveBrush, bases<SphericalBoundaryBrush> >( "CubicSCurveBrush", init<>() )
		.def( init<double>() );
}


#endif
