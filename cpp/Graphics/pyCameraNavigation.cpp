//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCAMERANAVIGATION_CPP__
#define PYCAMERANAVIGATION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/CameraNavigation.h>


void export_CameraNavigation()
{
	class_<CameraNavigation, bases<CameraRotator> >( "CameraNavigation", init<>() )
 		.def( "rotate", &CameraNavigation::rotate )
 		.def( "pan", &CameraNavigation::pan )
 		.def( "moveInZ", &CameraNavigation::moveInZ );

	class_<OrbitalCameraNavigation, bases<CameraNavigation> >( "OrbitalCameraNavigation", init<>() )
		.def( init<double>() );

	class_<FirstPersonCameraNavigation, bases<CameraNavigation> >( "FirstPersonCameraNavigation", init<>() );
}


#endif
