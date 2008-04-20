//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYVIEWSETTINGS_CPP__
#define PYVIEWSETTINGS_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <View/ViewSettings.h>



void export_ViewSettings()
{
	class_<ViewSettings>( "ViewSettings", init<>() )
		.def_readwrite( "bBackfaceCull", &ViewSettings::bBackfaceCull )
		.def_readwrite( "bMultilayer", &ViewSettings::bMultilayer )
		.def_readwrite( "bMeshLiveSubdivisionEnabled", &ViewSettings::bMeshLiveSubdivisionEnabled )
		.def_readwrite( "meshLiveSubdivisionIterations", &ViewSettings::meshLiveSubdivisionIterations )
		.def_readwrite( "meshLiveSubdivisionMaxFaces", &ViewSettings::meshLiveSubdivisionMaxFaces )
		.def_readwrite( "meshLiveSubdivisionNormalSharpness", &ViewSettings::meshLiveSubdivisionNormalSharpness )
		.def_readwrite( "meshLiveSubdivisionWireframeMode", &ViewSettings::meshLiveSubdivisionWireframeMode );
}


#endif
