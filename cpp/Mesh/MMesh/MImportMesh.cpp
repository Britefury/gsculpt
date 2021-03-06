//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MIMPORTMESH_CPP__
#define MIMPORTMESH_CPP__

#include <Mesh/MMesh/MImportMesh.h>



MImportMesh::MImportMesh()
{
	bFinalised = false;
}


void MImportMesh::reserve(int numVertices, int numTextureCoords, int numFaces)
{
	importVertices.reserve( numVertices );
	importTextureCoords.reserve( numTextureCoords );
	importFaces.reserve( numFaces );
}

int MImportMesh::addVertex(const Point3 &position)
{
	return importVertices.push_back( position );
}

int MImportMesh::addTextureCoord(const Point2f &tex)
{
	return importTextureCoords.push_back( tex );
}

int MImportMesh::addFace(const Face &face)
{
	return importFaces.push_back( face );
}


Array<Point3> & MImportMesh::getVertexArray()
{
	return importVertices;
}

Array<Point2f> & MImportMesh::getTextureCoordArray()
{
	return importTextureCoords;
}

Array<MImportMesh::Face> & MImportMesh::getFaceArray()
{
	return importFaces;
}



void MImportMesh::finalise()
{
	// Remove any duplicate vertices from the faces
	for (int faceI = importFaces.size() - 1; faceI >= 0; faceI--)
	{
		Face &face = importFaces[faceI];

		int vertI = face.size() - 1;
		for (int vertJ = 0; vertJ < face.size(); vertJ++)
		{
			if ( face[vertI].getVertexIndex() == face[vertJ].getVertexIndex() )
			{
				face.remove( vertJ );
				vertJ--;
			}
			else
			{
				// @vertI should only advance to @vertJ, if @vertJ was NOT removed
				vertI = vertJ;
			}
		}

		// Remove any face with less than 3 vertices
		if ( face.size() < 3 )
		{
			importFaces.remove( faceI );
		}
	}

	bFinalised = true;
}


bool MImportMesh::isFinalised() const
{
	return bFinalised;
}



#endif
