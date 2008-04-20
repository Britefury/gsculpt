//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYEULERROTATION_CPP
#define PYEULERROTATION_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/EulerRotation.h>

void export_EulerRotation()
{
	class_<EulerRotation>( "EulerRotation", init<>() )
		.def( init<double, double, double>() )
		.def( init<Vector3>() )
		.def( init<const EulerRotation &>() )
		.def_readwrite( "x", &EulerRotation::x )
		.def_readwrite( "y", &EulerRotation::y )
		.def_readwrite( "z", &EulerRotation::z )
		.def( "toVector3", &EulerRotation::toVector3 );
}


#endif
