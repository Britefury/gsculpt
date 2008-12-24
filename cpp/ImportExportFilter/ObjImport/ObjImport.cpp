//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJIMPORT_CPP__
#define OBJIMPORT_CPP__

#include <ImportExportFilter/ObjImport/ObjImport.h>

#include <ImportExportFilter/ObjImport/ObjData.h>

#include <Mesh/MMesh/MImportMesh.h>



static void convertObjDataGlobalModelToImportMesh(MImportMesh &importMesh, ObjData &objData)
{
	Array<Point3> &vertices = importMesh.getVertexArray();
	Array<Point2f> &texCoords = importMesh.getTextureCoordArray();
	Array<MImportMesh::Face> &faces = importMesh.getFaceArray();


	vertices.resize( objData.layout->numV );
	texCoords.resize( objData.layout->numVT );
	faces.resize( objData.layout->numF );

	for (int i = 0; i < objData.layout->numV; i++)
	{
		vertices[i] = Point3( objData.v[i].v[0],objData.v[i].v[1], objData.v[i].v[2] );
	}

	for (int i = 0; i < objData.layout->numVT; i++)
	{
		texCoords[i] = Point2f( objData.vt[i].v[0],objData.vt[i].v[1] );
	}

	for (int i = 0; i < objData.layout->numF; i++)
	{
		ObjFace &faceIn = objData.f[i];

		MImportMesh::Face &faceOut = faces[i];
		int faceSize = faceIn.numFV;

		faceOut.resize( faceSize );
		for (int j = 0; j < faceSize; j++)
		{
			faceOut[j] = MImportMesh::FaceVertex( faceIn.v[j].v, faceIn.v[j].vt );
		}
	}


	importMesh.finalise();
}




#endif
