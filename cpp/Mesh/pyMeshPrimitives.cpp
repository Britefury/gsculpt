//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMESHPRIMITIVES_CPP__
#define PYMESHPRIMITIVES_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Mesh/MeshPrimitives.h>



void export_MeshPrimitives()
{
	class_<MeshVertexList>( "MeshVertexList", init<>() )
		.def( "clear", &MeshVertexList::clear )
		.def( "setCapacityToZero", &MeshVertexList::setCapacityToZero )
		.def( "transform", &MeshVertexList::transform )
		.def( "savePositions", &MeshVertexList::savePositions )
		.def( "restorePositions", &MeshVertexList::restorePositions );

	class_<BoxPrimitive>( "BoxPrimitive", init<int, int, int, double, double, double, bool, bool, bool, bool, bool, bool>() )
		.def( "createBox", &BoxPrimitive::createBox );

	class_<PlanePrimitive>( "PlanePrimitive", init<Axis, int, int, double, double>() )
		.def( "createPlane", &PlanePrimitive::createPlane );

	class_<SpherePrimitive>( "SpherePrimitive", init<Axis, int, int, double>() )
		.def( "createSphere", &SpherePrimitive::createSphere );

	class_<CylinderPrimitive>( "CylinderPrimitive", init<Axis, int, int, int, double, double, double, bool, bool>() )
		.def( "createCylinder", &CylinderPrimitive::createCylinder );

	class_<TorusPrimitive>( "TorusPrimitive", init<Axis, int, int, double, double>() )
		.def( "createTorus", &TorusPrimitive::createTorus );

	enum_<TubePrimitive::CapMaterialMapping>( "TubePrimitiveCapMaterialMapping" )
		.value( "PLANAR", TubePrimitive::CAPMATERIALMAPPING_PLANAR )
		.value( "POLAR", TubePrimitive::CAPMATERIALMAPPING_POLAR );

	class_<TubePrimitive>( "TubePrimitive", init<Axis, int, int, int, double, double, double, bool, bool, bool, bool, TubePrimitive::CapMaterialMapping>() )
		.def( "createTube", &TubePrimitive::createTube );
}


#endif
