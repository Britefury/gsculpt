//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMESHPAINTER_CPP__
#define PYMESHPAINTER_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Mesh/MMesh/MImportMesh.h>



class PyMImportMesh : public MImportMesh
{
public:
	PyMImportMesh()
	{
	}


	void py_addFace(boost::python::list verts)
	{
		Face face;

		boost::python::object lenObject = verts.attr( "__len__" )();
		boost::python::extract<int> lenExtract( lenObject );
		if ( lenExtract.check() )
		{
			int numVerts = lenExtract;
			face.reserve( numVerts );
			for (int i = 0; i < numVerts; i++)
			{
				boost::python::object vertObj = verts[i];
				boost::python::extract<boost::python::tuple> vertExtract( vertObj );
				if ( vertExtract.check() )
				{
					boost::python::tuple vert = vertExtract;
					boost::python::object vertIndexObj = vert[0], texCoordIndexObj = vert[1];
					boost::python::extract<int> vertIndexExtract( vertIndexObj );
					boost::python::extract<int> texCoordIndexExtract( texCoordIndexObj );
					if ( vertIndexExtract.check()  &&  texCoordIndexExtract.check() )
					{
						face.push_back( FaceVertex( vertIndexExtract, texCoordIndexExtract ) );
					}
				}
			}
		}

		if ( face.size() >= 3 )
		{
			addFace( face );
		}
	}
};



void export_MImportMesh()
{
	class_<MImportMesh>( "_CPPMImportMesh", init<>() )
		.def( "addVertex", &MImportMesh::addVertex )
		.def( "addTextureCoord", &MImportMesh::addTextureCoord )
		.def( "finalise", &MImportMesh::finalise );

	class_<PyMImportMesh, bases<MImportMesh> >( "MImportMesh", init<>() )
		.def( "addFace", &PyMImportMesh::py_addFace );
}


#endif
