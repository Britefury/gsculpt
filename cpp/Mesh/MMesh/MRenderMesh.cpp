//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MRENDERMESH_CPP__
#define MRENDERMESH_CPP__

#include <Mesh/MMesh/MVertex.h>

#include <Mesh/MMesh/MRenderMesh.h>



MRenderMesh::MRenderMesh()
{
}


void MRenderMesh::init(int numVertices)
{
	pointData.clear();
	positionData.clear();
	materialData.clear();
	normalData.clear();

	buildVerts.clear();
	buildVertIndices.clear();
	buildVertIndices.fill( -1, numVertices );
}


int MRenderMesh::getRenderVertexIndex(MVertex *vertex, MVertexAttrib *material, MVertexNormal *normal)
{
	int srcVertexIndex = vertex->getIndex();
	int srcMaterialIndex = material->getIndex();
	int srcNormalIndex = normal->getIndex();

	int buildVertIndex = buildVertIndices[srcVertexIndex];
	int prevBuildVertIndex = -1;

	if ( vertex->getNumVertexAttribs() == 1  &&  vertex->getNumVertexNormals() == 1 )
	{
		// This vertex has only one material entry and one normal entry; then there can only be one combination
		// of material and normal entries, so no need to search
		if ( buildVertIndex != -1 )
		{
			return buildVertIndex;
		}
	}
	else
	{
		while ( buildVertIndex != -1 )
		{
			BuildVertex &v = buildVerts[buildVertIndex];
			if ( v.matches( srcMaterialIndex, srcNormalIndex ) )
			{
				// Found a match
				return buildVertIndex;
			}

			prevBuildVertIndex = buildVertIndex;
			buildVertIndex = v.getNextIndex();
		}
	}

	// Did not find a suitable vertex; create one
	buildVertIndex = buildVerts.push_back( BuildVertex( srcMaterialIndex, srcNormalIndex ) );
	positionData.push_back( vertex->getPosition() );
	materialData.push_back( material->getPoint() );
	normalData.push_back( normal->getNormal() );
	if ( prevBuildVertIndex == -1 )
	{
		// No previous vertex in the linked list; initialise the entry for the vertex in the build vertex indices table
		buildVertIndices[srcVertexIndex] = buildVertIndex;
	}
	else
	{
		// Append to the end of the linked list for this vertex
		buildVerts[prevBuildVertIndex].link( buildVertIndex );
	}

	return buildVertIndex;
}

void MRenderMesh::addFace(const Array<int> &indices)
{
	faceData.push_back( indices );
}





#endif //MRENDERMESH_CPP__
