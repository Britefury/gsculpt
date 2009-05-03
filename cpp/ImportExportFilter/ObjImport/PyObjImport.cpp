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

#include <Util/MonitoredThread.h>

#include <ImportExportFilter/ObjImport/LineReader.h>
#include <ImportExportFilter/ObjImport/ObjLayout.h>
#include <ImportExportFilter/ObjImport/ObjData.h>

#include <ImportExportFilter/ObjImport/ObjImport.h>



template <typename ResultType> class _ThreadBase : public MonitoredThread<ResultType>
{
protected:
	FILE *file;
	ThreadProgressMonitor<ResultType> scanStructureMonitor, readGeometryMonitor, buildModelsMonitor;
	
	
public:
	_ThreadBase(FILE *f) : MonitoredThread<ResultType>(), scanStructureMonitor( this, 0 ), readGeometryMonitor( this, 1 ), buildModelsMonitor( this, 2 )
	{
		file = f;
	}
};



template <typename ResultType, typename ThreadType> class _ThreadedImporter
{
private:
	FILE *file;
	_ThreadBase<ResultType> *thread;


public:
	_ThreadedImporter(const std::string &filename)
	{
		file = fopen( filename.c_str(), "r" );
		thread = new ThreadType( file );
		thread->run();
	}
	
	virtual ~_ThreadedImporter()
	{
		delete thread;
	}
	

	
	int getProgressStage()
	{
		return thread->getProgressStage();
	}

	float getProgress()
	{
		return thread->getProgress();
	}
	
	bool isFinished()
	{
		return thread->isFinished();
	}

	ResultType getResult()
	{
		thread->join();
		fclose( file );
		return thread->getResult();
	}
};





//
//
// IMPORT AS SINGLE MESH
//
//

class _ImportObjFileAsSingleMeshThread : public _ThreadBase<MImportMesh*>
{
public:
	_ImportObjFileAsSingleMeshThread(FILE *f) : _ThreadBase<MImportMesh*>( f )
	{
	}
	
	virtual MImportMesh * execute()
	{
		LineReader reader( file );
		ObjLayout layout( reader, false, &scanStructureMonitor );
		ObjData data( &layout, reader, &readGeometryMonitor, &buildModelsMonitor );
		return convertObjDataGlobalModelToImportMesh( data );
	}
};

class ImportObjFileAsSingleMeshThreaded : public _ThreadedImporter<MImportMesh*, _ImportObjFileAsSingleMeshThread>
{
public:
	ImportObjFileAsSingleMeshThreaded(const std::string &filename) : _ThreadedImporter<MImportMesh*, _ImportObjFileAsSingleMeshThread>( filename )
	{
	}
};


MImportMesh * py_importObjFileAsSingleMesh(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "r" );

	LineReader reader( file );
	ObjLayout layout( reader, false );
	ObjData data( &layout, reader );

	fclose( file );

	return convertObjDataGlobalModelToImportMesh( data );
}

ImportObjFileAsSingleMeshThreaded * py_importObjFileAsSingleMeshThreaded(std::string filename)
{
	return new ImportObjFileAsSingleMeshThreaded( filename );
}













//
//
// IMPORT AS MULTIPLE MESHES
//
//


class _ImportObjFileAsMultipleMeshesThread : public _ThreadBase<boost::python::object>
{
public:
	_ImportObjFileAsMultipleMeshesThread(FILE *f) : _ThreadBase<boost::python::object>( f )
	{
	}

	virtual boost::python::object execute()
	{
		LineReader reader( file );
		ObjLayout layout( reader, true, &scanStructureMonitor );
		ObjData data( &layout, reader, &readGeometryMonitor, &buildModelsMonitor );
		
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
};

class ImportObjFileAsMultipleMeshesThreaded : public _ThreadedImporter<boost::python::object, _ImportObjFileAsMultipleMeshesThread>
{
public:
	ImportObjFileAsMultipleMeshesThreaded(const std::string &filename) : _ThreadedImporter<boost::python::object, _ImportObjFileAsMultipleMeshesThread>( filename )
	{
	}
};


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

ImportObjFileAsMultipleMeshesThreaded * py_importObjFileAsMultipleMeshesThreaded(std::string filename)
{
	return new ImportObjFileAsMultipleMeshesThreaded( filename );
}












//
//
// IMPORT AS BACKGROUND MESH
//
//

class _ImportObjFileAsBackgroundMeshThread : public _ThreadBase<BackgroundMesh*>
{
protected:
	ThreadProgressMonitor<BackgroundMesh*> convertToBackgroundMeshMonitor, buildKDTreeMonitor;
	
public:
	
	_ImportObjFileAsBackgroundMeshThread(FILE *f) : _ThreadBase<BackgroundMesh*>( f ), convertToBackgroundMeshMonitor( this, 3 ), buildKDTreeMonitor( this, 4 )
	{
	}

	virtual BackgroundMesh * execute()
	{
		LineReader reader( file );
		ObjLayout layout( reader, false, &scanStructureMonitor );
		ObjData data( &layout, reader, &readGeometryMonitor, &buildModelsMonitor );
		return convertObjDataGlobalModelToBackgroundMesh( data, &convertToBackgroundMeshMonitor, &buildKDTreeMonitor );
	}
};

class ImportObjFileAsBackgroundMeshThreaded : public _ThreadedImporter<BackgroundMesh*, _ImportObjFileAsBackgroundMeshThread>
{
public:
	ImportObjFileAsBackgroundMeshThreaded(const std::string &filename) : _ThreadedImporter<BackgroundMesh*, _ImportObjFileAsBackgroundMeshThread>( filename )
	{
	}
};





BackgroundMesh * py_importObjFileAsBackgroundMesh(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "r" );

	LineReader reader( file );
	ObjLayout layout( reader, false );
	ObjData data( &layout, reader );

	fclose( file );

	return convertObjDataGlobalModelToBackgroundMesh( data );
}

ImportObjFileAsBackgroundMeshThreaded * py_importObjFileAsBackgroundMeshThreaded(std::string filename)
{
	return new ImportObjFileAsBackgroundMeshThreaded( filename );
}





void exportObjImport()
{
	class_<ImportObjFileAsSingleMeshThreaded>( "ImportObjFileAsSingleMeshThreaded", init<std::string>() )
			.def( "getProgress", &ImportObjFileAsSingleMeshThreaded::getProgress )
			.def( "getProgressStage", &ImportObjFileAsSingleMeshThreaded::getProgressStage )
			.def( "isFinished", &ImportObjFileAsSingleMeshThreaded::isFinished )
			.def( "getResult", &ImportObjFileAsSingleMeshThreaded::getResult, return_value_policy<manage_new_object>() );
	
	class_<ImportObjFileAsMultipleMeshesThreaded>( "ImportObjFileAsMultipleMeshesThreaded", init<std::string>() )
			.def( "getProgress", &ImportObjFileAsMultipleMeshesThreaded::getProgress )
			.def( "getProgressStage", &ImportObjFileAsMultipleMeshesThreaded::getProgressStage )
			.def( "isFinished", &ImportObjFileAsMultipleMeshesThreaded::isFinished )
			.def( "getResult", &ImportObjFileAsMultipleMeshesThreaded::getResult );
	
	class_<ImportObjFileAsBackgroundMeshThreaded>( "ImportObjFileAsBackgroundMeshThreaded", init<std::string>() )
			.def( "getProgress", &ImportObjFileAsBackgroundMeshThreaded::getProgress )
			.def( "getProgressStage", &ImportObjFileAsBackgroundMeshThreaded::getProgressStage )
			.def( "isFinished", &ImportObjFileAsBackgroundMeshThreaded::isFinished )
			.def( "getResult", &ImportObjFileAsBackgroundMeshThreaded::getResult, return_value_policy<manage_new_object>() );
	
	
	def( "importObjFileAsSingleMesh", py_importObjFileAsSingleMesh, return_value_policy<manage_new_object>() );
	def( "importObjFileAsSingleMeshThreaded", py_importObjFileAsSingleMeshThreaded, return_value_policy<manage_new_object>() );
	def( "importObjFileAsMultipleMeshes", py_importObjFileAsMultipleMeshes );
	def( "importObjFileAsMultipleMeshesThreaded", py_importObjFileAsMultipleMeshesThreaded, return_value_policy<manage_new_object>() );
	def( "importObjFileAsBackgroundMesh", py_importObjFileAsBackgroundMesh, return_value_policy<manage_new_object>() );
	def( "importObjFileAsBackgroundMeshThreaded", py_importObjFileAsBackgroundMeshThreaded, return_value_policy<manage_new_object>() );
}

BOOST_PYTHON_MODULE(ObjImport)
{
	exportObjImport();
}



#endif

