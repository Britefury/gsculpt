//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMODELLERGRID_CPP__
#define PYMODELLERGRID_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <LogGrid/ModellerGrid.h>



void export_ModellerGrid()
{
	enum_<ModellerGrid::GridPlane>( "GridPlane" )
		.value( "X", ModellerGrid::GRIDPLANE_X )
		.value( "Y", ModellerGrid::GRIDPLANE_Y )
		.value( "Z", ModellerGrid::GRIDPLANE_Z )
		.value( "AUTO", ModellerGrid::GRIDPLANE_AUTO );


	class_<ModellerGrid>( "ModellerGrid", init<>() )
		.def( "getColours", &ModellerGrid::py_getColours )
		.def( "setColours", &ModellerGrid::py_setColours )
		.def( "paint3d", &ModellerGrid::paint3d )
		.def( "worldToScreenChangedNotify", &ModellerGrid::worldToScreenChangedNotify )
		.add_property( "gridPlane", &ModellerGrid::getGridPlane, &ModellerGrid::setGridPlane )
		.add_property( "bEnabled", &ModellerGrid::isEnabled, &ModellerGrid::setEnabled );
}


#endif
