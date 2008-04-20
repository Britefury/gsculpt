//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCAMERAPOSITION_CPP__
#define PYCAMERAPOSITION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/CameraPosition.h>


void export_CameraPosition()
{
	class_<CameraRotator>( "CameraRotator" );

	class_<CameraPosition>( "CameraPosition", init<>() )
		.def( "front", &CameraPosition::front )
		.def( "back", &CameraPosition::back )
		.def( "top", &CameraPosition::top )
		.def( "bottom", &CameraPosition::bottom )
		.def( "left", &CameraPosition::left )
		.def( "right", &CameraPosition::right )
		.def( "origin", &CameraPosition::origin )
		.def( "viewZ0", &CameraPosition::viewZ0 )
		.def( "translate", &CameraPosition::translate )
		.def( "getForwardAxis", &CameraPosition::getForwardAxis, return_internal_reference<>() )
		.def( "getUpAxis", &CameraPosition::getUpAxis, return_internal_reference<>() )
		.def( "getRightAxis", &CameraPosition::getRightAxis, return_internal_reference<>() )
		.def( "getPosition", &CameraPosition::getCameraPosition, return_internal_reference<>() )
		.def( "getTargetPosition", &CameraPosition::getTargetPosition, return_internal_reference<>() )
		.def( "getWorldToCameraMatrix", &CameraPosition::getWorldToCameraMatrix, return_internal_reference<>() )
		.def( "getCameraToWorldMatrix", &CameraPosition::getCameraToWorldMatrix, return_internal_reference<>() );
}


#endif
