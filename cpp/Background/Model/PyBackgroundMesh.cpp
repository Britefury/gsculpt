//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYBACKGROUNDMESH_CPP__
#define PYBACKGROUNDMESH_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Background/Model/BackgroundMesh.h>


void export_BackgroundMesh()
{
	class_<BackgroundMesh>( "BackgroundMesh", init<>() )
		.def( "readFromFile", &BackgroundMesh::readFromFile )
		.def( "writeToFile", &BackgroundMesh::writeToFile )
		.def( "initGL", &BackgroundMesh::initGL )
		.def( "shutdownGL", &BackgroundMesh::shutdownGL )
		.def( "drawGL", &BackgroundMesh::drawGL )
		.def( "raytrace", &BackgroundMesh::py_raytrace );
}



#endif
