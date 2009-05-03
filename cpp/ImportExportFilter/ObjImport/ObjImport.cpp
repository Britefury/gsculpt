//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJIMPORT_CPP__
#define OBJIMPORT_CPP__

#include <Mesh/MMesh/MImportMesh.h>

#include <ImportExportFilter/ObjImport/LineReader.h>
#include <ImportExportFilter/ObjImport/ObjData.h>

#include <ImportExportFilter/ObjImport/ObjImport.h>



GS_DllExport MImportMesh * convertObjDataGlobalModelToImportMesh(ObjData &objData)
{
	MImportMesh *importMesh = new MImportMesh();
	Array<Point3> &vertices = importMesh->getVertexArray();
	Array<Point2f> &texCoords = importMesh->getTextureCoordArray();
	Array<MImportMesh::Face> &faces = importMesh->getFaceArray();


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


	importMesh->finalise();

	return importMesh;
}



GS_DllExport MImportMesh * convertObjModelToImportMesh(ObjData &objData, ObjModel &objModel)
{
	MImportMesh *importMesh = new MImportMesh();
	Array<Point3> &vertices = importMesh->getVertexArray();
	Array<Point2f> &texCoords = importMesh->getTextureCoordArray();
	Array<MImportMesh::Face> &faces = importMesh->getFaceArray();


	vertices.resize( objModel.numV );
	texCoords.resize( objModel.numVT );
	faces.resize( objModel.numF );

	for (int i = 0; i < objModel.numV; i++)
	{
		int x = objModel.vIndices[i];
		vertices[i] = Point3( objData.v[x].v[0],objData.v[x].v[1], objData.v[x].v[2] );
	}

	for (int i = 0; i < objModel.numVT; i++)
	{
		int x = objModel.vtIndices[i];
		texCoords[i] = Point2f( objData.vt[x].v[0],objData.vt[x].v[1] );
	}

	for (int i = 0; i < objModel.numF; i++)
	{
		ObjFace &faceIn = objModel.f[i];

		MImportMesh::Face &faceOut = faces[i];
		int faceSize = faceIn.numFV;

		faceOut.resize( faceSize );
		for (int j = 0; j < faceSize; j++)
		{
			faceOut[j] = MImportMesh::FaceVertex( faceIn.v[j].v, faceIn.v[j].vt );
		}
	}


	importMesh->finalise();

	return importMesh;
}





GS_DllExport BackgroundMesh * convertObjDataGlobalModelToBackgroundMesh(ObjData &objData, ProgressMonitor *convertMonitor, ProgressMonitor *kdtreeMonitor)
{
	Array<Point3f> vertices;
	Array<BackgroundMesh::IndexFace> faces;


	vertices.resize( objData.layout->numV );
	faces.resize( objData.layout->numF );


	for (int i = 0; i < objData.layout->numV; i++)
	{
		vertices[i] = Point3f( objData.v[i].v[0],objData.v[i].v[1], objData.v[i].v[2] );
	}

	for (int i = 0; i < objData.layout->numF; i++)
	{
		ObjFace &faceIn = objData.f[i];

		BackgroundMesh::IndexFace &faceOut = faces[i];
		int faceSize = faceIn.numFV;

		faceOut.resize( faceSize );
		for (int j = 0; j < faceSize; j++)
		{
			faceOut[j] = faceIn.v[j].v;
		}
	}

	BackgroundMesh *mesh = new BackgroundMesh( vertices, faces, convertMonitor, kdtreeMonitor );

	return mesh;
}





#endif
