//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYOBJIMPORT_CPP__
#define PYOBJIMPORT_CPP__

#include <string>

#include <boost/python.hpp>

using namespace boost::python;

#include <Background/Model/BackgroundMesh.h>

#include <Mesh/MMesh/MImportMesh.h>

#include <ImportExportFilter/ObjImport/LineReader.h>
#include <ImportExportFilter/ObjImport/ObjLayout.h>
#include <ImportExportFilter/ObjImport/ObjData.h>

#include <ImportExportFilter/ObjImport/ObjImport.h>





MImportMesh * py_importObjFileAsSingleMesh(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "r" );

	LineReader reader( file );
	ObjLayout layout( reader, false );
	ObjData data( &layout, reader );

	fclose( file );

	return convertObjDataGlobalModelToImportMesh( data );
}



boost::python::object py_importObjFileAsMultipleMeshes(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "r" );

	LineReader reader( file );
	ObjLayout layout( reader, true );
	ObjData data( &layout, reader );

	fclose( file );

	boost::python::list modelList;

	for (std::map<std::string, ObjModel*>::iterator iter = data.models.begin(); iter != data.models.end(); ++iter)
	{
		boost::python::str name = boost::python::str( iter->second->layout->name.c_str() );
		MImportMesh *mesh = convertObjModelToImportMesh( data, *iter->second );
		boost::python::object tuple = boost::python::make_tuple( name, mesh );
		modelList.append( tuple );
	}

	return modelList;
}




BackgroundMesh * py_importObjFileAsBackgroundMesh(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "r" );

	LineReader reader( file );
	ObjLayout layout( reader, false );
	ObjData data( &layout, reader );

	fclose( file );

	return convertObjDataGlobalModelToBackgroundMesh( data );
}



void exportObjImport()
{
	def( "importObjFileAsSingleMesh", py_importObjFileAsSingleMesh, return_value_policy<manage_new_object>() );
	def( "importObjFileAsMultipleMeshes", py_importObjFileAsMultipleMeshes );
	def( "importObjFileAsBackgroundMesh", py_importObjFileAsBackgroundMesh, return_value_policy<manage_new_object>() );
}


BOOST_PYTHON_MODULE(ObjImport)
{
	exportObjImport();	
}



#endif

