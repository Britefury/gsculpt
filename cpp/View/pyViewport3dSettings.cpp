//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVIEWPORT3DSETTINGS_CPP__
#define PYVIEWPORT3DSETTINGS_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <View/Viewport3dSettings.h>



void export_Viewport3dSettings()
{
	class_<Viewport3dSettings, bases<ViewportSettings> >( "Viewport3dSettings", init<ViewSettings*, Camera*>() )
		.def_readwrite( "bForegroundVertices", &Viewport3dSettings::bForegroundVertices )
		.def_readwrite( "bForegroundWireframe", &Viewport3dSettings::bForegroundWireframe )
		.def_readwrite( "bForegroundSolid", &Viewport3dSettings::bForegroundSolid )
		.def_readwrite( "bForegroundSmooth", &Viewport3dSettings::bForegroundSmooth )
		.def_readwrite( "bForegroundMaterial", &Viewport3dSettings::bForegroundMaterial )
		.def_readwrite( "bForegroundMarkedFaces", &Viewport3dSettings::bForegroundMarkedFaces )
		.def_readwrite( "bBackgroundWireframe", &Viewport3dSettings::bBackgroundWireframe )
		.def_readwrite( "bBackgroundSolid", &Viewport3dSettings::bBackgroundSolid )
		.def_readwrite( "bBackgroundSmooth", &Viewport3dSettings::bBackgroundSmooth )
		.def_readwrite( "bBackgroundMaterial", &Viewport3dSettings::bBackgroundMaterial )
		.def_readwrite( "bBackgroundTransparent", &Viewport3dSettings::bBackgroundTransparent );
}


#endif
