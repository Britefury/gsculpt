//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MMESH_CPP__
#define MMESH_CPP__

#include <algorithm>
#include <sstream>
#include <list>

#include <Math/MathConstants.h>
#include <Math/lerp.h>
#include <Math/Matrix3.h>

#include <Util/Timer.h>

#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>
#include <Mesh/MMesh/MeshBuild.h>
#include <Mesh/MMesh/MRenderMesh.h>
#include <Mesh/MMesh/MImportMesh.h>
#include <Mesh/MMesh/MBBTree.h>

#include <Polyline/Polyline.h>



#define MMESH_CUT_EDGE_SPLIT_MARGIN 1.0e-6
#define MMESH_BANDSAW_EDGE_SPLIT_MARGIN 1.0e-6
#define MMESH_KNIFE_EDGE_SPLIT_MARGIN 1.0e-6







/***************************************************************
							MMesh
 ***************************************************************/


MMesh::MMesh()
{
	firstDeletedVertex = firstDeletedEdge = firstDeletedFace = -1;

	flags.bbTreeRequiresRefresh = false;
	flags.bbTreeRequiresRebuild = false;
	flags.boundingBoxRequiresRefresh = false;
	flags.liveSubdRequiresRefresh = false;
	flags.finalised = true;

	liveSubdMesh = NULL;
	liveSubdPreserveNormalSharpness = MPRESERVENORMALSHARPNESS_IFSHARP;

	markedVertexCount = markedEdgeCount = markedFaceCount = 0;

	bbTree = new MBBTree( this );
}

MMesh::MMesh(const MMesh &mesh)
{
	firstDeletedVertex = firstDeletedEdge = firstDeletedFace = -1;

	flags.bbTreeRequiresRefresh = false;
	flags.bbTreeRequiresRebuild = false;
	flags.boundingBoxRequiresRefresh = false;
	flags.liveSubdRequiresRefresh = false;
	flags.finalised = true;

	liveSubdMesh = NULL;
	liveSubdPreserveNormalSharpness = MPRESERVENORMALSHARPNESS_IFSHARP;

	markedVertexCount = markedEdgeCount = markedFaceCount = 0;

	bbTree = new MBBTree( this );


	mesh.assertFinalised();

	copyMeshDataFrom( &mesh );
}

MMesh::~MMesh()
{
	clear();
	delete bbTree;
}



MMesh & MMesh::operator=(const MMesh &mesh)
{
	//destroy any already existing mesh
	clear();

	mesh.assertFinalised();

	copyMeshDataFrom( &mesh );

	return *this;
}





MMesh * MMesh::clone() const
{
	return new MMesh( *this );
}


void MMesh::read(FILE *f)
{
	//destroy any already existing mesh
	clear();

	int numVertices, numEdges, numFaces;
	fread( &numVertices, sizeof(int), 1, f );
	fread( &numEdges, sizeof(int), 1, f );
	fread( &numFaces, sizeof(int), 1, f );

	//allocate the vertices, edges and faces
	reserveVertices( numVertices );
	for (int i = 0; i < numVertices; i++)
	{
		newVertex();
	}

	reserveEdges( numEdges );
	for (int i = 0; i < numEdges; i++)
	{
		newEdge();
	}

	reserveFaces( numFaces );
	for (int i = 0; i < numFaces; i++)
	{
		newFace();
	}


	//read the vertices, faces, and edges
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->read( f, this );
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->read( f, this );
	}

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->read( f, this );
	}

	flags.bbTreeRequiresRefresh = true;
	flags.bbTreeRequiresRebuild = true;
	flags.boundingBoxRequiresRefresh = true;
	flags.finalised = false;

	finalise();
}

void MMesh::write(FILE *f)
{
	assertFinalised();

	int numVertices = vertices.size();
	fwrite( &numVertices, sizeof(int), 1, f );
	int numEdges = edges.size();
	fwrite( &numEdges, sizeof(int), 1, f );
	int numFaces = faces.size();
	fwrite( &numFaces, sizeof(int), 1, f );

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->write( f );
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->write( f );
	}

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->write( f );
	}
}

void MMesh::read(SENode &node)
{
	//destroy any already existing mesh
	clear();


	std::string header;
	node[0]  >>  header;
	if ( header == "MMesh" )
	{
		int numVertices = 0, numEdges = 0, numFaces = 0;

		SENode &vertsNode = node[1];
		SENode &edgesNode = node[2];
		SENode &facesNode = node[3];

		//allocate the vertices, edges and faces
		vertsNode[0]  >>  numVertices;
		reserveVertices( numVertices );
		for (int i = 0; i < numVertices; i++)
		{
			newVertex();
		}

		edgesNode[0]  >>  numEdges;
		reserveEdges( numEdges );
		for (int i = 0; i < numEdges; i++)
		{
			newEdge();
		}

		facesNode[0]  >>  numFaces;
		reserveFaces( numFaces );
		for (int i = 0; i < numFaces; i++)
		{
			newFace();
		}


		//read the vertices, faces, and edges
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i]->read( vertsNode[i+1], this );
		}

		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->read( edgesNode[i+1], this );
		}

		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->read( facesNode[i+1], this );
		}

		flags.bbTreeRequiresRefresh = true;
		flags.bbTreeRequiresRebuild = true;
		flags.boundingBoxRequiresRefresh = true;
		flags.finalised = false;
	}

	finalise();
}

void MMesh::write(SENode &node)
{
	assertFinalised();

	node.addItem()  <<  "MMesh";

	SENode &vertsNode = node.addList();
	vertsNode.addItem()  <<  vertices.size();
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->write( vertsNode.addList() );
	}

	SENode &edgesNode = node.addList();
	edgesNode.addItem()  <<  edges.size();
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->write( edgesNode.addList() );
	}

	SENode &facesNode = node.addList();
	facesNode.addItem()  <<  faces.size();
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->write( facesNode.addList() );
	}
}


void MMesh::clear()
{
	destroyLiveSubdivision();

	for (int i = 0; i < vertices.size(); i++)
	{
		delete vertices[i];
	}

	for (int i = 0; i < edges.size(); i++)
	{
		delete edges[i];
	}

	for (int i = 0; i < faces.size(); i++)
	{
		delete faces[i];
	}


	//if ( bbTree != NULL )
	//{
	//	delete bbTree;
	//}

	vertices.clear();
	edges.clear();
	faces.clear();

	flags.bbTreeRequiresRefresh = false;
	flags.bbTreeRequiresRebuild = false;
	flags.boundingBoxRequiresRefresh = false;
	flags.finalised = true;


	markedVertexCount = markedEdgeCount = markedFaceCount = 0;
}


void MMesh::mergeFrom(const MMesh &mesh)
{
	mesh.assertFinalised();

	copyMeshDataFrom( &mesh );
}




void MMesh::mergeWith(const Array<const MMesh*> &meshes)
{
	Flags oldFlags = flags;

	Array<int> vertexOffsets, edgeOffsets, faceOffsets;

	vertexOffsets.resize( meshes.size() );
	edgeOffsets.resize( meshes.size() );
	faceOffsets.resize( meshes.size() );

	int numVertices = vertices.size();
	int numEdges = edges.size();
	int numFaces = faces.size();

	for (int meshI = 0; meshI < meshes.size(); meshI++)
	{
		const MMesh *mesh = meshes[meshI];
		vertexOffsets[meshI] = numVertices;
		edgeOffsets[meshI] = numEdges;
		faceOffsets[meshI] = numFaces;
		numVertices += mesh->vertices.size();
		numEdges += mesh->edges.size();
		numFaces += mesh->faces.size();
	}


	int additionalVertices = numVertices - vertices.size();
	int additionalEdges= numEdges - edges.size();
	int additionalFaces = numFaces - faces.size();

	//allocate the vertices, edges and faces
	reserveVertices( numVertices );
	for (int i = 0; i < additionalVertices; i++)
	{
		newVertex();
	}

	reserveEdges( numEdges );
	for (int i = 0; i < additionalEdges; i++)
	{
		newEdge();
	}

	reserveFaces( numFaces );
	for (int i = 0; i < additionalFaces; i++)
	{
		newFace();
	}


	//copy over the vertices, faces, and edges
	for (int meshI = 0; meshI < meshes.size(); meshI++)
	{
		const MMesh *mesh = meshes[meshI];
		int vertexOffset = vertexOffsets[meshI];
		int edgeOffset = edgeOffsets[meshI];
		int faceOffset = faceOffsets[meshI];

		for (int i = 0, j = vertexOffset; i < mesh->vertices.size(); i++, j++)
		{
			vertices[j]->copyFrom( mesh->vertices[i], this, edgeOffset, faceOffset );
		}

		for (int i = 0, j = edgeOffset; i < mesh->edges.size(); i++, j++)
		{
			edges[j]->copyFrom( mesh->edges[i], this, vertexOffset, faceOffset );
		}

		for (int i = 0, j = faceOffset; i < mesh->faces.size(); i++, j++)
		{
			faces[j]->copyFrom( mesh->faces[i], this, vertexOffset, edgeOffset );
		}

		// Bounding box will require refresh if either require a refresh, or the destination mesh is not emtpy
		flags.boundingBoxRequiresRefresh = flags.boundingBoxRequiresRefresh  ||  mesh->flags.boundingBoxRequiresRefresh;
	}


	// Restore the flags
	flags = oldFlags;


	// BOUNDING BOX TREE
	// A rebuild is required
	flags.bbTreeRequiresRebuild = true;
	flags.bbTreeRequiresRefresh = true;


	// BOUNDING BOX
	// If no refresh required, extend the bounding box to cover the source box
	if ( !flags.boundingBoxRequiresRefresh )
	{
		for (int meshI = 0; meshI < meshes.size(); meshI++)
		{
			const MMesh *mesh = meshes[meshI];
			boundingBox.addBox( mesh->boundingBox );
		}
	}


	// Destroy any existing live subdivision mesh
	destroyLiveSubdivisionMesh();
	flags.liveSubdRequiresRefresh = false;

	// No need to copy @materialRefTable, as it will be updated when the faces are copied

	// Finalise
	finalise();
}



class MMesh::MeshImportRegionManager
{
public:
	typedef MFaceList Region;
	Array<Region*> regions;
	typedef std::map<const MFace *, Region*> FaceToRegionTable;
	FaceToRegionTable faceToRegion;


	MeshImportRegionManager()
	{
	}

	~MeshImportRegionManager()
	{
		for (int regionI = 0; regionI < regions.size(); regionI++)
		{
			delete regions[regionI];
		}
	}


	Region * getRegionForFace(const MFace *face)
	{
		FaceToRegionTable::iterator iter = faceToRegion.find( face );
		gs_assert( iter != faceToRegion.end(), "MMesh::MeshImportRegionManager::getNeighbouringRegions(): could not find region for face\n" );
		return iter->second;
	}

	void addFace(MFace *face)
	{
		Region *faceRegion = NULL;
		for (int edgeI = 0; edgeI < face->getSize(); edgeI++)
		{
			MEdge *edge = face->getEdge( edgeI );
			MFace *oppositeFace = edge->getOppositeFace( face );
			if ( oppositeFace != NULL )
			{
				Region *region = getRegionForFace( oppositeFace );
				if ( faceRegion == NULL )
				{
					// Add @face to @region
					faceRegion = region;
					faceRegion->push_back( face );
					faceToRegion[face] = faceRegion;
				}
				else if ( region != faceRegion )
				{
					// @face is already a member of @faceRegion

					// Join @region with @faceRegion
					faceRegion->extend( *region );
					for (int faceI = 0; faceI < region->size(); faceI++)
					{
						MFace *f = region->at( faceI );
						faceToRegion[f] = faceRegion;
					}

					// Destroy @region
					regions.findAndRemove( region );
					delete region;
				}
			}
		}

		// No region? If so, create a region containing only @face
		if ( faceRegion == NULL )
		{
			faceRegion = new Region();
			faceRegion->push_back( face );
			faceToRegion[face] = faceRegion;
			regions.push_back( faceRegion );
		}
	}
};


void MMesh::importMesh(const MImportMesh &importMesh)
{
	clear();


	gs_assert( importMesh.isFinalised(), "MMesh::importMesh(): import mesh has not been finalised; call MImportMesh::finalise()\n" );


	Array< std::map<int, MVertexAttrib*> > vertexToAttribTable;

	// Add the vertices
	for (int vertexIndex = 0; vertexIndex < importMesh.importVertices.size(); vertexIndex++)
	{
		addVertex( importMesh.importVertices[vertexIndex] );
	}
	vertexToAttribTable.resize( importMesh.importVertices.size() );


	// Add the faces
	MeshImportRegionManager regionManager;
	for (int faceIndex = 0; faceIndex < importMesh.importFaces.size(); faceIndex++)
	{
		const MImportMesh::Face &face = importMesh.importFaces[faceIndex];

		gs_assert( face.size() >= 3, "MMesh::importMesh(): face %d has less than 3 vertices\n", faceIndex );

		int faceVertI = face.size() - 1;
		int vertexIIndex = face[faceVertI].getVertexIndex();
		MVertex *vertexI = vertices[vertexIIndex];
		bool bCanAddFace = true;
		std::set<MFaceList*> checkedRegions;
		MVertexList verts;
		verts.resize( face.size() );
		for (int faceVertJ = 0; faceVertJ < face.size(); faceVertJ++)
		{
			int vertexJIndex = face[faceVertJ].getVertexIndex();
			MVertex *vertexJ = vertices[vertexJIndex];
			verts[faceVertJ] = vertexJ;

			gs_assert( vertexI != vertexJ, "MMesh::importMesh(): face %d has duplicate neighbouring vertices\n", faceIndex );

			MEdge *edge = vertexI->findEdgeTo( vertexJ, MFINDPOLICY_RETURNNULL );
			if ( edge != NULL )
			{
				if ( edge->getNumFaces() >= 2 )
				{
					bCanAddFace = false;
					break;
				}
				else if ( edge->getNumFaces() == 1 )
				{
					MFace *otherFace = edge->getFaceA();
					MeshImportRegionManager::Region *region = regionManager.getRegionForFace( otherFace );
					if ( edge->getVertexB() != vertexI )
					{
						// Edge order is incorrect, flip the region belonging to the opposing face if possible
						if ( checkedRegions.count( region )  !=  0 )
						{
							// This region has already been encountered. The last time it was encountered it was left in such a way as the order is consistent. It is not consistent
							// with this prospective face->edge->face connection, so the face cannot be added
							bCanAddFace = false;
							break;
						}
						else
						{
							// Flip the region
							flipFaceRegion( *region );
						}
					}
					checkedRegions.insert( region );
				}
			}


			faceVertI = faceVertJ;
			vertexI = vertexJ;
		}

		if ( bCanAddFace )
		{
			// Gather the attributes
			MVertexAttribList attribs;
			attribs.resize( face.size() );
			for (int faceVertI = 0; faceVertI < face.size(); faceVertI++)
			{
				int vtxIndex = face[faceVertI].getVertexIndex();
				int texIndex = face[faceVertI].getTextureCoordIndex();

				std::map<int, MVertexAttrib*>::iterator iter = vertexToAttribTable[vtxIndex].find( texIndex );
				MVertexAttrib *attrib = NULL;
				if ( iter == vertexToAttribTable[vtxIndex].end() )
				{
					if ( texIndex != -1 )
					{
						attrib = vertices[vtxIndex]->createVertexAttrib( MVertexAttrib( importMesh.importTextureCoords[texIndex], -1 ), MFINDPOLICY_FORCEDCREATE );
					}
					else
					{
						attrib = vertices[vtxIndex]->createVertexAttrib( MVertexAttrib( Point2f(), -1 ), MFINDPOLICY_FORCEDCREATE );
					}
				}
				else
				{
					attrib = iter->second;
				}
				attribs[faceVertI] = attrib;
			}

			// Add the face
			MFace *meshFace = addFace( verts, attribs, -1 );

			MFaceList finalFaces;
			meshFace->importMeshSanityCheck( finalFaces );

			for (int finalFaceI = 0; finalFaceI < finalFaces.size(); finalFaceI++)
			{
				regionManager.addFace( finalFaces[finalFaceI] );
			}
		}
	}

	// Delete isolated vertices
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->destroyIfIsolated();
	}

	compactAll();
	finalise();
}




void MMesh::preComputeFull()
{
	assertFinalised();

	// Build all data for vertices, edges, and faces
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->preComputeFull();
	}

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		edges[edgeI]->preComputeFull();
	}

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		faces[faceI]->preComputeFull();
	}


	// Refresh bounding box
	getBoundingBox();

	// Refresh the bounding box tree
	getBBTree();
}




void MMesh::reserveVertices(int n)
{
	vertices.reserve( n );
	if ( n > vertices.size() )
	{
		int extraSpaces = n - vertices.size();
		MVertex::poolAllocReserve( extraSpaces );
		MVertexAttrib::poolAllocReserve( extraSpaces );
		MVertexNormal::poolAllocReserve( extraSpaces );
	}
}

void MMesh::reserveEdges(int n)
{
	edges.reserve( n );
	if ( n > edges.size() )
	{
		int extraSpaces = n - edges.size();
		MEdge::poolAllocReserve( extraSpaces );
	}
}

void MMesh::reserveFaces(int n)
{
	faces.reserve( n );
	if ( n > faces.size() )
	{
		int extraSpaces = n - faces.size();
		MFace::poolAllocReserve( extraSpaces );
	}
}

void MMesh::reserveFacesAndEdges(int nFaces)
{
	reserveFaces( nFaces );
	reserveEdges( nFaces * 2 );
}




void MMesh::copyMeshDataFrom(const MMesh *mesh)
{
	Flags oldFlags = flags;

	int verticesOffset = vertices.size();
	int edgesOffset = edges.size();
	int facesOffset = faces.size();

	bool bDestEmpty = verticesOffset == 0  &&  edgesOffset == 0  &&  facesOffset == 0;

	//allocate the vertices, edges and faces
	reserveVertices( verticesOffset + mesh->vertices.size() );
	for (int i = 0; i < mesh->vertices.size(); i++)
	{
		newVertex();
	}

	reserveEdges( edgesOffset + mesh->edges.size() );
	for (int i = 0; i < mesh->edges.size(); i++)
	{
		newEdge();
	}

	reserveFaces( facesOffset + mesh->faces.size() );
	for (int i = 0; i < mesh->faces.size(); i++)
	{
		newFace();
	}


	//copy over the vertices, faces, and edges
	for (int i = 0, j = verticesOffset; i < mesh->vertices.size(); i++, j++)
	{
		vertices[j]->copyFrom( mesh->vertices[i], this, edgesOffset, facesOffset );
	}

	for (int i = 0, j = edgesOffset; i < mesh->edges.size(); i++, j++)
	{
		edges[j]->copyFrom( mesh->edges[i], this, verticesOffset, facesOffset );
	}

	for (int i = 0, j = facesOffset; i < mesh->faces.size(); i++, j++)
	{
		faces[j]->copyFrom( mesh->faces[i], this, verticesOffset, edgesOffset );
	}


	// Restore the flags
	flags = oldFlags;


	// BOUNDING BOX TREE
	// If the bounding box tree from the source mesh does *not* require rebuilding, and @this (the desination mesh) is empty, then simply copy it over.
	if ( !mesh->flags.bbTreeRequiresRebuild  &&  bDestEmpty )
	{
		// Copy the bounding box tree
		bbTree->copyFrom( mesh->bbTree );

		// A rebuild is not necessary
		flags.bbTreeRequiresRebuild = false;

		// A refresh will be necessary if the source mesh requires a refresh
		flags.bbTreeRequiresRefresh = mesh->flags.bbTreeRequiresRefresh;
	}
	else
	{
		// A rebuild is required
		flags.bbTreeRequiresRebuild = true;
		flags.bbTreeRequiresRefresh = true;
	}


	// BOUNDING BOX
	// Bounding box will require refresh if either require a refresh, or the destination mesh is not emtpy
	flags.boundingBoxRequiresRefresh = flags.boundingBoxRequiresRefresh  ||  mesh->flags.boundingBoxRequiresRefresh;
	// If no refresh required, extend the bounding box to cover the source box
	if ( !flags.boundingBoxRequiresRefresh )
	{
		boundingBox.addBox( mesh->boundingBox );
	}


	// Result mesh is finalised if both source and destination meshes are finalised, and the destination mesh is empty
	flags.finalised = flags.finalised  &&  mesh->flags.finalised  &&  bDestEmpty;

	// Destroy any existing live subdivision mesh
	destroyLiveSubdivisionMesh();
	flags.liveSubdRequiresRefresh = false;

	// No need to copy @materialRefTable, as it will be updated when the faces are copied

	// Finalise
	finalise();
}


void MMesh::optimiseMemoryUsage()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i]->isDestroyed() )
		{
			vertices[i]->optimiseMemoryUsage();
		}
	}

	for (int i = 0; i < faces.size(); i++)
	{
		if ( !faces[i]->isDestroyed() )
		{
			faces[i]->optimiseMemoryUsage();
		}
	}
}







void MMesh::vertexModified()
{
	flags.bbTreeRequiresRefresh = true;
	flags.boundingBoxRequiresRefresh = true;
	flags.liveSubdRequiresRefresh = true;
	flags.finalised = false;
}

void MMesh::clearVertexPositionChangedForMeshFlags()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i]->isDestroyed() )
		{
			vertices[i]->clearPositionChangedForMeshFlag();
		}
	}

}


void MMesh::faceModified(MFace *face)
{
	flags.bbTreeRequiresRefresh = true;
	flags.finalised = false;
	bbTree->faceModified( face->getIndex() );
}

void MMesh::topologyModified()
{
	destroyLiveSubdivisionMesh();
	flags.bbTreeRequiresRebuild = true;
	flags.bbTreeRequiresRefresh = true;
	flags.finalised = false;
	bbTree->topologyModified();
}

void MMesh::destroyLiveSubdivisionMesh()
{
	if ( liveSubdMesh != NULL )
	{
		delete liveSubdMesh;
		liveSubdMesh = NULL;
	}
}

void MMesh::edgeNormalSharpnessChanged()
{
	flags.liveSubdRequiresRefresh = true;
	flags.finalised = false;
}

void MMesh::vertexNormalSharpnessChanged()
{
	flags.liveSubdRequiresRefresh = true;
	flags.finalised = false;
}

void MMesh::edgeSharpnessChanged()
{
	flags.liveSubdRequiresRefresh = true;
	flags.finalised = false;
}

void MMesh::vertexSharpnessChanged()
{
	flags.liveSubdRequiresRefresh = true;
	flags.finalised = false;
}



MVertex * MMesh::newVertex()
{
	int index = vertices.size();

	MVertex *vertex = new MVertex( this, index );
	vertices.push_back( vertex );

	flags.bbTreeRequiresRefresh = true;
	flags.boundingBoxRequiresRefresh = true;
	flags.finalised = false;

	return vertex;
}

MVertexAttrib * MMesh::newVertexAttrib(int index)
{
	return new MVertexAttrib( index );
}

MVertexNormal * MMesh::newVertexNormal(int index)
{
	return new MVertexNormal( index );
}

MEdge * MMesh::newEdge()
{
	int index = edges.size();

	MEdge *edge = new MEdge( index );
	edges.push_back( edge );

	flags.finalised = false;

	return edge;
}

MFace * MMesh::newFace()
{
	int index = faces.size();

	MFace *face = new MFace( this, index );
	faces.push_back( face );

	flags.bbTreeRequiresRefresh = true;
	flags.bbTreeRequiresRebuild = true;
	flags.finalised = false;

	return face;
}




//************************** MESH BUILD OPERATIONS **************************
//***************************************************************************

MVertex * MMesh::addVertex(const Point3 &p)
{
	topologyModified();

	MVertex *vertex = newVertex();

	vertex->init( p );
	return vertex;
}

MVertexAttrib * MMesh::addVertexAttrib(const MVertexAttrib &va, int index)
{
	MVertexAttrib *attrib = newVertexAttrib( index );

	attrib->init( va );
	return attrib;
}

MEdge * MMesh::addEdge(MVertex *va, MVertex *vb)
{
	MEdge *edge = newEdge();

	edge->init( va, vb );
	return edge;
}

MFace * MMesh::addFace(const Array<int> &vts, const Array<Point2f> &texCoords, int materialID)
{
	Array<MVertexAttrib> attribs;
	attribs.reserve( texCoords.size() );
	for (int i = 0; i < texCoords.size(); i++)
	{
		attribs.push_back( MVertexAttrib( texCoords[i], materialID ) );
	}

	return addFace( vts, attribs, materialID );
}

MFace * MMesh::addFace(const Array<int> &vts, const Array<MVertexAttrib> &attribs, int materialID)
{
	Array<MVertex*> vertexPointers;
	vertexPointers.reserve( vts.size() );
	for (int i = 0; i < vts.size(); i++)
	{
		vertexPointers.push_back( vertices[ vts[i] ] );
	}

	return addFace( vertexPointers, attribs, materialID );
}

MFace * MMesh::addFace(const Array<MVertex*> &vts, const Array<Point2f> &texCoords, int materialID)
{
	Array<MVertexAttrib> attribs;
	attribs.reserve( texCoords.size() );
	for (int i = 0; i < texCoords.size(); i++)
	{
		attribs.push_back( MVertexAttrib( texCoords[i], materialID ) );
	}

	return addFace( vts, attribs, materialID );
}

MFace * MMesh::addFace(const Array<MVertex*> &vts, const Array<MVertexAttrib> &attribs, int materialID)
{
	MFace *face = newFace();

	face->init( vts, attribs, materialID );
	return face;
}

MFace * MMesh::addFaceAutoTexture(const Array<int> &vts, bool flipFlag, int materialID)
{
	Array<MVertex*> vertexPointers;
	vertexPointers.reserve( vts.size() );
	for (int i = 0; i < vts.size(); i++)
	{
		vertexPointers.push_back( vertices[ vts[i] ] );
	}

	return addFaceAutoTexture( vertexPointers, flipFlag, materialID );
}

MFace * MMesh::addFaceAutoTexture(const Array<MVertex*> &vts, bool flipFlag, int materialID)
{
	Array<Point3> vertexPoints;
	vertexPoints.reserve( vts.size() );
	for (int i = 0; i < vts.size(); i++)
	{
		vertexPoints.push_back( vts[i]->getPosition() );
	}

	//compute the plane that most closely matches that of the face
	Plane facePlane( vertexPoints );
	//get the axis that most closely represents the plane normal
	Axis normalAxis = facePlane.n.getMajorAxis();

	//boxes for computing a scale factor so that the texture co-ordinates
	//can be computed
	BBox3 box( vertexPoints );						//3d bbox containing the points
	BBox2 box2 = box.toBBox2( normalAxis );	//convert to 2d

	//compute the scale factors
	double uScale = 1.0 / box2.getWidth();
	double vScale = 1.0 / box2.getHeight();


	//compute the texture co-ordinates for the points, and store them in an Array
	//of vertex attributes
	Array<MVertexAttrib> vertexAttribs;
	vertexAttribs.reserve( vts.size() );
	for (int i = 0; i < vts.size(); i++)
	{
		Vector2 point = vertexPoints[i].toPoint2Axis( normalAxis ) - box2.getLower();
		point.x *= uScale;
		point.y *= vScale;
		Point2f tex( point.x, point.y );
		vertexAttribs.push_back( MVertexAttrib( tex, materialID ) );
	}

	//flip if necessary
	if ( flipFlag )
	{
		Array<MVertex*> faceVertices = vts;
		faceVertices.reverse();
		vertexAttribs.reverse();
		return addFace( faceVertices, vertexAttribs, materialID );
	}
	else
	{
		return addFace( vts, vertexAttribs, materialID );
	}
}

MFace * MMesh::addFace(const Array<MVertex*> &vts, const Array<MVertexAttrib*> &attribs, int materialID)
{
	MFace *face = newFace();

	face->init( vts, attribs, materialID );
	return face;
}

MFace * MMesh::addFace(const Array<MVertex*> &vts, const Array<MEdge*> &eds, const Array<MVertexAttrib*> &attribs, int materialID)
{
	MFace *face = newFace();

	face->init( vts, eds, attribs, materialID );
	return face;
}





void MMesh::getPoints(Array<Point3> &points) const
{
	points.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		points.push_back( vertices[i]->getPosition() );
	}
}


int MMesh::estimateCost() const
{
	int cost = sizeof(MMesh);

	cost += vertices.computeMemoryUsage();
	cost += edges.computeMemoryUsage();
	cost += faces.computeMemoryUsage();

	//if ( bbTree != NULL )
	//{
	//	cost += bbTree->estimateCost();
	//}
	cost += materialRefTable.estimateContentCost();

	if ( liveSubdMesh != NULL )
	{
		cost += liveSubdMesh->estimateCost();
	}

	cost += MVertex::estimateCost() * vertices.size();
	cost += MEdge::estimateCost() * edges.size();
	cost += MFace::estimateCost() * faces.size();

	return cost;
}



void MMesh::compactVertices()
{
	if ( firstDeletedVertex != -1 )
	{
		int i, j;

		i = firstDeletedVertex;

		for (j = firstDeletedVertex; j < vertices.size(); j++)
		{
			MVertex *src = vertices[j];

			if ( src->isDestroyed() )
			{
				delete src;
			}
			else
			{
				vertices[i] = src;
				src->index = i;
				i++;
			}
		}

		vertices.resize( i );
	}

	firstDeletedVertex = -1;
}

void MMesh::compactEdges()
{
	if ( firstDeletedEdge != -1 )
	{
		int i, j;

		i = firstDeletedEdge;

		for (j = firstDeletedEdge; j < edges.size(); j++)
		{
			MEdge *src = edges[j];

			if ( src->isDestroyed() )
			{
				delete src;
			}
			else
			{
				edges[i] = src;
				src->index = i;
				i++;
			}
		}

		edges.resize( i );
	}

	firstDeletedEdge = -1;
}

void MMesh::compactFaces()
{
	if ( firstDeletedFace != -1 )
	{
		int i, j;

		i = firstDeletedFace;

		for (j = firstDeletedFace; j < faces.size(); j++)
		{
			MFace *src = faces[j];

			if ( src->isDestroyed() )
			{
				delete src;
			}
			else
			{
				faces[i] = src;
				src->index = i;
				i++;
			}
		}

		faces.resize( i );
	}

	firstDeletedFace = -1;
}

void MMesh::compactAll()
{
	compactVertices();
	compactEdges();
	compactFaces();
	optimiseMemoryUsage();
}


void MMesh::destroyVertex(MVertex *v)
{
	if ( !v->isDestroyed() )
	{
		if ( v->isVertexMarked() )
		{
			decrementMarkedVertexCount();
		}

		v->markAsDestroyed();
		if ( firstDeletedVertex == -1 )
		{
			firstDeletedVertex = v->index;
		}
		else
		{
			firstDeletedVertex = std::min( firstDeletedVertex, v->index );
		}
	}
}

void MMesh::destroyEdge(MEdge *e)
{
	if ( !e->isDestroyed() )
	{
		if ( e->isEdgeMarked() )
		{
			decrementMarkedEdgeCount();
		}

		e->markAsDestroyed();
		if ( firstDeletedEdge == -1 )
		{
			firstDeletedEdge = e->index;
		}
		else
		{
			firstDeletedEdge = std::min( firstDeletedEdge, e->index );
		}
	}
}

void MMesh::destroyFace(MFace *f)
{
	if ( !f->isDestroyed() )
	{
		if ( f->isFaceMarked() )
		{
			decrementMarkedFaceCount();
		}

		f->markAsDestroyed();
		if ( firstDeletedFace == -1 )
		{
			firstDeletedFace = f->index;
		}
		else
		{
			firstDeletedFace = std::min( firstDeletedFace, f->index );
		}
	}
	flags.bbTreeRequiresRebuild = true;
}

void MMesh::destroyVertexAttrib(MVertexAttrib *va)
{
	delete va;
}

void MMesh::destroyVertexNormal(MVertexNormal *n)
{
	delete n;
}






void MMesh::discoverMarkedEdgeRuns(Array<MEdgeRun> &runs)
{
	runs.clear();

	//start at vertices where incident marked edges != 2
	//eg: find the open runs first
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i];

		int markedEdgeCount = v->getMarkedEdgeCount();

		if ( markedEdgeCount != 2 )
		{
			MEdge *e = v->getMarkedEdgeSecondaryUnmarked();

			while ( e != NULL )
			{
				MEdgeRun &run = runs.push_back();

				MVertex *lastVertex;
				e->generateMarkedEdgeRun( v, run, lastVertex );

				e = v->getMarkedEdgeSecondaryUnmarked();
			}
		}
	}

	//start at vertices where incident marked (and not visited) edges == 2
	//eg: find the closed runs
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i];

		int markedEdgeCount = v->getMarkedEdgeCount();

		if ( markedEdgeCount == 2 )
		{
			MEdge *e = v->getMarkedEdgeSecondaryUnmarked();

			if ( e != NULL )
			{
				MEdgeRun &run = runs.push_back();

				MVertex *lastVertex;
				e->generateMarkedEdgeRun( v, run, lastVertex );
			}
		}
	}


	//secondary unmark all edges
	for (int i = 0; i < runs.size(); i++)
	{
		for (int j = 0; j < runs[i].size(); j++)
		{
			runs[i][j]->secondaryUnmark();
		}
	}
}

void MMesh::discoverMarkedEdgeRegions(Array<MEdgeList> &regions)
{
	regions.clear();

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  !edges[i]->isDestroyed()  &&  !edges[i]->isSecondaryMarked() )
		{
			MEdgeList &region = regions.push_back();

			MEdge *e = edges[i];

			e->getEdgeMarkedRegion( region );
		}
	}


	for (int i = 0; i < regions.size(); i++)
	{
		for (int j = 0; j < regions[i].size(); j++)
		{
			regions[i][j]->secondaryUnmark();
		}
	}
}




void MMesh::discoverMarkedFaces(MFaceList &markedFaces)
{
	markedFaces.clear();
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() && !faces[i]->isDestroyed() )
		{
			markedFaces.push_back( faces[i] );
		}
	}
}

void MMesh::discoverMarkedFaceRegionBoundaries(MFaceList &region, Array<MEdgeRun> &boundaries)
{
	boundaries.clear();

	for (int i = 0; i < region.size(); i++)
	{
		MEdge *e = region[i]->getFaceMarkBoundaryEdge();

		if ( e != NULL )
		{
			if ( !e->isSecondaryMarked() )
			{
				MEdgeRun &boundary = boundaries.push_back();

				e->getFaceMarkBoundaryEdges( boundary );
			}
		}
	}

	//NOTE:
	//it would initially appear that under some circumstances that
	//the for loop would need to be restarted, so that the edges
	//can be searched again, since their state has changed.
	//this is not necessary, since the change in state will only
	//reduce the number of edges that will pass through the nested
	//if statements.
	//Therefore, if the first edge to pass was at index i,
	//even after the state changes, no edges before i will satisfy
	//the conditions in the if statements

	for (int i = 0; i < boundaries.size(); i++)
	{
		for (int j = 0; j < boundaries[i].size(); j++)
		{
			boundaries[i][j]->secondaryUnmark();
		}
	}
}


void MMesh::discoverMarkedFaceRegions(Array<MFaceList> &regions)
{
	regions.clear();

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  &&  !faces[i]->isDestroyed()  &&  !faces[i]->isSecondaryMarked() )
		{
			MFaceList &region = regions.push_back();

			MFace *f = faces[i];

			f->getFaceMarkedRegion( region );
		}
	}


	//NOTE:
	//it would initially appear that under some circumstances that....
	//see above

	for (int i = 0; i < regions.size(); i++)
	{
		for (int j = 0; j < regions[i].size(); j++)
		{
			regions[i][j]->secondaryUnmark();
		}
	}
}

int MMesh::countMarkedFaceRegions()
{
	int regions = 0;

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  &&  !faces[i]->isDestroyed()  &&  !faces[i]->isSecondaryMarked() )
		{
			regions++;

			MFace *f = faces[i];
			f->visitFaceMarkedRegion();
		}
	}


	//NOTE:
	//it would initially appear that under some circumstances that....
	//see above

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->secondaryUnmark();
	}

	return regions;
}

void MMesh::discoverMarkedFaceRegionsAndBoundaries(MarkedFaceRegionList &regions)
{
	regions.clear();

	Array<MFace*> markedFaces;
	discoverMarkedFaces( markedFaces );

	for (int i = 0; i < markedFaces.size(); i++)
	{
		if ( !markedFaces[i]->isSecondaryMarked() )
		{
			MarkedFaceRegion &region = regions.push_back();

			MFace *f = markedFaces[i];

			f->getFaceMarkedRegion( region.faces );


			discoverMarkedFaceRegionBoundaries( region.faces, region.boundaries );
		}
	}


	//NOTE:
	//it would initially appear that under some circumstances that....
	//see above

	for (int i = 0; i < regions.size(); i++)
	{
		for (int j = 0; j < regions[i].faces.size(); j++)
		{
			regions[i].faces[j]->secondaryUnmark();
		}
	}
}

void MMesh::discoverMarkedFaceBoundaries(Array<MEdgeRun> &boundaries)
{
	boundaries.clear();

	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *e = edges[i];

		if ( e->isFaceMarkBoundary()  &&  !e->isSecondaryMarked() )
		{
			MEdgeRun &boundary = boundaries.push_back();

			e->getFaceMarkBoundaryEdges( boundary );
		}
	}

	//NOTE:
	//it would initially appear that under some circumstances that....
	//see above

	for (int i = 0; i < boundaries.size(); i++)
	{
		for (int j = 0; j < boundaries[i].size(); j++)
		{
			boundaries[i][j]->secondaryUnmark();
		}
	}
}




//************************** MEDH EDIT OPERATIONS **************************
//**************************************************************************

void MMesh::removeMarkedFaces()
{
	assertFinalised();

	//destroy each marked face
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->destroy( true, true );
		}
	}

	//compact the lists
	compactAll();

	finalise();
}





MMesh* MMesh::getLiveSubdivisionMesh(int iterations, int maxFaces, MPreserveNormalSharpness preserveNormalSharpness)
{
	if ( iterations == 0  ||  faces.size() > maxFaces )
	{
		//destroy any extra live subdivision levels
		destroyLiveSubdivision();
		return this;
	}
	else
	{
		if ( preserveNormalSharpness != liveSubdPreserveNormalSharpness )
		{
			destroyLiveSubdivision();
		}

		if ( liveSubdMesh == NULL )
		{
			subdivideLive( preserveNormalSharpness );
			liveSubdivisionRefreshed();
		}
		else if ( flags.liveSubdRequiresRefresh )
		{
			refreshLiveSubdivision();
		}

		return liveSubdMesh->getLiveSubdivisionMesh( iterations - 1, maxFaces, preserveNormalSharpness );
	}
}


void MMesh::destroyLiveSubdivision()
{
	if ( liveSubdMesh != NULL )
	{
		delete liveSubdMesh;
		liveSubdMesh = NULL;
	}
}





void MMesh::liveSubdivisionRefreshed()
{
	flags.liveSubdRequiresRefresh = false;

	clearVertexPositionChangedForMeshFlags();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->liveSubdivisionRefreshed();
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->liveSubdivisionRefreshed();
	}

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->liveSubdivisionRefreshed();
	}
}





void MMesh::subdivideLive(MPreserveNormalSharpness preserveNormalSharpness)
{
	assertFinalised();

	if ( liveSubdMesh != NULL )
	{
		delete liveSubdMesh;
	}

	liveSubdMesh = new MMesh();


	liveSubdMesh->vertices.reserve( vertices.size()  +  edges.size() * 2  +  faces.size() * 4 );
	//copy even vertices over
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = liveSubdMesh->addVertex( vertices[i]->getPosition() );
		//create single attribute
		v->createVertexAttrib( MVertexAttrib( Point2f(), -1 ), MFINDPOLICY_FORCEDCREATE );
	}

	liveSubdFaceVerticesOffset = liveSubdMesh->vertices.size();

	//create face subdivision vertices
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->createLiveSubdivisionVertex( liveSubdMesh );
	}

	liveSubdEdgeVerticesOffset = liveSubdMesh->vertices.size();


	//create edge subdivision vertices
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->createLiveSubdivisionVertex( liveSubdMesh, liveSubdFaceVerticesOffset );
	}


	//reposition the vertices
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->liveSubdivideReposition( liveSubdMesh, liveSubdEdgeVerticesOffset, liveSubdFaceVerticesOffset, preserveNormalSharpness );
	}


	//process faces
	for (int i = 0; i < faces.size(); i++)
	{
		//subdivide the face
		faces[i]->createLiveSubdivisionFaces( liveSubdMesh, liveSubdEdgeVerticesOffset, liveSubdFaceVerticesOffset, preserveNormalSharpness );
	}


	//copy vertex sharpness from *this, and decrease
	for (int i = 0; i < vertices.size(); i++)
	{
		double s = vertices[i]->getSharpness();
		liveSubdMesh->vertices[i]->setSharpness( s >= 1.0  ?  s - 1.0  :  0.0 );
	}

	liveSubdPreserveNormalSharpness = preserveNormalSharpness;

	liveSubdMesh->compactAll();
	liveSubdMesh->finalise();
}

void MMesh::refreshLiveSubdivision()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->liveSubdivisionRefreshInit();
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->liveSubdivisionRefreshInit();
	}


	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->refreshLiveSubdivisionVertex( liveSubdMesh, liveSubdFaceVerticesOffset );
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->refreshLiveSubdivisionVertex( liveSubdMesh, liveSubdEdgeVerticesOffset, liveSubdFaceVerticesOffset );
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->refreshLiveSubdivision( liveSubdMesh, liveSubdEdgeVerticesOffset, liveSubdFaceVerticesOffset, liveSubdPreserveNormalSharpness );
	}

	liveSubdMesh->finalise();

	clearVertexPositionChangedForMeshFlags();

	flags.liveSubdRequiresRefresh = false;
}




//mesh subdivision
void MMesh::subdivide(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness)
{
	assertFinalised();

	//space for copies of the original edge and face lists
	//NOTE: the data is not placed in the list now, it is done later,
	//along with other tasks to improve cache coherency
	Array<MEdge*> oldEdges;
	Array<MFace*> oldFaces;
	//allocate
	oldEdges.reserve( edges.size() );
	oldFaces.reserve( faces.size() );

	int numControlVertices = vertices.size();


	if ( !catmullClark )
	{
		preserveNormalSharpness = MPRESERVENORMALSHARPNESS_PRESERVE;
	}


	//create the new vertex at the centre of each face,
	//and fill the original face list
	for (int i = 0; i < faces.size(); i++)
	{
		MFace *f = faces[i];
		f->createSubdivisionVertex();
		oldFaces.push_back( f );
	}

	//create a subidivision vertex for each edge,
	//and fill the original edge list
	if ( catmullClark )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			MEdge *e = edges[i];
			e->createCatmullClarkSubdivisionVertex();
			oldEdges.push_back( e );
		}
	}
	else
	{
		for (int i = 0; i < edges.size(); i++)
		{
			MEdge *e = edges[i];
			e->createLinearSubdivisionVertex();
			oldEdges.push_back( e );
		}
	}

	//if using smooth Catmull Clark subdivision
	if ( catmullClark )
	{
		//need to save the positions of all vertices first
		for (int i = 0; i < numControlVertices; i++)
		{
			vertices[i]->savePosition();
		}

		//reposition the vertices
		for (int i = 0; i < numControlVertices; i++)
		{
			MVertex *v = vertices[i];
			//reposition
			v->computeNewPositionCatmullClark( preserveNormalSharpness );

			//clear the links from v to the incident edges and faces
			v->clearLinks();
		}
	}
	else
	{
		//clear the links from the vertices to the incident edges and faces
		for (int i = 0; i < numControlVertices; i++)
		{
			vertices[i]->clearLinks();
		}
	}


	//clear the edge and face lists
	edges.clear();
	faces.clear();

	markedEdgeCount = 0;
	markedFaceCount = 0;



	//process faces
	for (int a = 0; a < oldFaces.size(); a++)
	{
		MFace *f = oldFaces[a];

		//subdivide the face
		f->createSubdivisionFaces( catmullClark, preserveNormalSharpness );

		//unref the vertex attributes used by f
		for (int j = 0; j < f->getSize(); j++)
		{
			f->getVertexAttrib( j )->unref();
		}

		//destroy f
		delete f;
	}


	//destroy all of the original edges
	for (int i = 0; i < oldEdges.size(); i++)
	{
		delete oldEdges[i];
	}


	//handle edge and vertex sharpness
	if ( catmullClark )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			double s = vertices[i]->getSharpness();
			vertices[i]->setSharpness( s >= 1.0  ?  s - 1.0  :  0.0 );
		}
	}


	compactAll();
	finalise();
}

void MMesh::subdivideMarked(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness)
{
	assertFinalised();

	//a list for edges which are on the face mark boundary, but not
	//on the mesh boundary
	MEdgeList markBoundaryEdges;

	int nVertices = vertices.size();
	int nFaces = faces.size();

	if ( !catmullClark )
	{
		preserveNormalSharpness = MPRESERVENORMALSHARPNESS_PRESERVE;
	}

	//create the new vertex at the centre of each face
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->createSubdivisionVertex();
		}
	}

	//create a vertex for each edge
	if ( catmullClark )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			//if the edge is on the face mark boundary, but not a mesh boundary,
			//use linear subdivision
			bool faceMarkBoundary = edges[i]->isFaceMarkBoundary();
			bool meshBoundary = edges[i]->isBoundary();

			bool useCatmullClarkRules = false, useLinearRules = false;


			if ( edges[i]->isFaceMarkInternal() )
			{
				useCatmullClarkRules = true;
			}
			else if ( meshBoundary  &&  edges[i]->hasFaceMarkedVertices() )
			{
				useCatmullClarkRules = true;
			}

			if ( faceMarkBoundary  &&  !meshBoundary )
			{
				useLinearRules = true;
			}


			if ( useCatmullClarkRules )
			{
				edges[i]->createCatmullClarkSubdivisionVertex();
			}

			if ( useLinearRules )
			{
				edges[i]->createLinearSubdivisionVertex();
			}


			if ( faceMarkBoundary  &&  !meshBoundary )
			{
				markBoundaryEdges.push_back( edges[i] );
			}
		}
	}
	else
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isFaceMarked() )
			{
				edges[i]->createLinearSubdivisionVertex();
			}
			if ( edges[i]->isFaceMarkBoundary()  &&  !edges[i]->isBoundary() )
			{
				markBoundaryEdges.push_back( edges[i] );
			}
		}
	}

	//reposition the vertices
	if ( catmullClark )
	{
		//only process vertices who are face mark internal
		for (int i = 0; i < nVertices; i++)
		{
			//the positions of all vertices adjacent to those being processed
			//is require to be saved: thats all face marked vertices
			if ( vertices[i]->isFaceMarked() )
			{
				vertices[i]->savePosition();
			}
		}

		for (int i = 0; i < nVertices; i++)
		{
			bool faceMarkInternal = vertices[i]->isFaceMarkInternal();
			bool faceMarkBoundary = vertices[i]->isOnFaceMarkBoundary();
			bool meshBoundary = vertices[i]->isOnBoundary();

			if ( faceMarkInternal )
			{
				vertices[i]->computeNewPositionCatmullClark( preserveNormalSharpness );
			}

			if ( faceMarkBoundary  &&  meshBoundary )
			{
				vertices[i]->computeNewPositionCatmullClark( preserveNormalSharpness );
			}
		}
	}



	//subdivide the faces
	for (int i = 0; i < nFaces; i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->createSubdivisionFaces( catmullClark, preserveNormalSharpness );
		}
	}

	//destroy the original faces
	for (int i = 0; i < nFaces; i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			MFace *f = faces[i];

			f->destroy( true, true );
		}
	}


	//handle sharpness
	if ( catmullClark )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked() )
			{
				double s = vertices[i]->getSharpness();
				vertices[i]->setSharpness( s >= 1.0  ?  s - 1.0  :  0.0 );
			}
		}
	}

	//for an edge that is face mark boundary, but not mesh boundary:
	//if faceA is marked and faceB is not:
	//when createSubdivisionFaces() is called for faceA, faceA will
	//utilise the two halves of the split edge, but faceB will continue
	//using the unsplit edge.
	//subdivideMarkedBoundary() splits the unsplit edge, by inserting
	//the subdivision vertex of the edge, resulting in faceB
	//making use of the two split edges, thus sealing the whole in the mesh
	for (int i = 0; i < markBoundaryEdges.size(); i++)
	{
		markBoundaryEdges[i]->subdivideMarkedBoundary();
	}


	//in the section where edges were given subdivision vertices,
	//if Catmull-Clark rules were used:
	//		if an edge was a boundary edge, and had face marked vertices,
	//		it was given a subdivision vertex
	//		those edges which were not face marked, never had their subdivision
	//		vertices	used
	//destroy the isolated subdivision vertices
	if ( catmullClark )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isBoundary()  &&  edges[i]->hasFaceMarkedVertices()  &&  !edges[i]->isFaceMarked() )
			{
				edges[i]->destroySubdivisionVertex();
			}
		}
	}

	compactAll();

	finalise();
}



void MMesh::extrudeMarkedFaces(MVertexAdjustList &vertexAdjusts, MExtrudeFaceDirection direction, int numSegments,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	assertFinalised();

	vertexAdjusts.clear();

	Array<MFaceList> regions;



	/*
	o--------o--------o--------o
	|........|........|........|
	|........|........|........|
	|........|........|........|
	|........|........|........|
	|........|........|........|
	o--------o--------o--------o
	|........|        |........|
	|........|        |........|
	|........|        |........|
	|........|        |........|
	o--------o--------o--------o
	|........|........|        |
	|........|........|        |
	|........|........|        |
	|........|........|        |
	|........|........|        |
	o--------o--------o--------o
	*/

	if ( direction == MEXTRUDEFACEDIRECTION_PERGROUP )
	{
		discoverMarkedFaceRegions( regions );

		for (int i = 0; i < regions.size(); i++)
		{
			MFaceList &region = regions[i];
			Vector3 *groupDirection = new Vector3();

			for (int j = 0; j < region.size(); j++)
			{
				*groupDirection += region[j]->getPlane().n;
				region[j]->extrudeFaceSetGroupDirection( groupDirection );
			}

			groupDirection->normalise();
		}
	}

	//extrude the vertices
	//MVertex::extrudeFaceExtrudeVertex()
	//-moves vertices that are internal
	//-creates extruded versions of vertices on the boundary
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->extrudeFaceExtrudeVertex( vertexAdjusts, direction, numSegments );
		}
	}

	faceExtrudeFaces( numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges, NULL );

	for (int i = 0; i < regions.size(); i++)
	{
		//since all members of the region share the same vector, just delete the
		//vector pointed to by the first member
		delete regions[i].front()->extrudeFaceGetGroupDirection();
	}

	compactAll();
}

void MMesh::extrudeMarkedFaces(MVertexVectorAdjustList &vertexAdjusts, int numSegments,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	assertFinalised();

	vertexAdjusts.clear();



	/*
	o--------o--------o--------o
	|........|........|........|
	|........|........|........|
	|........|........|........|
	|........|........|........|
	|........|........|........|
	o--------o--------o--------o
	|........|        |........|
	|........|        |........|
	|........|        |........|
	|........|        |........|
	o--------o--------o--------o
	|........|........|        |
	|........|........|        |
	|........|........|        |
	|........|........|        |
	|........|........|        |
	o--------o--------o--------o
	*/

	//extrude the vertices
	//MVertex::extrudeFaceVectorExtrudeVertex()
	//-moves vertices that are internal
	//-creates extruded versions of vertices on the boundary
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->extrudeFaceVectorExtrudeVertex( vertexAdjusts, numSegments );
		}
	}

	faceExtrudeFaces( numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges, NULL );

	compactAll();
}


int MMesh::pathExtrudeMarkedFaces(const Polyline &extrusionPath, MPathExtrudeFaceMode mode, bool correctiveScaling, PolylinePathOrientation orientation, double pathRotation,
								bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	assertFinalised();


	if ( extrusionPath.isClosed() )
	{
		return -1;					//only works for an open path
	}


	//if the path has less than one edge vertices, no extrusion needs to be performed
	if ( extrusionPath.getNumEdges() < 1 )
	{
		return 0;
	}




	//AN INITIAL TRANSFORMATION MATRIX IS NEEDED FOR EACH ITERATION OF THE LOOP:
	//it must:
	//- translate the path such that vertex 0 is at the origin
	//- rotate the path around the vector of its first edge by the amount
	//  specified by pathRotation

	//get the vertex list from the path
	const Point3 &vertex0Pos = extrusionPath[0].getPosition();
	Vector3 firstEdgeDirection = extrusionPath[1].getPosition() - extrusionPath[0].getPosition();
	firstEdgeDirection.normalise();
	//compute the initial transformation matrix
	Matrix4 pathInitialXform = Matrix4::translate( -vertex0Pos.toVector3() )  *  Matrix4::rotate( firstEdgeDirection, pathRotation );




	//COMPUTE THE LIST OF MARKED REGIONS
	Array<MFaceList> regions;
	discoverMarkedFaceRegions( regions );

	//PER REGION MATRICES TABLE
	Array< Array<Matrix4>* > perRegionMatricesTable;
	perRegionMatricesTable.reserve( regions.size() );

	//FOR EACH REGION
	for (int i = 0; i < regions.size(); i++)
	{
		MFaceList &region = regions[i];

		//COMPUTE THE GROUP DIRECTION AND CENTRE
		Vector3 regionDirection;
		Point3 regionCentre;
		for (int j = 0; j < region.size(); j++)
		{
			MFace *f = region[j];

			regionDirection += f->getPlane().n;
			regionCentre.cumulativeAdd( f->computeCentroid() );
		}

		regionDirection.normalise();
		regionCentre.scale( 1.0 / (double)region.size() );



		//TAKE A COPY OF THE EXTRUSION PATH THAT TRANSFORMATIONS CAN BE APPLIED TO IT
		Polyline path = extrusionPath;
		path.transform( pathInitialXform, false );


		//PERFORM PATH ALIGNMENT IF DESIRED
		if ( mode == MPATHEXTRUDEFACEMODE_ALIGNPATHTOFACES )
		{
			//transform the direction of the first edge of the path to the
			//region direction
			path.transform( Matrix4::axisToAxis( firstEdgeDirection, regionDirection ), false );
		}


		//GENERATE THE PATH TRANSFORMATIONS
		Array<Matrix4> *regionMatrices = new Array<Matrix4>();
		computePathMatrices( path, *regionMatrices, regionDirection, correctiveScaling, orientation );


		//add to the region matrix table
		perRegionMatricesTable.push_back( regionMatrices );


		//NOTE: the transformations must be centred around the region centre, so
		//- prefix each transformation with a translation from centre -> origin
		//- suffix each transformation with a translation from origin -> centre
		for (int j = 0; j < regionMatrices->size(); j++)
		{
			regionMatrices->at( j ) = Matrix4::translate( -regionCentre.toVector3() ) *
								regionMatrices->at( j ) *
								Matrix4::translate( regionCentre.toVector3() );
		}


		//SET THE PATH EXTRUDE MATRICES FOR THE FACES THAT ARE WITHIN THIS REGION
		for (int j = 0; j < region.size(); j++)
		{
			region[j]->pathExtrudeSetMatrices( regionMatrices );
		}
	}


	//extrude the vertices
	//MVertex::faceExtrudeExtrudeVertex()
	//-moves vertices that are internal
	//-creates extruded versions of vertices on the boundary
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->pathExtrudeFaceExtrudeVertices( mode );
		}
	}



	//GENERATE THE EXTRUDED FACES
	faceExtrudeFaces( extrusionPath.getNumEdges(), markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges, &extrusionPath );



	//delete contents of the per region matrices table
	for (int i = 0; i < perRegionMatricesTable.size(); i++)
	{
		delete perRegionMatricesTable[i];
	}

	compactAll();
	finalise();

	return 0;
}


void MMesh::faceExtrudeFaces(int numSegments, bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges, const Polyline *extrusionPath)
{
	Array<MFace*> extrudedFaces;
	Array<MEdgeRun> boundaries;

	//generate a list of faces which are to be extruded
	discoverMarkedFaces( extrudedFaces );


	/*
	o--------o--------o--------o
	|                          |
	| x      x        x      x |		x : extruded boundary vertex
	|                          |
	|      x |        | x      |		NOTE:
	|        |        |        |		vertex A is extruded to create two
	o x -----o--------o----- x o		vertices; B and C
	|        |        |        |
	|        |        |        |
	|        |        | xC   x |		a broken edge line means that it
	|        |        |        |		does not pass through the vertex
	o x -----o--------o--------o
	|        |        |A       |
	|      x |     Bx |        |
	|                 |        |
	| x      x      x |        |
	|                 |        |
	o--------o--------o--------o
	*/



	//get the boundaries surrounding the marked area(s)
	discoverMarkedFaceBoundaries( boundaries );



	//create extruded versions of the boundary edges:
	//this does not extrude an edge e into a face; it creates an
	//extruded duplicate of e, using the extruded vertices
	for (int i = 0; i < boundaries.size(); i++)
	{
		MEdgeRun &boundary = boundaries[i];

		for (int j = 0; j < boundary.size(); j++)
		{
			MEdge *e = boundary[j];

			//set as indestructible so that the vertex replace actions in
			//MFace::extrudeFace() do not destroy edge e
			e->setIndestructible();

			//create the extruded duplicate of the edge
			e->extrudeFaceCreateExtrudedDuplicates( markBoundaryEdges, nsharpBoundaryEdges, extrusionPath );

			if ( markBoundaryEdges )
			{
				e->edgeMark();
			}
			if ( nsharpBoundaryEdges )
			{
				e->setNormalSharp();
			}
		}
	}

	/*
	o--------o--------o--------o
	|                          |
	| x------x--------x------x |
	| |                      | |
	| |    x-|--------|-x    | |
	| |    | |        | |    | |
	o x -----o--------o----- x o
	| |    | |        | |    | |
	| |    | |        | |    | |
	| |    | |        | x----x |
	| |    | |        |        |
	o x -----o--------o--------o
	| |    | |        |        |
	| |    x-|------x |        |
	| |             | |        |
	| x------x------x |        |
	|                 |        |
	o--------o--------o--------o
	*/



	//create extruded versions of the faces;
	//these methods make use of the vertices and edges that have
	//already been extruded
	for (int i = 0; i < extrudedFaces.size(); i++)
	{
		extrudedFaces[i]->extrudeFace();
	}

	/*
	o--------o--------o--------o
	|                          |
	| x------x--------x------x |
	| |     /          \     | |
	| |   _x------------x_   | |
	| |__/ |            | \__| |
	o x    | o--------o |    x o
	| |    | |        | |    | |
	| |    | |        | |    | |
	| |    | |        | x----x |
	| |    | |        |        |
	o x__  | o--------o--------o
	| |  \_|          |        |
	| |    x--------x |        |
	| |     \       | |        |
	| x------x------x |        |
	|                 |        |
	o--------o--------o--------o
	*/


	//COMPUTE THE EDGE TEXTURE TABLE
	PolylineEdgeTextureTable ptex;

	if ( extrusionPath != NULL )
	{
		extrusionPath->computeEdgeTextureTable( ptex );
	}
	else
	{
		Polyline::computeIdentityTextureTable( numSegments, ptex );
	}


	//stitch the extruded faces and the original mesh together
	//by creating quads that link the original boundary edges to their
	//extruded counterparts created by MEdge::createFaceExtrudedEdge()
	for (int i = 0; i < boundaries.size(); i++)
	{
		MEdgeRun &boundary = boundaries[i];


		MVertexList boundaryVertices;
		Array<double> lengthArray;
		boundary.extractVertices( boundaryVertices );
		boundary.generateUnitCumulativeLengthArray( lengthArray );




		for (int j = 0; j < boundary.size(); j++)
		{
			MEdge *e = boundary[j];

			float u[2];
			u[0] = lengthArray[j];
			u[1] = lengthArray[ j + 1 ];

			if ( boundaryVertices[j] == e->getVertexB() )
			{
				std::swap( u[0], u[1] );
			}

			e->extrudeFaceExtrudeToQuads( numSegments, -1, u, markExtrudedEdges, nsharpExtrudedEdges, ptex );
			e->setDestructible();
		}
	}

	/*
	o--------o--------o--------o
	|\       |        |       /|
	| x------x--------x------x |
	| |     /          \     | |
	| |   _x------------x_   | |
	| |__/ |\          /| \__| |
	o-x    | o--------o |    x-o
	| |    | |        | |    | |
	| |    | |        | |    | |
	| |    | |        | x----x |
	| |    | |        |/      \|
	o-x__  | o--------o--------o
	| |  \_|/        /|        |
	| |    x--------x |        |
	| |     \       | |        |
	| x------x------x |        |
	|        |       \|        |
	o--------o--------o--------o
	*/


	//destroy isolated edges
	for (int i = 0; i < edges.size(); i++)
	{
		if ( !edges[i]->isDestroyed() )
		{
			edges[i]->destroyIfIsolated( true );
		}
	}

	//delete the extra data created for the extrude operation
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked()  &&  !vertices[i]->isDestroyed() )
		{
			vertices[i]->extrudeFaceDeleteExtrudeData();
		}
	}

	//CANNOT USE:
	//markFaces_all( MARKPREDICATE_UNMARK );
	//as mesh has not yet been finalised
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->faceUnmark();
	}


	for (int i = 0; i < extrudedFaces.size(); i++)
	{
		extrudedFaces[i]->faceMark();
	}
}

void MMesh::extrudeMarkedFacesSingular(MVertexAdjustList &vertexAdjusts, int numSegments,
									bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	assertFinalised();

	vertexAdjusts.clear();

	int nFaces = faces.size();



	if ( markBoundaryEdges  ||  nsharpBoundaryEdges )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isFaceMarkBoundary() )
			{
				if ( markBoundaryEdges )
				{
					edges[i]->edgeMark();
				}
				if ( nsharpBoundaryEdges )
				{
					edges[i]->setNormalSharp();
				}
			}
		}
	}


	for (int i = 0; i < nFaces; i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->extrudeSingular( vertexAdjusts, numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges, -1 );
		}
	}

	compactAll();
}


void MMesh::duplicateMarkedFaces(MVertexVectorAdjustList &vertexAdjusts, bool markOriginalFaces, bool markDuplicateFaces)
{
	assertFinalised();


	// Create the duplicate vertices
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];
		if ( v->isFaceMarked() )
		{
			v->duplicateFacesCreateDuplicateVertex( vertexAdjusts );
		}
	}

	// Create the duplicate faces
	// (since new faces are created, only iterate up to the original list size)
	int numFaces = faces.size();
	for (int faceI = 0; faceI < numFaces; faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			MFace *duplicate = f->createDuplicateFace();

			if ( !markOriginalFaces )
			{
				f->faceUnmark();
			}

			if ( !markDuplicateFaces )
			{
				duplicate->faceUnmark();
			}
		}
	}

	compactAll();
}


void MMesh::insetExpandMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle, MInsetExpandOperation insetExpandOp,
									bool flowThruMarkedEdges, bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	assertFinalised();

	// Check for bad parameter combinations
	gs_assert( !( insetExpandOp == MINSETEXPANDOP_EXPAND  &&  flowThruMarkedEdges ), "MMesh::insetExpandMarkedFaces(): bad parameters: cannot flow through edges when "
				"in expand mode\n" );
	gs_assert( !( insetExpandOp == MINSETEXPANDOP_EXPAND  &&  insetStyle == MINSETFACESTYLE_TOCENTRE ), "MMesh::insetExpandMarkedFaces(): bad parameters: cannot "
				"use 'to centre' direciton when in expand mode\n" );

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();


	//The inset process modifies the vertex list of the faces on the
	//face mark boundary. As a result, the average vertex position of
	//the face changes. To counter this, for each face, store the centre
	//which is used, thus the centre that is used remains constant
	//throughout the procedure

	MEdgeList edgesToCheck;
	MFaceList facesWithStoredCentres;
	MFaceList facesToUnmark;

	for (int i = 0; i < faces.size(); i++)
	{
		if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
		{
			if ( faces[i]->isInsetExpandTarget( insetExpandOp ) )
			{
				if ( faces[i]->insetExpandFaceStoreCentre() )
				{
					facesWithStoredCentres.push_back( faces[i] );
				}
			}
		}
		faces[i]->insetExpandFaceStorePlane( insetExpandOp );
	}


	/*
	o--------o--------o
	|........|			|
	|........|			|
	|........|			|
	|........|			|
	|........|			|
	o--------o--------o
	|........|........|
	|........|........|
	|........|........|
	|........|........|
	|........|........|
	o--------o--------o
	*/

	//insetFacesSplitInternalEdges():
	//		split incident edges which are face mark internal
	//insetFacesInsertCornerVertices():
	//		adds a vertex into the convex corners of marked polygons; at a vertex
	//		where the edges adjacent to the vertex are both face mark boundary

	// Record the number of vertices, as these operations may create additional vertices
	int numVertices = vertices.size();
	for (int vertexI = 0; vertexI < numVertices; vertexI++)
	{
		MVertex *v = vertices[vertexI];
		if ( v->isOnFaceMarkBoundary() )
		{
			if ( insetExpandOp == MINSETEXPANDOP_INSET )
			{
				v->insetFacesSplitInternalEdges( vertexAdjusts, vertexAttribAdjusts, insetStyle, flowThruMarkedEdges, markInsetEdges, nsharpInsetEdges );
			}
			else
			{
				v->expandFacesSplitInternalEdges( vertexAdjusts, vertexAttribAdjusts, markInsetEdges, nsharpInsetEdges );
			}

			v->insetExpandFacesInsertCornerVertices( edgesToCheck, vertexAdjusts, vertexAttribAdjusts, insetStyle, flowThruMarkedEdges, insetExpandOp );
		}
	}

	/*
	o--------o--------o
	|\....../|			|
	|.x....x.|			|
	|........|			|		o = original vertex
	|........|			|		* = vertex created by (internal) edge split
	|........|			|		x = vertex inserted into corner
	o-*----*-o--------o
	|........|......./|
	|........*......x.|
	|........|........|
	|.x......*......x.|
	|/.......|.......\|
	o--------o--------o
	*/

	//Generate a list of the edges that need to be used to split quads away.
	//Must be done before the insetFacesSplitCoincidentInternalFaces() stage,
	//since insetFacesSplitCoincidentInternalFaces() changes face mark properties
	MEdgeList edgesToSplitOff;
	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *e = edges[i];

		if ( e->isFaceMarkBoundary()  &&  !e->isInsetFaceFlowEdge( flowThruMarkedEdges ) )
		{
			edgesToSplitOff.push_back( e );

			if ( markBoundaryEdges )
			{
				e->edgeMark();
			}
			if ( nsharpBoundaryEdges )
			{
				e->setNormalSharp();
			}
		}
	}

	//split triangles away from any incident face where the two adjacent edges
	//are face mark internal
	for (int i = 0; i < numVertices; i++)
	{
		if ( vertices[i]->isOnFaceMarkBoundary() )
		{
			vertices[i]->insetExpandFacesSplitIncidentInternalFaces( insetExpandOp, flowThruMarkedEdges, markBoundaryEdges, nsharpBoundaryEdges );
		}
	}

	/*
	o--------o--------o
	|\....../|			|
	|.x....x.|			|
	|........|			|
	|........|			|
	|........|			|
	o-*----*-o--------o			The triangle in the centre has been split away
	|.......\|......./|
	|........*......x.|
	|........|........|
	|.x......*......x.|
	|/.......|.......\|
	o--------o--------o
	*/

	//Split quads away from face mark boundary edges (from marked faces)
	for (int i = 0; i < edgesToSplitOff.size(); i++)
	{
		edgesToSplitOff[i]->insetExpandFaceSplitOffIncidentQuads( insetExpandOp, markBoundaryEdges, nsharpBoundaryEdges );
	}

	/*
	o--------o--------o
	|\....../|			|
	|.x----x.|			|
	|.|....|.|			|
	|.|....|.|			|
	|.|....|.|			|
	o-*----*-o--------o
	|.|.....\|......./|
	|.|......*------x.|
	|.|......|......|.|
	|.x------*------x.|
	|/.......|.......\|
	o--------o--------o
	*/

	//delete the centre which was allocated by insetFaceStoreCentre()
	if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
	{
		for (int i = 0; i < facesWithStoredCentres.size(); i++)
		{
			facesWithStoredCentres[i]->insetExpandFaceDeleteCentre();
		}
	}

	//Note that the edges going from the vertices towards the face centres,
	//created by the MVertex::insertFaces() method may now have the wrong
	//orientation. This is the reason that a list of them was kept.
	//Ensure that their orientation is correct
	for (int i = 0; i < edgesToCheck.size(); i++)
	{
		MEdge *e = edgesToCheck[i];

		//these edges are also inset edges; mark them if this is desired
		if ( markInsetEdges )
		{
			e->edgeMark();
		}
		if ( nsharpInsetEdges )
		{
			e->setNormalSharp();
		}

		if ( !e->getFaceA()->isEdgeInOrder_search( e ) )
		{
			e->swapVertices();
		}
	}


	//apply extrusion to any internal vertices
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( ( insetExpandOp == MINSETEXPANDOP_INSET  &&  vertices[i]->isFaceMarkInternal() )   ||
			( insetExpandOp == MINSETEXPANDOP_EXPAND  &&  vertices[i]->isFaceMarked() ) )
		{
			vertices[i]->insetExpandFacesMarkedInternalVertexExtrude( vertexAdjusts );
		}
	}


	// Clear up stored planes
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->insetExpandFaceDeletePlane();
	}


	compactAll();
}

void MMesh::insetMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle, bool flowThruMarkedEdges,
							bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	insetExpandMarkedFaces( vertexAdjusts, vertexAttribAdjusts, insetStyle, MINSETEXPANDOP_INSET, flowThruMarkedEdges,
							markBoundaryEdges, markInsetEdges, nsharpBoundaryEdges, nsharpInsetEdges );
}

void MMesh::expandMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
							bool markBoundaryEdges, bool markExpansionEdges, bool nsharpBoundaryEdges, bool nsharpExpansionEdges)
{
	insetExpandMarkedFaces( vertexAdjusts, vertexAttribAdjusts, MINSETFACESTYLE_FROMEDGES, MINSETEXPANDOP_EXPAND, false,
							markBoundaryEdges, markExpansionEdges, nsharpBoundaryEdges, nsharpExpansionEdges );
}

void MMesh::insetMarkedFacesSingular(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle,
								bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();

	int nFaces = faces.size();


	if ( markBoundaryEdges  ||  nsharpBoundaryEdges )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isFaceMarked() )
			{
				if ( markBoundaryEdges )
				{
					edges[i]->edgeMark();
				}
				if ( nsharpBoundaryEdges )
				{
					edges[i]->setNormalSharp();
				}
			}
		}
	}


	for (int i = 0; i < nFaces; i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->insetSingular( vertexAdjusts, vertexAttribAdjusts, insetStyle, markBoundaryEdges, markInsetEdges, nsharpBoundaryEdges, nsharpInsetEdges );
		}
	}

	compactAll();
}

void MMesh::insetMarkedFacesToPoints(MVertexAdjustList &vertexAdjusts, int numSegments, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	assertFinalised();

	vertexAdjusts.clear();


	if ( markBoundaryEdges  ||  nsharpBoundaryEdges )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isFaceMarked() )
			{
				if ( markBoundaryEdges )
				{
					edges[i]->edgeMark();
				}
				if ( nsharpBoundaryEdges )
				{
					edges[i]->setNormalSharp();
				}
			}
		}
	}


	MFaceList markedFaces;
	discoverMarkedFaces( markedFaces );

	//we know how many vertex adjust objects are needed
	vertexAdjusts.reserve( markedFaces.size() );



	for (int i = 0; i < markedFaces.size(); i++)
	{
		markedFaces[i]->insetToPoint( vertexAdjusts, numSegments );
	}

	compactAll();
}

void MMesh::expandMarkedVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges, bool normalSharpenBoundaryEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  &&  !vertices[i]->isDestroyed() )
		{
			vertices[i]->expand( vertexAdjusts, vertexAttribAdjusts, markBoundaryEdges, normalSharpenBoundaryEdges );
		}
	}

	/*
	o--------o--------o
	|        |			|		o = original vertex
	|        |			|		8 = selected vertex
	|        *			|		* = new vertex
	|       /|\	      |
	|      / | \      |
	o-----*--8--*-----o
	|      \ | /		|
	|       \|/ 		|
	|        *			|
	|        |			|
	|        |			|
	o--------o--------o
	*/

	compactAll();
}

void MMesh::bevelMarkedVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
							bool useExistingTexturing, bool markBoundaryEdges, bool normalSharpenBoundaryEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();

	MVertexList verticesToBevel;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  &&  !vertices[i]->isDestroyed() )
		{
			verticesToBevel.push_back( vertices[i] );
		}
	}



	for (int i = 0; i < verticesToBevel.size(); i++)
	{
		verticesToBevel[i]->bevel( vertexAdjusts, vertexAttribAdjusts, useExistingTexturing, markBoundaryEdges, normalSharpenBoundaryEdges );
	}
	/*
	o--------o--------o
	|        |			|		o = original vertex
	|        |			|
	|        *			|		* = new vertex
	|       / \	      |
	|      /   \      |
	o-----*     *-----o
	|      \   /		|
	|       \ / 		|
	|        *			|
	|        |			|
	|        |			|
	o--------o--------o
	*/

	compactAll();
}

void MMesh::expandMarkedVerticesInMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  &&  !vertices[i]->isDestroyed() )
		{
			vertices[i]->expandInMarkedFaces( vertexAdjusts, vertexAttribAdjusts, markBoundaryEdges );
		}
	}

	/*
	o--------o--------o
	|........|........|		o = original vertex
	|........|........|		8 = selected vertex
	|........*........|		* = new vertex
	|......./|\.......|
	|....../.|.\......|
	o-----*--8--*-----o
	|        |  		|
	|        |  		|
	|        |			|
	|        |			|
	|        |			|
	o--------o--------o
	*/

	compactAll();
}

void MMesh::expandMarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
							bool markBoundaryEdges, bool nsharpBoundaryEdges, bool unmarkOriginalEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	vertexAttribAdjusts.clear();

	MEdgeList edgesToCheck;

	//The expand procedure creates extra edges.
	//If the boundary edges that are created are marked, (markBoundaryEdges flag)
	//then these will also be expanded
	//So: build a list of boundary edges, and mark these later
	MEdgeList boundaryEdges;


	/*
	o--------o--------o--------o
	|        |        |        |
	|        |        |        |
	|        |        |        |
	|        |        |        |
	o--------o========o--------o
	|        H        |        |
	|        H        |        |
	|        H        |        |
	|        H        |        |
	o--------o--------o--------o
	|        |        |        |
	|        |        |        |
	|        |        |        |
	|        |        |        |
	o--------o--------o--------o
	*/

	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i];

		//for each vertex which is incident to one or more marked edges:
		if ( v->isEdgeMarked() )
		{
			//v->expandEdgeInit();
			//compute the expansion vector for extrusion
			v->expandEdgeMoveVertex( vertexAdjusts );
			//split the edges incident to v which are NOT marked
			v->expandEdgeSplitUnmarkedEdges( vertexAdjusts, vertexAttribAdjusts, boundaryEdges );
		}
	}

	/*
	o--------o--------o--------o
	|        |        |        |
	|        |        |        |
	|        *        *        |
	|       /|        |\       |
	o------*-o========o-*------o
	|        H        |/       |
	|        H        *        |
	|        H        |        |
	|        H        |        |
	o------*-o-*------o--------o
	|        |        |        |
	|        *        |        |
	|        |        |        |
	|        |        |        |
	o--------o--------o--------o
	*/

	for (int i = 0; i < faces.size(); i++)
	{
		//for each face which is incident to one or more marked edges:
		if ( faces[i]->isEdgeMarked() )
		{
			//if two adjacent edges of faces[i] are marked, the insert a vertex
			//between the shared vertex (shared by the edges) and the centre
			//of the face
			faces[i]->expandEdgeInsertCornerVertices( vertexAdjusts, vertexAttribAdjusts, edgesToCheck );
		}
	}

	/*
	o--------o--------o--------o
	|        |        |        |
	|        |        |        |
	|        *        *        |		o - original vertex
	|       /|        |\       |		* - vertex created by edge split
	o------*-o========o-*------o		x - vertex inserted into corner (see above)
	|        H\       |/       |
	|        H x      *        |		= H  -  marked edge
	|        H        |        |
	|        H        |        |
	o------*-o-*------o--------o
	|        |        |        |
	|        *        |        |
	|        |        |        |
	|        |        |        |
	o--------o--------o--------o
	*/



	for (int i = 0; i < edges.size(); i++)
	{
		//for each marked edge
		if ( edges[i]->isEdgeMarked() )
		{
			//split away the incident quads
			edges[i]->expandEdgeSplitOffIncidentQuads( boundaryEdges );

			if ( unmarkOriginalEdges )
			{
				edges[i]->edgeUnmark();
			}
		}
	}


	/*
	o--------o--------o--------o
	|        |        |        |
	|        |        |        |
	|        *--------*        |
	|       /|        |\       |
	o------*-o========o-*------o
	|      | H\       |/       |
	|      | H x------*        |
	|      | H |      |        |
	|      | H |      |        |
	o------*-o-*------o--------o
	|       \|/       |        |
	|        *        |        |
	|        |        |        |
	|        |        |        |
	o--------o--------o--------o
	*/

	//the edges created by MFace::expandEdgeCreateVertices() may be the wrong way
	//around, fix this
	for (int i = 0; i < edgesToCheck.size(); i++)
	{
		MEdge *e = edgesToCheck[i];

		if ( !e->getFaceA()->isEdgeInOrder_search( e ) )
		{
			e->swapVertices();
		}
	}

	if ( markBoundaryEdges  ||  nsharpBoundaryEdges )
	{
		for (int i = 0; i < boundaryEdges.size(); i++)
		{
			if ( markBoundaryEdges )
			{
				boundaryEdges[i]->edgeMark();
			}
			if ( nsharpBoundaryEdges )
			{
				boundaryEdges[i]->setNormalSharp();
			}
		}
	}

	compactAll();
}




void MMesh::bevelMarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &attribAdjusts, bool useExistingTexturing,
							bool markBoundaryEdges, bool normalSharpedBoundaryEdges)
{
	assertFinalised();

	vertexAdjusts.clear();
	attribAdjusts.clear();

	MVertexList bevelledVertices;
	MEdgeList bevelledEdges;
	MFaceList bevelledFaces, newFaces;


	//***** STEP ONE: INITIALISE *****
	//build a list of faces which have incident edge marked vertices
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->hasIncidentEdgeMarkedVertices() )
		{
			bevelledFaces.push_back( faces[i] );
		}
	}

	//- initialise edges for bevelling
	//- mark marked edges as indestructible
	//- build a list of edges to be bevelled
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->bevelInit();
		if ( edges[i]->isEdgeMarked() )
		{
			bevelledEdges.push_back( edges[i] );
			edges[i]->setIndestructible();
		}
	}

	//initialise vertices for bevelling
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isEdgeMarked() )
		{
			bevelledVertices.push_back( vertices[i] );
			vertices[i]->bevelEdgeInit( true );
		}
		else
		{
			vertices[i]->bevelEdgeInit( false );
		}
	}



	//***** STEP TWO: CREATE BOUNDARY VERTICES *****
	for (int i = 0; i < bevelledVertices.size(); i++)
	{
		bevelledVertices[i]->bevelEdgeCreateBoundaryVertices( vertexAdjusts, attribAdjusts );
	}


	Array<MVertex::_BevelledEdgeQuadStrip> quadStrips;

	//***** STEP THREE: EXTRACT BEVELLED EDGE QUAD STRIPS *****
	//find open strips, then closed ones
	//Bevelling an edge turns it into a quad. A run of marked edges
	//where each vertex along the way has only TWO incident marked edges
	//will form a quad strip. A quad strip ends with a verted with
	//ONE or MORE THAN TWO incident marked edges. A closed strip loops
	//back on itself, an open one has ends.
	for (int i = 0; i < bevelledVertices.size(); i++)
	{
		bevelledVertices[i]->bevelEdgeExtractOpenQuadstrips( quadStrips, useExistingTexturing );
	}

	for (int i = 0; i < bevelledVertices.size(); i++)
	{
		bevelledVertices[i]->bevelEdgeExtractClosedQuadstrips( quadStrips, useExistingTexturing );
	}

	//secondary unmark all bevelled edges
	for (int i = 0; i < bevelledEdges.size(); i++)
	{
		bevelledEdges[i]->secondaryUnmark();
	}


	//***** STEP FOUR: REPLACE THE BEVELLED VERTICES WITH THE BOUNDARY VERTICES *****
	for (int i = 0; i < bevelledFaces.size(); i++)
	{
		bevelledFaces[i]->bevelEdgeReplaceVerticesWithBoundaryVerts();
	}


	//***** STEP FIVE: CREATE BEVELLED VERTEX FACES *****
	for (int i = 0; i < bevelledVertices.size(); i++)
	{
		int currentNewFaces = newFaces.size();

		bevelledVertices[i]->bevelEdgeCreateVertexFace( useExistingTexturing, newFaces );

		for (int j = currentNewFaces; j < newFaces.size(); j++)
		{
			newFaces[j]->secondaryMark();
		}
	}



	//***** STEP SIX: CREATE BEVELLED EDGE QUAD STRIPS *****
	Array<MVertex*> quad;
	Array<Point2f> texCoords;
	Array<MVertexAttrib*> attribs;
	quad.resize( 4 );
	texCoords.resize( 4 );
	attribs.resize( 4 );

	for (int stripI = 0; stripI < quadStrips.size(); stripI++)
	{
		const MVertex::_BevelledEdgeQuadStrip &strip = quadStrips[stripI];
		bool flipFlag = false;

		//get the vertices for the first quad
		MVertex *quad0Verts[4];
		quad0Verts[0] = strip.vertices[0];
		quad0Verts[1] = strip.vertices[1];
		quad0Verts[2] = strip.vertices[3];		//flip last pair
		quad0Verts[3] = strip.vertices[2];

		//attempt to find an edge that is already used, so that the state of the flip
		//flag can be determined
		for (int edgeI = 0; edgeI < 4; edgeI++)
		{
			MEdge *e = quad0Verts[edgeI]->findEdgeTo( quad0Verts[ ( edgeI + 1 ) % 4 ], MFINDPOLICY_RETURNNULL );
			if ( e != NULL )
			{
				flipFlag = e->getVertexA() == quad0Verts[edgeI];
				break;
			}
		}


		MVertexAttrib *previousAttribs[2];
		previousAttribs[0] = NULL;
		previousAttribs[1] = NULL;


		//j for indexing into the strip vertices; there is 2n+2 vertices for n quads
		//t for indexing into the strip tex V co-ords; there is n+1 tex V's for n quads
		//t is also used for indexing into the list of edge materials that comes
		//with @strip, when @useExistingTextureing is enabled
		for (int j = 0, t = 0; j < strip.vertices.size() - 2; j += 2, t++)
		{
			bool addQuad = true;

			quad[0] = strip.vertices[j];
			quad[1] = strip.vertices[ j + 1 ];
			quad[2] = strip.vertices[ j + 3 ];		//flip last pair
			quad[3] = strip.vertices[ j + 2 ];

			for (int k = 0; k < 4; k++)
			{
				MEdge *e = quad[k]->findEdgeTo( quad[ ( k + 1 ) % 4 ], MFINDPOLICY_RETURNNULL );
				if ( e != NULL )
				{
					//if the edge order opposes the order for this strip, then
					//miss this quad out
					if ( ( e->getVertexA() == quad[k] )  !=  flipFlag )
					{
						addQuad = false;
						break;
					}
				}
			}

			if ( addQuad )
			{
				MFace *f;
				if ( useExistingTexturing )
				{
					attribs[0] = quad[0]->getVertexAttrib( 0 );
					attribs[1] = quad[1]->getVertexAttrib( 0 );
					attribs[2] = quad[2]->getVertexAttrib( 0 );
					attribs[3] = quad[3]->getVertexAttrib( 0 );

					int materialID = strip.edgeMaterials[t];

					if ( flipFlag )
					{
						quad.reverse();
						attribs.reverse();
					}

					f = addFace( quad, attribs, materialID );
				}
				else
				{
					//generate texture co-ordinates
					texCoords[0] = Point2f( 0.0, strip.textureV[t] );
					texCoords[1] = Point2f( 1.0, strip.textureV[t] );
					texCoords[2] = Point2f( 1.0, strip.textureV[ t + 1 ] );
					texCoords[3] = Point2f( 0.0, strip.textureV[ t + 1 ] );

					//Make absolutely sure that NEW VERTEX ATTRIBUTES are generated for
					//this quad strip, as SHARING vertex attributes with faces that
					//are NOT part of this quad strip is NOT desirable here

					if ( j == 0 )
					{
						//first quad; no previous quad to share attribs with;
						//create new attribs
						previousAttribs[0] = quad[0]->createVertexAttrib( MVertexAttrib( texCoords[0], -1 ), MFINDPOLICY_FORCEDCREATE );
						previousAttribs[1] = quad[1]->createVertexAttrib( MVertexAttrib( texCoords[1], -1 ), MFINDPOLICY_FORCEDCREATE );
					}
					//build the attribs Array:
					//0 and 1 should be shared from the previous quad
					attribs[0] = previousAttribs[0];
					attribs[1] = previousAttribs[1];
					//2 and 3 are new
					attribs[2] = quad[2]->createVertexAttrib( MVertexAttrib( texCoords[2], -1 ), MFINDPOLICY_FORCEDCREATE );
					attribs[3] = quad[3]->createVertexAttrib( MVertexAttrib( texCoords[3], -1 ), MFINDPOLICY_FORCEDCREATE );
					//share 2 and 3 with the next quad, MUST ALSO FLIP, as above
					//where the vertex pointers for entries 2 and 3 in @quad
					//are flipped when they are copied from @stip.vertices
					previousAttribs[1] = attribs[2];
					previousAttribs[0] = attribs[3];

					if ( flipFlag )
					{
						quad.reverse();
						attribs.reverse();
					}

					f = addFace( quad, attribs, -1 );
				}
				newFaces.push_back( f );
				f->secondaryMark();
			}
		}
	}


	//***** STEP EIGHT: MARK BOUNDARY EDGES (IF REQUESTED) *****
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceSecondaryMarkBoundary() )
		{
			edges[i]->setEdgeMark( markBoundaryEdges );
			edges[i]->setNormalSharpness( normalSharpedBoundaryEdges );
		}
	}


	//***** STEP SEVEN: FINISH UP *****
	//finish bevelled vertices
	for (int i = 0; i < bevelledVertices.size(); i++)
	{
		bevelledVertices[i]->bevelEdgeFinish();
	}

	// Call MEdge::bevelShutdown() on all edges
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->bevelShutdown();
	}

	//mark bevelled edges destructible, and destroy them
	for (int i = 0; i < bevelledEdges.size(); i++)
	{
		bevelledEdges[i]->setDestructible();
		bevelledEdges[i]->destroyIfIsolated( true );
	}

	//newly created faces were secondary marked
	for (int i = 0; i < newFaces.size(); i++)
	{
		newFaces[i]->secondaryUnmark();
	}

	compactAll();
}




int MMesh::discoverExtrudeableEdgeRuns(Array<MEdgeRun> &edgeRuns)
{
	//STAGE ONE:
	//check all of the marked edges and make sure they are boundary edges
	//if not, return false (error)
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			if ( !edges[i]->isBoundary() )
			{
				return -1;
			}
		}
	}


	//STAGE TWO-A:
	//build edge runs
	//start by finding vertices with # of extrudeable edges != 2 && != 0
	//and starting from there
	for (int i = 0; i < vertices.size(); i++)
	{
		int count = vertices[i]->getExtrudeableEdgeCount();

		if ( count != 2  &&  count != 0 )
		{
			//found a vertex to start from
			MVertex *startingVertex = vertices[i];
			//find an edge to start with
			MEdge *startingEdge = startingVertex->getExtrudeableEdge();

			edgeRuns.push_back( MEdgeRun() );
			MEdgeRun &run = edgeRuns.back();

			//generate the edge run, from startingVertex
			startingEdge->generateExtrudeableEdgeRun( run, startingVertex );
		}
	}

	//STAGE TWO-B:
	//finish building edge runs
	//edge runs that are closed, and do not cross vertices with > 2
	//extrudeable incident edges will not be picked up in stage TWO-A
	//deal with these, now, by finding any remaining extrudeable edges,
	//which have not been visited
	for (int i = 0; i < vertices.size(); i++)
	{
		int count = vertices[i]->getExtrudeableEdgeCount();

		//note: an edge which has already been visited is not considered
		//extrudeable
		if ( count == 2 )
		{
			//found a vertex to start from
			MVertex *startingVertex = vertices[i];
			//find an edge to start with
			MEdge *startingEdge = startingVertex->getExtrudeableEdge();

			edgeRuns.push_back( MEdgeRun() );
			MEdgeRun &run = edgeRuns.back();

			//generate the edge run
			startingEdge->generateExtrudeableEdgeRun( run, NULL );
		}
	}


	// Secondary unmark all edges that formed edge runs
	for (int runI = 0; runI < edgeRuns.size(); runI++)
	{
		MEdgeRun &run = edgeRuns[runI];

		for (int edgeI = 0; edgeI < run.size(); edgeI++)
		{
			run[edgeI]->secondaryUnmark();
		}
	}


	return 0;
}


int MMesh::extrudeMarkedEdges(MVertexVectorAdjustList &adjusts, MVertexAttribVectorAdjustList &attribAdjusts, int numSegments, bool useExistingTextures,
						bool bMarkExtrudedEdges)
{
	assertFinalised();

	Array<MEdgeRun> edgeRuns;

	int discoverResult = discoverExtrudeableEdgeRuns( edgeRuns );

	if ( discoverResult != 0 )
	{
		return discoverResult;
	}

	adjusts.clear();
	attribAdjusts.clear();


	// Generate faces

	double vInc = 1.0 / (double)numSegments;

	//FOR EACH EDGE RUN:
	for (int i = 0; i < edgeRuns.size(); i++)
	{
		MEdgeRun &run = edgeRuns[i];


		//make sure the edge run has the correct order
		if ( run.isEdge0InOrder() )
		{
			run.flip();
		}


		//generate a vertex list
		MVertexList runVertices;
		MVertexAttribList runAttribs;
		if ( useExistingTextures )
		{
			run.extractVerticesAndAttribs( runVertices, runAttribs );
		}
		else
		{
			run.extractVertices( runVertices );
		}


		//counts and sizes needed
		int numRunVertices = runVertices.size();
		int numFacesAcross = run.size();
		int numVerticesAcross = numFacesAcross + 1;

		//generate a texture co-ordinate list
		Array<Point2f> texCoords;
		Array<double> uTexCoords;
		if ( useExistingTextures )
		{
			texCoords.reserve( runAttribs.size() );
			for (int j = 0; j < runAttribs.size(); j++)
			{
				texCoords.push_back( runAttribs[j]->getPoint() );
			}
		}
		else
		{
			//generate texture co-ordinates
			run.generateUnitCumulativeLengthArray( uTexCoords );
		}


		//copy the initial run of vertices
		Array<MVertex*> vertices;
		vertices.reserve( numVerticesAcross  *  ( numSegments + 1 ) );
		vertices.extend( runVertices );

		//repeat the last one if the run is closed
		if ( run.isClosed() )
		{
			MVertex *first = vertices.front();
			vertices.push_back( first );
		}


		double v = vInc;
		for (int j = 0; j < numSegments; j++)
		{
			int sizeAtRunStart = vertices.size();

			for (int k = 0; k < numRunVertices; k++)
			{
				MVertex *vertex = addVertex( runVertices[k]->getPosition() );
				vertices.push_back( vertex );
				adjusts.push_back( MVertexVectorAdjust( vertex, vertex->getPosition(), v ) );
			}
			v += vInc;

			if ( run.isClosed() )
			{
				MVertex *first = vertices[sizeAtRunStart];
				vertices.push_back( first );
			}
		}

		Array<MVertex*> verts;
		Array<MVertexAttrib> attribs;

		verts.resize( 4 );
		attribs.resize( 4 );


		MVertexAttribList rowAttribs;
		if ( useExistingTextures )
		{
			rowAttribs.reserve( numFacesAcross * 2 );
		}

		double prevV = 0.0;
		v = vInc;
		for (int j = 0; j < numSegments; j++)
		{
			rowAttribs.clear();
			for (int k = 0; k < numFacesAcross; k++)
			{
				verts[0] = vertices[ j * numVerticesAcross  +  k ];
				verts[1] = vertices[ j * numVerticesAcross  +  k + 1 ];
				verts[2] = vertices[ ( j + 1 ) * numVerticesAcross  +  k + 1 ];
				verts[3] = vertices[ ( j + 1 ) * numVerticesAcross  +  k ];

				int materialID = -1;
				if ( useExistingTextures )
				{
					materialID = run[k]->getFaceA()->getMaterialID();
				}

				if ( useExistingTextures )
				{
					attribs[0] = MVertexAttrib( texCoords[ k * 2 ], materialID );
					attribs[1] = MVertexAttrib( texCoords[ k * 2 + 1 ], materialID );
					attribs[2] = MVertexAttrib( texCoords[ k * 2 + 1 ], materialID );
					attribs[3] = MVertexAttrib( texCoords[ k * 2 ], materialID );
				}
				else
				{
					attribs[0] = MVertexAttrib( Point2f( uTexCoords[k], prevV ), -1 );
					attribs[1] = MVertexAttrib( Point2f( uTexCoords[ k + 1 ], prevV ), -1 );
					attribs[2] = MVertexAttrib( Point2f( uTexCoords[ k + 1 ], v ), -1 );
					attribs[3] = MVertexAttrib( Point2f( uTexCoords[k], v ), -1 );
				}

				MFace *f = addFace( verts, attribs, materialID );

				if ( useExistingTextures )
				{
					rowAttribs.push_back( f->getVertexAttrib( verts[2] ) );
					rowAttribs.push_back( f->getVertexAttrib( verts[3] ) );
				}
			}

			if ( useExistingTextures )
			{
				attribAdjusts.push_back( MVertexAttribVectorAdjust( rowAttribs[0], rowAttribs[0]->getPoint(), v ) );
				for (int x = 1; x < rowAttribs.size(); x++)
				{
					if ( rowAttribs[x]  !=  rowAttribs[ x - 1 ] )
					{
						attribAdjusts.push_back( MVertexAttribVectorAdjust( rowAttribs[x], rowAttribs[x]->getPoint(), v ) );
					}
				}
				if ( rowAttribs.back()  !=  rowAttribs.front() )
				{
					attribAdjusts.push_back( MVertexAttribVectorAdjust( rowAttribs.back(), rowAttribs.back()->getPoint(), v ) );
				}
			}

			prevV = v;
			v += vInc;
		}

		if ( bMarkExtrudedEdges )
		{
			for (int edgeI = 0; edgeI < numFacesAcross; edgeI++)
			{
				int offset = numSegments * numVerticesAcross;
				MEdge *edge = vertices[ offset + edgeI ]->findEdgeTo( vertices[ offset + edgeI + 1 ], MFINDPOLICY_RETURNNULL );
				gs_assert( edge != NULL, "MMesh::extrudeMarkedEdges(): could not get extruded edge\n" );
				edge->edgeMark();
			}
		}
	}


	// If the extruded edges are to be marked, unmark all the original edges.
	for (int runI = 0; runI < edgeRuns.size(); runI++)
	{
		MEdgeRun &run = edgeRuns[runI];

		if ( bMarkExtrudedEdges )
		{
			for (int edgeI = 0; edgeI < run.size(); edgeI++)
			{
				run[edgeI]->edgeUnmark();
			}
		}
	}


	//no need to compactAll(): no items are deleted
	finalise();


	return 0;
}


int MMesh::fillMarkedEdges()
{
	assertFinalised();

	Array<MEdgeRun> edgeRuns;

	int discoverResult = discoverExtrudeableEdgeRuns( edgeRuns );

	if ( discoverResult != 0 )
	{
		return discoverResult;
	}


	// Generate faces

	// STAGE ONE:
	//FOR EACH CLOSED EDGE RUN:
	for (int i = 0; i < edgeRuns.size(); i++)
	{
		MEdgeRun &run = edgeRuns[i];

		if ( run.isClosed() )
		{
			//make sure the edge run has the correct order
			if ( run.isEdge0InOrder() )
			{
				run.flip();
			}

			//generate a vertex list
			MVertexList runVertices;
			run.extractVertices( runVertices );


			if ( runVertices.size() >= 3 )
			{
				addFaceAutoTexture( runVertices, false, -1 );
			}
		}
	}


	// STAGE TWO:
	// Gather the open edge runs (up to 2)

	MEdgeRun *openRuns[2] = { NULL, NULL };

	MVertexList faceVertices;

	bool bTooManyOpenRuns = false;

	for (int i = 0; i < edgeRuns.size(); i++)
	{
		MEdgeRun &run = edgeRuns[i];

		if ( !run.isClosed() )
		{
			if ( openRuns[1] != NULL )
			{
				// Already got 2 runs; cannot use another
				bTooManyOpenRuns = true;
				break;
			}

			//make sure the edge run has the correct order
			if ( run.isEdge0InOrder() )
			{
				run.flip();
			}

			if ( openRuns[0] == NULL )
			{
				openRuns[0] = &run;
			}
			else if ( openRuns[1] == NULL )
			{
				openRuns[1] = &run;
			}
			else
			{
				gs_assert_not_reached( "MMesh::fil(): could not find slot for open edge run\n" );
			}
		}
	}


	int result = 0;

	if ( bTooManyOpenRuns )
	{
		result = -2;
	}
	else
	{
		if ( openRuns[0] != NULL )
		{
			// Extract vertices from run 0
			MVertexList run0Vertices;
			openRuns[0]->extractVertices( run0Vertices );

			if ( openRuns[1] != NULL )
			{
				// Extract vertices from run 1
				MVertexList run1Vertices;
				openRuns[1]->extractVertices( run1Vertices );

				// Add to run 0 vertices
				run0Vertices.extend( run1Vertices );
			}

			if ( run0Vertices.size() >= 3 )
			{
				addFaceAutoTexture( run0Vertices, false, -1 );
			}
		}

		result = 0;
	}


	// Unmark run edges.
	for (int runI = 0; runI < edgeRuns.size(); runI++)
	{
		MEdgeRun &run = edgeRuns[runI];

		for (int edgeI = 0; edgeI < run.size(); edgeI++)
		{
			run[edgeI]->edgeUnmark();
		}
	}

	//no need to compactAll(): no items are deleted
	finalise();


	return result;
}



int MMesh::riftMarkedEdges(MVertexVectorAdjustList &adjusts, MEdgeRiftStyle style, int numSegments, bool fill, bool pinchAtMarkedVertices)
{
	assertFinalised();

	adjusts.clear();

	//number of segments is irrelavent if no faces are to be created
	if ( !fill )
	{
		numSegments = 1;
	}

/*
	*------*------*------*			*------*------*------*
	|      |      |      |			|      |      |      |
	|      |      |      |			|     /        \     |
	|      |      |      |			|  _-*----------*-_  |
	|      |      |      |			|_/  |\        /|  \_|
	*------*======*------*			*    | \      / |    *
	|      H      H      |			|    |  *----*  |    |
	|      H      H      |	->		|    |  |    |  |    |
	|      H      H      |			|    |  |    |  |    |
	|      H      H      |			|    |  *----*  |    |
	*------*======*------*			*_   | /      \ |   _*
	|      |      |      |			| \_ |/        \| _/ |
	|      |      |      |			|   -*----------*-   |
	|      |      |      |			|     \        /     |
	|      |      |      |			|      |      |      |
	*------*------*------*			*------*------*------*

	ABOVE: circular/closed edge rift: replaces the marked edges wtih quads


	BELOW: linear/open edge rift: replaces the marked edges wtih quads,
	except the ones at the ends which are replaced with triangles

	*-----*-----*-----*-----*		*-----*-----*-----*-----*
	|     |     |     |     |		|     |     |     |     |
	|     |     |     |     |		|   _-*-----*-----*-_   |
	|     |     |     |     |		|_--  |     |     |  --_|
	*=====*=====*=====*=====* ->	*_    |     |     |    _*
	|     |     |     |     |		| --_ |     |     | _-- |
	|     |     |     |     |		|    -*-----*-----*-    |
	|     |     |     |     |		|     |     |     |     |
	*-----*-----*-----*-----*		*-----*-----*-----*-----*


	each vertex which has TWO incident mark edges is 'split' in two
*/

	//STAGE ONE
	//make sure operation can take place
	//make sure that none of the marked edges are boundary edges
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  edges[i]->isBoundary() )
		{
			return -1;
		}
	}


	//STAGE TWO
	//generate edge runs
	Array<MEdgeRun> runs;

	discoverMarkedEdgeRuns( runs );


	//STAGE THREE
	//process the runs
	//storage used in the loops
	MVertexList vertices;			//the vertices that are on the edge run
	Array<MVertexList> rift;		//the lists at the front and back are the two
											//parallel lists of vertices that form the
											//edges of the rift

	rift.resize( numSegments + 1 );

	//for each run
	for (int i = 0; i < runs.size(); i++)
	{
		MEdgeRun &run = runs[i];

		if ( run.size() < 2 )
		{
			continue;
		}

		vertices.clear();
		run.extractVertices( vertices );

		int riftSize = run.isClosed()  ?  vertices.size() + 1  :  vertices.size();
		rift.front().clear();
		rift.back().clear();
		rift.front().reserve( riftSize );
		rift.back().reserve( riftSize );


		Array<MFaceList> neighbourhoodFaceLists;
		neighbourhoodFaceLists.resize( vertices.size() );

		//FIRST:
		//for each vertex, generate a list of faces along the B-side of the run
		//these faces must later replace these vertices with newly created ones
		//this list is generated now, because if the vertex replacing is done
		//as the vertices are processed later, circular edge runs will cause a crash
		//since the last marked edge will NOT link the last and first vertices

		/*
					(1)								(2)
		A-SIDE		B-SIDE			A-SIDE		B-SIDE
				v0 *-----*-----*				v0 *-----*-----*
					H..../...._-|					H..../...._-
					H.../..._-..|					H.../..._-
				e0 H../.._-....|				e0 H../.._- e2
					H./._-......|					H./._-
					H/_-........|					H/_-
				v1 *-----------*				v1 *
					H\-_........|					H\-_
					H.\.-_......|					H.\.-_
				e1 H..\..-_....|				e1 H..\..-_ e3
					H...\...-_..|					H...\...-_
					H....\....-_|					H....\....-_
				v2 *-----*-----*				v2 *-----*-----*

				... = face
				H   = marked edge
				-	 = edge

		The normal way of discovering which faces incident to v1 are on the
		B-SIDE of e0 and e1, is to discover the faces and edges that are incident
		to v1, starting at e0, and walking round to e1, making a list of faces
		passed through along the way. This works for (1).
		However, this will not work for (2), since there are no faces between e2
		and e3. The solution: walk around from e0 generating list 0, then
		around from e1, generating list 1. We have two face lists. Now, compare
		the last face in list 0, with the first face in list 1. If they are the same,
		then the lists are duplicates (albeit in reversed order) of eachother,
		and only one needs to be used. Else, concatenate the lists.
		*/
		MFaceList neighbourhood2;
		for (int j = 0; j < vertices.size(); j++)
		{
			MVertex *v = vertices[j];

			//open runs result in rifts that end in triangles (see FIRST comment block)
			//if the run is open, and j indexes the first or the last vertex,
			//then the vertex is NOT split in two, so no face list is needed
			if ( run.isClosed()  ||  ( j > 0  &&  j < run.size() ) )
			{
				bool pinchAtV = v->isVertexMarked()  &&  pinchAtMarkedVertices;
				neighbourhood2.clear();

				MEdge *e, *neighbourhoodBoundaries[2];
				MFace *f;
				bool faceB;

				if ( !pinchAtV )
				{
					//GENERATE LIST 0
					e = run[j];

					//determine which face of e is required: if the edge e is in the
					//same order as its vertices appear in the vertex list, then use faceB,
					//else use face A
					faceB = vertices.isEdgeInOrder( e, j );
					f = faceB  ?  e->getFaceB()  :  e->getFaceA();

					//discover
					v->discoverNeighbourhoodBetweenMarkedEdges( e, f, NULL, &neighbourhoodFaceLists[j], neighbourhoodBoundaries );
				}

				//detect the double cone case, where the discovery process will loop back on itself
				if ( neighbourhoodBoundaries[0] == neighbourhoodBoundaries[1]  ||  pinchAtV )
				{
					//have the rift boundaries meet at this point
					rift.front().push_back( v );
					rift.back().push_back( v );
				}
				else
				{
					//GENERATE LIST 1
					//this time use the edge previous to the current vertex
					int k = prevIndex( j, run.size() );
					e = run[k];
					//get face from e (see above)
					faceB = vertices.isEdgeInOrder( e, k );
					f = faceB  ?  e->getFaceB()  :  e->getFaceA();

					//discover
					v->discoverNeighbourhoodBetweenMarkedEdges( e, f, NULL, &neighbourhood2, NULL );

					//if the lists are not the same (see comment block above)
					if ( neighbourhood2.front() != neighbourhoodFaceLists[j].back() )
					{
						//concatenate the two lists
						neighbourhoodFaceLists[j].extend( neighbourhood2 );
					}

					//create the new vertex
					MVertex *newVertex = addVertex( v->getPosition() );

					rift.front().push_back( v );
					rift.back().push_back( newVertex );

					//create adjustments; the position/adjustment of v and the
					//new vertex depend on the rift style
					Point3 pos = v->getPosition();
					switch ( style )
					{
					case MEDGERIFTSTYLE_A:
						//rift on A-SIDE of edges, move v
						adjusts.push_back( MVertexVectorAdjust( v, pos, 1.0 ) );
						break;
					case MEDGERIFTSTYLE_B:
						//rift on B-SIDE of edges, move newVertex
						adjusts.push_back( MVertexVectorAdjust( newVertex, pos, 1.0 ) );
						break;
					case MEDGERIFTSTYLE_CENTRE:
						//rift is centred around edges, move both in opposite directions,
						//but half the amount
						adjusts.push_back( MVertexVectorAdjust( v, pos, -0.5 ) );
						adjusts.push_back( MVertexVectorAdjust( newVertex, pos, 0.5 ) );
						break;
					default:
						break;
					}
				}
			}
			else
			{
				rift.front().push_back( v );
				rift.back().push_back( v );
			}
		}


		//'split' the vertices in two, and attach the relevant faces to the new
		//vertices
		for (int j = 0; j < vertices.size(); j++)
		{
			MVertex *v0 = rift.front()[j];
			MVertex *v1 = rift.back()[j];


			//this vertex will have a list of faces associated with it
			if ( v0 != v1 )
			{
				MFaceList &facesForV = neighbourhoodFaceLists[j];
				for (int k = 0; k < facesForV.size(); k++)
				{
					facesForV[k]->replaceVertex( v0, v1, true, true, true );
				}
			}
		}

		//generate intermediate vertices if number of segments > 1
		if ( numSegments > 1 )
		{
			//clear and reserve space
			for (int j = 1; j < numSegments; j++)
			{
				rift[j].clear();
				rift[j].reserve( riftSize );
			}

			//generate intermediate vertices
			double initialT = 0.0, tInc = 1.0 / (double)numSegments;

			switch ( style )
			{
			case MEDGERIFTSTYLE_A:
				initialT = 1.0 - tInc;
				tInc = -tInc;
				break;
			case MEDGERIFTSTYLE_B:
				initialT = tInc;
				break;
			case MEDGERIFTSTYLE_CENTRE:
				initialT = -0.5 + tInc;
				break;
			default:
				initialT = 0.0;
				break;
			}

			for (int j = 0; j < rift.front().size(); j++)
			{
				MVertex *va = rift.front()[j];
				MVertex *vb = rift.back()[j];

				if ( va == vb )
				{
					for (int k = 1; k < numSegments; k++)
					{
						rift[k].push_back( va );
					}
				}
				else
				{
					double t = initialT;
					for (int k = 1; k < numSegments; k++)
					{
						MVertex *v = addVertex( va->getPosition() );
						const Point3 &pos = va->getPosition();

						adjusts.push_back( MVertexVectorAdjust( v, pos, t ) );

						rift[k].push_back( v );
						t += tInc;
					}
				}
			}
		}


		//if the edge run is closed, repeat the first vertices at the end
		if ( run.isClosed() )
		{
			MVertex *v;

			for (int j = 0; j <= numSegments; j++)
			{
				v = rift[j].front();
				rift[j].push_back( v );
			}
		}


		//if the rift needs to be filled with faces
		if ( fill )
		{
			//need length Array for texture coordinates
			Array<double> lengthArray;

			run.generateUnitCumulativeLengthArray( lengthArray );

			//the u-coordinated of an end point depends upon the style
			double endPointU;
			switch ( style )
			{
			case MEDGERIFTSTYLE_A:
				//v is moved, newVertex remains stationary (see above)
				//the stationary vertex (newVertex) is at u=1.0, so:
				endPointU = 1.0;
				break;
			case MEDGERIFTSTYLE_B:
				//newVertex is moved, v remains stationary (see above)
				//the stationary vertex (v) is at u=0.0, so:
				endPointU = 0.0;
				break;
			case MEDGERIFTSTYLE_CENTRE:
				//end point is at centre, 'between' v and newVertex; use 0.5 for u
				endPointU = 0.5;
				break;
			default:
				endPointU = 0.0;
				break;
			}


			Array<MVertex*> verts;
			verts.reserve( 4 );
			Array<Point2f> texCoords;
			texCoords.reserve( 4 );


			double uInc = 1.0 / (double)numSegments, u0 = 0.0, u1 = uInc;
			for (int j = 0; j < numSegments; j++)
			{
				//generate the quads to fill the rift
				for (int k = 0; k < run.size(); k++)
				{
					verts.clear();
					texCoords.clear();

					MVertex *va = rift[j][k];
					MVertex *vb = rift[j+1][k];
					MVertex *vc = rift[j+1][k+1];
					MVertex *vd = rift[j][k+1];

					if ( va == vb )
					{
						verts.push_back( va );
						texCoords.push_back( Point2f( endPointU, lengthArray[k] ) );
					}
					else
					{
						verts.push_back( va );
						verts.push_back( vb );
						texCoords.push_back( Point2f( u0, lengthArray[k] ) );
						texCoords.push_back( Point2f( u1, lengthArray[k] ) );
					}

					if ( vc == vd )
					{
						verts.push_back( vc );
						texCoords.push_back( Point2f( endPointU, lengthArray[k+1] ) );
					}
					else
					{
						verts.push_back( vc );
						verts.push_back( vd );
						texCoords.push_back( Point2f( u1, lengthArray[k+1] ) );
						texCoords.push_back( Point2f( u0, lengthArray[k+1] ) );
					}

					if ( verts.size() >= 3 )
					{
						addFace( verts, texCoords, -1 );
					}
				}

				u0 = u1;
				u1 += uInc;
			}
		}
	}


	//STAGE FOUR
	//finish up: secondary unmark vertices and edges
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->secondaryUnmark();
	}
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->secondaryUnmark();
	}


	//clean up
	compactAll();
	finalise();

	return 0;
}


void MMesh::slideMarkedEdges(MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts, int slideVersion)
{
	Array<MEdgeRun> edgeRuns;

	// Generate the edge runs
	discoverMarkedEdgeRuns( edgeRuns );

	// Process each edge run
	for (int edgeRunI = 0; edgeRunI < edgeRuns.size(); edgeRunI++)
	{
		slideMarkedEdgeRun( edgeRuns[edgeRunI], adjusts, attribAdjusts, slideVersion );
	}

	//finish up
	finalise();
}

void MMesh::slideMarkedEdgeRun(MEdgeRun &run, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts, int slideVersion)
{
	if ( run.size() == 1 )
	{
		MEdge *e = run[0];
		MVertex *v0 = e->getVertexA();
		MVertex *v1 = e->getVertexB();

		MEdge *eNegative = NULL, *ePositive = NULL;

		v0->slideEdgeRunStartVertex( e, eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
		v1->slideEdgeRunEndVertex( e, eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
	}
	else if ( run.size() >= 2 )
	{
		if ( run.isClosed() )
		{
			MEdge *eNegative = NULL, *ePositive = NULL;

			MEdge *ePrev = run.back();
			MEdgeRun::VertexIterator iter = run.verticesBegin();
			for (int edgeI = 0; edgeI < run.size(); edgeI++)
			{
				iter->slideEdgeRunInternalVertex( ePrev, eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
				++iter;
				ePrev = run[edgeI];
			}
		}
		else
		{
			MEdge *eNegative = NULL, *ePositive = NULL;

			MEdgeRun::VertexIterator iter = run.verticesBegin();
			iter->slideEdgeRunStartVertex( run[0], eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
			++iter;
			for (int edgeI = 0; edgeI < run.size() - 1; edgeI++)
			{
				iter->slideEdgeRunInternalVertex( run[edgeI], eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
				++iter;
			}
			iter->slideEdgeRunEndVertex( run.back(), eNegative, ePositive, adjusts, attribAdjusts, slideVersion );
		}
	}
}






int MMesh::collapseMarkedEdges(bool lockMarkedVertices)
{
	assertFinalised();

	int successCount, failureCount;

	do
	{
		//initialise counts
		successCount = failureCount = 0;

		//collapse all marked (and not yet destroyed) edges
		for (int i = 0; i < edges.size(); i++)
		{
			MEdge *e = edges[i];

			if ( !e->isDestroyed()  &&  e->isEdgeMarked() )
			{
				MVertex *v = e->collapse( false, lockMarkedVertices );

				if ( v != NULL )
				{
					successCount++;
				}
				else
				{
					failureCount++;
				}
			}
		}

		//attempt another pass, if some failed and some succeeded;
		//if all failed, then nothing will have changed, so no point retrying
	} while ( successCount > 0  &&  failureCount > 0 );


	compactAll();
	finalise();

	//determine result code
	int result =  failureCount > 0  ?  -1  :  0;
	return result;
}

int MMesh::collapseMarkedFaces()
{
	assertFinalised();

	int successCount, failureCount;

	do
	{
		//initialise counts
		successCount = failureCount = 0;

		//collapse all marked (and not yet destroyed) faces
		for (int i = 0; i < faces.size(); i++)
		{
			MFace *f = faces[i];

			if ( !f->isDestroyed()  &&  f->isFaceMarked() )
			{
				MVertex *v = f->collapseToVertex();

				if ( v != NULL )
				{
					successCount++;
				}
				else
				{
					failureCount++;
				}
			}
		}

		//attempt another pass, if some failed and some succeeded;
		//if all failed, then nothing will have changed, so no point retrying
	} while ( successCount > 0  &&  failureCount > 0 );



	compactAll();
	finalise();

	//determine result code
	int result =  failureCount > 0  ?  -1  :  0;
	return result;
}







MVertex * MMesh::knifeComputeSnapVertex(const MKnifeTarget &target)
{
	if ( target.constraint == MKnifeTarget::NOTHING )
	{
		return NULL;
	}
	else if ( target.constraint == MKnifeTarget::VERTEX )
	{
		return pickVertex( target.pick );
	}
	else if ( target.constraint == MKnifeTarget::EDGE )
	{
		Point3 closestPoint;
		MEdge *edge = pickEdge( target.pick, closestPoint );

		if ( edge != NULL )
		{
			double t;
			edge->getSegment().closestPointTo( closestPoint, t );

			t = clamp( t, MMESH_KNIFE_EDGE_SPLIT_MARGIN, 1.0 - MMESH_KNIFE_EDGE_SPLIT_MARGIN );

			MEdge *newEdge;
			return edge->split( t, newEdge );
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		gs_assert_not_reached( "MMesh::knifeSnap(): invalid constraints\n" );
		return NULL;
	}
}

void MMesh::knifeSnap(const MKnifeTarget &target, MVertex *&vertex, Point3 &nearPoint, Point3 &farPoint, Point2 &screenPoint)
{
	vertex = knifeComputeSnapVertex( target );

	Matrix4 screenToWorldMatrix = target.pick.projection.getWorldToScreenMatrix().inverse();

	if ( vertex == NULL )
	{
		screenPoint = target.pick.screenPos;
		nearPoint = screenToWorldMatrix.transformHomogeneous( Point3( target.pick.screenPos, 0.0 ) );
		farPoint = screenToWorldMatrix.transformHomogeneous( Point3( target.pick.screenPos, 1.0 ) );
	}
	else
	{
		farPoint = vertex->getPosition();
		screenPoint = target.pick.projection.getWorldToScreenMatrix().transformHomogeneous2d( farPoint );
		nearPoint = screenToWorldMatrix.transformHomogeneous( Point3( screenPoint, 0.0 ) );
	}
}

void MMesh::knifeComputePlanes(const Point3 &startNear, const Point3 &startFar, const Point3 &endNear, const Point3 &endFar, Plane &cutPlane, Plane boundingPlanes[2])
{
	Vector3 startVector = startFar - startNear;
	Vector3 endVector = endFar - endNear;

	Vector3 startToEnd = endFar - startFar;

	cutPlane = Plane( startNear, startToEnd, startVector );
	boundingPlanes[0] = Plane( startNear, cutPlane.n, -startVector );
	boundingPlanes[1] = Plane( endNear, cutPlane.n, endVector );
}

void MMesh::knife(const MKnifeTarget &startTarget, const MKnifeTarget &endTarget, bool faceMarkedOnlyFlag, const BackfaceCull *cull, bool markCutEdges)
{
	assertFinalised();

	/*
	THE KNIFE ALGORITHM

	A. Cut edges that intersect the plane, keeping a list of the cut
	   vertices that resulted from splitting the edges.
	B. Add to the list of cut vertices, any vertices that lie on the plane,
	   and any vertices that are used as end points.
	C. For each face that has cut vertices incident to it, perform splits to
	   link the vertices appropriately.


	1. INITIALISE FACES
		Find all faces that are bisected by the cut plane, are within
		the boundaries of the cut, and are front facing and/or marked.
		Initialise them by secondary marking them (secondary marking is used to
		indicate that they are to be cut), and calling MFace::knifeInitialise()
		which initialises data structures used by the knife code.
	2. HANDLE THE END POINTS
	   If there are vertices which are set as end points, then call
	   MVertex::knife(), thus making them cut vertices
	   Mark the end points as visited
	   Mark all edges incident to end points as visited (done in MVertex::knife()).
	3. KNIFE THRU VERTICES ON CUT PLANE
	   For each vertex that is on the cut plane, but has not already been visited
	   (see 2), call MVertex::knife(), to make it a cut vertex
	   Mark all edges incident to vertex as visited (done in MVertex::knife()).
	4. FIND EDGES TO CUT
	   For each edge that crosses the cut plane, and has not already been visited,
	   add it to a list of edges to split
	5. CUT THE EDGES
	   For each edge in the list created in 4, knife it using MEdge::knifeSplit()
	6. SPLIT THE FACES
	   For each secondary marked face, perform the necessary splits to link the
	   cut vertices. The algorithm for this step is described in MFace::knife()

	Finish up by secondary unmarking all faces, all edges, and the end points


	HOW FPU INACCURACY IS DEALT WITH
	It isn't.
	The FPU is treated as if it is 100% accurate.
	The potential problem that can be caused by this is that if a vertex
	is marked as a cut vertex, either due to it lying on the plane, or due
	to it being and end point, one of its indicident edges may get split,
	generating an additional (spurious) cut vertex, messing up the face splits.
	The solution is to find cut vertices by finding vertices, that lie on the
	cut plane, or that are end points, use these as cut vertices,
	but prevent any of their incident edges from generating cut vertices.
	This is the reason that indicent edges are marked as visited in stages
	2 and 3.
	*/



	//update the bounding box to check if cutPlane even intersects the mesh
	const BBox3 & bbox = getBoundingBox();


	// Snap the targets
	Point3 startNear, startFar, endNear, endFar;
	Point2 startScreen, endScreen;
	MVertex *startVertex, *endVertex;

	knifeSnap( startTarget, startVertex, startNear, startFar, startScreen );
	knifeSnap( endTarget, endVertex, endNear, endFar, endScreen );


	// Compute the knife planes
	Plane cutPlane, boundingPlanes[2];
	knifeComputePlanes( startNear, startFar, endNear, endFar, cutPlane, boundingPlanes );



	//cutPlane must bisect the mesh, or else no cuts can occur, in which
	//case, exit
	if ( bbox.side( cutPlane ) != SIDE_BOTH )
	{
		return;
	}

	//make sure the bounding box is within the bounding planes too
	if ( bbox.side( boundingPlanes[0] ) == SIDE_NEGATIVE  ||  bbox.side( boundingPlanes[1] ) == SIDE_NEGATIVE )
	{
		return;
	}


	//STAGE ONE
	//find all faces which are to be cut, and initialise them for cutting
	//also, secondary mark these faces; secondary marking indicates
	//that the face is to be cut, in the context of this function

	for (int i = 0; i < faces.size(); i++)
	{
		MFace *f = faces[i];

		bool cutFaceFlag = false;

		//for each face which can be cut
		cutFaceFlag = f->isFaceMarked()  ||  !faceMarkedOnlyFlag;

		if ( cutFaceFlag  &&  cull != NULL )
		{
			cutFaceFlag = f->isFrontFace( cull );
		}


		if ( cutFaceFlag )
		{
			//if f is cut by the cut plane
			if ( f->side( cutPlane, true ) == SIDE_BOTH )
			{
				//if f is within the bounding planes too
				if ( f->side( boundingPlanes[0], true ) != SIDE_NEGATIVE  &&  f->side( boundingPlanes[1], true ) != SIDE_NEGATIVE )
				{
					//initialise for knife operation
					f->knifeInitialise();
					//mark to cut
					f->secondaryMark();
				}
			}
		}
	}



	//STAGE TWO
	//Add cut points at the end points (if there are any)
	//mark end points as visited
	if ( startVertex != NULL )
	{
		startVertex->knife();
		startVertex->secondaryMark();
	}
	if ( endVertex != NULL )
	{
		endVertex->knife();
		endVertex->secondaryMark();
	}


	//STAGE THREE
	//add cut points to vertices that lie on the cut plane
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i];

		bool cutVertexFlag = false;

		//for each vertex that may be cut
		//(a vertex which is secondary marked has already been visited; see above
		//where the end points are dealt with)
		cutVertexFlag = ( v->isFaceMarked()  ||  !faceMarkedOnlyFlag )  &&  !v->isSecondaryMarked();

		if ( cutVertexFlag  &&  cull != NULL )
		{
			cutVertexFlag = v->isFrontFacing( cull );
		}

		if ( cutVertexFlag )
		{
			Point3 position = v->getPosition();
			//if the vertex lies on the plane
			//NOTE: EXACT MATHS USED, as opposed to taking error into account,
			//as this is also the policy used for edge cutting (see STAGE
			//TWO above)
			if ( cutPlane.side( position, false )  ==  SIDE_ON )
			{
				//vertex inside bounding planes?
				if ( position >= boundingPlanes[0]  &&  position >= boundingPlanes[1] )
				{
					//cut the vertex
					v->knife();
				}
			}
		}
	}




	//STAGE FOUR
	//build a list of edges that must be cut

	//list of edges that need cutting
	Array<MEdge*> edgesToCut;

	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *e = edges[i];

		bool cutEdgeFlag = false;

		//for each edge that may be cut
		//(an edge which is secondary marked has already been visited;
		//vertices which are end points or are on the cut plane will have
		//been processed as cut points, which involved marking all incident
		//edges as visited)
		cutEdgeFlag = ( e->isFaceMarked()  ||  !faceMarkedOnlyFlag )  &&  !e->isSecondaryMarked();

		if ( cutEdgeFlag  &&  cull != NULL )
		{
			cutEdgeFlag = e->isFrontFacing( cull );
		}

		if ( cutEdgeFlag )
		{
			//intersects(): intersection must lie between endpoints:
			//cannot lie on.
			//this means that should an intersection lie on a vertex of a face,
			//two adjacent edges will not be cut
			//however, it also means that vertices must be 'cut' too:
			//see STAGE THREE below
			if ( e->intersects( cutPlane ) )
			{
				double t = e->computeIntersection( cutPlane );
				Point3 intersection = e->getPoint( t );

				//make sure that the intersection is within the cut planes
				if ( intersection >= boundingPlanes[0]  &&  intersection >= boundingPlanes[1] )
				{
					//add the edge to the edge list
					edgesToCut.push_back( e );
				}
			}
		}
	}



	//STAGE FIVE
	//cut the edges
	for (int i = 0; i < edgesToCut.size(); i++)
	{
		//for each edge that is to be cut
		MEdge *e = edgesToCut[i];

		double t;

		//compute the intersection
		e->intersectStrict( cutPlane, t );
		//cut the edge
		e->knifeSplit( t );
	}



	//STAGE SIX
	//knife the faces
	//Record the number of faces, as the splits performed by MFace::knife()
	//will increase the number of faces in @faces.
	int numFaces = faces.size();
	for (int i = 0; i < numFaces; i++)
	{
		MFace *f = faces[i];
		//if the face is marked for cutting
		if ( f->isSecondaryMarked() )
		{
			//Secondary unmark; do this first; when the face is split by
			//MFace::knife() all of the resulting faces will also be
			//secondary marked. Unmarking them ensures that there will be
			//no faces that will need unmarking later.
			f->secondaryUnmark();
			//knife it
			f->knife( cutPlane, startScreen, endScreen, startTarget.pick.projection.getWorldToScreenMatrix(), markCutEdges );
			//finish up
			f->knifeFinish();
		}
	}



	//FINISH UP
	//secondary unmark all edges
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->secondaryUnmark();
	}

	//deal with the end points
	if ( startVertex != NULL )
	{
		startVertex->secondaryUnmark();
	}
	if ( endVertex != NULL )
	{
		endVertex->secondaryUnmark();
	}

	compactAll();
	finalise();
}





void MMesh::markEdgeRingTraverseThruFace(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes, MEdge *e, MFace *f, std::list<MEdge*> &edgeStack)
{
	//f may be NULL
	if ( f != NULL )
	{
		//only pass thru a marked face if desired
		if ( !( stopAtMarkedFaces  &&  f->isFaceMarked() ) )
		{
			//only pass thru a previously visited face if desired
			if ( !f->isSecondaryMarked()  ||  traverseFacesMultipleTimes )
			{
				//the the opposite edge to e, in f
				MEdge *oppositeEdge = f->quadGetOppositeEdge( e );

				if ( oppositeEdge != NULL )
				{
					//only process e if it hasn't already been marked
					if ( !oppositeEdge->isEdgeMarked() )
					{
						//e is visited; mark it
						oppositeEdge->edgeMark();
						//add it to the stack of edges to be processed
						edgeStack.push_back( oppositeEdge );

						if ( !traverseFacesMultipleTimes )
						{
							//mark as visited
							f->secondaryMark();
						}
					}
				}
			}
		}
	}
}

void MMesh::markEdgeLoopTraverseThruVertex(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes, MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack)
{
	//only pass thru a marked vertex if desired
	if ( !( stopAtMarkedVertices  &&  v->isVertexMarked() ) )
	{
		//only pass thru a previously visited vertex if desired
		if ( !v->isSecondaryMarked()  ||  traverseVerticesMultipleTimes )
		{
			//the the opposite edge to e, in v
			MEdge *oppositeEdge = v->markEdgeLoopGetOppositeEdge( e );

			if ( oppositeEdge != NULL )
			{
				//only process e if it hasn't already been marked
				if ( !oppositeEdge->isEdgeMarked() )
				{
					//e is visited; mark it
					oppositeEdge->edgeMark();
					//add it to the stack of edges to be processed
					edgeStack.push_back( oppositeEdge );

					if ( !traverseVerticesMultipleTimes )
					{
						//mark as visited
						v->secondaryMark();
					}
				}
			}
		}
	}
}

void MMesh::markEdgeBoundaryTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack)
{
	//the next boundary edge
	MEdge *nextEdge = v->getNextBoundaryEdge( e );

	if ( nextEdge != NULL )
	{
		//only process @nextEdge if it hasn't already been visited
		if ( !nextEdge->isEdgeMarked() )
		{
			//add it to the stack of edges to be processed
			edgeStack.push_back( nextEdge );
			//mark @nextEdge
			nextEdge->edgeMark();
		}
	}
}


void MMesh::discoverEdgeRingTraverseThruFace(MEdge *e, MFace *f, std::list<MEdge*> &edgeStack, MEdgeList &ring)
{
	//f may be NULL
	if ( f != NULL )
	{
		//the opposite edge to e, in f
		MEdge *oppositeEdge = f->quadGetOppositeEdge( e );

		if ( oppositeEdge != NULL )
		{
			//only process e if it hasn't already been visited
			if ( !oppositeEdge->isSecondaryMarked() )
			{
				//e is visited; mark it
				oppositeEdge->secondaryMark();
				//add it to the stack of edges to be processed
				edgeStack.push_back( oppositeEdge );
				//add to the list of edges that make up the edge ring
				ring.push_back( oppositeEdge );
			}
		}
	}
}

void MMesh::discoverEdgeLoopTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack, MEdgeList &loop)
{
	//the opposite edge to e, in v
	MEdge *oppositeEdge = v->markEdgeLoopGetOppositeEdge( e );

	if ( oppositeEdge != NULL )
	{
		//only process e if it hasn't already been visited
		if ( !oppositeEdge->isSecondaryMarked() )
		{
			//e is visited; mark it
			oppositeEdge->secondaryMark();
			//add it to the stack of edges to be processed
			edgeStack.push_back( oppositeEdge );
			//add to the list of edges that make up the edge loop
			loop.push_back( oppositeEdge );
		}
	}
}

void MMesh::discoverEdgeBoundaryTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack, MEdgeList &boundary)
{
	//the next boundary edge
	MEdge *nextEdge = v->getNextBoundaryEdge( e );

	if ( nextEdge != NULL )
	{
		//only process @nextEdge if it hasn't already been visited
		if ( !nextEdge->isSecondaryMarked() )
		{
			//@nextEdge is visited; mark it
			nextEdge->secondaryMark();
			//add it to the stack of edges to be processed
			edgeStack.push_back( nextEdge );
			//add to the list of edges that make up the boundary
			boundary.push_back( nextEdge );
		}
	}
}



bool MMesh::discoverMarkedEdgeRingTraverseThruFace(MEdge *seed, MFace *f, BandsawRing &ring, MFaceList *ringFaces)
{
	MEdge *e = seed;
	bool eInOrder = f->isEdgeInOrder( e );

	// @flipFlag is used to keep track of the edge direction
	bool flipFlag = false;

	ring.push_back( BandsawRingEntry( seed, flipFlag ) );

	while ( e != NULL  &&  f != NULL )
	{
		// The opposite edge to @e, in @f
		MEdge *eNext = f->getOtherMarkedEdge( e );

		if ( eNext != NULL )
		{
			// Only process @eNext if it hasn't already been visited
			if ( !eNext->isSecondaryMarked() )
			{
				// Need to keep track of the ordering of the edges
				bool eNextInOrder = f->isEdgeInOrder( eNext );

				if ( eInOrder == eNextInOrder )
				{
					// @e and @eNext have the same direction w.r.t. @f; one will need to be flipped, and the other not,
					// in order to ensure that the bandsaw is applied to the same 'side' of each edge in the ring; flip the
					// 'flip flag'
					flipFlag = !flipFlag;
				}

				// Add @eNext along with @flipFlag to @ring
				ring.push_back( BandsawRingEntry( eNext, flipFlag ) );
				if ( ringFaces != NULL )
				{
					ringFaces->push_back( f );
				}

				// Mark @eNext as visited
				eNext->secondaryMark();

				// @eNext becomes @e
				e = eNext;
				// Get the next face
				MFace *fNext = e->getOppositeFace( f );

				// The order of eNext w.r.t. @f will be opposite to the order of @eNext w.r.t. @fNext
				eInOrder = !eNextInOrder;

				// @fNext becomes @f
				f = fNext;
			}
			else if ( eNext == seed )
			{
				// Special case; looped back to @seed; the ring is closed.
				if ( ringFaces != NULL )
				{
					ringFaces->push_back( f );
				}

				return true;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return false;
}

bool MMesh::discoverMarkedEdgeRing(MEdge *seed, BandsawRing &ring, MFaceList *ringFaces)
{
	// Mark as visited so that the graph traversal algorithm cannot loop back onto @seed
	seed->secondaryMark();


	bool closedFlag = discoverMarkedEdgeRingTraverseThruFace( seed, seed->getFaceA(), ring, ringFaces );

	// If discoverBandsawEdges() produced an open edge ring, and there are faces on the opposite side of @seed, then we need to
	// discover in the opposite direction to acquire the full bandsaw ring
	if ( seed->getFaceB() != NULL  &&  !closedFlag )
	{
		BandsawRing ringB;
		MFaceList ringBFaces;
		discoverMarkedEdgeRingTraverseThruFace( seed, seed->getFaceB(), ringB, ringFaces != NULL  ?  &ringBFaces  :  NULL );


		ring.reverse();
		if ( ringFaces != NULL )
		{
			ringFaces->reverse();
		}
		// Remove @seed from end of @ring, as it occurs at the beginning of @ringB
		ring.pop_back();
		// Append @ringB to @ring
		ring.extend( ringB );
		if ( ringFaces != NULL )
		{
			ringFaces->extend( ringBFaces );
		}
	}

	// discoverBandsawEdges() leaves the edges in @ring secondary marked; unmark them
	for (int edgeI = 0; edgeI < ring.size(); edgeI++)
	{
		ring[edgeI].edge->secondaryUnmark();
	}

	return closedFlag;
}



void MMesh::markEdgeRing(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes)
{
	//stack based bredth first graph traversal
	std::list<MEdge*> edgeStack;

	//add each marked edge to the stack
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			if ( !( stopAtMarkedFaces  &&  edges[i]->isFaceMarkInternal() ) )
			{
				edgeStack.push_back( edges[i] );
			}
		}
	}


	//until the stack runs out
	while ( !edgeStack.empty() )
	{
		//get an edge to process from the front, and remove it
		MEdge *e = edgeStack.front();
		edgeStack.pop_front();

		//get the faces incident to e
		MFace *a = e->getFaceA();
		MFace *b = e->getFaceB();

		//process them
		markEdgeRingTraverseThruFace( stopAtMarkedFaces, traverseFacesMultipleTimes, e, a, edgeStack );
		markEdgeRingTraverseThruFace( stopAtMarkedFaces, traverseFacesMultipleTimes, e, b, edgeStack );
	}


	if ( !traverseFacesMultipleTimes )
	{
		//faces were marked as visited; unmark them now
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->secondaryUnmark();
		}
	}
}

void MMesh::markEdgeLoop(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes)
{
	//stack based bredth first graph traversal
	std::list<MEdge*> edgeStack;

	//add each marked edge to the stack
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			if ( !( stopAtMarkedVertices  &&  edges[i]->isWhollyVertexMarked() ) )
			{
				edgeStack.push_back( edges[i] );
			}
		}
	}


	//until the stack runs out
	while ( !edgeStack.empty() )
	{
		//get an edge to process from the front, and remove it
		MEdge *e = edgeStack.front();
		edgeStack.pop_front();

		//get the vertices incident to e
		MVertex *a = e->getVertexA();
		MVertex *b = e->getVertexB();

		//process them
		markEdgeLoopTraverseThruVertex( stopAtMarkedVertices, traverseVerticesMultipleTimes, e, a, edgeStack );
		markEdgeLoopTraverseThruVertex( stopAtMarkedVertices, traverseVerticesMultipleTimes, e, b, edgeStack );
	}


	if ( !traverseVerticesMultipleTimes )
	{
		//vertices were marked as visited; unmark them now
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i]->secondaryUnmark();
		}
	}
}

void MMesh::markEdgeBoundary()
{
	//stack based bredth first graph traversal
	std::list<MEdge*> edgeStack;

	//add each marked edge to the stack
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  edges[i]->isBoundary() )
		{
			edgeStack.push_back( edges[i] );
		}
	}


	//until the stack runs out
	while ( !edgeStack.empty() )
	{
		//get an edge to process from the front, and remove it
		MEdge *e = edgeStack.front();
		edgeStack.pop_front();

		//get the vertices incident to @e
		MVertex *a = e->getVertexA();
		MVertex *b = e->getVertexB();

		//process them
		markEdgeBoundaryTraverseThruVertex( e, a, edgeStack );
		markEdgeBoundaryTraverseThruVertex( e, b, edgeStack );
	}
}


void MMesh::discoverEdgeRing(MEdge *seed, MEdgeList &ring)
{
	// Stack based bredth first graph traversal
	std::list<MEdge*> edgeStack;

	// Mark @seed as visited
	seed->secondaryMark();
	// Add @seed to the edge stack
	edgeStack.push_back( seed );
	// Add @seed to the edge ring
	ring.push_back( seed );

	// Until the stack runs out...
	while ( !edgeStack.empty() )
	{
		// Get an edge to process from the front, and remove it
		MEdge *e = edgeStack.front();
		edgeStack.pop_front();

		// Get the faces incident to @e
		MFace *a = e->getFaceA();
		MFace *b = e->getFaceB();

		// Process them
		discoverEdgeRingTraverseThruFace( e, a, edgeStack, ring );
		discoverEdgeRingTraverseThruFace( e, b, edgeStack, ring );
	}


	//secondary unmark the edges in the ring
	for (int edgeI = 0; edgeI < ring.size(); edgeI++)
	{
		ring[edgeI]->secondaryUnmark();
	}
}

void MMesh::discoverEdgeLoop(MEdge *seed, MEdgeList &loop)
{
	// Stack based bredth first graph traversal
	std::list<MEdge*> edgeStack;

	// Mark @seed as visited
	seed->secondaryMark();
	// Add @seed to the edge stack
	edgeStack.push_back( seed );
	// Add @seed to the edge loop
	loop.push_back( seed );


	// Until the stack runs out...
	while ( !edgeStack.empty() )
	{
		// Get an edge to process from the front, and remove it
		MEdge *e = edgeStack.front();
		edgeStack.pop_front();

		// Get the vertices incident to @e
		MVertex *a = e->getVertexA();
		MVertex *b = e->getVertexB();

		// Process them
		discoverEdgeLoopTraverseThruVertex( e, a, edgeStack, loop );
		discoverEdgeLoopTraverseThruVertex( e, b, edgeStack, loop );
	}


	//secondary unmark the edges in the loop
	for (int edgeI = 0; edgeI < loop.size(); edgeI++)
	{
		loop[edgeI]->secondaryUnmark();
	}
}

void MMesh::discoverEdgeBoundary(MEdge *seed, MEdgeList &boundary)
{
	if ( seed->isBoundary() )
	{
		// Stack based bredth first graph traversal
		std::list<MEdge*> edgeStack;

		// Mark @seed as visited
		seed->secondaryMark();
		// Add @seed to the edge stack
		edgeStack.push_back( seed );
		// Add @seed to the boundary
		boundary.push_back( seed );


		// Until the stack runs out...
		while ( !edgeStack.empty() )
		{
			// Get an edge to process from the front, and remove it
			MEdge *e = edgeStack.front();
			edgeStack.pop_front();

			// Get the vertices incident to @e
			MVertex *a = e->getVertexA();
			MVertex *b = e->getVertexB();

			// Process them
			discoverEdgeBoundaryTraverseThruVertex( e, a, edgeStack, boundary );
			discoverEdgeBoundaryTraverseThruVertex( e, b, edgeStack, boundary );
		}


		//secondary unmark the edges in the boundary
		for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
		{
			boundary[edgeI]->secondaryUnmark();
		}
	}
}




bool MMesh::discoverEdgeLoopPath(MVertex *from, MVertex *to, MEdgeRun &path)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->discoverPathInit();
	}

	bool result = from->discoverEdgeLoopPathTo( to, path );

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->discoverPathFinish();
	}

	return result;
}

bool MMesh::discoverEdgeRingPath(MEdge *from, MEdge *to, MEdgeList &path)
{
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->discoverPathInit();
	}

	bool result = from->discoverEdgeRingPathTo( to, path );

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->discoverPathFinish();
	}

	return result;
}






void MMesh::connectMarkedVertices(bool markedFacesOnly, bool markCutEdges)
{
	assertFinalised();

	// Secondary mark all marked vertices, as MFace::connectVertices() connects secondaryMarkedVertices to
	// one another
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			vertices[i]->secondaryMark();
		}
	}

	//Splitting faces will add new faces onto the end of the list.
	//The new faces will not need splitting, so don't try;
	//record the number of faces, and only loop up to that number.
	int numMeshFaces = faces.size();
	for (int i = 0; i < numMeshFaces; i++)
	{
		if ( faces[i]->isFaceMarked()  ||  !markedFacesOnly )
		{
			faces[i]->connectVertices( markCutEdges );
		}
	}

	// Finish up; secondary unmark all vertices
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->secondaryUnmark();
	}

	compactAll();
	finalise();
}

void MMesh::connectMarkedEdges(const MPickList &cutPickList, const MPickList &terminalVertexPickList, bool unmarkOriginalEdges)
{
	assertFinalised();

	Array<ConnectMarkedEdgesRingEntry*> edgeRings;
	std::map<MEdge*, ConnectMarkedEdgesRingEntry*> edgeToRing;


	// Get the list of terminal vertices
	MVertexList terminalVertices;
	connectMarkedEdgesGetTerminalVertices( terminalVertexPickList, terminalVertices );


	//
	// 1. Discover the lists of 'edge rings' that are to be cut, along with a list of 'split parameters' (t-values) for each ring
	//

	for (int pickI = 0; pickI < cutPickList.size(); pickI++)
	{
		// Pick the edge
		Point3 closestPoint;
		MEdge *seed = pickEdge( cutPickList[pickI], closestPoint );

		if ( seed != NULL )
		{
			// Ensure that it is edge marked
			if ( seed->isEdgeMarked() )
			{
				std::map<MEdge*, ConnectMarkedEdgesRingEntry*>::iterator edgeToRingIter = edgeToRing.find( seed );
				ConnectMarkedEdgesRingEntry *ringEntry = NULL;

				if ( edgeToRingIter == edgeToRing.end() )
				{
					// This ring has not been discovered yet: discover it
					ringEntry = new ConnectMarkedEdgesRingEntry();
					ringEntry->closedFlag = discoverMarkedEdgeRing( seed, ringEntry->ring, &ringEntry->ringFaces );
					edgeRings.push_back( ringEntry );

					// Map each edge in the ring to the ring
					for (int edgeI = 0; edgeI < ringEntry->ring.size(); edgeI++)
					{
						MEdge *edge = ringEntry->ring[edgeI].edge;
						edgeToRing[edge] = ringEntry;
					}

					// Gather the terminal vertices that are at the ends of the ring
					if ( !ringEntry->closedFlag )
					{
						for (int vertexI = 0; vertexI < terminalVertices.size(); vertexI++)
						{
							MVertex *vertex = terminalVertices[vertexI];
							if ( ringEntry->ring.front().edge->findCommonFaceWith( vertex )  !=  NULL )
							{
								ringEntry->startTerminal = vertex;
							}
							if ( ringEntry->ring.back().edge->findCommonFaceWith( vertex )  !=  NULL )
							{
								ringEntry->endTerminal = vertex;
							}
						}
					}
				}
				else
				{
					// This ring has already been discovered
					ringEntry = edgeToRingIter->second;
				}


				// Compute @t, the parameter for cutting
				double t;
				seed->getSegment().closestPointTo( closestPoint, t );
				t = clamp( t, MMESH_CUT_EDGE_SPLIT_MARGIN, 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN );
				int seedIndex = ringEntry->ring.find( seed );
				if ( ringEntry->ring[seedIndex].flipFlag )
				{
					t = 1.0 - t;
				}
				ringEntry->connectParameters.insert( t );
			}
		}
	}


	//
	// 2. Perform the splits
	//

	for (int ringI = 0; ringI < edgeRings.size(); ringI++)
	{
		ConnectMarkedEdgesRingEntry *ringEntry = edgeRings[ringI];

		MVertexList vFirst, v0, v1;
		MVertexList *vPrev, *v;
		MEdgeList splitEdges;
		ringEntry->ring[0].edge->splitIntoSegments( ringEntry->connectParameters, ringEntry->ring[0].flipFlag, vFirst, splitEdges );
		if ( unmarkOriginalEdges )
		{
			for (int splitEdgeI = 0; splitEdgeI < splitEdges.size(); splitEdgeI++)
			{
				splitEdges[splitEdgeI]->edgeUnmark();
			}
		}
		v0 = vFirst;

		vPrev = &v0;
		v = &v1;


		for (int edgeI = 1; edgeI < ringEntry->ring.size(); edgeI++)
		{
			int faceI = edgeI - 1;
			v->clear();
			splitEdges.clear();
			ringEntry->ring[edgeI].edge->splitIntoSegments( ringEntry->connectParameters, ringEntry->ring[edgeI].flipFlag, *v, splitEdges );
			if ( unmarkOriginalEdges )
			{
				for (int splitEdgeI = 0; splitEdgeI < splitEdges.size(); splitEdgeI++)
				{
					splitEdges[splitEdgeI]->edgeUnmark();
				}
			}

			ringEntry->ringFaces[faceI]->nCutPerformSplits( *vPrev, *v );

			std::swap( v, vPrev );
		}

		// If the edge ring is closed, then form the last link
		if ( ringEntry->closedFlag )
		{
			ringEntry->ringFaces.back()->nCutPerformSplits( *vPrev, vFirst );
		}
		else
		{
			// Not closed; link to terminal vertices
			if ( ringEntry->startTerminal != NULL )
			{
				MFace *startFace = ringEntry->startTerminal->findBestSplitFace( vFirst );

				MVertexList terminal;
				terminal.push_back( ringEntry->startTerminal );

				startFace->nCutPerformSplits( terminal, vFirst );
			}

			if ( ringEntry->endTerminal != NULL )
			{
				MFace *endFace = ringEntry->endTerminal->findBestSplitFace( *vPrev );

				MVertexList terminal;
				terminal.push_back( ringEntry->endTerminal );

				endFace->nCutPerformSplits( terminal, *vPrev );
			}
		}
	}

	for (int ringI = 0; ringI < edgeRings.size(); ringI++)
	{
		delete edgeRings[ringI];
	}

	compactAll();
	finalise();
}

bool MMesh::connectMarkedEdgesGetRingSegments(MEdge *seed, Array<Segment3> &ringSegments)
{
	BandsawRing ring;
	bool closedFlag = discoverMarkedEdgeRing( seed, ring, NULL );

	ringSegments.resize( ring.size() );
	for (int edgeI = 0; edgeI < ring.size(); edgeI++)
	{
		MEdge *edge = ring[edgeI].edge;
		ringSegments[edgeI] = edge->getSegment( ring[edgeI].flipFlag  ?  edge->getVertexB()  :  edge->getVertexA() );
	}

	return closedFlag;
}

void MMesh::connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList, MVertexList &terminalVertices)
{
	for (int pickI = 0; pickI < terminalVertexPickList.size(); pickI++)
	{
		MVertex *vertex = pickVertex( terminalVertexPickList[pickI] );
		if ( terminalVertices.find( vertex ) == -1 )
		{
			terminalVertices.push_back( vertex );
		}
	}
}




void MMesh::nConnectMarkedEdges(int numSegments)
{
	assertFinalised();

	MVertexList splitVertices;

	//splitting the marked edges creates MORE marked edges, so if we simply iterate over 'edges',
	//the generation of more edges will result in an infinite loop, so stop when the original end of
	//the list is reached

	// Split the edges
	MEdgeList splitEdges;
	int numMeshEdges = edges.size();
	for (int edgeIndex = 0; edgeIndex < numMeshEdges; edgeIndex++)
	{
		// If it is marked
		if ( edges[edgeIndex]->isEdgeMarked() )
		{
			splitEdges.clear();

			// Split into @numSegments segments
			edges[edgeIndex]->splitIntoSegments( numSegments, splitVertices, splitEdges );

			// Unmark the newly split edges
			for (int splitEdgeIndex = 0; splitEdgeIndex < splitEdges.size(); splitEdgeIndex++)
			{
				splitEdges[splitEdgeIndex]->edgeUnmark();
			}
		}
	}

	// Seconary mark all split vertices
	for (int vertexIndex = 0; vertexIndex < splitVertices.size(); vertexIndex++)
	{
		splitVertices[vertexIndex]->secondaryMark();
	}

	// Perform the face splits
	int numMeshFaces = faces.size();
	for (int faceIndex = 0; faceIndex < numMeshFaces; faceIndex++)
	{
		faces[faceIndex]->connectVertices( true );
	}

	// Finish up: Dissolve any unused split vertices, and secondary unmark all the split vertices
	for (int vertexIndex = 0; vertexIndex < splitVertices.size(); vertexIndex++)
	{
		// If the valence of a split vertex is two, then it resulted from splitting an edge, but all attempts to connect the vertex
		// to other split vertices failed. In this case, dissolve it away.
		if ( splitVertices[vertexIndex]->getValence() == 2 )
		{
			splitVertices[vertexIndex]->dissolveSimpleValenceTwoVertex();
		}

		splitVertices[vertexIndex]->secondaryUnmark();
	}

	compactAll();
	finalise();
}




void MMesh::splitEdges(const MPickList &pickList)
{
	assertFinalised();

	for (int pickI = 0; pickI < pickList.size(); pickI++)
	{
		// Pick the edge
		Point3 closestPoint;
		MEdge *edge = pickEdge( pickList[pickI], closestPoint );

		// Compute @t, the parameter for splitting
		double t;
		edge->getSegment().closestPointTo( closestPoint, t );
		t = clamp( t, MMESH_CUT_EDGE_SPLIT_MARGIN, 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN );

		// Split
		MEdge *newEdgeB;
		MVertex *splitVertex = edge->split( t, newEdgeB );

		// Mark the split vertex
		splitVertex->vertexMark();
	}

	finalise();
}

void MMesh::nSplitMarkedEdges(int numSegments)
{
	assertFinalised();

	// Splitting edges results in additional edges being created. We do not want to
	// split these too, so take note of the size of the orignal edge list, and iterate
	// over this only.
	int numEdges = edges.size();

	// Lists for the resulting vertices and edges, needed by MEdge::splitIntoSegments()
	MVertexList splitVertices;
	MEdgeList splitEdges;

	for (int edgeI = 0; edgeI < numEdges; edgeI++)
	{
		if ( edges[edgeI]->isEdgeMarked() )
		{
			splitVertices.clear();
			splitEdges.clear();
			edges[edgeI]->splitIntoSegments( numSegments, splitVertices, splitEdges );

			// Mark the resulting vertices
			for (int vertexI = 0; vertexI < splitVertices.size(); vertexI++)
			{
				splitVertices[vertexI]->vertexMark();
			}

			// Unmark the split edges
			for (int splitEdgeI = 0; splitEdgeI < splitEdges.size(); splitEdgeI++)
			{
				splitEdges[splitEdgeI]->edgeUnmark();
			}
		}
	}

	finalise();
}



void MMesh::cut(const MCutTargetList &cutTargets, MVertex *&lastSplitVertex, MFace *&currentFace)
{
	assertFinalised();

	lastSplitVertex = NULL;
	currentFace = NULL;

	if ( cutTargets.size() >= 1 )
	{
		int faceTargetStart = 0;
		int numFaceTargets = 0;
		bool allFaceTargetsValid = true;

		for (int cutIndex = 0; cutIndex < cutTargets.size(); cutIndex++)
		{
			const MCutTarget &target = cutTargets[cutIndex];

			if ( target.targetType == MCutTarget::VERTEX )
			{
				MVertex *vertex;
				bool gotTargetFlag = cutChooseVertexTarget( target.pick, lastSplitVertex, vertex, currentFace );

				if ( gotTargetFlag )
				{
					if ( currentFace != NULL )
					{
						currentFace->cutSplitFace( lastSplitVertex, vertex, allFaceTargetsValid, cutTargets, faceTargetStart + 1, numFaceTargets );
					}
				}

				lastSplitVertex = vertex;
				currentFace = NULL;
				faceTargetStart = cutIndex;
				numFaceTargets = 0;
				allFaceTargetsValid = true;
			}
			else if ( target.targetType == MCutTarget::EDGE )
			{
				Point3 closestPoint;
				MEdge *edge;

				bool gotTargetFlag = cutChooseEdgeTarget( target.pick, lastSplitVertex, edge, currentFace, closestPoint );

				MVertex *vertex = NULL;

				if ( gotTargetFlag )
				{
					double t;
					edge->getSegment().closestPointTo( closestPoint, t );

					t = clamp( t, MMESH_CUT_EDGE_SPLIT_MARGIN, 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN );

					MEdge *newEdge;
					vertex = edge->split( t, newEdge );

					if ( currentFace != NULL )
					{
						currentFace->cutSplitFace( lastSplitVertex, vertex, allFaceTargetsValid, cutTargets, faceTargetStart + 1, numFaceTargets );
					}
				}

				lastSplitVertex = vertex;
				currentFace = NULL;
				faceTargetStart = cutIndex;
				numFaceTargets = 0;
				allFaceTargetsValid = true;
			}
			else if ( target.targetType == MCutTarget::FACE )
			{
				if ( allFaceTargetsValid )
				{
					allFaceTargetsValid = cutChooseFaceTarget( target.pick, lastSplitVertex, currentFace );

					if ( allFaceTargetsValid )
					{
						numFaceTargets++;
					}
				}
			}
			else
			{
				gs_assert_not_reached( "MMesh::cut(): invalid target type\n" );
			}
		}

		compactAll();
		finalise();
	}
}

bool MMesh::isValidCutTarget(const MCutTarget &target, MVertex *lastSplitVertex, MFace *currentFace)
{
	if ( target.targetType == MCutTarget::VERTEX )
	{
		MVertex *vertex;

		return cutChooseVertexTarget( target.pick, lastSplitVertex, vertex, currentFace );
	}
	else if ( target.targetType == MCutTarget::EDGE )
	{
		MEdge *edge;
		Point3 closestPoint;

		return cutChooseEdgeTarget( target.pick, lastSplitVertex, edge, currentFace, closestPoint );
	}
	else if ( target.targetType == MCutTarget::FACE )
	{
		return cutChooseFaceTarget( target.pick, lastSplitVertex, currentFace );
	}
	else
	{
		return false;
	}
}

bool MMesh::cutChooseVertexTarget(const MPick &pick, MVertex *lastSplitVertex, MVertex *&vertex, MFace *&currentFace)
{
	vertex = pickVertex( pick );

	if ( vertex != NULL )
	{
		if ( vertex == lastSplitVertex )
		{
			return false;
		}

		if ( currentFace == NULL )
		{
			if ( lastSplitVertex != NULL )
			{
				if ( vertex->findEdgeTo( lastSplitVertex, MFINDPOLICY_RETURNNULL ) != NULL )
				{
					return false;
				}

				currentFace = lastSplitVertex->findBestSplitFace( vertex );

				return currentFace != NULL;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return vertex->isIncidentTo( currentFace );
		}
	}
	else
	{
		return false;
	}
}

bool MMesh::cutChooseEdgeTarget(const MPick &pick, MVertex *lastSplitVertex, MEdge *&edge, MFace *&currentFace, Point3 &closestPoint)
{
	edge = pickEdge( pick, closestPoint );

	if ( edge != NULL )
	{
		if ( currentFace == NULL )
		{
			if ( lastSplitVertex != NULL )
			{
				if ( edge->isIncidentTo( lastSplitVertex ) )
				{
					return false;
				}

				currentFace = edge->findBestSplitFace( lastSplitVertex );

				return currentFace != NULL;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return edge->isIncidentTo( currentFace );
		}
	}
	else
	{
		return false;
	}
}

bool MMesh::cutChooseFaceTarget(const MPick &pick, MVertex *lastSplitVertex, MFace *&currentFace)
{
	if ( lastSplitVertex == NULL )
	{
		return false;
	}

	MFace *face = pickFace( pick );

	if ( face != NULL )
	{
		if ( currentFace == NULL )
		{
			if ( face->isIncidentTo( lastSplitVertex ) )
			{
				currentFace = face;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return face == currentFace;
		}
	}
	else
	{
		return false;
	}
}



void MMesh::nCut(int numSegments, const MCutTargetList &cutTargets, MVertexList *lastSplitVertices)
{
	assertFinalised();

	if ( cutTargets.size() >= 1 )
	{
		MVertexList splitVerts0, splitVerts1;
		MVertexList *splitVertsA = &splitVerts0, *splitVertsB = &splitVerts1;

		nCutPerformCut( numSegments, cutTargets[0], *splitVertsB, *splitVertsA );

		for (int targetIndex = 1; targetIndex < cutTargets.size(); targetIndex++)
		{
			nCutPerformCut( numSegments, cutTargets[targetIndex], *splitVertsA, *splitVertsB );

			std::swap( splitVertsA, splitVertsB );
		}

		if ( lastSplitVertices != NULL )
		{
			*lastSplitVertices = *splitVertsA;
		}

		compactAll();
		finalise();
	}
}

bool MMesh::isValidNCutTarget(const MCutTarget &target, MVertexList &lastSplitVertices)
{
	if ( target.targetType == MCutTarget::VERTEX )
	{
		MVertex *vertex;
		MFace *faceToSplit;

		return nCutChooseVertexTarget( target.pick, lastSplitVertices, vertex, faceToSplit );
	}
	else if ( target.targetType == MCutTarget::EDGE )
	{
		MEdge *edge;
		MFace *faceToSplit;

		return nCutChooseEdgeTarget( target.pick, lastSplitVertices, edge, faceToSplit );
	}
	else
	{
		gs_assert_not_reached( "MMesh::isValidNCutTarget(): invalid target\n" );
		return false;
	}
}

void MMesh::nCutPerformCut(int numSegments, const MCutTarget &target, MVertexList &prevSplitVertices, MVertexList &splitVertices)
{
	if ( target.targetType == MCutTarget::VERTEX )
	{
		MVertex *vertex;
		MFace *faceToSplit;

		bool gotTargetFlag = nCutChooseVertexTarget( target.pick, prevSplitVertices, vertex, faceToSplit );

		splitVertices.clear();

		if ( gotTargetFlag )
		{
			// Write @v in to @splitVertices
			splitVertices.push_back( vertex );

			// Split the face
			if ( faceToSplit != NULL )
			{
				faceToSplit->nCutPerformSplits( prevSplitVertices, splitVertices );
			}
		}
	}
	else if ( target.targetType == MCutTarget::EDGE )
	{
		MEdge *edge;
		MFace *faceToSplit;

		bool gotTargetFlag = nCutChooseEdgeTarget( target.pick, prevSplitVertices, edge, faceToSplit );

		splitVertices.clear();

		if ( gotTargetFlag )
		{
			// Split the edge
			MEdgeList splitEdges;
			edge->splitIntoSegments( numSegments, splitVertices, splitEdges );

			// Split the face
			if ( faceToSplit != NULL )
			{
				faceToSplit->nCutPerformSplits( prevSplitVertices, splitVertices );
			}
		}
	}
	else
	{
		gs_assert_not_reached( "MMesh::nCutPerformCut(): invalid target\n" );
	}
}

bool MMesh::nCutChooseVertexTarget(const MPick &pick, MVertexList &prevSplitVertices, MVertex *&vertex, MFace *&faceToSplit)
{
	vertex = pickVertex( pick );

	if ( vertex != NULL )
	{
		// Ensure that @vertex is not one of the split vertices from the previous split
		if ( prevSplitVertices.contains( vertex ) )
		{
			return false;
		}

		// Ensure that @vertex is not incident to an edge that is incident to any of the split vertices from the previous split
		for (int i = 0; i < prevSplitVertices.size(); i++)
		{
			if ( vertex->findEdgeTo( prevSplitVertices[i], MFINDPOLICY_RETURNNULL )  !=  NULL )
			{
				return false;
			}
		}

		// Ensure that @vertex is incident to a face that is also incident to all of the split vertices from the previous split
		faceToSplit = vertex->findBestSplitFace( prevSplitVertices );
		if ( faceToSplit == NULL  &&  prevSplitVertices.size() > 0 )
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool MMesh::nCutChooseEdgeTarget(const MPick &pick, MVertexList &prevSplitVertices, MEdge *&edge, MFace *&faceToSplit)
{
	Point3 closestPoint;
	MEdgeList newEdges;

	edge = pickEdge( pick, closestPoint );

	if ( edge != NULL )
	{
		// Ensure that @edge is not incident to any of the split vertices from the previous split
		for (int i = 0; i < prevSplitVertices.size(); i++)
		{
			if ( edge->isIncidentTo( prevSplitVertices[i] ) )
			{
				return false;
			}
		}

		// Ensure that @edge is incident to a face that is also incident to all of the split vertices from the previous splits
		faceToSplit = edge->findBestSplitFace( prevSplitVertices );
		if ( faceToSplit == NULL  &&  prevSplitVertices.size() > 0 )
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}





void MMesh::bandsaw(const MBandsawTargetList &targetList)
{
	assertFinalised();

	BandsawRing ring;
	MFaceList ringFaces;

	for (int targetI = 0; targetI < targetList.size(); targetI++)
	{
		const MPick &pick = targetList[targetI].pick;
		bool stopAtMarkedEdges = targetList[targetI].stopAtMarkedEdges;

		Point3 closestPointOnEdge;
		MFace *seedFace = NULL;
		MEdge *seed = pickEdge( pick, closestPointOnEdge, seedFace );

		if ( seed != NULL  &&  seedFace != NULL )
		{
			ring.clear();
			ringFaces.clear();

			int seedIndex;
			bool closedFlag = discoverBandsawEdgeRing( seed, seedFace, ring, &ringFaces, stopAtMarkedEdges, seedIndex );


			double t;
			seed->getSegment().closestPointTo( closestPointOnEdge, t );
			t = clamp( t, MMESH_CUT_EDGE_SPLIT_MARGIN, 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN );

			MEdge *newEdgeB;
			MVertex *vFirst = ring[0].edge->split( ring[0].flipFlag  ?  1.0 - t  :  t,  newEdgeB );
			MVertex *vPrev = vFirst;

			MEdge *splitEdge;
			for (int edgeI = 1; edgeI < ring.size(); edgeI++)
			{
				int faceI = edgeI - 1;

				MFace *faceToSplit = ringFaces[faceI];
				MVertex *v = ring[edgeI].edge->split( ring[edgeI].flipFlag  ?  1.0 - t  :  t,  newEdgeB );

				MFace *resultFace = ringFaces[faceI]->split( vPrev, v, splitEdge );
				splitEdge->edgeMark();
				vPrev = v;

				for (int ringFaceI = faceI + 1; ringFaceI < ringFaces.size(); ringFaceI++)
				{
					if ( ringFaces[ringFaceI] == faceToSplit )
					{
						// This face has been split; update the ring edges and ring faces
						MEdge *edgeA = ring[ringFaceI].edge;
						MEdge *edgeB = ring[ nextIndex( ringFaceI, ring.size() ) ].edge;
						MFace *faceA = NULL;
						if ( edgeA->isIncidentTo( faceToSplit )  &&  edgeB->isIncidentTo( resultFace ) )
						{
							ringFaces.insert( ringFaceI + 1, resultFace );
							faceA = faceToSplit;
						}
						else if ( edgeB->isIncidentTo( faceToSplit )  &&  edgeA->isIncidentTo( resultFace ) )
						{
							ringFaces.insert( ringFaceI, resultFace );
							faceA = resultFace;
						}
						else
						{
							gs_assert_not_reached( "MMesh::bandsaw(): could not determine where to insert into list\n" );
						}


						// Determine if the edge should be flipped
						bool bEdgeAInOrder = faceA->isEdgeInOrder( edgeA );
						bool bSplitEdgeInOrder = faceA->isEdgeInOrder( splitEdge );
						bool bInvert = bEdgeAInOrder == bSplitEdgeInOrder;
						bool bFlip = bInvert  ?  !ring[ringFaceI].flipFlag  :  ring[ringFaceI].flipFlag;

						ring.insert( ringFaceI + 1, BandsawRingEntry( splitEdge, bFlip ) );
						break;
					}
				}
			}

			// If the edge ring is closed, then form the last link
			if ( closedFlag )
			{
				ringFaces.back()->split( vPrev, vFirst, splitEdge );
				splitEdge->edgeMark();
			}
		}
	}

	compactAll();
	finalise();
}

void MMesh::nBandsaw(const MNBandsawTargetList &targetList)
{
	assertFinalised();

	BandsawRing ring;
	MFaceList ringFaces;
	MVertexList vFirst, v0, v1;
	MEdgeList splitEdgeSegments, splitEdges;

	for (int targetI = 0; targetI < targetList.size(); targetI++)
	{
		const MNBandsawTarget &target = targetList[targetI];
		const MPick &pick = target.pick;
		bool stopAtMarkedEdges = target.stopAtMarkedEdges;
		int numSegments = target.numSegments;
		int numCuts = target.numSegments - 1;

		Point3 closestPointOnEdge;
		MFace *seedFace = NULL;
		MEdge *seed = pickEdge( pick, closestPointOnEdge, seedFace );

		if ( seed != NULL  &&  seedFace != NULL )
		{
			ring.clear();
			ringFaces.clear();
			int seedIndex;
			bool closedFlag = discoverBandsawEdgeRing( seed, seedFace, ring, &ringFaces, stopAtMarkedEdges, seedIndex );


			vFirst.clear();
			v0.clear();
			v1.clear();
			MVertexList *vPrev, *v;
			ring[0].edge->splitIntoSegments( numSegments, vFirst, splitEdgeSegments );
			v0 = vFirst;

			vPrev = &v0;
			v = &v1;


			for (int edgeI = 1; edgeI < ring.size(); edgeI++)
			{
				int faceI = edgeI - 1;
				MFace *faceToSplit = ringFaces[faceI];

				v->clear();
				splitEdgeSegments.clear();
				ring[edgeI].edge->splitIntoSegments( numSegments, *v, splitEdgeSegments );

				splitEdges.clear();
				splitEdges.reserve( numCuts );
				faceToSplit->nCutPerformSplits( *vPrev, *v, &splitEdges );

				std::swap( v, vPrev );

				for (int ringFaceI = faceI + 1; ringFaceI < ringFaces.size(); ringFaceI++)
				{
					if ( ringFaces[ringFaceI] == faceToSplit )
					{
						// This face has been split; update the ring edges and ring faces
						MEdge *edgeA = ring[ringFaceI].edge;
						MEdge *edgeB = ring[ nextIndex( ringFaceI, ring.size() ) ].edge;

						MFace *fFirst = edgeA->getSharedFace( splitEdges.front() );
						MFace *fLast = edgeB->getSharedFace( splitEdges.back() );
						if ( fFirst == NULL  ||  fLast == NULL )
						{
							splitEdges.reverse();
							fFirst = edgeA->getSharedFace( splitEdges.front() );
							fLast = edgeB->getSharedFace( splitEdges.back() );
						}

						gs_assert( fFirst != NULL  &&  fLast != NULL, "MMesh::nBandsaw(): could not determine fFirst and fLast\n" );

						// Insert space for faces
						ringFaces.insert( ringFaceI, numCuts, NULL );
						// Put the split faces into the ring face array
						ringFaces[ringFaceI] = fFirst;
						for (int splitFaceI = 0; splitFaceI < numCuts - 1; splitFaceI++)
						{
							MFace *fSplit = splitEdges[splitFaceI]->getSharedFace( splitEdges[splitFaceI+1] );
							gs_assert( fSplit != NULL, "MMesh::nBandsaw(): could not get split face\n" );
							ringFaces[ringFaceI + 1 + splitFaceI] = fSplit;
						}
						ringFaces[ringFaceI + numCuts] = fLast;


						// Add edges to the ring edge array
						// Note that we need to compute 'flip' flags to ensure that the bandsaw goes in the right direction
						MEdge *ePrev = edgeA;
						MFace *commonFace = ringFaces[ringFaceI];
						bool bInOrderPrev = commonFace->isEdgeInOrder( ePrev );
						bool bFlipPrev = ring[ringFaceI].flipFlag;
						ring.insert( ringFaceI + 1, numCuts, BandsawRingEntry() );

						for (int splitEdgeI = 0; splitEdgeI < numCuts; splitEdgeI++)
						{
							commonFace = ringFaces[ringFaceI+splitEdgeI];

							MEdge *splitE = splitEdges[splitEdgeI];
							bool bInOrderSplitE = commonFace->isEdgeInOrder( splitE );
							bool bInvert = bInOrderPrev == bInOrderSplitE;
							bool bFlip = bInvert  ?  !bFlipPrev  :  bFlipPrev;

							ring[ringFaceI+splitEdgeI+1] = BandsawRingEntry( splitE, bFlip );

							ePrev = splitE;
							bInOrderPrev = !bInOrderSplitE;
							bFlipPrev = bFlip;
						}

						break;
					}
				}
			}

			// If the edge ring is closed, then form the last link
			if ( closedFlag )
			{
				ringFaces.back()->nCutPerformSplits( *vPrev, vFirst );
			}
		}
	}

	compactAll();
	finalise();
}

bool MMesh::bandsawGetRingSegments(MEdge *seed, MFace *seedFace, Array<Segment3> &ringSegments, bool stopAtMarkedEdges)
{
	int seedIndex;
	BandsawRing ring;
	bool closedFlag = discoverBandsawEdgeRing( seed, seedFace, ring, NULL, stopAtMarkedEdges, seedIndex );

	ringSegments.resize( ring.size() );
	for (int edgeI = 0; edgeI < ring.size(); edgeI++)
	{
		MEdge *edge = ring[edgeI].edge;
		ringSegments[edgeI] = edge->getSegment( ring[edgeI].flipFlag  ?  edge->getVertexB()  :  edge->getVertexA() );
	}

	return closedFlag;
}


bool MMesh::discoverBandsawEdgeRing(MEdge *seed, MFace *seedFace, BandsawRing &ring, MFaceList *ringFaces, bool stopAtMarkedEdges, int &indexOfSeedInRing)
{
	indexOfSeedInRing = 0;

	// Mark as visited so that the graph traversal algorithm cannot loop back onto @seed
	seed->secondaryMark();


	// Start with face A if a seed face has not been supplied
	if ( seedFace == NULL )
	{
		seedFace = seed->getFaceA();
	}

	bool closedFlag = discoverBandsawEdges( seed, seedFace, ring, ringFaces, stopAtMarkedEdges );

	// If discoverBandsawEdges() produced an open edge ring, and there are faces on the opposite side of @seed, then we need to
	// discover in the opposite direction to acquire the full bandsaw ring
	if ( seed->getFaceB() != NULL  &&  !closedFlag )
	{
		// If @seed is edge marked and @stopAtMarkedEdges is true, then we do not scan from @seed in the opposite direction
		if ( !( seed->isEdgeMarked()  &&  stopAtMarkedEdges ) )
		{
			BandsawRing ringB;
			MFaceList ringBFaces;
			discoverBandsawEdges( seed, seed->getOppositeFace( seedFace ), ringB, ringFaces != NULL  ?  &ringBFaces  :  NULL, stopAtMarkedEdges );


			// Detect the special case where @stopAtMarkedEdges is true, and the discovery process is terminated at the same marked edge, thus forming a
			// closed ring
			if ( stopAtMarkedEdges  &&  ring.back().edge == ringB.back().edge )
			{
				// The marked edge will be the last edge in @ring and @ringB.
				// Remove it from @ringB to prevent duplication
				ringB.pop_back();
				// Set the closed flag
				closedFlag = true;
			}


			ring.reverse();
			if ( ringFaces != NULL )
			{
				ringFaces->reverse();
			}
			// Remove @seed from end of @ring, as it occurs at the beginning of @ringB
			ring.pop_back();
			// Note index of @seed in @ring
			indexOfSeedInRing = ring.size();
			// Append @ringB to @ring
			ring.extend( ringB );
			if ( ringFaces != NULL )
			{
				ringFaces->extend( ringBFaces );
			}
		}
	}

	// discoverBandsawEdges() leaves the edges in @ring secondary marked; unmark them
	for (int edgeI = 0; edgeI < ring.size(); edgeI++)
	{
		ring[edgeI].edge->secondaryUnmark();
	}

	return closedFlag;
}



bool MMesh::discoverBandsawEdges(MEdge *seed, MFace *f, BandsawRing &ring, MFaceList *ringFaces, bool stopAtMarkedEdges)
{
	MEdge *e = seed;
	bool eInOrder = f->isEdgeInOrder( e );

	// @flipFlag is used to keep track of the edge direction
	bool flipFlag = false;

	ring.push_back( BandsawRingEntry( seed, flipFlag ) );

	while ( e != NULL  &&  f != NULL )
	{
		// The opposite edge to @e, in @f
		MEdge *eNext = f->quadGetOppositeEdge( e );

		if ( eNext != NULL )
		{
			// Only process @eNext if it hasn't already been visited
			if ( !eNext->isSecondaryMarked() )
			{
				// Need to keep track of the ordering of the edges
				bool eNextInOrder = f->isEdgeInOrder( eNext );

				if ( eInOrder == eNextInOrder )
				{
					// @e and @eNext have the same direction w.r.t. @f; one will need to be flipped, and the other not,
					// in order to ensure that the bandsaw is applied to the same 'side' of each edge in the ring; flip the
					// 'flip flag'
					flipFlag = !flipFlag;
				}

				// Add @eNext along with @flipFlag to @ring
				ring.push_back( BandsawRingEntry( eNext, flipFlag ) );
				if ( ringFaces != NULL )
				{
					ringFaces->push_back( f );
				}

				// Exit early if @eNext is marked, and we are stopping at marked edges
				if ( eNext->isEdgeMarked()  &&  stopAtMarkedEdges )
				{
					break;
				}

				// Mark @eNext as visited
				eNext->secondaryMark();

				// @eNext becomes @e
				e = eNext;
				// Get the next face
				MFace *fNext = e->getOppositeFace( f );

				// The order of eNext w.r.t. @f will be opposite to the order of @eNext w.r.t. @fNext
				eInOrder = !eNextInOrder;

				// @fNext becomes @f
				f = fNext;
			}
			else if ( eNext == seed )
			{
				// Special case; looped back to @seed; the ring is closed.
				if ( ringFaces != NULL )
				{
					ringFaces->push_back( f );
				}

				return true;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return false;
}






void MMesh::roundMarkedFaces()
{
	assertFinalised();

	MVertexList splitVertices;

	//get a list of edges to process
	MEdgeList markedEdges;
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceMarked() )
		{
			markedEdges.push_back( edges[i] );
		}
	}

	//process them
	for (int i = 0; i < markedEdges.size(); i++)
	{
		MEdge *e = markedEdges[i];
		MEdge *newEdgeB, *newEdgeB0, *newEdgeB1;

		double fraction0 = 1.0  /  ( 2.0 + M_SQRT2 );
		double fraction1 = 1.0 - (  fraction0  /  ( 1.0 - fraction0 )  );

		MVertex *splitVertex0 = e->split( fraction0, newEdgeB );
		MVertex *splitVertex1 =
				newEdgeB->split( splitVertex0, fraction1, newEdgeB0, newEdgeB1 );

		splitVertex0->secondaryMark();
		splitVertex1->secondaryMark();

		splitVertices.push_back( splitVertex0 );
		splitVertices.push_back( splitVertex1 );
	}

	//get a list of faces to process
	MFaceList markedFaces;
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			markedFaces.push_back( faces[i] );
		}
	}
	//process them
	for (int i = 0; i < markedFaces.size(); i++)
	{
		markedFaces[i]->round();
	}

	for (int i = 0; i < splitVertices.size(); i++)
	{
		splitVertices[i]->secondaryUnmark();
	}


	compactAll();
	finalise();
}






int MMesh::weldMarkedVertices(double thresholdDistance, bool weldLinkedVertices, bool markMergedEdges)
{
	assertFinalised();

	// Get the list of marked vertices
	Array<MVertex*> markedVertices;
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			markedVertices.push_back( vertices[i] );
		}
	}


	double thresholdSquared = thresholdDistance * thresholdDistance;
	bool success = true;

	// For each pair of vertices:
	for (int vertexI = 0; vertexI < markedVertices.size(); vertexI++)
	{
		MVertex *a = markedVertices[vertexI];

		if ( a != NULL )
		{
			for (int vertexJ = vertexI + 1; vertexJ < markedVertices.size(); vertexJ++)
			{
				MVertex *b = markedVertices[vertexJ];

				if ( b != NULL )
				{
					double sqrDist = a->sqrDistanceTo( b );

					if ( sqrDist <= thresholdSquared )
					{
     						// Distance is within the threshold

						// Skip this pair if they are linked, and linked vertices should not be welded to one another
						if ( !weldLinkedVertices )
						{
							if ( a->isLinkedTo( b ) )
							{
								continue;
							}
						}

						// Perform the weld
						MVertex *v = a->weldToMidPointWith( b, markMergedEdges );
						if ( v != NULL )
						{
							markedVertices[vertexI] = v;
							markedVertices[vertexJ] = NULL;
							a = v;
						}
						else
						{
							success = false;
						}
					}
				}
			}
		}
	}


	compactAll();
	finalise();

	return success  ?  0  :  -1;
}



void MMesh::targetWeld(const MWeldTargetList &weldTargets)
{
	assertFinalised();

	for (int weldI = 0; weldI < weldTargets.size(); weldI++)
	{
		const MWeldTarget &weld = weldTargets[weldI];

		MVertex *sourceVertex = pickVertex( weld.sourcePick );
		MVertex *targetVertex = NULL;


		if ( sourceVertex != NULL )
		{
			if ( weld.targetType == MWeldTarget::VERTEX )
			{
				targetVertex = pickVertex( weld.targetPick );
				if ( targetVertex == sourceVertex )
				{
					targetVertex = NULL;
				}
			}
			else if ( weld.targetType == MWeldTarget::EDGE_CLOSEST_POINT_TO_SOURCE  ||  weld.targetType == MWeldTarget::EDGE )
			{
				Point3 closestPoint;
				MEdge *targetEdge = pickEdge( weld.targetPick, closestPoint );


				if ( targetEdge != NULL )
				{
					if ( weld.targetType == MWeldTarget::EDGE_CLOSEST_POINT_TO_SOURCE )
					{
						double t;
						targetEdge->getSegment().closestPointTo( sourceVertex->getPosition(), t );

						if ( t < MMESH_CUT_EDGE_SPLIT_MARGIN )
						{
							targetVertex = targetEdge->getVertexA();
						}
						else if ( t  > 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN )
						{
							targetVertex = targetEdge->getVertexB();
						}
						else
						{
							MEdge *newEdge;
							targetVertex = targetEdge->split( t, newEdge );
						}
					}
					else if ( weld.targetType == MWeldTarget::EDGE )
					{
						double t;
						targetEdge->getSegment().closestPointTo( closestPoint, t );

						t = clamp( t, MMESH_CUT_EDGE_SPLIT_MARGIN, 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN );

						MEdge *newEdge;
						targetVertex = targetEdge->split( t, newEdge );
					}
				}
			}
			else
			{
				gs_assert_not_reached( "MMesh::targetWeld(): invalid weld target type\n" );
			}

			if ( targetVertex != NULL )
			{
				sourceVertex->weldTo( targetVertex, false );
			}
		}
	}


	compactAll();
	finalise();
}


bool MMesh::isValidWeldTargetVertex(MVertex *sourceVertex, MVertex *targetVertex)
{
	return sourceVertex->canWeldTo( targetVertex );
}

bool MMesh::isValidWeldTargetEdge(MVertex *sourceVertex, MEdge *targetEdge, MWeldTarget::TargetType targetType)
{
	if ( targetType == MWeldTarget::EDGE_CLOSEST_POINT_TO_SOURCE )
	{
		double t;
		targetEdge->getSegment().closestPointTo( sourceVertex->getPosition(), t );

		if ( t < MMESH_CUT_EDGE_SPLIT_MARGIN )
		{
			return sourceVertex->canWeldTo( targetEdge->getVertexA() );
		}
		else if ( t  > 1.0 - MMESH_CUT_EDGE_SPLIT_MARGIN )
		{
			return sourceVertex->canWeldTo( targetEdge->getVertexB() );
		}
		else
		{
			return sourceVertex->canWeldTo( targetEdge );
		}
	}
	else if ( targetType == MWeldTarget::EDGE )
	{
		return sourceVertex->canWeldTo( targetEdge );
	}
	else
	{
		gs_assert_not_reached( "MMesh::isValidWeldTarget(): invalid weld target type\n" );
		return false;
	}
}




int MMesh::dissolveMarkedVertices(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces)
{
	assertFinalised();

	int successCount, failureCount;


	do
	{
		//initialise counts
		successCount = failureCount = 0;

		//dissolve all marked (and not yet destroyed) vertices
		for (int i = 0; i < vertices.size(); i++)
		{
			MVertex *v = vertices[i];

			if ( !v->isDestroyed()  &&  v->isVertexMarked() )
			{
				bool dissolveSuccess = v->dissolve( twoEdgesOnly, twoFacesOnly, markCreatedFaces );

				if ( dissolveSuccess )
				{
					successCount++;
				}
				else
				{
					failureCount++;
				}
			}
		}

		//attempt another pass, if some failed and some succeeded;
		//if all failed, then nothing will have changed, so no point retrying
	} while ( successCount > 0  &&  failureCount > 0 );



	compactAll();
	finalise();

	//determine result code
	int result =  failureCount > 0  ?  -1  :  0;
	return result;
}

int MMesh::dissolveMarkedEdges(MEdgeDissolveStyle style, bool dissolveSimpleValenceTwoVertices)
{
	assertFinalised();


	int successCount, failureCount;

	MVertexList verticesToDissolve;
	if ( dissolveSimpleValenceTwoVertices )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			MVertex *v = vertices[i];

			int numMarkedEdges = v->getMarkedEdgeCount();
			if ( numMarkedEdges == 1  ||  numMarkedEdges == 2 )
			{
				if ( v->isTopologySimple() )
				{
					if ( numMarkedEdges == 1  &&  v->getValence() == 3 )
					{
						if ( v->getNumFaces() == 3 )
						{
							verticesToDissolve.push_back( v );
						}
						else if ( v->getNumFaces() ==2  )
						{
							MEdge *e = v->getMarkedEdge();
							if ( !e->isBoundary() )
							{
								verticesToDissolve.push_back( v );
							}
						}
					}
					else if ( numMarkedEdges == 2  &&  v->getValence() == 4  &&  v->getNumFaces() == 4 )
					{
						verticesToDissolve.push_back( v );
					}
				}
			}
		}
	}

	do
	{
		//initialise counts
		successCount = failureCount = 0;

		//dissolve all marked (and not yet destroyed) edges
		for (int i = 0; i < edges.size(); i++)
		{
			MEdge *e = edges[i];

			if ( !e->isDestroyed()  &&  e->isEdgeMarked() )
			{
				MFace *mergedFace = e->dissolve( style );

				if ( mergedFace != NULL )
				{
					successCount++;
				}
				else
				{
					failureCount++;
				}
			}
		}

		//attempt another pass, if some failed and some succeeded;
		//if all failed, then nothing will have changed, so no point retrying
	} while ( successCount > 0  &&  failureCount > 0 );


	if ( dissolveSimpleValenceTwoVertices )
	{
		int dissolveVertexSuccessCount, dissolveVertexFailureCount;

		do
		{
			//initialise counts
			dissolveVertexSuccessCount = dissolveVertexFailureCount = 0;

			// Attempt to dissolve vertices
			for (int vertexI = 0; vertexI < verticesToDissolve.size(); vertexI++)
			{
				MVertex *v = verticesToDissolve[vertexI];

				if ( !v->isDestroyed() )
				{
					if ( !v->isEdgeMarked() )
					{
						bool dissolveSuccess = v->dissolveSimpleValenceTwoVertex();

						if ( dissolveSuccess )
						{
							dissolveVertexSuccessCount++;
						}
						else
						{
							dissolveVertexFailureCount++;
						}
					}
				}
			}

			//attempt another pass, if some failed and some succeeded;
			//if all failed, then nothing will have changed, so no point retrying
		} while ( dissolveVertexSuccessCount > 0  &&  dissolveVertexFailureCount > 0 );

		failureCount += dissolveVertexFailureCount;
	}


	compactAll();
	finalise();

	//determine result code
	int result =  failureCount > 0  ?  -1  :  0;
	return result;
}

int MMesh::dissolveMarkedFaces(bool bMarkMergedFaces)
{
	assertFinalised();

	// Discover the marked face regions
	MarkedFaceRegionList regions;
	discoverMarkedFaceRegionsAndBoundaries( regions );


	bool multipleBoundaryFailureFlag = false;
	bool selfIntersectingBoundaryFailureFlag = false;


	// For each region:
	for (int regionI = 0; regionI < regions.size(); regionI++)
	{
		MarkedFaceRegion &region = regions[regionI];

		// Cannot process region if it has more than 1 boundary
		if ( region.boundaries.size() == 1 )
		{
			MEdgeRun &boundary = region.boundaries.front();

			// Check the region boundary to ensure that it is not self intersecting
			MEdgeRun::VertexIterator iter = boundary.verticesBegin();
			bool selfIntersectingFlag = false;
			for (int vertexI = 0; vertexI < boundary.getNumVertices(); vertexI++)
			{
				// If we come across a vertex that has already been visited, then the boundary intersects itself
				if ( iter->isSecondaryMarked() )
				{
					selfIntersectingFlag = true;
					break;
				}

				// Mark the vertex as visited
				iter->secondaryMark();
				++iter;
			}

			// Mark all vertices as not visited
			iter = boundary.verticesBegin();
			for (int vertexI = 0; vertexI < boundary.getNumVertices(); vertexI++)
			{
				iter->secondaryUnmark();
				++iter;
			}

			if ( !selfIntersectingFlag )
			{
				// Merge the region into one face
				gs_assert( boundary.isClosed(), "MMesh::dissolveMarkedFaces(): boundary is open\n" );

				MVertexList faceVertices;
				boundary.extractVertices( faceVertices );

				//
				//  TEXTURE CO-ORDINATES
				//
				// Generate the texture co-ordinates by averageing the co-ordinates used by the marked faces from the
				// region that are incident to each vertex on the boundary
				Array<Point2f> texcoords;
				texcoords.reserve( boundary.getNumVertices() );

				// Need the two edges adjacent the boundary vertex
				MEdge *ePrev = boundary.back();
				for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
				{
					Vector2f tex;
					int numFaceTexcoords = 0;

					MEdge *e = boundary[edgeI];
					MVertex *v = faceVertices[edgeI];

					// Iterate around the neighbourhood of the vertex at @vIter, summing the texcoords along the way
					MFace *f = ePrev->getMarkedFace();
					MVertex::NeighbourhoodIterator nIter = v->neighbourhoodIterator( ePrev, f );
					while ( nIter.edge() != e )
					{
						tex += nIter.face()->getVertexAttrib( v )->getPoint().toVector2f();
						numFaceTexcoords++;

						bool iterSuccess = nIter.next();
						gs_assert( iterSuccess, "MMesh::dissolveMarkedFaces(): neighbourhood iteration failed\n" );
					}
					ePrev = e;

					texcoords.push_back( tex  *  ( 1.0f / (float)numFaceTexcoords ) );
				}


				//
				//  DELETE THE EXISTING FACES IN THE REGION, AND COUNT HOW MANY FACES USE EACH MATERIAL ID
				//
				// Need to keep the boundary edges around
				for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
				{
					boundary[edgeI]->setIndestructible();
				}

				RefCountTable<int> materialUsageTable;
				for (int faceI = 0; faceI < region.faces.size(); faceI++)
				{
					// Count the face's material ID
					materialUsageTable.ref( region.faces[faceI]->getMaterialID() );

					region.faces[faceI]->destroy( true, true );
				}


				//
				//  DETERMINE THE MOST POPULAR MATERIAL ID
				//
				int bestMaterialID = -1, bestMaterialIDCount = 0;
				for (RefCountTable<int>::const_iterator iter = materialUsageTable.begin(); iter != materialUsageTable.end(); ++iter)
				{
					if ( iter->second > bestMaterialIDCount )
					{
						bestMaterialID = iter->first;
						bestMaterialIDCount = iter->second;
					}
				}


				//
				//  CREATE NEW FACE
				//
				// Determine if the vertex ordering needs to be reversed
				bool reverseOrderFlag = false;
				for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
				{
					if ( boundary[edgeI]->getFaceA() != NULL )
					{
						reverseOrderFlag = boundary.isEdgeInOrder( edgeI );
						break;
					}
				}

				if ( reverseOrderFlag )
				{
					faceVertices.reverse();
					texcoords.reverse();
				}


				// Create the face
				MFace *face = addFace( faceVertices, texcoords, bestMaterialID );

				if ( bMarkMergedFaces )
				{
					face->faceMark();
				}


				// Set the boundary edges as destructible
				for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
				{
					boundary[edgeI]->setDestructible();
				}
			}
			else
			{
				selfIntersectingBoundaryFailureFlag = true;
			}
		}
		else
		{
			multipleBoundaryFailureFlag = true;
		}
	}


	compactAll();
	finalise();


	if ( multipleBoundaryFailureFlag  &&  !selfIntersectingBoundaryFailureFlag )
	{
		return -1;
	}
	else if ( !multipleBoundaryFailureFlag  &&  selfIntersectingBoundaryFailureFlag )
	{
		return -2;
	}
	else if ( multipleBoundaryFailureFlag  &&  selfIntersectingBoundaryFailureFlag )
	{
		return -3;
	}
	else
	{
		return 0;
	}
}


int MMesh::spinMarkedEdges(int turns)
{
	assertFinalised();

	// Gather the list of marked edges
	MEdgeList markedEdges;
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		if ( edges[edgeI]->isEdgeMarked() )
		{
			markedEdges.push_back( edges[edgeI] );
		}
	}

	bool bFailure = false;

	// Spin the edges
	for (int edgeI = 0; edgeI < markedEdges.size(); edgeI++)
	{
		if ( !markedEdges[edgeI]->isDestroyed() )
		{
			bool result = markedEdges[edgeI]->spin( turns );

			if ( !result )
			{
				bFailure = true;
			}
		}
	}

	compactAll();
	finalise();

	return bFailure  ?  -1  :  0;
}




int MMesh::rewireEdges(const MRewireEdgeTargetList &rewireTargets)
{
	assertFinalised();


	bool bFailure = false;
	// For each target:
	for (int targetI = 0; targetI < rewireTargets.size(); targetI++)
	{
		const MRewireEdgeTarget &target = rewireTargets[targetI];

		// Pick a vertex, thru a picked edge
		MFace *thruFace = NULL;
		MEdge *edgeToRewire = NULL;
		MVertex *sourceVertex = pickVertexThruEdge( target.sourcePick, thruFace, edgeToRewire );

		if ( sourceVertex != NULL  &&  edgeToRewire != NULL )
		{
			// Pick the target vertex
			MVertex *targetVertex = pickVertex( target.targetPick );

			if ( targetVertex != NULL )
			{
				// Check that the rewire is valid
				if ( isValidRewireEdgeTarget( edgeToRewire, sourceVertex, targetVertex ) )
				{
					MVertex *oppositeVertex = edgeToRewire->getOppositeVertex( sourceVertex );

					// Record the edge properties
					MEdge::EdgeProperties props = edgeToRewire->getProperties();

					// There may be an existing edge linking @oppositeVertex to @targetVertex
					MEdge *rewiredEdge = oppositeVertex->findEdgeTo( targetVertex, MFINDPOLICY_RETURNNULL );

					if ( rewiredEdge != NULL )
					{
						// Ensure that @rewiredEdge is incident to @mergedFace
						gs_assert( rewiredEdge->isIncidentTo( edgeToRewire->getFaceA() )  ||  rewiredEdge->isIncidentTo( edgeToRewire->getFaceB() ),
								"MMesh::rewireEdges(): existing rewired edge is not incident to the faces that are incident to the edge that is to be rewired\n" );
					}
					else
					{
						// No existing edge, create one by splitting from @oppositeVertex to @targetVertex
						MFace *splitFace = oppositeVertex->findBestSplitFace( targetVertex );

						// We have already check to see if the split edge already exists, so there should be no problem here
						gs_assert( splitFace != NULL, "MMesh::rewireEdges(): could not determine best split face\n" );

						// Attempt the split
						MFace *f = splitFace->split( oppositeVertex, targetVertex, rewiredEdge );

						gs_assert( f != NULL, "MMesh::rewireEdges(): face split failed\n" );
					}

					gs_assert( rewiredEdge != NULL, "MMesh::rewireEdges(): rewired edge is NULL\n" );
					// Copy the edge properties to the destination edge
					rewiredEdge->setProperties( props );

					// Dissolve the edge to rewire
					MFace *mergedFace = edgeToRewire->dissolve();

					if ( mergedFace == NULL )
					{
						bFailure = true;
					}
				}
			}
		}
	}


	compactAll();
	finalise();

	return bFailure  ?  -1  :  0;
}

bool MMesh::isValidRewireEdgeTarget(MEdge *edgeToRewire, MVertex *sourceVertex, MVertex *targetVertex)
{
	gs_assert( sourceVertex->isIncidentTo( edgeToRewire ), "MMesh::isValidRewireEdgeTarget(): @sourceVertex is not incident to @edgeToRewire\n" );

	if ( edgeToRewire->getNumFaces() == 2 )
	{
		if ( !targetVertex->isIncidentTo( edgeToRewire ) )
		{
			if ( targetVertex->isIncidentTo( edgeToRewire->getFaceA() )  ||  targetVertex->isIncidentTo( edgeToRewire->getFaceB() ) )
			{
				if ( edgeToRewire->canRewire() )
				{
					return true;
				}
			}
		}
	}

	return false;
}




int MMesh::getEdgeRunsForJoining(Array<MEdgeRun> &runs)
{
	//STAGE ONE:
	//check all of the marked edges and make sure they are boundary edges
	//if not, return -1 (error)
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  !edges[i]->isBoundary() )
		{
			if ( !edges[i]->isBoundary() )
			{
				return -1;
			}
		}
	}

	//check all edge marked vertices and make sure each one does not have
	//more than two incident marked edges
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->getMarkedEdgeCount() > 2 )
		{
			return -2;
		}
	}

	//STAGE TWO
	//generate edge runs
	discoverMarkedEdgeRuns( runs );


	//can only work on two runs
	if ( runs.size() != 2 )
	{
		return -3;
	}


	MEdgeRun &run0 = runs[0];
	MEdgeRun &run1 = runs[1];


	//make sure they are either both open or both closed
	if ( run0.isClosed()  !=  run1.isClosed() )
	{
		return -4;
	}



	//STAGE THREE
	//Ensure correct ordering

	//The edges must be matched such that the faces of the new edges
	//will have the correct ordering with respect to eachother
	//
	//The vertices are extracted from the boundaries (which are lists of edges)
	//in the same order as that of the edges that appear in the boundaries
	//
	//Due to the fact that all edges in both runs are boundary edges,
	//the order of these edges will be identical to that of the faces
	//that make use of them.
	//
	//Run 0 and run 1 should have OPPOSING orders at the end of this stage,
	//edge 0 of run 0 should oppose the order of run 0
	//edge 0 of run 1 should not oppose the order of run 1
	if ( run0.isEdge0InOrder() )
	{
		run0.flip();
	}

	if ( !run1.isEdge0InOrder() )
	{
		run1.flip();
	}


	return 0;
}



int MMesh::getFaceBoundariesForJoining(Array<MEdgeRun> &boundaries)
{
	//make sure only 2 regions are selected
	if ( countMarkedFaceRegions() != 2 )
	{
		return -1;
	}

	//STAGE ONE
	//Compute the edge lists of the boundaries

	//using the face mark information, compute the boundaries that
	//surround the two marked areas
	discoverMarkedFaceBoundaries( boundaries );


	//can only work with two boundaries
	if ( boundaries.size() != 2 )
	{
		//not 2 boundaries:
		return -1;
	}



	MEdgeRun &boundary0 = boundaries[0];
	MEdgeRun &boundary1 = boundaries[1];



	//STAGE TWO
	//Ensure correct ordering

	//The new faces must be created with the correct vertex ordering.
	//This ordering should be the same as that of the marked faces.

	//The vertices are extracted from the boundaries (which are lists of edges)
	//in the same order as that of the edges that appear in the boundaries

	//The faces created use vertices from boundary 0 in the order that they
	//come from the vertex list, so we must ensure that the ordering of
	//the edges in the boundaries is correct
	//The faces use vertices from boundary 1 in the opposite order than
	//that in which they come

	//The ordering of the vertices in a boundary is the same as the order
	//of the vertices in the first edge, edge 0.

	//When processing boundary 0:
	//there are two situations that edge 0 can be in:
	// 1. Has two incident faces: one marked, one not.
	//		If the ordering is opposite to that of the marked face, then reverse
	//		the ordering of the boundary
	// 2. Has one incident face which is marked
	//		In this case, the order of the edge is the same as the incident face
	//		so leave the boundary ordering alone

	//NOTE THAT THE ABOVE ASSUMES THAT THE EDGE 0 OF EACH BOUNDARY IS IN
	//THE SAME ORDER AS THE BOUNDARY. IF THIS IS NOT THE CASE, THE OPPOSITE
	//ACTION MUST BE TAKEN: eg reverse -> not reverse & vice verse

	//take the opposite actions for boundary 1


	MFace *markedFace;


	markedFace = boundary0[0]->getMarkedFace();
	if ( markedFace != NULL )
	{
		//reverse order to that of marked face
		//		OR
		//reverse order to boundary
		//		NOT BOTH
		if ( markedFace->isEdgeInOrder( boundary0[0] ) !=  boundary1.isEdge0InOrder())
		{
			boundary0.flip();
		}
	}

	markedFace = boundary1[0]->getMarkedFace();
	if ( markedFace != NULL )
	{
		//reverse order to that of marked face
		//		AND
		//reverse order to boundary
		//		OR NEITHER
		if ( markedFace->isEdgeInOrder( boundary1[0] ) ==  boundary1.isEdge0InOrder() )
		{
			boundary1.flip();
		}
	}



	//STAGE THREE
	//Destroy the marked faces, leaving holes where the selected regions
	//were.

	//make the boundary edges indesctructible before destroying the faces;
	//when the marked faces are deleted, these edges must not be destroyed
	for (int i = 0; i < boundary0.size(); i++)
	{
		boundary0[i]->setIndestructible();
	}
	for (int i = 0; i < boundary1.size(); i++)
	{
		boundary1[i]->setIndestructible();
	}


	//destroy the marked faces
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			faces[i]->destroy( true, true );
		}
	}


	//Make the boundary edges destructible again
	for (int i = 0; i < boundary0.size(); i++)
	{
		boundary0[i]->setDestructible();
	}
	for (int i = 0; i < boundary1.size(); i++)
	{
		boundary1[i]->setDestructible();
	}


	return 0;
}




int MMesh::weldMarkedEdges(const PolyBlend *blend, MWeldStyle style)
{
	assertFinalised();



	//STAGE ONE
	//get the two edge runs to join
	Array<MEdgeRun> runs;

	int getRunsResult = getEdgeRunsForJoining( runs );

	if ( getRunsResult != 0 )
	{
		return getRunsResult;
	}

	int weldResult = weldEdgeRuns( blend, style, runs );


	compactAll();
	finalise();

	if ( weldResult != 0 )
	{
		return weldResult - 4;
	}
	else
	{
		return 0;
	}
}


int MMesh::weldMarkedFaces(const PolyBlend *blend, MWeldStyle style)
{
	assertFinalised();



	Array<MEdgeRun> boundaries;
	int boundariesResult = getFaceBoundariesForJoining( boundaries );

	if ( boundariesResult != 0 )
	{
		return boundariesResult;
	}


	int weldResult = weldEdgeRuns( blend, style, boundaries );


	compactAll();
	finalise();

	if ( weldResult != 0 )
	{
		return weldResult - 1;
	}
	else
	{
		return 0;
	}
}






int MMesh::bridge(const PolyBlend *blend, int numSegments)
{
	assertFinalised();


	//STAGE ONE
	//get the two edge runs to join
	Array<MEdgeRun> runs;

	int getRunsResult = getEdgeRunsForJoining( runs );

	if ( getRunsResult != 0 )
	{
		return getRunsResult;
	}


	int createFacesResult = createTunnelFaces( blend, numSegments, runs[0], runs[1] );


	//faces may have been deleted so do a compactAll()
	compactAll();
	finalise();


	if ( createFacesResult != 0 )
	{
		return createFacesResult - 4;
	}
	else
	{
		return 0;
	}
}



int MMesh::tunnel(const PolyBlend *blend, int numSegments)
{
	assertFinalised();

	Array<MEdgeRun> boundaries;


	int boundariesResult = getFaceBoundariesForJoining( boundaries );

	if ( boundariesResult != 0 )
	{
		return boundariesResult;
	}


	//STAGE FOUR
	//create the tunnel faces
	int createFacesResult = createTunnelFaces( blend, numSegments, boundaries[0], boundaries[1] );


	//the original marked faces are deleted, so do a compactAll()
	compactAll();
	finalise();


	if ( createFacesResult != 0 )
	{
		return createFacesResult - 1;
	}
	else
	{
		return 0;
	}
}


int MMesh::createTunnelFaces(const PolyBlend *blend, int numSegments, MEdgeRun &run0, MEdgeRun &run1)
{
	//STAGE ONE
	//creating the tunnel between the two boundaries is effectively the same
	//as morphing/blending one boundary into another. The vertices that make
	//up the boundary must be matched, so that they morph into one another.
	//In cases where the boundaries to not consist of the same number of vertices,
	//several vertices from one boundary may morph into one vertex of another.
	//The result: edges may either morph into other edges (in which case a quad
	//is generated), or into a vertex (in which case a triangle is generated)


	//generate lists of vertices from the boundaries
	MVertexList v0, v1;
	//lower down: the first vertices of each vertex list (v0, v1) are added to
	//the end, so an extra space is required: reserve to save time
	v0.reserve( run0.size() + 1 );
	v1.reserve( run1.size() + 1 );
	run0.extractVertices( v0 );
	run1.extractVertices( v1 );


	//CHECK FOR SHARED VERTICES
	//tunnel operation cannot proceed if the two boundaries share 1 or more vertices
	for (int vertexI = 0; vertexI < v0.size(); vertexI++)
	{
		for (int vertexJ = 0; vertexJ < v1.size(); vertexJ++)
		{
			if ( v0[vertexI] == v1[vertexJ] )
			{
				return -1;
			}
		}
	}


	//The blending code uses BlendingPolyline objects, so convert the vertex lists
	//into polylines
	BlendingPolyline poly0, poly1;
	vertexListToBlendingPolyline( v0, poly0 );
	vertexListToBlendingPolyline( v1, poly1 );


	//compute the blending
	PolyBlendPointList blendPath;
	blend->computeBlendPath( poly0, poly1, blendPath );




	//STAGE TWO
	//Need texture coords
	Array<double> tex0, tex1;
	PolyBlend::generateTextureCoords( poly0, poly1, blendPath, tex0, tex1 );



	//STAGE THREE

	//FACE FLAGS
	//The tunnel algorithm MUST detect any existing faces which are equivalent
	//to the faces which would be generated by tunnelling. In this case,
	//the existing face and the new face must CANCEL OUT. This is done by:
	// 1. Finding an existing face
	// 2. destroying it
	// 3. not generating a face to replace it
	//
	//To ensure that no face is generated to replace the existing face, an
	//Array of boolean flags is computed, where each flag corresponds to
	//a face that WOULD be created. If a flags has a FALSE value, the corresponding
	//face will not be created.
	Array<bool> faceFlags;
	faceFlags.reserve( blendPath.size() - 1 );


	Array<MVertex*> vertsForFace;

	for (int blendI = 0; blendI < blendPath.size() - 1; blendI++)
	{
		int blendINext = blendI + 1;

		//vertex indices
		int a = blendPath[blendI].p0Index;
		int b = blendPath[blendINext].p0Index;
		int c = blendPath[blendINext].p1Index;
		int d = blendPath[blendI].p1Index;


		//vertices
		MVertex *va = v0[a];
		MVertex *vb = v0[b];
		MVertex *vc = v1[c];
		MVertex *vd = v1[d];


		//build a vertex list, avoiding duplicates
		vertsForFace.clear();

		vertsForFace.push_back( va );
		if ( vb != va )
		{
			vertsForFace.push_back( vb );
		}
		vertsForFace.push_back( vc );
		if ( vd != vc )
		{
			vertsForFace.push_back( vd );
		}


		if ( vertsForFace.size() >= 3 )
		{
			//more than 3 vertices, try to find an existing face
			MFace *existingFace = MVertex::findCommonFaceAmong( vertsForFace );

			if ( existingFace != NULL )
			{
				//destroy the existing face
				existingFace->destroy( false, false );
			}

			//true if no existing face, false otherwise
			faceFlags.push_back( existingFace == NULL );
		}
		else
		{
			//less than 3 vertices; no face to create
			faceFlags.push_back( false );
		}
	}



	//STAGE FOUR
	//Handle pre-existing edges
	//IF an edge e exists between the BLEND VERTICES va and vb, e is a BLEND EDGE.
	//If e exists, use this existing edge, instead of trying to create new ones.
	//When multiple segments are used (see numSegments parameter), split e
	//and use the vertices resulting from the split, rather than create new
	//vertices (note: the splitting is done in stage FIVE)
	//Only create new vertices when e cannot be found.
	//if e exists, then CHECK the following:
	//	- if e already has two incident faces, then do not create any faces
	//	  which use va and vb; do this by setting the face flags corresponding
	//	  to the surrouding faces to false
	// - if e already has one incident face:
	//   - if the face flags corresponding to both surrouding faces are true,
	//   	 set the previous one to false
	//DEFINITION OF 'BLEND VERTICES':
	//va and vb are blend vertices if the blend path would have va morph into vb
	//(or the other way around)
	//DEFINITION OF 'BLEND EDGE':
	//e is a blend edge if its end points va, and vb are BLEND VERTICES


	//if the runs are closed, there is no need to go right to the end, as
	//the last entries are repetitions of the first
	int blendPathEnd = run0.isClosed()  ?  blendPath.size() - 1 : blendPath.size();

	//index of (first) previous face flag: if the edge runs are open, then -1
	int firstPrevFaceFlagIndex = run0.isClosed()  ?  blendPathEnd - 1 : -1;
	int prevFaceFlagI = firstPrevFaceFlagIndex;

	//get the face flag corresponding to the previous face
	bool prevFaceFlag = false;
	if ( prevFaceFlagI != -1 )
	{
		prevFaceFlag = faceFlags[prevFaceFlagI];
	}

	for (int faceFlagI = 0; faceFlagI < blendPathEnd; faceFlagI++)
	{
		//pointer to the current face flag
		bool currentFaceFlag = false;
		if ( faceFlagI < faceFlags.size() )
		{
			currentFaceFlag = faceFlags[faceFlagI];
		}


		//only need to test if one or more of the surrounding faces is going to
		//be created
		if ( prevFaceFlag  ||  currentFaceFlag )
		{
			//vertex indices
			int a = blendPath[faceFlagI].p0Index;
			int b = blendPath[faceFlagI].p1Index;

			//BLEND VERTICES va and vb
			MVertex *va = v0[a];
			MVertex *vb = v1[b];

			//BLEND EDGE e; try to find
			MEdge *e = va->findEdgeTo( vb, MFINDPOLICY_RETURNNULL );


			if ( e != NULL )
			{
				//e exists; test it
				int faceCount = e->getNumFaces();

				if ( faceCount == 2 )
				{
					//already got 2 incident faces, cannot add more
					prevFaceFlag = currentFaceFlag = false;
				}
				else
				{
					if ( faceCount == 1 )
					{
						//got 1 incident face
						if ( prevFaceFlag  &&  currentFaceFlag )
						{
							//want to add 2 incident faces
							//can only add 1
							prevFaceFlag = false;
						}
					}
				}
			}
		}


		//write the face flags back
		if ( prevFaceFlagI != -1 )
		{
			faceFlags[prevFaceFlagI] = prevFaceFlag;
		}

		if ( faceFlagI < faceFlags.size() )
		{
			faceFlags[faceFlagI] = currentFaceFlag;
		}


		//set previous values for next iteration
		prevFaceFlag = currentFaceFlag;
		prevFaceFlagI = faceFlagI;
	}


	//STAGE FIVE
	//Generate vertex lists and texture co-ordinate lists
	Array<MVertexList> vertexLists;
	Array< Array<Point2f> > texLists;

	//reserve space in the vertex and texture co-ordinate lists
	vertexLists.resize( numSegments + 1 );
	texLists.resize( numSegments + 1 );

	vertexLists.front().reserve( blendPath.size() );
	vertexLists.back().reserve( blendPath.size() );
	texLists.front().reserve( blendPath.size() );
	texLists.back().reserve( blendPath.size() );

	for (int blendI = 0; blendI < blendPath.size(); blendI++)
	{
		int a = blendPath[blendI].p0Index;
		int b = blendPath[blendI].p1Index;

		vertexLists.front().push_back( v0[a] );
		vertexLists.back().push_back( v1[b] );
		texLists.front().push_back( Point2f( tex0[blendI], 0.0 ) );
		texLists.back().push_back( Point2f( tex1[blendI], 1.0 ) );
	}

	for (int segmentI = 1; segmentI < numSegments; segmentI++)
	{
		vertexLists[segmentI].reserve( blendPath.size() );
		texLists[segmentI].reserve( blendPath.size() );
	}

	if ( numSegments > 1 )
	{
		//get a pointer to the face flag corresponding to the previous face
		prevFaceFlag = firstPrevFaceFlagIndex == -1  ?  false : faceFlags[firstPrevFaceFlagIndex];

		double vInc = 1.0 / (double)numSegments;
		for (int blendI = 0; blendI < blendPathEnd; blendI++)
		{
			//pointer to the current face flag
			bool currentFaceFlag = blendI < faceFlags.size()  ?  faceFlags[blendI]  :  false;

			if ( prevFaceFlag  ||  currentFaceFlag )
			{
				//the BLEND VERTICES va and vb
				MVertex *va = vertexLists.front()[blendI];
				MVertex *vb = vertexLists.back()[blendI];

				//try to find the BLEND EDGE e
				MEdge *e = va->findEdgeTo( vb, MFINDPOLICY_RETURNNULL );

				if ( e != NULL )
				{
					//if we have got this far,
					//(been through if ( faceFlags[prev]  ||  faceFlags[i] ), see above)
					//then a should be split
					//(e does not have too many incident faces, this was checked
					//in STAGE SEVEN)

					//split the existing edge, rather than add new isolated vertices
					MVertexList splitVertices;
					MEdgeList splitEdges;
					e->splitIntoSegments( va, numSegments, splitVertices, splitEdges );

					for (int segI = 1; segI < numSegments; segI++)
					{
						vertexLists[segI].push_back( splitVertices[segI-1] );
					}
				}
				else
				{
					//e does not exist:
					//create new vertices
					const Point3 &position0 = va->getPosition();
					const Point3 &position1 = vb->getPosition();

					double v = vInc;
					for (int segI = 1; segI < numSegments; segI++)
					{
						MVertex *newVertex = addVertex( lerp( position0, position1, v ) );
						vertexLists[segI].push_back( newVertex );
						v += vInc;
					}
				}
			}
			else
			{
				//pad the vertex lists with NULLs
				for (int segI = 1; segI < numSegments; segI++)
				{
					vertexLists[segI].push_back( NULL );
				}
			}

			prevFaceFlag = currentFaceFlag;
		}

		if ( run0.isClosed() )
		{
			//if closed, in inner vertex lists, repeat the first vertices at the ends
			for (int j = 1; j < numSegments; j++)
			{
				MVertex *first = vertexLists[j].front();
				vertexLists[j].push_back( first );
			}
		}

		//generate texture co-ordinates
		for (int blendI = 0; blendI < blendPath.size(); blendI++)
		{
			const Point2f &tex0 = texLists.front()[blendI];
			const Point2f &tex1 = texLists.back()[blendI];

			double v = vInc;
			for (int segI = 1; segI < numSegments; segI++)
			{
				texLists[segI].push_back( lerp( tex0, tex1, v ) );
				v += vInc;
			}
		}
	}




	//STAGE SIX
	//Generate faces
	MVertexList verts;
	Array<MVertexAttrib> attribs;

	verts.reserve( 4 );
	attribs.reserve( 4 );


	for (int segI = 0; segI < numSegments; segI++)
	{
		int segINext = segI + 1;

		for (int blendI = 0; blendI < blendPath.size() - 1; blendI++)
		{
			if ( faceFlags[blendI] )
			{
				int blendINext = blendI + 1;

				//get the four vertices and texture co-ordinates
				MVertex *va = vertexLists[segI][blendI];
				MVertex *vb = vertexLists[segI][blendINext];
				MVertex *vc = vertexLists[segINext][blendINext];
				MVertex *vd = vertexLists[segINext][blendI];

				const Point2f &ta = texLists[segI][blendI];
				const Point2f &tb = texLists[segI][blendINext];
				const Point2f &tc = texLists[segINext][blendINext];
				const Point2f &td = texLists[segINext][blendI];


				//create the vertex and attribute lists
				verts.clear();
				attribs.clear();


				//add vertex A
				verts.push_back( va );
				attribs.push_back( MVertexAttrib( ta, -1 ) );

				//if vertex B == vertex A, then the face is a triangle, so don't add
				//it (again)
				if ( vb != va )
				{
					verts.push_back( vb );
					attribs.push_back( MVertexAttrib( tb, -1 ) );
				}

				//add vertex C
				verts.push_back( vc );
				attribs.push_back( MVertexAttrib( tc, -1 ) );

				//if vertex C == vertex D, then the face is a triangle, so don't add
				//it (again)
				if ( vd != vc )
				{
					verts.push_back( vd );
					attribs.push_back( MVertexAttrib( td, -1 ) );
				}


				//(try to) create the face
				if ( verts.size() >= 3 )
				{
					bool canCreateFace = true;

					//scan the edges that would be created (just in case)
					int p = verts.size() - 1;
					for (int q = 0; q < verts.size(); q++)
					{
						MEdge *edge = verts[p]->findEdgeTo( verts[q], MFINDPOLICY_RETURNNULL );

						if ( edge != NULL )
						{
							//edge already exists; check to see if it already has 2
							//incident faces
							if ( edge->getNumFaces() >= 2 )
							{
								//is has more than 2 incident faces, cannot add another face
								canCreateFace = false;
								break;
							}
						}

						p = q;
					}

					if ( canCreateFace )
					{
						addFace( verts, attribs, -1 );
					}
				}
			}
		}
	}


	//STAGE SEVEN
	//clear up
	//some edges may have been left isolated
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		if ( !edges[edgeI]->isDestroyed() )
		{
			edges[edgeI]->destroyIfIsolated( true );
		}
	}
	//just in case: some isolated vertices may have been created; destroy them
	for (int vertexListI = 0; vertexListI < vertexLists.size(); vertexListI++)
	{
		for (int vertexI = 0; vertexI < vertexLists[vertexListI].size(); vertexI++)
		{
			if ( vertexLists[vertexListI][vertexI] != NULL )
			{
				if ( !vertexLists[vertexListI][vertexI]->isDestroyed() )
				{
					vertexLists[vertexListI][vertexI]->destroyIfIsolated();
				}
			}
		}
	}


	return 0;
}



int MMesh::weldEdgeRuns(const PolyBlend *blend, MWeldStyle style, Array<MEdgeRun> &runs)
{
	MEdgeRun &run0 = runs[0];
	MEdgeRun &run1 = runs[1];



	//STAGE TWO
	//Use polyline blending to match the vertices in the edge runs for welding
	//See tunnel tool for more info

	//generate lists of vertices from the edge runs
	MVertexList v0, v1;
	v0.reserve( run0.getNumVertices() );
	v1.reserve( run1.getNumVertices() );
	run0.extractVertices( v0 );
	run1.extractVertices( v1 );


	//The blending code uses BlendingPolyline objects, so convert the vertex lists
	//into polylines
	BlendingPolyline poly0, poly1;
	vertexListToBlendingPolyline( v0, poly0 );
	vertexListToBlendingPolyline( v1, poly1 );


	//compute the blending
	PolyBlendPointList blendPath;
	blend->computeBlendPath( poly0, poly1, blendPath );

	//closed polylines result in a blend path that repeats the first match
	//at the end; remove it
	if ( run0.isClosed() )
	{
		blendPath.pop_back();
	}



	//STAGE THREE
	//destroy any isolated edges which are members of the edge runs
	for (int runI = 0; runI < run0.size(); runI++)
	{
		//need isolated vertices intact
		run0[runI]->destroyIfIsolated( false );
	}
	for (int runI = 0; runI < run1.size(); runI++)
	{
		//need isolated vertices intact
		run1[runI]->destroyIfIsolated( false );
	}


	//STAGE FOUR
	//weld the corresponding vertices together
	bool success = true;

	for (int blendI = 0; blendI < blendPath.size(); blendI++)
	{
		int a = blendPath[blendI].p0Index;
		int b = blendPath[blendI].p1Index;
		MVertex *va = v0[a];
		MVertex *vb = v1[b];

		//Isolated edges are destroyed in stage three, above. Isolated vertices,
		//however, are left intact. If va and vb are both isolated, welding
		//is unnecessary:
		//only bother with a weld if at least one vertex is NOT isolated
		if ( !va->isIsolated()  ||  !vb->isIsolated() )
		{
			//NOTE: if either va or vb is isolated, MVertex::weldTo() destroys the
			//isolated vertex. Initially, one may think that this would need that
			//the 'if' statement above would need to check if the vertices
			//are destroyed, in case a previous loop destroyed them. HOWEVER,
			//this is not the case, since their entries in the vertex lists
			//are replaced by the welded vertex
			MVertex *welded = va->weldTo( vb, style, false );

			if ( welded != NULL )
			{
				//one of va and vb has been replaced, make sure the vertex lists no longer
				//point to the vertex that was replaced (and destroyed)
				v0[a] = welded;
				v1[b] = welded;
			}

			success = success  &&  welded != NULL;
		}
	}



	//STAGE FIVE
	//destroy any isolated vertices
	//NOTE: only need to scan the vertex lists v0 and v1
	for (int vertexI = 0; vertexI < v0.size(); vertexI++)
	{
		//make sure the vertex hasn't already been destroyed
		if ( !v0[vertexI]->isDestroyed() )
		{
			v0[vertexI]->destroyIfIsolated();
		}
	}
	for (int vertexI = 0; vertexI < v1.size(); vertexI++)
	{
		if ( !v1[vertexI]->isDestroyed() )
		{
			v1[vertexI]->destroyIfIsolated();
		}
	}


	return success  ?  0 : -1;
}



void MMesh::flipFaceRegion(MFaceList &faceList)
{
	for (int i = 0; i < faceList.size(); i++)
	{
		faceList[i]->flip();
		faceList[i]->secondaryMark();
	}

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceSecondaryMarked() )
		{
			edges[i]->swapVertices();
		}
	}

	for (int i = 0; i < faceList.size(); i++)
	{
		faceList[i]->secondaryUnmark();
	}
}



void MMesh::flip()
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->flip();
	}

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->swapVertices();
	}


	finalise();
}



void MMesh::drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip, MDrawQuadsState &state)
{
	assertFinalised();

	state.drawQuads( this, points, bCloseRows, bCloseColumns, numColumns, bFlip );

	compactAll();
	finalise();
}

void MMesh::drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip)
{
	MDrawQuadsState state;

	drawQuads( points, bCloseRows, bCloseColumns, numColumns, bFlip, state );
}



void MMesh::drawFace(const MDrawFacePointList &points, bool bFlip, MDrawFaceState &state)
{
	assertFinalised();

	state.drawFace( this, points, bFlip );

	compactAll();
	finalise();
}

void MMesh::drawFace(const MDrawFacePointList &points, bool bFlip)
{
	MDrawFaceState state;

	drawFace( points, bFlip, state );
}



void MMesh::transform(const Matrix4 &m)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setPosition( vertices[i]->getPosition() * m );
	}

	finalise();
}




//*********************** TRANSFORMATION OPERATIONS ***********************
//*************************************************************************

void MMesh::transformVerticesInit(MTransformationTarget &target, bool perGroup)
{
	target.clear();

	if ( perGroup )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isVertexMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				MTransformationTarget::XformGroup &g = target.newGroup();

				MVertex *v = vertices[i];
				v->discoverVertexMarkedRegion( g.vertices );
			}
		}


		for (int i = 0; i < target.groups.size(); i++)
		{
			for (int j = 0; j < target.groups[i].vertices.size(); j++)
			{
				MVertex *v = target.groups[i].vertices[j];
				v->secondaryUnmark();
				v->savePosition();
			}

			target.groups[i].finalise();
		}
	}
	else
	{
		MTransformationTarget::XformGroup &g = target.newGroup();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isVertexMarked() )
			{
				g.vertices.push_back( vertices[i] );
				vertices[i]->savePosition();
			}
		}

		g.finalise();
	}

	target.finalise();
	target.setMesh( this );
}

void MMesh::transformEdgesInit(MTransformationTarget &target, bool perGroup)
{
	target.clear();

	if ( perGroup )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				MTransformationTarget::XformGroup &g = target.newGroup();

				MVertex *v = vertices[i];
				v->discoverEdgeMarkedRegionVertices( g.vertices );
			}
		}


		for (int i = 0; i < target.groups.size(); i++)
		{
			for (int j = 0; j < target.groups[i].vertices.size(); j++)
			{
				MVertex *v = target.groups[i].vertices[j];
				v->secondaryUnmark();
				v->savePosition();
			}

			target.groups[i].finalise();
		}
	}
	else
	{
		MTransformationTarget::XformGroup &g = target.newGroup();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked() )
			{
				g.vertices.push_back( vertices[i] );
				vertices[i]->savePosition();
			}
		}

		g.finalise();
	}

	target.finalise();
	target.setMesh( this );
}


void MMesh::transformFacesInit(MTransformationTarget &target, bool perGroup)
{
	target.clear();

	if ( perGroup )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				MTransformationTarget::XformGroup &g = target.newGroup();

				MVertex *v = vertices[i];
				v->discoverFaceMarkedRegionVertices( g.vertices );
			}
		}


		for (int i = 0; i < target.groups.size(); i++)
		{
			for (int j = 0; j < target.groups[i].vertices.size(); j++)
			{
				MVertex *v = target.groups[i].vertices[j];
				v->secondaryUnmark();
				v->savePosition();
			}

			target.groups[i].finalise();
		}

		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->secondaryUnmark();
		}
	}
	else
	{
		MTransformationTarget::XformGroup &g = target.newGroup();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked() )
			{
				g.vertices.push_back( vertices[i] );
				vertices[i]->savePosition();
			}
		}

		g.finalise();
	}

	target.finalise();
	target.setMesh( this );
}




void MMesh::transformVertices(const Matrix4 &m, bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !markedOnly  ||  vertices[i]->isVertexMarked() )
		{
			vertices[i]->i_transformPosition( m, false );
		}
	}

	vertexTransformFinalise();
	finalise();
}

void MMesh::transformVertices(const Transformation *x, bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !markedOnly  ||  vertices[i]->isVertexMarked() )
		{
			vertices[i]->setPosition( x->transformPoint( vertices[i]->getPosition() ) );
		}
	}

	finalise();
}

void MMesh::saveVertexPositions(bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !markedOnly  ||  vertices[i]->isVertexMarked() )
		{
			vertices[i]->savePosition();
		}
	}
}

void MMesh::restoreVertexPositions(bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !markedOnly  ||  vertices[i]->isVertexMarked() )
		{
			vertices[i]->restorePosition();
		}
	}

	finalise();
}


void MMesh::transformEdgeMarkedVertices(const Matrix4 &m)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isEdgeMarked() )
		{
			vertices[i]->i_transformPosition( m, false );
		}
	}

	vertexTransformFinalise();
	finalise();
}

void MMesh::saveEdgeMarkedVertexPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isEdgeMarked() )
		{
			vertices[i]->savePosition();
		}
	}
}

void MMesh::restoreEdgeMarkedVertexPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isEdgeMarked() )
		{
			vertices[i]->restorePosition();
		}
	}

	finalise();
}


void MMesh::transformFaceMarkedVertices(const Matrix4 &m)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->i_transformPosition( m, false );
		}
	}

	vertexTransformFinalise();
	finalise();
}

void MMesh::saveFaceMarkedVertexPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->savePosition();
		}
	}
}

void MMesh::restoreFaceMarkedVertexPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->restorePosition();
		}
	}

	finalise();
}


void MMesh::transformMarkedComponents(const Matrix4 &m)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  vertices[i]->isEdgeMarked()  ||  vertices[i]->isFaceMarked() )
		{
			vertices[i]->i_transformPosition( m, false );
		}
	}

	vertexTransformFinalise();
	finalise();
}

void MMesh::saveMarkedComponentPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  vertices[i]->isEdgeMarked()  ||  vertices[i]->isFaceMarked() )
		{
			vertices[i]->savePosition();
		}
	}
}

void MMesh::restoreMarkedComponentPositions()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  vertices[i]->isEdgeMarked()  ||  vertices[i]->isFaceMarked() )
		{
			vertices[i]->restorePosition();
		}
	}

	finalise();
}



void MMesh::vertexTransformFinalise()
{
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->vertexTransformFinalise();
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->clearTransformedFlag();
	}
}




void MMesh::tweakAddVertex(std::set<MVertex*> &vertexSelection, std::set<MVertex*> &tweakSelection, MVertex *vertex)
{
	vertexSelection.insert( vertex );
	tweakSelection.insert( vertex );
}

void MMesh::tweakAddEdge(std::set<MEdge*> &edgeSelection, std::set<MVertex*> &tweakSelection, MEdge *edge)
{
	edgeSelection.insert( edge );
	tweakSelection.insert( edge->getVertexA() );
	tweakSelection.insert( edge->getVertexB() );
}

void MMesh::tweakAddFace(std::set<MFace*> &faceSelection, std::set<MVertex*> &tweakSelection, MFace *face)
{
	faceSelection.insert( face );
	for (int vertexI = 0; vertexI < face->getSize(); vertexI++)
	{
		tweakSelection.insert( face->getVertex( vertexI ) );
	}
}



void MMesh::tweakComponents(const MTweakComponentList &tweaks, bool bRestoreSelection)
{
	assertFinalised();

	if ( bRestoreSelection )
	{
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			vertices[vertexI]->vertexMarkSave();
		}
		for (int edgeI = 0; edgeI < edges.size(); edgeI++)
		{
			edges[edgeI]->edgeMarkSave();
		}
		for (int faceI = 0; faceI < faces.size(); faceI++)
		{
			faces[faceI]->faceMarkSave();
		}
	}

	std::set<MVertex*> vertexSelection;
	std::set<MEdge*> edgeSelection;
	std::set<MFace*> faceSelection;
	std::set<MVertex*> tweakSelection;
	MEdgeList ringOrLoop;


	if ( tweaks.size() > 0 )
	{
		// Initialise the selection
		if ( tweaks[0].clearSelectionFlag )
		{
			for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
			{
				vertices[vertexI]->vertexUnmark();
			}
			for (int edgeI = 0; edgeI < edges.size(); edgeI++)
			{
				edges[edgeI]->edgeUnmark();
			}
			for (int faceI = 0; faceI < faces.size(); faceI++)
			{
				faces[faceI]->faceUnmark();
			}
		}
		else
		{
			for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
			{
				if ( vertices[vertexI]->isVertexMarked() )
				{
					tweakAddVertex( vertexSelection, tweakSelection, vertices[vertexI] );
				}
			}
			for (int edgeI = 0; edgeI < edges.size(); edgeI++)
			{
				if ( edges[edgeI]->isEdgeMarked() )
				{
					tweakAddEdge( edgeSelection, tweakSelection, edges[edgeI] );
				}
			}
			for (int faceI = 0; faceI < faces.size(); faceI++)
			{
				if ( faces[faceI]->isFaceMarked() )
				{
					tweakAddFace( faceSelection, tweakSelection, faces[faceI] );
				}
			}
		}



		// For each tweak
		for (int tweakI = 0; tweakI < tweaks.size(); tweakI++)
		{
			const MTweakComponent &tweak = tweaks[tweakI];

			// If the selection is to be cleared:
			if ( tweak.clearSelectionFlag )
			{
				// Unmark components in selection
				for (std::set<MVertex*>::iterator iter = vertexSelection.begin(); iter != vertexSelection.end(); ++iter)
				{
					(*iter)->vertexUnmark();
				}
				for (std::set<MEdge*>::iterator iter = edgeSelection.begin(); iter != edgeSelection.end(); ++iter)
				{
					(*iter)->edgeUnmark();
				}
				for (std::set<MFace*>::iterator iter = faceSelection.begin(); iter != faceSelection.end(); ++iter)
				{
					(*iter)->faceUnmark();
				}

				// Clear the selection
				vertexSelection.clear();
				edgeSelection.clear();
				faceSelection.clear();
				tweakSelection.clear();
			}



			// Pick the component, mark it, and add it to the selection
			Point3 closestPoint;
			if ( tweak.target == MTweakComponent::VERTICES )
			{
				MVertex *vertex = pickVertex( tweak.pick );
				if ( vertex != NULL )
				{
					vertex->vertexMark();
					vertexSelection.insert( vertex );
					tweakSelection.insert( vertex );
				}
			}
			else if ( tweak.target == MTweakComponent::EDGES )
			{
				MEdge *edge = pickEdge( tweak.pick, closestPoint );
				if ( edge != NULL )
				{
					edge->edgeMark();
					edgeSelection.insert( edge );
					tweakSelection.insert( edge->getVertexA() );
					tweakSelection.insert( edge->getVertexB() );
				}
			}
			else if ( tweak.target == MTweakComponent::EDGERINGS  ||  tweak.target == MTweakComponent::EDGELOOPS )
			{
				ringOrLoop.clear();
				MEdge *seed;
				if ( tweak.target == MTweakComponent::EDGERINGS )
				{
					seed = pickEdgeRing( ringOrLoop, tweak.pick, closestPoint );
				}
				else if ( tweak.target == MTweakComponent::EDGELOOPS )
				{
					seed = pickEdgeLoop( ringOrLoop, tweak.pick, closestPoint );
				}

				if ( seed != NULL )
				{
					for (int edgeI = 0; edgeI < ringOrLoop.size(); edgeI++)
					{
						MEdge *edge = ringOrLoop[edgeI];
						edge->edgeMark();
						edgeSelection.insert( edge );
						tweakSelection.insert( edge->getVertexA() );
						tweakSelection.insert( edge->getVertexB() );
					}
				}
			}
			else if ( tweak.target == MTweakComponent::FACES )
			{
				MFace *face = pickFace( tweak.pick );
				if ( face != NULL )
				{
					face->faceMark();
					faceSelection.insert( face );
					for (int vertexI = 0; vertexI < face->getSize(); vertexI++)
					{
						MVertex *vertex = face->getVertex( vertexI );
						tweakSelection.insert( vertex );
					}
				}
			}

			// Apply the translation to the vertices that are to be tweaked
			for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
			{
				(*iter)->translate( tweak.translation );
			}
		}
	}


	if ( bRestoreSelection )
	{
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			vertices[vertexI]->vertexMarkRestore();
		}
		for (int edgeI = 0; edgeI < edges.size(); edgeI++)
		{
			edges[edgeI]->edgeMarkRestore();
		}
		for (int faceI = 0; faceI < faces.size(); faceI++)
		{
			faces[faceI]->faceMarkRestore();
		}
	}

	finalise();
}





MSurfaceTweakComponent::Target MMesh::getSurfaceTweakComponentType(MSurfaceTweakComponent::Target target)
{
	if ( target == MSurfaceTweakComponent::EDGERINGS  ||  target == MSurfaceTweakComponent::EDGELOOPS )
	{
		return MSurfaceTweakComponent::EDGES;
	}
	else
	{
		return target;
	}
}

void MMesh::surfaceTweakComponents(const MSurfaceTweakComponentList &tweaks, bool bRestoreSelection)
{
	assertFinalised();

	if ( bRestoreSelection )
	{
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			vertices[vertexI]->vertexMarkSave();
		}
		for (int edgeI = 0; edgeI < edges.size(); edgeI++)
		{
			edges[edgeI]->edgeMarkSave();
		}
		for (int faceI = 0; faceI < faces.size(); faceI++)
		{
			faces[faceI]->faceMarkSave();
		}
	}

	std::set<MVertex*> vertexSelection;
	std::set<MEdge*> edgeSelection;
	std::set<MFace*> faceSelection;
	std::set<MVertex*> tweakSelection;
	MVertexSurfaceTweakAdjustList surfaceAdjust;
	MVertexAdjustList normalAdjust;
	MEdgeList ringOrLoop;


	if ( tweaks.size() > 0 )
	{
		// Initialise the selection
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			vertices[vertexI]->vertexUnmark();
		}
		for (int edgeI = 0; edgeI < edges.size(); edgeI++)
		{
			edges[edgeI]->edgeUnmark();
		}
		for (int faceI = 0; faceI < faces.size(); faceI++)
		{
			faces[faceI]->faceUnmark();
		}


		MSurfaceTweakComponent::Target prevComponentType = getSurfaceTweakComponentType( tweaks[0].target );



		// For each tweak
		for (int tweakI = 0; tweakI < tweaks.size(); tweakI++)
		{
			const MSurfaceTweakComponent &tweak = tweaks[tweakI];
			MSurfaceTweakComponent::Target componentType = getSurfaceTweakComponentType( tweak.target );


			// If the selection is to be cleared, or if the component type has changed:
			if ( tweak.clearSelectionFlag  ||  prevComponentType != componentType )
			{
				// Unmark components in selection
				for (std::set<MVertex*>::iterator iter = vertexSelection.begin(); iter != vertexSelection.end(); ++iter)
				{
					(*iter)->vertexUnmark();
				}
				for (std::set<MEdge*>::iterator iter = edgeSelection.begin(); iter != edgeSelection.end(); ++iter)
				{
					(*iter)->edgeUnmark();
				}
				for (std::set<MFace*>::iterator iter = faceSelection.begin(); iter != faceSelection.end(); ++iter)
				{
					(*iter)->faceUnmark();
				}

				// Clear the selection
				vertexSelection.clear();
				edgeSelection.clear();
				faceSelection.clear();
				tweakSelection.clear();
			}



			// Pick the component, mark it, and add it to the selection
			Point3 closestPoint;
			if ( tweak.target == MSurfaceTweakComponent::VERTICES )
			{
				MVertex *vertex = pickVertex( tweak.pick );
				if ( vertex != NULL )
				{
					vertex->vertexMark();
					vertexSelection.insert( vertex );
					tweakSelection.insert( vertex );
				}
			}
			else if ( tweak.target == MSurfaceTweakComponent::EDGES )
			{
				MEdge *edge = pickEdge( tweak.pick, closestPoint );
				if ( edge != NULL )
				{
					edge->edgeMark();
					edgeSelection.insert( edge );
					tweakSelection.insert( edge->getVertexA() );
					tweakSelection.insert( edge->getVertexB() );
				}
			}
			else if ( tweak.target == MSurfaceTweakComponent::EDGERINGS  ||  tweak.target == MSurfaceTweakComponent::EDGELOOPS )
			{
				ringOrLoop.clear();
				MEdge *seed;
				if ( tweak.target == MSurfaceTweakComponent::EDGERINGS )
				{
					seed = pickEdgeRing( ringOrLoop, tweak.pick, closestPoint );
				}
				else if ( tweak.target == MSurfaceTweakComponent::EDGELOOPS )
				{
					seed = pickEdgeLoop( ringOrLoop, tweak.pick, closestPoint );
				}

				if ( seed != NULL )
				{
					for (int edgeI = 0; edgeI < ringOrLoop.size(); edgeI++)
					{
						MEdge *edge = ringOrLoop[edgeI];
						edge->edgeMark();
						edgeSelection.insert( edge );
						tweakSelection.insert( edge->getVertexA() );
						tweakSelection.insert( edge->getVertexB() );
					}
				}
			}
			else if ( tweak.target == MSurfaceTweakComponent::FACES )
			{
				MFace *face = pickFace( tweak.pick );
				if ( face != NULL )
				{
					face->faceMark();
					faceSelection.insert( face );
					for (int vertexI = 0; vertexI < face->getSize(); vertexI++)
					{
						MVertex *vertex = face->getVertex( vertexI );
						tweakSelection.insert( vertex );
					}
				}
			}

			surfaceAdjust.clear();
			normalAdjust.clear();


			if ( tweak.mode == MSurfaceTweakComponent::NORMAL )
			{
				normalAdjust.reserve( tweakSelection.size() );
			}
			else if ( tweak.mode == MSurfaceTweakComponent::SURFACE )
			{
				surfaceAdjust.reserve( tweakSelection.size() );
				surfaceAdjust.setViewVector( tweak.viewVector );
			}


			// Tweak the vertices
			if ( componentType == MSurfaceTweakComponent::VERTICES )
			{
				if ( tweak.mode == MSurfaceTweakComponent::NORMAL )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->vertexTweakAlongNormalAdjustable( normalAdjust.push_back() );
					}
				}
				else if ( tweak.mode == MSurfaceTweakComponent::SURFACE )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->vertexTweakAlongSurfaceAdjustable( surfaceAdjust.push_back(), tweak.viewVector );
					}
				}
			}
			else if ( componentType == MSurfaceTweakComponent::EDGES )
			{
				if ( tweak.mode == MSurfaceTweakComponent::NORMAL )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->edgeTweakAlongNormalAdjustable( normalAdjust.push_back() );
					}
				}
				else if ( tweak.mode == MSurfaceTweakComponent::SURFACE )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->edgeTweakAlongSurfaceAdjustable( surfaceAdjust.push_back(), tweak.viewVector );
					}
				}
			}
			else if ( componentType == MSurfaceTweakComponent::FACES )
			{
				if ( tweak.mode == MSurfaceTweakComponent::NORMAL )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->faceTweakAlongNormalAdjustable( normalAdjust.push_back() );
					}
				}
				else if ( tweak.mode == MSurfaceTweakComponent::SURFACE )
				{
					for ( std::set<MVertex*>::iterator iter = tweakSelection.begin(); iter != tweakSelection.end(); ++iter)
					{
						(*iter)->faceTweakAlongSurfaceAdjustable( surfaceAdjust.push_back(), tweak.viewVector );
					}
				}
			}

			if ( tweak.mode == MSurfaceTweakComponent::NORMAL )
			{
				normalAdjust.setParameters( tweak.displacementAlongNormal, 0.0 );
			}
			else if ( tweak.mode == MSurfaceTweakComponent::SURFACE )
			{
				surfaceAdjust.tweak( tweak.displacementAlongSurface );
			}
		}
	}


	if ( bRestoreSelection )
	{
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			vertices[vertexI]->vertexMarkRestore();
		}
		for (int edgeI = 0; edgeI < edges.size(); edgeI++)
		{
			edges[edgeI]->edgeMarkRestore();
		}
		for (int faceI = 0; faceI < faces.size(); faceI++)
		{
			faces[faceI]->faceMarkRestore();
		}
	}

	finalise();
}





void MMesh::surfaceTweakAdjustMarkedVertices(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector)
{
	adjusts.setViewVector( viewVector );
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isVertexMarked() )
		{
			vertices[vertexI]->vertexTweakAlongSurfaceAdjustable( adjusts.push_back(), viewVector );
		}
	}
}

void MMesh::surfaceTweakAdjustMarkedEdges(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector)
{
	adjusts.setViewVector( viewVector );
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isEdgeMarked() )
		{
			vertices[vertexI]->edgeTweakAlongSurfaceAdjustable( adjusts.push_back(), viewVector );
		}
	}
}

void MMesh::surfaceTweakAdjustMarkedFaces(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector)
{
	adjusts.setViewVector( viewVector );
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isFaceMarked() )
		{
			vertices[vertexI]->faceTweakAlongSurfaceAdjustable( adjusts.push_back(), viewVector );
		}
	}
}

void MMesh::surfaceTweakNormalAdjustMarkedVertices(MVertexAdjustList &adjusts)
{
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isVertexMarked() )
		{
			vertices[vertexI]->vertexTweakAlongNormalAdjustable( adjusts.push_back() );
		}
	}
}

void MMesh::surfaceTweakNormalAdjustMarkedEdges(MVertexAdjustList &adjusts)
{
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isEdgeMarked() )
		{
			vertices[vertexI]->edgeTweakAlongNormalAdjustable( adjusts.push_back() );
		}
	}
}

void MMesh::surfaceTweakNormalAdjustMarkedFaces(MVertexAdjustList &adjusts)
{
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( vertices[vertexI]->isFaceMarked() )
		{
			vertices[vertexI]->faceTweakAlongNormalAdjustable( adjusts.push_back() );
		}
	}
}





void MMesh::repositionVertices(const MVertexRepositionList &targets)
{
	assertFinalised();

	for (int targetI = 0; targetI < targets.size(); targetI++)
	{
		const MVertexReposition &target = targets[targetI];

		MVertex *vertex = pickVertex( target.pick );
		if ( vertex != NULL )
		{
			vertex->setPosition( target.position );
		}
	}

	finalise();
}

void MMesh::setVertexPosition(MVertex *v, const Point3 &position)
{
	assertFinalised();

	v->setPosition( position );

	finalise();
}




void MMesh::getBrushedVertices(const Point3 &brushPosition, const Brush &brush, Array< std::pair<MVertex*, double> > &vertexWeightPairs, Array<MVertex*> &vertexCheckList)
{
	const BBox3 &bbox = getBoundingBox();

	vertexCheckList.clear();


	// If the brush intersects the mesh bounding box
	if ( brush.intersectsBBox( brushPosition, bbox ) )
	{
		// Use the bounding box tree to build a list of vertices that may intersect the brush
		const MBBTree *tree = getBBTree();
		MBBTree::TraversalState state;
		const int *start, *end;
		while ( tree->brush( brush, brushPosition, state, start, end ) )
		{
			for (const int *indexPtr = start; indexPtr < end; indexPtr++)
			{
				MFace *face = faces[*indexPtr];
				if ( !face->isDestroyed() )
				{
					for (int vertexI = 0; vertexI < face->getSize(); vertexI++)
					{
						MVertex *v = face->getVertex( vertexI );
						if ( !v->isSecondaryMarked() )
						{
							vertexCheckList.push_back( v );
							v->secondaryMark();
						}
					}
				}
			}
		}


		// Generate the vertex-weight-pair list
		for (int vertexI = 0; vertexI < vertexCheckList.size(); vertexI++)
		{
			MVertex *v = vertexCheckList[vertexI];
			double weight = brush.computePointWeight( brushPosition, v->getPosition() );
			if ( weight > 0.0 )
			{
				vertexWeightPairs.push_back( std::pair<MVertex*, double>( v, weight ) );
			}
		}


		// Secondary unmark all vertices
		for (int vertexI = 0; vertexI < vertexCheckList.size(); vertexI++)
		{
			vertexCheckList[vertexI]->secondaryUnmark();
		}

		vertexCheckList.clear();
	}
}



void MMesh::proportionalAdjusterInit(const Point3 &brushPosition, const Brush &brush, MProportionalAdjuster *adjuster)
{
	adjuster->vertexWeightPairs.clear();
	getBrushedVertices( brushPosition, brush, adjuster->vertexWeightPairs, adjuster->vertexCheckList );
}


void MMesh::proportionalMove(const Point3 &brushPosition, const Brush &brush, const Vector3 &translation, MProportionalAdjuster *adjuster)
{
	MProportionalAdjuster _localAdjuster;
	if ( adjuster == NULL )
	{
		adjuster = &_localAdjuster;
	}

	proportionalAdjusterInit( brushPosition, brush, adjuster );
	adjuster->move( translation );
	finalise();
}

void MMesh::proportionalMoveNormal(const Point3 &brushPosition, const Brush &brush, double distance, MProportionalAdjuster *adjuster)
{
	MProportionalAdjuster _localAdjuster;
	if ( adjuster == NULL )
	{
		adjuster = &_localAdjuster;
	}

	proportionalAdjusterInit( brushPosition, brush, adjuster );
	adjuster->moveNormal( distance );
	finalise();
}

void MMesh::proportionalScale(const Point3 &brushPosition, const Brush &brush, double scaleFactor, MProportionalAdjuster *adjuster)
{
	MProportionalAdjuster _localAdjuster;
	if ( adjuster == NULL )
	{
		adjuster = &_localAdjuster;
	}

	proportionalAdjusterInit( brushPosition, brush, adjuster );
	adjuster->scale( brushPosition, scaleFactor );
	finalise();
}

void MMesh::proportionalRotate(const Point3 &brushPosition, const Brush &brush, const Vector3 &rotationAxis, double rotation, MProportionalAdjuster *adjuster)
{
	MProportionalAdjuster _localAdjuster;
	if ( adjuster == NULL )
	{
		adjuster = &_localAdjuster;
	}

	proportionalAdjusterInit( brushPosition, brush, adjuster );
	adjuster->rotate( brushPosition, rotationAxis, rotation );
	finalise();
}

void MMesh::proportionalRelax(const Point3 &brushPosition, const Brush &brush, double relaxAmount, MProportionalAdjuster *adjuster)
{
	MProportionalAdjuster _localAdjuster;
	if ( adjuster == NULL )
	{
		adjuster = &_localAdjuster;
	}

	proportionalAdjusterInit( brushPosition, brush, adjuster );
	adjuster->relax( relaxAmount );
	finalise();
}






void MMesh::flattenVertices(const Vector3 &axis, bool perRegion)
{
	Array<MVertexList> regions;
	discoverVertexTransformRegions( perRegion, regions );

	for (int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
	{
		MVertexList &region = regions[regionIndex];
		Point3 centre;

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			centre.cumulativeAdd( region[vertexIndex]->getPosition() );
		}
		centre.scale( 1.0 / (double)region.size() );

		Plane plane( axis, centre );

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( region[vertexIndex]->getPosition() );
			region[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}

void MMesh::flattenVerticesNormal(bool perRegion)
{
	Array<MVertexList> regions;
	discoverVertexTransformRegions( perRegion, regions );

	for (int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
	{
		MVertexList &region = regions[regionIndex];
		Point3 centre;
		Vector3 normal;

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			centre.cumulativeAdd( region[vertexIndex]->getPosition() );
			normal += region[vertexIndex]->computeAverageNormal();
		}
		centre.scale( 1.0 / (double)region.size() );
		normal.normalise();

		Plane plane( normal, centre );

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( region[vertexIndex]->getPosition() );
			region[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}

void MMesh::flattenEdges(const Vector3 &axis, bool perRegion)
{
	Array<MVertexList> regions;
	discoverEdgeTransformRegions( perRegion, regions );

	for (int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
	{
		MVertexList &region = regions[regionIndex];
		Point3 centre;

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			centre.cumulativeAdd( region[vertexIndex]->getPosition() );
		}
		centre.scale( 1.0 / (double)region.size() );

		Plane plane( axis, centre );

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( region[vertexIndex]->getPosition() );
			region[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}

void MMesh::flattenEdgesNormal(bool perRegion)
{
	Array<MVertexList> regionVertices;
	Array<MEdgeList> regionEdges;
	discoverEdgeTransformRegions( perRegion, regionVertices, regionEdges );

	for (int regionIndex = 0; regionIndex < regionVertices.size(); regionIndex++)
	{
		MVertexList &v = regionVertices[regionIndex];
		MEdgeList &e = regionEdges[regionIndex];
		Point3 centre;
		Vector3 normal;

		for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
		{
			centre.cumulativeAdd( v[vertexIndex]->getPosition() );
		}
		centre.scale( 1.0 / (double)v.size() );

		for (int edgeIndex = 0; edgeIndex < e.size(); edgeIndex++)
		{
			normal += e[edgeIndex]->getNormal();
		}
		normal.normalise();

		Plane plane( normal, centre );

		for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( v[vertexIndex]->getPosition() );
			v[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}

void MMesh::flattenEdgeRuns()
{
	Array<MEdgeRun> edgeRuns;

	discoverMarkedEdgeRuns( edgeRuns );

	for (int runIndex = 0; runIndex < edgeRuns.size(); runIndex++)
	{
		MEdgeRun &run = edgeRuns[runIndex];

		MVertexList v;
		run.extractVertices( v );

		bool loopFlag = v.front() == v.back();

		Plane runPlane;
		if ( loopFlag )
		{
			v.pop_back();
		}

		if ( v.size() >= 3 )
		{
			// Compute the weighted average normal of the 'face' formed by the edge run
			const Point3 &a = v[0]->getPosition();
			Vector3 edge0 = v[1]->getPosition() - a;
			Vector3 normal;
			for (int k = 2; k < v.size(); k++)
			{
				Vector3 edge1 = v[k]->getPosition() - a;
				normal += edge0.cross( edge1 );
				edge0 = edge1;
			}

			if ( normal.sqrLength() > EPSILON )
			{
				normal.normalise();

				if ( run.isClosed() )
				{
					// Closed loop, intersects no other runs

					// Plane of best fit

					double d = 0.0;
					for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
					{
						d += v[vertexIndex]->getPosition().dot( normal );
					}
					d *= 1.0 / (double)v.size();

					runPlane = Plane( normal, d );
				}
				else
				{
					if ( loopFlag )
					{
						// Loop, first vertex must not be moved

						runPlane = Plane( normal, v.front()->getPosition() );
					}
					else
					{
						// Open run, first and last vertices must not be moved
						Vector3 fixedEdge = v.back()->getPosition() - v.front()->getPosition();

						if ( fixedEdge.sqrLength() <= EPSILON )
						{
							runPlane = Plane( normal, v.front()->getPosition() );
						}
						else
						{
							Vector3 biNormal = normal.cross( fixedEdge );
							normal = fixedEdge.cross( biNormal );

							if ( normal.sqrLength() <= EPSILON )
							{
								break;
							}

							normal.normalise();

							runPlane = Plane( normal, v.front()->getPosition() );
						}
					}
				}

				// Project the vertices onto the plane
				for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
				{
					Point3 p = runPlane.projectPoint( v[vertexIndex]->getPosition() );
					v[vertexIndex]->setPosition( p );
				}
			}
		}
	}

	finalise();
}

void MMesh::straightenEdgeRuns()
{
	Array<MEdgeRun> edgeRuns;

	discoverMarkedEdgeRuns( edgeRuns );

	for (int runIndex = 0; runIndex < edgeRuns.size(); runIndex++)
	{
		MEdgeRun &run = edgeRuns[runIndex];

		MVertexList v;
		run.extractVertices( v );

		bool loopFlag = v.front() == v.back();

		if ( loopFlag )
		{
			v.pop_back();
		}

		Point3 runLineA, runLineB;

		if ( v.size() >= 3 )
		{
			if ( run.isClosed() )
			{
				// Find the two most distance vertices in the run, and flatten onto the line linking them
				MVertex *bestA = NULL, *bestB = NULL;
				double bestSqrDist = 0.0;

				for (int aIndex = 0; aIndex < v.size(); aIndex++)
				{
					MVertex *a = v[aIndex];
					for (int bIndex = aIndex + 1; bIndex < v.size(); bIndex++)
					{
						MVertex *b = v[bIndex];
						double sqrDist = a->sqrDistanceTo( b );
						if ( sqrDist > bestSqrDist )
						{
							bestA = a;
							bestB = b;
							bestSqrDist = sqrDist;
						}
					}
				}

				runLineA = bestA->getPosition();
				runLineB = bestB->getPosition();
			}
			else
			{
				if ( loopFlag )
				{
					// The the vertex most distant from the first vertex, and flatten onto the line linking them
					MVertex *bestA = NULL;
					double bestSqrDist = 0.0;
					for (int aIndex = 1; aIndex < v.size(); aIndex++)
					{
						MVertex *a = v[aIndex];
						double sqrDist = v.front()->sqrDistanceTo( a );
						if ( sqrDist > bestSqrDist )
						{
							bestA = a;
							bestSqrDist = sqrDist;
						}
					}

					runLineA = v.front()->getPosition();
					runLineB = bestA->getPosition();
				}
				else
				{
					runLineA = v.front()->getPosition();
					runLineB = v.back()->getPosition();
				}
			}


			BBox3 runBox = v.computeBBox();
			double runSize = runBox.getMajorSize();

			if ( runLineA.sqrDistanceTo( runLineB )  >=  ( runSize * runSize * SQR_EPSILON ) )
			{
				Line3 runLine( runLineA, runLineB );
				// Project the vertices onto the line
				for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
				{
					Point3 p = runLine.closestPointTo( v[vertexIndex]->getPosition() );
					v[vertexIndex]->setPosition( p );
				}
			}
		}
	}

	finalise();
}


void MMesh::flattenFaces(const Vector3 &axis, bool perRegion)
{
	Array<MVertexList> regions;
	discoverFaceTransformRegions( perRegion, regions );

	for (int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
	{
		MVertexList &region = regions[regionIndex];
		Point3 centre;

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			centre.cumulativeAdd( region[vertexIndex]->getPosition() );
		}
		centre.scale( 1.0 / (double)region.size() );

		Plane plane( axis, centre );

		for (int vertexIndex = 0; vertexIndex < region.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( region[vertexIndex]->getPosition() );
			region[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}

void MMesh::flattenFacesNormal(bool perRegion)
{
	Array<MVertexList> regionVertices;
	Array<MFaceList> regionFaces;
	discoverFaceTransformRegions( perRegion, regionVertices, regionFaces );

	for (int regionIndex = 0; regionIndex < regionVertices.size(); regionIndex++)
	{
		MVertexList &v = regionVertices[regionIndex];
		MFaceList &f = regionFaces[regionIndex];
		Point3 centre;
		Vector3 normal;

		for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
		{
			centre.cumulativeAdd( v[vertexIndex]->getPosition() );
		}
		centre.scale( 1.0 / (double)v.size() );

		for (int faceIndex = 0; faceIndex < f.size(); faceIndex++)
		{
			normal += f[faceIndex]->computeTwiceAreaNormalProduct();
		}
		normal.normalise();

		Plane plane( normal, centre );

		for (int vertexIndex = 0; vertexIndex < v.size(); vertexIndex++)
		{
			Point3 p = plane.projectPoint( v[vertexIndex]->getPosition() );
			v[vertexIndex]->setPosition( p );
		}
	}

	finalise();
}




void MMesh::moveVerticesNormal(MVertexAdjustList &adjusts)
{
	adjusts.clear();

	Array<Vector3> normals;
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];

		if ( v->isVertexMarked() )
		{
			//get a list of the normals of the faces that are incident to @v
			normals.resize( v->getNumFaces() );
			for (int faceI = 0; faceI < v->getNumFaces(); faceI++)
			{
				normals[faceI] = v->getFace( faceI )->getPlane().n;
			}

			//compute the extrusion vector
			Vector3 extrusionVector = MMesh::computeExpansionVector( normals );

			adjusts.push_back( MVertexAdjust( v, v->getPosition(), extrusionVector ) );
		}
	}
}

void MMesh::moveEdgesNormal(MVertexAdjustList &adjusts)
{
	adjusts.clear();

	Array<Vector3> normals;
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];

		if ( v->isEdgeMarked() )
		{
			//get a list of the normals of the faces that are incident to @v and incident to marked edges
			normals.clear();
			for (int faceI = 0; faceI < v->getNumFaces(); faceI++)
			{
				const MFace *f = v->getFace( faceI );
				if ( f->isEdgeMarked() )
				{
					normals.push_back( f->getPlane().n );
				}
			}

			//compute the extrusion vector
			Vector3 extrusionVector = MMesh::computeExpansionVector( normals );

			adjusts.push_back( MVertexAdjust( v, v->getPosition(), extrusionVector, Vector3() ) );
		}
	}
}

void MMesh::moveFacesNormal(MVertexAdjustList &adjusts)
{
	adjusts.clear();

	Array<Vector3> normals;
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];

		if ( v->isFaceMarked() )
		{
			//get a list of the normals of the marked faces that are incident to @v
			normals.clear();
			for (int faceI = 0; faceI < v->getNumFaces(); faceI++)
			{
				const MFace *f = v->getFace( faceI );
				if ( f->isFaceMarked() )
				{
					normals.push_back( f->getPlane().n );
				}
			}

			//compute the extrusion vector
			Vector3 extrusionVector = MMesh::computeExpansionVector( normals );

			adjusts.push_back( MVertexAdjust( v, v->getPosition(), extrusionVector, Vector3() ) );
		}
	}
}




void MMesh::discoverVertexTransformRegions(bool perRegion, Array<MVertexList> &regions)
{
	if ( perRegion )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isVertexMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				MVertexList &region = regions.push_back();

				MVertex *v = vertices[i];
				v->discoverVertexMarkedRegion( region );
			}
		}


		for (int i = 0; i < regions.size(); i++)
		{
			for (int j = 0; j < regions[i].size(); j++)
			{
				regions[i][j]->secondaryUnmark();
			}
		}
	}
	else
	{
		MVertexList &region = regions.push_back();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isVertexMarked() )
			{
				region.push_back( vertices[i] );
			}
		}
	}
}

void MMesh::discoverEdgeTransformRegions(bool perRegion, Array<MVertexList> &regionVertices)
{
	if ( perRegion )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				regionVertices.push_back();

				MVertex *v = vertices[i];
				v->discoverEdgeMarkedRegionVertices( regionVertices.back() );
			}
		}


		for (int i = 0; i < regionVertices.size(); i++)
		{
			for (int j = 0; j < regionVertices[i].size(); j++)
			{
				regionVertices[i][j]->secondaryUnmark();
			}
		}
	}
	else
	{
		MVertexList &v = regionVertices.push_back();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked() )
			{
				v.push_back( vertices[i] );
			}
		}
	}
}

void MMesh::discoverEdgeTransformRegions(bool perRegion, Array<MVertexList> &regionVertices, Array<MEdgeList> &regionEdges)
{
	if ( perRegion )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				regionVertices.push_back();
				regionEdges.push_back();

				MVertex *v = vertices[i];
				v->discoverEdgeMarkedRegionVertices( regionVertices.back(), regionEdges.back() );
			}
		}


		for (int i = 0; i < regionVertices.size(); i++)
		{
			for (int j = 0; j < regionVertices[i].size(); j++)
			{
				regionVertices[i][j]->secondaryUnmark();
			}
		}

		for (int i = 0; i < regionEdges.size(); i++)
		{
			for (int j = 0; j < regionEdges[i].size(); j++)
			{
				regionEdges[i][j]->secondaryUnmark();
			}
		}
	}
	else
	{
		MVertexList &v = regionVertices.push_back();
		MEdgeList &e = regionEdges.push_back();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isEdgeMarked() )
			{
				v.push_back( vertices[i] );
			}
		}

		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isEdgeMarked() )
			{
				e.push_back( edges[i] );
			}
		}
	}
}

void MMesh::discoverFaceTransformRegions(bool perRegion, Array<MVertexList> &regionVertices)
{
	if ( perRegion )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				regionVertices.push_back();

				MVertex *v = vertices[i];
				v->discoverFaceMarkedRegionVertices( regionVertices.back() );
			}
		}


		for (int i = 0; i < regionVertices.size(); i++)
		{
			for (int j = 0; j < regionVertices[i].size(); j++)
			{
				regionVertices[i][j]->secondaryUnmark();
			}
		}

		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->secondaryUnmark();
		}
	}
	else
	{
		MVertexList &v = regionVertices.push_back();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked() )
			{
				v.push_back( vertices[i] );
			}
		}
	}
}

void MMesh::discoverFaceTransformRegions(bool perRegion, Array<MVertexList> &regionVertices, Array<MFaceList> &regionFaces)
{
	if ( perRegion )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked()  &&  !vertices[i]->isDestroyed()  &&  !vertices[i]->isSecondaryMarked() )
			{
				regionVertices.push_back();
				regionFaces.push_back();

				MVertex *v = vertices[i];
				v->discoverFaceMarkedRegionVertices( regionVertices.back(), regionFaces.back() );
			}
		}


		for (int i = 0; i < regionVertices.size(); i++)
		{
			for (int j = 0; j < regionVertices[i].size(); j++)
			{
				regionVertices[i][j]->secondaryUnmark();
			}
		}

		for (int i = 0; i < regionFaces.size(); i++)
		{
			for (int j = 0; j < regionFaces[i].size(); j++)
			{
				regionFaces[i][j]->secondaryUnmark();
			}
		}
	}
	else
	{
		MVertexList &v = regionVertices.push_back();
		MFaceList &f = regionFaces.push_back();

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i]->isFaceMarked() )
			{
				v.push_back( vertices[i] );
			}
		}

		for (int i = 0; i < faces.size(); i++)
		{
			if ( faces[i]->isFaceMarked() )
			{
				f.push_back( faces[i] );
			}
		}
	}
}






//************************** SHARPNESS OPERATIONS *************************
//*************************************************************************

void MMesh::setMarkedEdgeSharpness(double sharpness, MSharpenFunction func)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			edges[i]->setSharpness( sharpness, func );
		}
	}

	finalise();
}

void MMesh::setMarkedEdgeNormalSharpness(bool sharpness)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			edges[i]->setNormalSharpness( sharpness );
		}
	}

	finalise();
}

void MMesh::saveMarkedEdgeSharpness(Array<float> &sharpnessBackup)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			sharpnessBackup.push_back( edges[i]->getSharpness() );
		}
	}

	//backup: no need to finalise
}

void MMesh::restoreMarkedEdgeSharpness(Array<float> &sharpnessBackup)
{
	assertFinalised();

	int j = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			edges[i]->setSharpness( sharpnessBackup[j] );

			j++;
		}
	}

	finalise();
}



void MMesh::setEdgesByVertexAsNormalSharp(MVertexList &verts)
{
	int i = verts.isClosed()  ?  verts.size() - 1  :  0;
	int start = verts.isClosed()  ?  0  :  1;

	for (int j = start; j < verts.size(); j++)
	{
		MVertex *vertexA = verts[i];
		MVertex *vertexB = verts[j];
		MEdge *edge = vertexA->findEdgeTo( vertexB, MFINDPOLICY_RETURNNULL );

		if ( edge != NULL )
		{
			edge->setNormalSharp();
		}

		i = j;
	}
}




void MMesh::setMarkedVertexSharpness(double sharpness, MSharpenFunction func)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			vertices[i]->setSharpness( sharpness, func );
		}
	}

	finalise();
}

void MMesh::setMarkedVertexNormalSharpness(bool sharpness)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			vertices[i]->setNormalSharpness( sharpness );
		}
	}

	finalise();
}

void MMesh::saveMarkedVertexSharpness(Array<float> &sharpnessBackup)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			sharpnessBackup.push_back( vertices[i]->getSharpness() );
		}
	}

	//backup: no need to finalise
}

void MMesh::restoreMarkedVertexSharpness(Array<float> &sharpnessBackup)
{
	assertFinalised();

	int j = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			vertices[i]->setSharpness( sharpnessBackup[j] );

			j++;
		}
	}

	finalise();
}


//************************** MATERIAL OPERATIONS **************************
//*************************************************************************

void MMesh::refMaterial(int materialID)
{
	if ( materialID != -1 )
	{
		materialRefTable.ref( materialID );
	}
}

void MMesh::unrefMaterial(int materialID)
{
	if ( materialID != -1 )
	{
		materialRefTable.unref( materialID );
	}
}




// ************ BY MARKED REGION ************

void MMesh::setMaterial(int mat)
{
	assertFinalised();

	bool markedFacesOnly = getNumMarkedFaces() > 0;

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  ||  !markedFacesOnly )
		{
			faces[i]->setMaterial( mat );
		}
	}
}

void MMesh::transformMaterial(const Matrix4 &xform, bool fixMarkedVertices)
{
	assertFinalised();

	bool markedFacesOnly = getNumMarkedFaces() > 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( ( vertices[i]->isFaceMarkInternal()  ||  !markedFacesOnly )   &&   ( !fixMarkedVertices  ||  !vertices[i]->isVertexMarked() ) )
		{
			vertices[i]->transformMaterial( xform );
		}
	}

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  ||  !markedFacesOnly )
		{
			faces[i]->transformMaterial( xform, fixMarkedVertices );
		}
	}
}

void MMesh::transformMaterialSave(bool fixMarkedVertices, MMaterialTransformBackup &b)
{
	assertFinalised();

	b.clear();

	bool markedFacesOnly = getNumMarkedFaces() > 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( ( vertices[i]->isFaceMarkInternal()  ||  !markedFacesOnly )   &&   ( !fixMarkedVertices  ||  !vertices[i]->isVertexMarked() ) )
		{
			vertices[i]->transformMaterialSave();
		}
	}

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  ||  !markedFacesOnly )
		{
			faces[i]->transformMaterialSave( fixMarkedVertices, b );
		}
	}
}

void MMesh::transformMaterialRestore(bool fixMarkedVertices, const MMaterialTransformBackup &b)
{
	assertFinalised();

	bool markedFacesOnly = getNumMarkedFaces() > 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( ( vertices[i]->isFaceMarkInternal()  ||  !markedFacesOnly )   &&   ( !fixMarkedVertices  ||  !vertices[i]->isVertexMarked() ) )
		{
			vertices[i]->transformMaterialRestore();
		}
	}

	int position = 0;
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked()  ||  !markedFacesOnly )
		{
			faces[i]->transformMaterialRestore( fixMarkedVertices, b, position );
		}
	}
}


void MMesh::normaliseMaterialCoords(bool perGroup)
{
	if ( getNumMarkedFaces() > 0 )
	{
		if ( perGroup )
		{
			Array<MFaceList> markedFaceRegions;

			discoverMarkedFaceRegions( markedFaceRegions );

			for (int i = 0; i < markedFaceRegions.size(); i++)
			{
				normaliseFaceGroupMaterialCoords( markedFaceRegions[i] );
			}
		}
		else
		{
			MFaceList markedFaces;

			discoverMarkedFaces( markedFaces );

			normaliseFaceGroupMaterialCoords( markedFaces );
		}
	}
	else
	{
		normaliseFaceGroupMaterialCoords( faces );
	}
}


void MMesh::normaliseFaceGroupMaterialCoords(MFaceList &faceList)
{
	if ( faceList.size() > 0 )
	{
		BBox2 box = faceList[0]->getMaterialBBox();

		for (int i = 1; i < faceList.size(); i++)
		{
			faceList[i]->addToMaterialBBox( box );
		}

		Vector2 xlat = -box.getLower().toVector2();
		Vector2 scale = Vector2( 1.0 / box.getWidth(), 1.0 / box.getHeight() );
		Matrix4 normalisationMatrix = Matrix4::translate( xlat ) * Matrix4::scale( scale );

		transformMaterial( normalisationMatrix, false );
	}
}





//********** UV MAPPING ***************

void MMesh::uvMap(UVMapper *mapper)
{
	assertFinalised();

	uvmapInitialise();

	Array<Point3> facePoints;
	Array<Vector3f> faceNormals;
	Array<Point2f> texCoords;

	bool hasMarkedFaces = getNumMarkedFaces()  >  0;

	for (int i = 0; i < faces.size(); i++)
	{
		MFace *f = faces[i];

		if ( f->isFaceMarked()  ||  !hasMarkedFaces )
		{
			facePoints.clear();
			faceNormals.clear();
			texCoords.clear();
			int material;

			f->getFacePoints( facePoints );
			f->getUVMapNormals( faceNormals );
			const Plane &plane = f->getPlane();

			mapper->uvMap( facePoints, faceNormals, plane, texCoords, material );

			if ( material == -1 )
			{
				f->setMaterialCoords( texCoords );
			}
			else
			{
				f->setMaterialCoords( texCoords, material );
			}
		}
	}

	uvmapShutdown();
}



void MMesh::uvmapMarkedEdges(bool syncAtMarkedVertices, bool invert, const Point2f &uvStart, const Point2f &uvEnd)
{
	assertFinalised();

	//get the edge runs
	Array<MEdgeRun> edgeRuns;
	discoverMarkedEdgeRuns( edgeRuns );
	Array<double> unitLengthArray;

	//for each edge run
	for (int i = 0; i < edgeRuns.size(); i++)
	{
		unitLengthArray.clear();
		MEdgeRun &run = edgeRuns[i];

		//if sync at vertices is required, and the run is closed
		if ( syncAtMarkedVertices  &&  run.isClosed() )
		{
			//iterate through the vertices in the run
			MEdgeRun::VertexIterator iter = run.verticesBegin();

			for (int i = 0; i < run.size(); i++)
			{
				//find a marked vertex
				MVertex *v = iter.vertex();
				++iter;

				if ( v->isVertexMarked() )
				{
					//rotate the edge run forward such that v is now at the front
					run.rotateForward( i );
					//break out
					break;
				}
			}
		}

		//compute the length Array
		run.generateUnitCumulativeLengthArray( unitLengthArray );


		//need to iterate through the vertices in the run
		MEdgeRun::VertexIterator iter = run.verticesBegin();

		Point2f uva = invert ? uvEnd : uvStart;
		Point2f uvb;
		for (int i = 0; i < run.size(); i++)
		{
			MEdge *edge = run[i];

			//compute the uv coord
			double fraction = unitLengthArray[ i + 1 ];
			if ( invert )
			{
				fraction = 1.0 - fraction;
			}
			uvb = lerp( uvStart, uvEnd, fraction );


			//uv map the edge
			if ( iter.vertex() == edge->getVertexA() )
			{
				//edge is in same direction as edge run
				edge->uvmapEdge( uva, uvb );
			}
			else
			{
				//edge is in opposite direction to edge run; flip uv's
				edge->uvmapEdge( uvb, uva );
			}


			uva = uvb;
			iter++;
		}
	}
}



void MMesh::uvmapInitialise()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->uvmapInitialise();
		}
	}
}

void MMesh::uvmapShutdown()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isFaceMarked() )
		{
			vertices[i]->uvmapShutdown();
		}
	}
}







//************************** MISC OPERATIONS **************************
//*********************************************************************

void MMesh::vertexListToBlendingPolyline(MVertexList &v, BlendingPolyline &poly)
{
	poly.clear();

	for (int i = 0; i < v.size(); i++)
	{
		poly.push_back( v[i]->getPosition() );
	}

	if ( v.isClosed() )
	{
		poly.close();
	}
	else
	{
		poly.open();
	}
}







void MMesh::markVertices_save()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->vertexMarkSave();
	}
}

void MMesh::markVertices_restore()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->vertexMarkRestore();
	}
}

void MMesh::markEdges_save()
{
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->edgeMarkSave();
	}
}

void MMesh::markEdges_restore()
{
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->edgeMarkRestore();
	}
}

void MMesh::markFaces_save()
{
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->faceMarkSave();
	}
}

void MMesh::markFaces_restore()
{
	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->faceMarkRestore();
	}
}



void MMesh::markVertices_all(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, true );
	}
}

void MMesh::markEdges_all(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, true );
	}
}

void MMesh::markFaces_all(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->setFaceMark( pred, true );
	}
}



void MMesh::markVertices_edgeMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isEdgeMarked() );
	}
}

void MMesh::markVertices_faceMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isFaceMarked() );
	}
}

void MMesh::markVertices_faceMarkBoundary(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isOnFaceMarkBoundary() );
	}
}

void MMesh::markVertices_faceMarkInternal(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isFaceMarkInternal() );
	}
}

void MMesh::markVertices_boundary(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isOnBoundary() );
	}
}

void MMesh::markVertices_internal(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, !vertices[i]->isOnBoundary() );
	}
}


void MMesh::markEdges_vertexMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isVertexMarked() );
	}
}

void MMesh::markEdges_whollyVertexMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isWhollyVertexMarked() );
	}
}

void MMesh::markEdges_faceMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isFaceMarked() );
	}
}

void MMesh::markEdges_faceMarkBoundary(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isFaceMarkBoundary() );
	}
}

void MMesh::markEdges_faceMarkInternal(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isFaceMarkInternal() );
	}
}

void MMesh::markEdges_boundary(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, edges[i]->isBoundary() );
	}
}

void MMesh::markEdges_internal(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->setEdgeMark( pred, !edges[i]->isBoundary() );
	}
}

void MMesh::markFaces_vertexMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->setFaceMark( pred, faces[i]->isVertexMarked() );
	}
}

void MMesh::markFaces_whollyVertexMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->setFaceMark( pred, faces[i]->isWhollyVertexMarked() );
	}
}

void MMesh::markFaces_edgeMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->setFaceMark( pred, faces[i]->isEdgeMarked() );
	}
}

void MMesh::markFaces_whollyEdgeMarked(MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->setFaceMark( pred, faces[i]->isWhollyEdgeMarked() );
	}
}




void MMesh::markVertices_region(const Region3d *region, MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isInside( region ) );
	}
}

void MMesh::markVertices_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred)
{
	assertFinalised();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i]->setVertexMark( pred, vertices[i]->isInside( region, faceCull ) );
	}
}

void MMesh::markEdges_region(const Region3d *region, MarkPredicate pred, bool containsAllOf)
{
	assertFinalised();

	if ( containsAllOf )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->setEdgeMark( pred, edges[i]->isWhollyInside( region ) );
		}
	}
	else
	{
		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->setEdgeMark( pred, edges[i]->isPartiallyInside( region ) );
		}
	}
}

void MMesh::markEdges_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf)
{
	assertFinalised();

	if ( containsAllOf )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->setEdgeMark( pred, edges[i]->isWhollyInside( region, faceCull ) );
		}
	}
	else
	{
		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->setEdgeMark( pred, edges[i]->isPartiallyInside( region, faceCull ) );
		}
	}
}

void MMesh::markFaces_region(const Region3d *region, MarkPredicate pred, bool containsAllOf)
{
	assertFinalised();

	if ( containsAllOf )
	{
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->setFaceMark( pred, faces[i]->isWhollyInside( region ) );
		}
	}
	else
	{
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->setFaceMark( pred, faces[i]->isPartiallyInside( region ) );
		}
	}
}

void MMesh::markFaces_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf)
{
	assertFinalised();

	if ( containsAllOf )
	{
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->setFaceMark( pred, faces[i]->isWhollyInside( region, faceCull ) );
		}
	}
	else
	{
		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->setFaceMark( pred, faces[i]->isPartiallyInside( region, faceCull ) );
		}
	}
}


void MMesh::markVertices_pick(const MPickList &pick)
{
	assertFinalised();

	for (int i = 0; i < pick.size(); i++)
	{
		MVertex *v = pickVertex( pick[i] );

		if ( v != NULL )
		{
			v->vertexMarkInvert();
		}
	}
}

void MMesh::markEdges_pick(const MPickList &pick)
{
	assertFinalised();

	Point3 closestPoint;

	for (int i = 0; i < pick.size(); i++)
	{
		MEdge *e = pickEdge( pick[i], closestPoint );

		if ( e != NULL )
		{
			e->edgeMarkInvert();
		}
	}
}

void MMesh::markEdgeRings_pick(const MPickList &pick)
{
	assertFinalised();

	Point3 closestPoint;

	MEdgeList ring;

	for (int i = 0; i < pick.size(); i++)
	{
		ring.clear();
		MEdge *seed = pickEdgeRing( ring, pick[i], closestPoint );

		if ( seed != NULL )
		{
			bool mark = !seed->isEdgeMarked();

			for (int edgeI = 0; edgeI < ring.size(); edgeI++)
			{
				ring[edgeI]->setEdgeMark( mark );
			}
		}
	}
}

void MMesh::markEdgeLoops_pick(const MPickList &pick)
{
	assertFinalised();

	Point3 closestPoint;

	MEdgeList loop;

	for (int i = 0; i < pick.size(); i++)
	{
		loop.clear();
		MEdge *seed = pickEdgeLoop( loop, pick[i], closestPoint );

		if ( seed != NULL )
		{
			bool mark = !seed->isEdgeMarked();

			for (int edgeI = 0; edgeI < loop.size(); edgeI++)
			{
				loop[edgeI]->setEdgeMark( mark );
			}
		}
	}
}

void MMesh::markEdgeBoundaries_pick(const MPickList &pick)
{
	assertFinalised();

	Point3 closestPoint;

	MEdgeList boundary;

	for (int i = 0; i < pick.size(); i++)
	{
		MEdge *seed = pickEdgeBoundary( boundary, pick[i], closestPoint );

		if ( seed != NULL )
		{
			bool mark = !seed->isEdgeMarked();

			for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
			{
				boundary[edgeI]->setEdgeMark( mark );
			}
		}
	}
}

void MMesh::markEdgeLoopPath_pick(const MPickList &pickList)
{
	assertFinalised();

	if ( pickList.size() >= 2 )
	{
		Point3 closestPoint;
		MVertex *prevVertex = pickVertex( pickList.front() );

		for (int pickI = 1; pickI < pickList.size(); pickI++)
		{
			MVertex *curVertex = pickVertex( pickList[pickI] );

			if ( curVertex != NULL  &&  prevVertex != NULL )
			{
				MEdgeRun path;
				discoverEdgeLoopPath( prevVertex, curVertex, path );
				for (int pathI = 0; pathI < path.size(); pathI++)
				{
					path[pathI]->edgeMark();
				}
			}

			prevVertex = curVertex;
		}
	}
}

void MMesh::markEdgeRingPath_pick(const MPickList &pickList)
{
	assertFinalised();

	if ( pickList.size() >= 2 )
	{
		Point3 closestPoint;
		MEdge *prevEdge = pickEdge( pickList.front(), closestPoint );

		for (int pickI = 1; pickI < pickList.size(); pickI++)
		{
			MEdge *curEdge = pickEdge( pickList[pickI], closestPoint );

			if ( curEdge != NULL  &&  prevEdge != NULL )
			{
				MEdgeList path;
				discoverEdgeRingPath( prevEdge, curEdge, path );
				for (int pathI = 0; pathI < path.size(); pathI++)
				{
					path[pathI]->edgeMark();
				}
			}

			prevEdge = curEdge;
		}
	}
}

void MMesh::markFaces_pick(const MPickList &pick)
{
	assertFinalised();

	for (int i = 0; i < pick.size(); i++)
	{
		MFace *f = pickFace( pick[i] );

		if ( f != NULL )
		{
			f->faceMarkInvert();
		}
	}
}


void MMesh::growMarkedVertices()
{
	assertFinalised();

	MVertexList verticesToMark;
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];
		if ( !v->isVertexMarked() )
		{
			if ( v->hasNeighbouringMarkedVertices() )
			{
				verticesToMark.push_back( v );
			}
		}
	}

	for (int vertexI = 0; vertexI < verticesToMark.size(); vertexI++)
	{
		verticesToMark[vertexI]->vertexMark();
	}
}

void MMesh::shrinkMarkedVertices()
{
	assertFinalised();

	MVertexList verticesToUnmark;
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *v = vertices[vertexI];
		if ( v->isVertexMarked() )
		{
			if ( v->hasNeighbouringUnmarkedVertices() )
			{
				verticesToUnmark.push_back( v );
			}
		}
	}

	for (int vertexI = 0; vertexI < verticesToUnmark.size(); vertexI++)
	{
		verticesToUnmark[vertexI]->vertexUnmark();
	}
}

void MMesh::growMarkedEdges()
{
	assertFinalised();

	MEdgeList edgesToMark;
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( !e->isEdgeMarked() )
		{
			if ( e->hasNeighbouringMarkedEdges() )
			{
				edgesToMark.push_back( e );
			}
		}
	}

	for (int edgeI = 0; edgeI < edgesToMark.size(); edgeI++)
	{
		edgesToMark[edgeI]->edgeMark();
	}
}

void MMesh::shrinkMarkedEdges()
{
	assertFinalised();

	MEdgeList edgesToUnmark;
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( e->isEdgeMarked() )
		{
			if ( e->hasNeighbouringUnmarkedEdges() )
			{
				edgesToUnmark.push_back( e );
			}
		}
	}

	for (int edgeI = 0; edgeI < edgesToUnmark.size(); edgeI++)
	{
		edgesToUnmark[edgeI]->edgeUnmark();
	}
}

void MMesh::growMarkedFaces()
{
	assertFinalised();

	MFaceList facesToMark;
	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( !f->isFaceMarked() )
		{
			if ( f->hasNeighbouringMarkedFaces() )
			{
				facesToMark.push_back( f );
			}
		}
	}

	for (int faceI = 0; faceI < facesToMark.size(); faceI++)
	{
		facesToMark[faceI]->faceMark();
	}
}

void MMesh::shrinkMarkedFaces()
{
	assertFinalised();

	MFaceList facesToUnmark;
	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			if ( f->hasNeighbouringUnmarkedFaces() )
			{
				facesToUnmark.push_back( f );
			}
		}
	}

	for (int faceI = 0; faceI < facesToUnmark.size(); faceI++)
	{
		facesToUnmark[faceI]->faceUnmark();
	}
}




Side MMesh::areVerticesInside(const Region3d *region) const
{
	assertFinalised();

	Side side = SIDE_NEGATIVE;

	int i;
	for (i = 0; i < vertices.size(); i++)
	{
		side = vertices[i]->isInside( region )  ?  SIDE_POSITIVE  :  SIDE_NEGATIVE;
		break;
	}

	for (; i < vertices.size(); i++)
	{
		Side s = vertices[i]->isInside( region )  ?  SIDE_POSITIVE  :  SIDE_NEGATIVE;

		if ( s != side )
		{
			return SIDE_BOTH;
		}
	}

	return side;
}





BBox3 MMesh::getVerticesBBox(bool markedOnly) const
{
	int i;
	BBox3 box;

	for (i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box = BBox3( vertices[i]->getPosition() );
			break;
		}
	}
	for (; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box.addPoint( vertices[i]->getPosition() );
		}
	}

	return box;
}

BBox3 MMesh::getVerticesBBox(const Matrix4 &m, bool markedOnly) const
{
	int i;
	BBox3 box;

	for (i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box = BBox3( vertices[i]->getPosition() * m );
			break;
		}
	}
	for (; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box.addPoint( vertices[i]->getPosition() * m );
		}
	}

	return box;
}

BBox3 MMesh::getVerticesBBox(const Transformation *x, bool markedOnly) const
{
	int i;
	BBox3 box;

	for (i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box = BBox3(  x->transformPoint( vertices[i]->getPosition() )  );
			break;
		}
	}
	for (; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box.addPoint(  x->transformPoint( vertices[i]->getPosition() )  );
		}
	}

	return box;
}

BBox3 MMesh::getVerticesBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	int i;
	BBox3 box;

	for (i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box = BBox3(  x->transformPoint( vertices[i]->getPosition() ) * m  );
			break;
		}
	}
	for (; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked()  ||  !markedOnly )
		{
			box.addPoint(  x->transformPoint( vertices[i]->getPosition() ) * m  );
		}
	}

	return box;
}


BBox3 MMesh::getMarkedEdgesBBox() const
{
	BBox3 box;

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( e->isEdgeMarked() )
		{
			box.addPoint( e->getVertexA()->getPosition() );
			box.addPoint( e->getVertexB()->getPosition() );
		}
	}

	return box;
}

BBox3 MMesh::getMarkedEdgesBBox(const Matrix4 &m) const
{
	BBox3 box;

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( e->isEdgeMarked() )
		{
			MVertex *a = e->getVertexA();
			MVertex *b = e->getVertexB();

			if ( !a->isSecondaryMarked() )
			{
				box.addPoint( a->getPosition() * m );
				a->secondaryMark();
			}

			if ( !b->isSecondaryMarked() )
			{
				box.addPoint( b->getPosition() * m );
				b->secondaryMark();
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}

BBox3 MMesh::getMarkedEdgesBBox(const Transformation *x) const
{
	BBox3 box;

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( e->isEdgeMarked() )
		{
			MVertex *a = e->getVertexA();
			MVertex *b = e->getVertexB();

			if ( !a->isSecondaryMarked() )
			{
				box.addPoint( x->transformPoint( a->getPosition() ) );
				a->secondaryMark();
			}

			if ( !b->isSecondaryMarked() )
			{
				box.addPoint( x->transformPoint( b->getPosition() ) );
				b->secondaryMark();
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}

BBox3 MMesh::getMarkedEdgesBBox(const Matrix4 &m, const Transformation *x) const
{
	BBox3 box;

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];
		if ( e->isEdgeMarked() )
		{
			MVertex *a = e->getVertexA();
			MVertex *b = e->getVertexB();

			if ( !a->isSecondaryMarked() )
			{
				box.addPoint( x->transformPoint( a->getPosition() * m ) );
				a->secondaryMark();
			}

			if ( !b->isSecondaryMarked() )
			{
				box.addPoint( x->transformPoint( b->getPosition() * m ) );
				b->secondaryMark();
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}



BBox3 MMesh::getMarkedFacesBBox() const
{
	BBox3 box;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			for (int vertexI = 0; vertexI < f->getSize(); vertexI++)
			{
				MVertex *v = f->getVertex( vertexI );
				box.addPoint( v->getPosition() );
			}
		}
	}

	return box;
}

BBox3 MMesh::getMarkedFacesBBox(const Matrix4 &m) const
{
	BBox3 box;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			for (int vertexI = 0; vertexI < f->getSize(); vertexI++)
			{
				MVertex *v = f->getVertex( vertexI );

				if ( !v->isSecondaryMarked() )
				{
					box.addPoint( v->getPosition() * m );
					v->secondaryMark();
				}
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}

BBox3 MMesh::getMarkedFacesBBox(const Transformation *x) const
{
	BBox3 box;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			for (int vertexI = 0; vertexI < f->getSize(); vertexI++)
			{
				MVertex *v = f->getVertex( vertexI );

				if ( !v->isSecondaryMarked() )
				{
					box.addPoint( x->transformPoint( v->getPosition() ) );
					v->secondaryMark();
				}
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}

BBox3 MMesh::getMarkedFacesBBox(const Matrix4 &m, const Transformation *x) const
{
	BBox3 box;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		MFace *f = faces[faceI];
		if ( f->isFaceMarked() )
		{
			for (int vertexI = 0; vertexI < f->getSize(); vertexI++)
			{
				MVertex *v = f->getVertex( vertexI );

				if ( !v->isSecondaryMarked() )
				{
					box.addPoint( x->transformPoint( v->getPosition() * m ) );
					v->secondaryMark();
				}
			}
		}
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI]->secondaryUnmark();
	}

	return box;
}


BBox3 MMesh::getMarkedFaceRegionBBox(const MFaceList &region) const
{
	BBox3 box;

	for (int faceI = 0; faceI < region.size(); faceI++)
	{
		MFace *f = region[faceI];

		for (int vertexI = 0; vertexI < f->getSize(); vertexI++)
		{
			MVertex *v = f->getVertex( vertexI );

			box.addPoint( v->getPosition() );
			v->secondaryMark();
		}
	}

	return box;
}



Vector3 MMesh::computeMarkedFacesWeightedNormal() const
{
	Vector3 normal;
	double maxSqrArea = 0.0;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		if ( faces[faceI]->isFaceMarked() )
		{
			Vector3 n = faces[faceI]->computeTwiceAreaNormalProduct();
			normal += n;
			maxSqrArea = std::max( maxSqrArea, n.sqrLength() );
		}
	}

	if ( normal.sqrLength()  <  ( maxSqrArea * SQR_EPSILON ) )
	{
		return Vector3();
	}
	else
	{
		return normal.getNormalised();
	}
}

Vector3 MMesh::computeWeightedNormal() const
{
	Vector3 normal;
	double maxSqrArea = 0.0;

	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		Vector3 n = faces[faceI]->computeTwiceAreaNormalProduct();
		normal += n;
		maxSqrArea = std::max( maxSqrArea, n.sqrLength() );
	}

	if ( normal.sqrLength()  <  ( maxSqrArea * SQR_EPSILON ) )
	{
		return Vector3();
	}
	else
	{
		return normal.getNormalised();
	}
}



void MMesh::fix()
{
	assertFinalised();

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->fix();
	}

	finalise();
}





const MBBTree * MMesh::getBBTree()
{
	if ( flags.bbTreeRequiresRebuild )
	{
		clearVertexPositionChangedForMeshFlags();

		flags.bbTreeRequiresRebuild = false;
		flags.bbTreeRequiresRefresh = false;

		bbTree->rebuild();
	}
	else if ( flags.bbTreeRequiresRefresh )
	{
		clearVertexPositionChangedForMeshFlags();

		flags.bbTreeRequiresRefresh = false;

		bbTree->refresh();
	}

	return bbTree;
}

const BBox3 & MMesh::getBoundingBox()
{
	if ( flags.boundingBoxRequiresRefresh )
	{
		clearVertexPositionChangedForMeshFlags();

		flags.boundingBoxRequiresRefresh = false;

		if ( vertices.size() > 0 )
		{
			boundingBox = BBox3( vertices[0]->getPosition() );

			for (int i = 1; i < vertices.size(); i++)
			{
				boundingBox.addPoint( vertices[i]->getPosition() );
			}

			boundingBox.enlargeForClipping();
		}
		else
		{
			boundingBox = BBox3();
		}
	}

	return boundingBox;
}





MFace * MMesh::raytraceFaceClipRay(Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint, int &tri)
{
	const BBox3 &bbox = getBoundingBox();


	//take a temporary copy of the line as BBox3::clipLine() may modify the line
	//even if the line does not intersect the mesh
	Segment3 tempRay = ray;


	if ( !bbox.clipSegment( tempRay ) )
	{
		//line does not intersect bbox: no hit
		return NULL;
	}


	tempRay = ray;


	const MBBTree *tree = getBBTree();
	MBBTree::TraversalState state;
	const int *start, *end;
	int triangleIndex = -1;
	MFace *hitFace = NULL;
	while ( tree->raytrace( tempRay, state, start, end ) )
	{
		for (const int *indexPtr = start; indexPtr < end; indexPtr++)
		{
			MFace *face = faces[*indexPtr];
			if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
			{
				if ( face->hits( tempRay, backfaceCullingFlag, t, intersectionPoint, triangleIndex ) )
				{
					//shorten the ray to a line segment
					tempRay.b = intersectionPoint;
					ray.b = intersectionPoint;

					hitFace = face;
					tri = triangleIndex;
				}
			}
		}
	}


	return hitFace;
}

MFace * MMesh::raytraceFaceClipRay(Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint)
{
	int tri;
	return raytraceFaceClipRay( ray, backfaceCullingFlag, faceMarkedOnlyFlag, t, intersectionPoint, tri );
}

MFace * MMesh::raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint, int &tri)
{
	Segment3 tempRay = ray;
	return raytraceFaceClipRay( tempRay, backfaceCullingFlag, faceMarkedOnlyFlag, t, intersectionPoint, tri );
}

MFace * MMesh::raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint)
{
	Segment3 tempRay = ray;
	return raytraceFaceClipRay( tempRay, backfaceCullingFlag, faceMarkedOnlyFlag, t, intersectionPoint );
}



MFace * MMesh::pickFaceIncidentToVertexClosestToP(MVertex *vertex, const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag)
{
	Polygon3 projectedPoly3;
	Polygon2 projectedPoly2;


	double bestSqrDist = std::numeric_limits<double>::max();
	double bestZ = std::numeric_limits<double>::max();
	MFace *bestFace = NULL;


	for (int faceI = 0; faceI < vertex->getNumFaces(); faceI++)
	{
		MFace *face = vertex->getFace( faceI );


		if ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )
		{
			if ( !backfaceCullingFlag  ||  ray.a >= face->getPlane() )
			{
				projectedPoly3.clear();
				projectedPoly2.clear();

				face->generatePolygon( projectedPoly3 );
				projection.clipAndProjectPoly( projectedPoly3 );

				if ( projectedPoly3.size() > 0 )
				{
					projectedPoly3.generatePolygon2( projectedPoly2 );

					double sqrDist = projectedPoly2.sqrDistanceTo( p );

					if ( almostEqualRelative( sqrDist, bestSqrDist ) )
					{
						// Distances equal; decide using the Z of the centroid
						double z = projectedPoly3.computeCentroidZ();

						if ( z < bestZ )
						{
							bestSqrDist = sqrDist;
							bestZ = z;
							bestFace = face;
						}
					}
					else if ( sqrDist < bestSqrDist )
					{
						bestSqrDist = sqrDist;
						bestZ = projectedPoly3.computeCentroidZ();
						bestFace = face;
					}
				}
			}
		}
	}

	return bestFace;
}



MFace * MMesh::pickFaceIncidentToEdgeClosestToP(MEdge *edge, const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag)
{
	if ( edge->getFaceB() == NULL )
	{
		if ( !backfaceCullingFlag  ||  ray.a >= edge->getFaceA()->getPlane() )
		{
			return edge->getFaceA();
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		MFace *a = edge->getFaceA();
		MFace *b = edge->getFaceB();

		if ( faceMarkedOnlyFlag )
		{
			if ( a->isFaceMarked()  &&  !b->isFaceMarked() )
			{
				return a;
			}
			else if ( !a->isFaceMarked()  &&  b->isFaceMarked() )
			{
				return b;
			}
			else if ( !a->isFaceMarked()  &&  !b->isFaceMarked() )
			{
				return NULL;
			}
		}

		Polygon3 projectedPoly3;
		Polygon2 projectedPoly2;


		// Face A
		a->generatePolygon( projectedPoly3 );
		projection.clipAndProjectPoly( projectedPoly3 );

		double bestSqrDist = std::numeric_limits<double>::max();
		double bestZ = std::numeric_limits<double>::max();
		MFace *f = NULL;

		if ( projectedPoly3.size() > 0 )
		{
			projectedPoly3.generatePolygon2( projectedPoly2 );

			bestSqrDist = projectedPoly2.sqrDistanceToExternalPoint( p );
			bestZ = projectedPoly3.computeCentroidZ();
			f = a;
		}


		// Face B
		projectedPoly3.clear();
		projectedPoly2.clear();

		b->generatePolygon( projectedPoly3 );
		projection.clipAndProjectPoly( projectedPoly3 );

		if ( projectedPoly3.size() > 0 )
		{
			projectedPoly3.generatePolygon2( projectedPoly2 );

			double sqrDist = projectedPoly2.sqrDistanceToExternalPoint( p );

			if ( almostEqualRelative( sqrDist, bestSqrDist ) )
			{
				// Distances equal; decide using the Z of the centroid
				double z = projectedPoly3.computeCentroidZ();

				if ( z < bestZ )
				{
					bestSqrDist = sqrDist;
					f = b;
				}
			}
			else if ( sqrDist < bestSqrDist )
			{
				bestSqrDist = sqrDist;
				f = b;
			}
		}

		return f;
	}
}


MFace * MMesh::pickFace(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, double &t, Point3 &intersectionPoint)
{
	const BBox3 &bbox = getBoundingBox();


	//take a temporary copy of the line as BBox3::clipLine() may modify the line
	//even if the line does not intersect the mesh
	Segment3 tempRay = ray;


	MFace *pickedFace = NULL;
	if ( bbox.clipSegment( tempRay ) )
	{
		// Do not use the clipped ray.
		// When a face is not planar, an average plane is computed for it. As a result, a segment-plane intersection *can* occur outside the
		// mesh bounding box, which will be beyond the bounds of the clipped segment. This causes the segment-plane intersection
		// computed in MFace::pick() to fail. This problem is avoided by using the unclipped ray.
		tempRay = ray;


		const MBBTree *tree = getBBTree();
		MBBTree::TraversalState state;
		const int *start, *end;
		while ( tree->raytrace( tempRay, state, start, end ) )
		{
			for (const int *indexPtr = start; indexPtr < end; indexPtr++)
			{
				MFace *face = faces[*indexPtr];
				if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
				{
					if ( face->pick( p, projection, tempRay, backfaceCullingFlag, t, intersectionPoint ) )
					{
						//shorten the segment
						tempRay.b = intersectionPoint;

						pickedFace = face;
					}
				}
			}
		}
	}


	if ( pickedFace == NULL  &&  ( bAllowPickOutsideBounds  ||  !bSolidPick ) )
	{
		const MBBTree *tree = getBBTree();
		MBBTree::TraversalState state;
		const int *start, *end;

		double bestSqrDist = std::numeric_limits<double>::max();
		double bestZ = std::numeric_limits<double>::max();

		while ( tree->findClosestScreenPoint( p, bestSqrDist, projection, state, start, end ) )
		{
			for (const int *indexPtr = start; indexPtr < end; indexPtr++)
			{
				MFace *face = faces[*indexPtr];
				if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
				{
					if ( !backfaceCullingFlag  ||  ray.a >= face->getPlane() )
					{
						Polygon3 projectedPoly3;
						Polygon2 projectedPoly2;

						face->generatePolygon( projectedPoly3 );
						projection.clipAndProjectPoly( projectedPoly3 );

						if ( projectedPoly3.size() > 0 )
						{
							projectedPoly3.generatePolygon2( projectedPoly2 );

							double sqrDist = projectedPoly2.sqrDistanceToExternalPoint( p );
							if ( almostEqualRelative( sqrDist, bestSqrDist ) )
							{
								// Distances equal; decide using the Z of the centroid
								double z = projectedPoly3.computeCentroidZ();

								if ( z < bestZ )
								{
									bestSqrDist = sqrDist;
									bestZ = z;
									pickedFace = face;
								}
							}
							else if ( sqrDist < bestSqrDist )
							{
								bestSqrDist = sqrDist;
								bestZ = projectedPoly3.computeCentroidZ();
								pickedFace = face;
							}
						}
					}
				}
			}
		}
	}

	return pickedFace;
}

MEdge * MMesh::pickEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, Point3 &closestPoint, MFace *&thruFace)
{
	double t;
	Point3 intersectionPoint;

	thruFace = NULL;

	if ( bSolidPick )
	{
		thruFace = pickFace( p, projection, ray, backfaceCullingFlag, false, true, false, t, intersectionPoint );
	}

	if ( thruFace != NULL )
	{
		return thruFace->pickEdge( p, projection, screenToWorldMatrix, faceMarkedOnlyFlag, closestPoint );
	}
	else
	{
		if ( bAllowPickOutsideBounds  ||  !bSolidPick )
		{
			const MBBTree *tree = getBBTree();
			MBBTree::TraversalState state;
			const int *start, *end;

			MEdge *bestEdge = NULL;
			double bestSqrDist = std::numeric_limits<double>::max();
			double bestZ = std::numeric_limits<double>::max();
			Point3 bestClosestPointProjected;

			while ( tree->findClosestScreenPoint( p, bestSqrDist, projection, state, start, end ) )
			{
				for (const int *indexPtr = start; indexPtr < end; indexPtr++)
				{
					MFace *face = faces[*indexPtr];
					if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
					{
						if ( !backfaceCullingFlag  ||  ray.a >= face->getPlane() )
						{
							for (int edgeI = 0; edgeI < face->getSize(); edgeI++)
							{
								MEdge *edge = face->getEdge( edgeI );

								Segment3 edgeSegment = edge->getSegment();
								Segment3 clippedAndProjectedSegment;
								if ( projection.clipAndProjectSegment( clippedAndProjectedSegment, edgeSegment ) )
								{
									Segment2 screenSegment = clippedAndProjectedSegment.toSegment2();

									double t;
									Point2 closest = screenSegment.closestPointTo( p, t );

									double sqrDist = closest.sqrDistanceTo( p );
									if ( almostEqualRelative( sqrDist, bestSqrDist ) )
									{
										// Distances equal; decide using the Z of the mid-point
										double z = clippedAndProjectedSegment.getMidPoint().z;

										if ( z < bestZ )
										{
											bestSqrDist = sqrDist;
											bestZ = z;
											bestEdge = edge;
											bestClosestPointProjected = clippedAndProjectedSegment.getPoint( t );
										}
									}
									else 	if ( sqrDist < bestSqrDist )
									{
										bestSqrDist = sqrDist;
										bestZ = clippedAndProjectedSegment.getMidPoint().z;
										bestEdge = edge;
										bestClosestPointProjected = clippedAndProjectedSegment.getPoint( t );
									}
								}
							}
						}
					}
				}
			}


			if ( bestEdge != NULL )
			{
				closestPoint = screenToWorldMatrix.transformHomogeneous( bestClosestPointProjected );
				thruFace = pickFaceIncidentToEdgeClosestToP( bestEdge, p, projection, ray, backfaceCullingFlag, faceMarkedOnlyFlag );
				return bestEdge;
			}
		}

		return NULL;
	}
}

MVertex * MMesh::pickVertex(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, MFace *&thruFace)
{
	double t;
	Point3 intersectionPoint;

	thruFace = NULL;

	if ( bSolidPick )
	{
		thruFace = pickFace( p, projection, ray, backfaceCullingFlag, false, true, false, t, intersectionPoint );
	}

	if ( thruFace != NULL )
	{
		return thruFace->pickVertex( p, projection, faceMarkedOnlyFlag );
	}
	else
	{
		if ( bAllowPickOutsideBounds  ||  !bSolidPick )
		{
			const MBBTree *tree = getBBTree();
			MBBTree::TraversalState state;
			const int *start, *end;

			MVertex *bestVertex = NULL;
			double bestSqrDist = std::numeric_limits<double>::max();
			double bestZ = std::numeric_limits<double>::max();

			while ( tree->findClosestScreenPoint( p, bestSqrDist, projection, state, start, end ) )
			{
				for (const int *indexPtr = start; indexPtr < end; indexPtr++)
				{
					MFace *face = faces[*indexPtr];
					if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
					{
						if ( !backfaceCullingFlag  ||  ray.a >= face->getPlane() )
						{
							for (int vertexI = 0; vertexI < face->getSize(); vertexI++)
							{
								MVertex *vertex = face->getVertex( vertexI );

								Point3 projectedVertex;
								if ( projection.checkAndProjectPoint( projectedVertex, vertex->getPosition() ) )
								{
									Point2 screenVertex = projectedVertex.toPoint2();
									double sqrDist = screenVertex.sqrDistanceTo( p );
									if ( almostEqualRelative( sqrDist, bestSqrDist ) )
									{
										// Distances almost equal; decide based on Z
										double z = projectedVertex.z;

										if ( z < bestZ )
										{
											bestSqrDist = sqrDist;
											bestZ = z;
											bestVertex = vertex;
										}
									}
									else if ( sqrDist < bestSqrDist )
									{
										bestSqrDist = sqrDist;
										bestZ = projectedVertex.z;
										bestVertex = vertex;
									}
								}
							}
						}
					}
				}
			}


			if ( bestVertex != NULL )
			{
				thruFace = pickFaceIncidentToVertexClosestToP( bestVertex, p, projection, ray, backfaceCullingFlag, faceMarkedOnlyFlag );
				return bestVertex;
			}
		}

		return NULL;
	}
}

MVertex * MMesh::pickVertexThruEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, MFace *&thruFace, MEdge *&thruEdge)
{
	double t;
	Point3 intersectionPoint;

	thruFace = NULL;

	if ( bSolidPick )
	{
		thruFace = pickFace( p, projection, ray, backfaceCullingFlag, false, true, false, t, intersectionPoint );
	}

	if ( thruFace != NULL )
	{
		return thruFace->pickVertexThruEdge( p, projection, screenToWorldMatrix, faceMarkedOnlyFlag, thruEdge );
	}
	else
	{
		if ( bAllowPickOutsideBounds  ||  !bSolidPick )
		{
			const MBBTree *tree = getBBTree();
			MBBTree::TraversalState state;
			const int *start, *end;

			MEdge *bestEdge = NULL;
			MVertex *bestVertex = NULL;
			double bestSqrDist = std::numeric_limits<double>::max();
			double bestZ = std::numeric_limits<double>::max();

			while ( tree->findClosestScreenPoint( p, bestSqrDist, projection, state, start, end ) )
			{
				for (const int *indexPtr = start; indexPtr < end; indexPtr++)
				{
					MFace *face = faces[*indexPtr];
					if ( ( !faceMarkedOnlyFlag  ||  face->isFaceMarked() )  &&  !face->isDestroyed() )
					{
						if ( !backfaceCullingFlag  ||  ray.a >= face->getPlane() )
						{
							int vertexI = face->getSize() - 1;
							MVertex *vertex = face->getVertex( vertexI );
							for (int vertexINext = 0; vertexINext < face->getSize(); vertexINext++)
							{
								MVertex *vertexNext = face->getVertex( vertexINext );
								MEdge *edge = face->getEdge( vertexI );

								Segment3 edgeSegment = edge->getSegment( vertex );
								Segment3 clippedAndProjectedSegment;
								if ( projection.clipAndProjectSegment( clippedAndProjectedSegment, edgeSegment ) )
								{
									Segment2 screenSegment = clippedAndProjectedSegment.toSegment2();

									double t;
									Point2 closest = screenSegment.closestPointTo( p, t );

									double sqrDist = closest.sqrDistanceTo( p );
									if ( almostEqualRelative( sqrDist, bestSqrDist ) )
									{
										// Distances equal; decide using the Z of the mid-point
										double z = clippedAndProjectedSegment.getMidPoint().z;

										if ( z < bestZ )
										{
											bestSqrDist = sqrDist;
											bestZ = z;
											bestEdge = edge;
											bestVertex = t <= 0.5  ?  vertex  :  vertexNext;
										}
									}
									else 	if ( sqrDist < bestSqrDist )
									{
										bestSqrDist = sqrDist;
										bestZ = clippedAndProjectedSegment.getMidPoint().z;
										bestEdge = edge;
										bestVertex = t <= 0.5  ?  vertex  :  vertexNext;
									}
								}

								vertexI = vertexINext;
								vertex = vertexNext;
							}
						}
					}
				}
			}


			if ( bestVertex != NULL )
			{
				thruFace = pickFaceIncidentToEdgeClosestToP( bestEdge, p, projection, ray, backfaceCullingFlag, faceMarkedOnlyFlag );
				thruEdge = bestEdge;
				return bestVertex;
			}
		}

		return NULL;
	}
}


MFace * MMesh::pickFace(const MPick &pick, Point3 &intersectionPoint)
{
	Segment3 ray;
	Matrix4 screenToWorldMatrix = pick.projection.getWorldToScreenMatrix().inverse();

	ray.a = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 0.0 ) );
	ray.b = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 1.0 ) );

	double t;

	return pickFace( pick.screenPos, pick.projection, ray, pick.backfaceCullingFlag, false, pick.bSolidPick, pick.bAllowPickOutsideBounds, t, intersectionPoint );
}

MFace * MMesh::pickFace(const MPick &pick)
{
	Point3 intersectionPoint;

	return pickFace( pick, intersectionPoint );
}

MEdge * MMesh::pickEdge(const MPick &pick, Point3 &closestPoint, MFace *&thruFace)
{
	Segment3 ray;
	Matrix4 screenToWorldMatrix = pick.projection.getWorldToScreenMatrix().inverse();

	ray.a = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 0.0 ) );
	ray.b = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 1.0 ) );

	return pickEdge( pick.screenPos, pick.projection, screenToWorldMatrix, ray, pick.backfaceCullingFlag, false, pick.bSolidPick, pick.bAllowPickOutsideBounds, closestPoint, thruFace );
}

MEdge * MMesh::pickEdge(const MPick &pick, Point3 &closestPoint)
{
	MFace *thruFace = NULL;
	return pickEdge( pick, closestPoint, thruFace );
}

MVertex * MMesh::pickVertex(const MPick &pick, MFace *&thruFace)
{
	Segment3 ray;
	Matrix4 screenToWorldMatrix = pick.projection.getWorldToScreenMatrix().inverse();

	ray.a = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 0.0 ) );
	ray.b = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 1.0 ) );

	return pickVertex( pick.screenPos, pick.projection, ray, pick.backfaceCullingFlag, false, pick.bSolidPick, pick.bAllowPickOutsideBounds, thruFace );
}

MVertex * MMesh::pickVertex(const MPick &pick)
{
	MFace *thruFace = NULL;
	return pickVertex( pick, thruFace );
}

MVertex * MMesh::pickVertexThruEdge(const MPick &pick, MFace *&thruFace, MEdge *&thruEdge)
{
	Segment3 ray;
	Matrix4 screenToWorldMatrix = pick.projection.getWorldToScreenMatrix().inverse();

	ray.a = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 0.0 ) );
	ray.b = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 1.0 ) );

	return pickVertexThruEdge( pick.screenPos, pick.projection, screenToWorldMatrix, ray, pick.backfaceCullingFlag, false, pick.bSolidPick, pick.bAllowPickOutsideBounds, thruFace, thruEdge );
}

MEdge * MMesh::pickEdgeRing(MEdgeList &ring, const MPick &pick, Point3 &closestPoint, MFace *&thruFace)
{
	MEdge *seed = pickEdge( pick, closestPoint, thruFace );

	if ( seed != NULL )
	{
		discoverEdgeRing( seed, ring );
	}

	return seed;
}

MEdge * MMesh::pickEdgeRing(MEdgeList &ring, const MPick &pick, Point3 &closestPoint)
{
	MFace *thruFace = NULL;
	return pickEdgeRing( ring, pick, closestPoint, thruFace );
}

MEdge * MMesh::pickEdgeLoop(MEdgeList &loop, const MPick &pick, Point3 &closestPoint, MFace *&thruFace)
{
	MEdge *seed = pickEdge( pick, closestPoint, thruFace );

	if ( seed != NULL )
	{
		discoverEdgeLoop( seed, loop );
	}

	return seed;
}

MEdge * MMesh::pickEdgeLoop(MEdgeList &loop, const MPick &pick, Point3 &closestPoint)
{
	MFace *thruFace = NULL;
	return pickEdgeLoop( loop, pick, closestPoint, thruFace );
}

MEdge * MMesh::pickEdgeBoundary(MEdgeList &boundary, const MPick &pick, Point3 &closestPoint, MFace *&thruFace)
{
	MEdge *seed = pickEdge( pick, closestPoint, thruFace );

	if ( seed != NULL )
	{
		if ( seed->isBoundary() )
		{
			discoverEdgeBoundary( seed, boundary );
			return seed;
		}
	}

	return NULL;
}

MEdge * MMesh::pickEdgeBoundary(MEdgeList &boundary, const MPick &pick, Point3 &closestPoint)
{
	MFace *thruFace = NULL;
	return pickEdgeBoundary( boundary, pick, closestPoint, thruFace );
}






void MMesh::assertFinalised() const
{
	gs_assert( flags.finalised, "MMesh::assertFinalised(): mesh not finalised\n" );
}

void MMesh::finalise()
{
	flags.finalised = true;
	//checkIntegrity();
}



void MMesh::checkIntegrity()
{
	bool passed = true;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( !edges[i]->checkIntegrity() )
		{
			passed = false;
		}
	}
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i]->checkIntegrity() )
		{
			passed = false;
		}
	}
	for (int i = 0; i < faces.size(); i++)
	{
		if ( !faces[i]->checkIntegrity() )
		{
			passed = false;
		}
	}

	// Check the marked vertex, edge, and face counts
	int vcount = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i]->isVertexMarked() )
		{
			vcount++;
		}
	}

	int ecount = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			ecount++;
		}
	}

	int fcount = 0;
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			fcount++;
		}
	}

	if ( vcount != markedVertexCount )
	{
		printf( "MMesh::checkIntegrity(): incorrect marked vertex count\n" );
		passed = false;
	}

	if ( ecount != markedEdgeCount )
	{
		printf( "MMesh::checkIntegrity(): incorrect marked vertex count\n" );
		passed = false;
	}

	if ( fcount != markedFaceCount )
	{
		printf( "MMesh::checkIntegrity(): incorrect marked vertex count\n" );
		passed = false;
	}


	gs_assert( passed, "MMesh::checkIntegrity(): integrity check failed\n" );
}


void MMesh::display()
{
	printf( "Faces (%d):\n", faces.size() );
	for (int i = 0; i < faces.size(); i++)
	{
		printf( "Face %d @ %p:\n", i, faces[i] );
		faces[i]->display();
	}
	printf( "========\n" );
}




void MMesh::buildRenderMesh(MRenderMesh &renderMesh)
{
	Array<int> renderMeshFaceVertIndices;

	renderMesh.init( vertices.size() );


	for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++)
	{
		MFace *f = faces[faceIndex];
		renderMeshFaceVertIndices.resize( f->getSize() );

		for (int faceVertIndex = 0; faceVertIndex < f->getSize(); faceVertIndex++)
		{
			renderMeshFaceVertIndices[faceVertIndex] = renderMesh.getRenderVertexIndex( f->getVertex( faceVertIndex ), f->getVertexAttrib( faceVertIndex ), f->getVertexNormal( faceVertIndex ) );
		}

		renderMesh.addFace( renderMeshFaceVertIndices );
	}
}





Vector3 MMesh::computeExpansionVector(const Array<Vector3> &normals)
{
	if ( normals.size() == 1 )
	{
		return normals[0];
	}
	else if ( normals.size() == 2 )
	{
		Vector3 expansion = ( normals[0] + normals[1] ).getNormalised();
		return expansion  *  ( 1.0  /  expansion.dot( normals[0] ) );
	}
	else
	{
/*		// Use least-mean-square method to determine the best extrusion vector;
		// attempt to build a matrix equation and solve it.

		Vector3 validNormal;
		// Where V = @expansion and
		// 'Mt' means 'transpose( M )'
		//
		//	V =
		//	[		Vx		Vy		Vz	]
		//
		//	A =
		//	[	N1x		N2x		...		Nmx	]
		//	[	N1y		N2y		...		Nmy	]
		//	[	N1z		N2z		...		Nmz	]
		//
		//	1 =
		//	[	1		1		...		1		]
		//
		//
		//
		//	V . A  =  1			V . A . At  =  1 . At
		//
		//	Solve for V:
		//
		//	V  =  1 . A^-1			V  =  1 . At . ( A . At )^-1
		Matrix3 aAt;		// A . At
		Vector3 oneAt;	// 1 . At

		// Matrices are initialised to identity; set the diagonal elements to 0
		aAt.at(0,0) = aAt.at(1,1) = aAt.at(2,2) = 0.0;

		int normalCount = 0;
		Vector3 cumulativeNormal;

		// For each normal
		for (int normalI = 0; normalI < normals.size(); normalI++)
		{
			const Vector3 &n = normals[normalI];

			// Check if @n is a equivalent to, or opposite to a normal that has already been processed; these normals
			// are 'degenerate'
			bool degenerateFlag = false;
			for (int cmpI = 0; cmpI < normalI; cmpI++)
			{
				if ( fabs( n.dot( normals[cmpI] ) )  >  MMESH_ONEMINUSEPSILON )
				{
					// @n is equivalent to or opposite to the normal at index @cmpI
					degenerateFlag = true;
					break;
				}
			}

			if ( !degenerateFlag )
			{
				// Not degenerate; use for the accumulation of aAt and oneAt
				// Build up aAt and oneAt
				double xy = n.x * n.y;
				double xz = n.x * n.z;
				double yz = n.y * n.z;

				oneAt.x += n.x;
				oneAt.y += n.y;
				oneAt.z += n.z;

				aAt.at(0,0) += n.x * n.x;
				aAt.at(0,1) += xy;
				aAt.at(0,2) += xz;

				aAt.at(1,0) += xy;
				aAt.at(1,1) += n.y * n.y;
				aAt.at(1,2) += yz;

				aAt.at(2,0) += xz;
				aAt.at(2,1) += yz;
				aAt.at(2,2) += n.z * n.z;

				// Count the number of normals used
				normalCount++;

				// We might end up with less than three non-degenerate normals, so use the average-normal method as a fall back in
				// this case; accumulate the normals, and set @validNormal to be the most recent valid (non-degenerate) normal
				cumulativeNormal += n;
				validNormal = n;
			}
		}

		if ( normalCount >= 3 )
		{
			// We got >= 3 normals; invert the matrix A . At
			if ( aAt.invert() )
			{
				// Inversion successful; compute the expansion vector
				return oneAt * aAt;
			}
		}

		// normalCount is <= 2  or  the matrix inversion failed
		if ( normalCount >= 2 )
		{
			// Use average normals method
			cumulativeNormal.normalise();
			return cumulativeNormal  *  ( 1.0  /  cumulativeNormal.dot( validNormal ) );
		}
		else
		{
			// Only one valid normal; use this
			return validNormal;
		}*/

		Vector3 averageNormal;
		for (int normalI = 0; normalI < normals.size(); normalI++)
		{
			averageNormal += normals[normalI];
		}
		averageNormal.normalise();

		double averageAlpha = 0.0;
		for (int normalI = 0; normalI < normals.size(); normalI++)
		{
			averageAlpha += averageNormal.dot( normals[normalI] );
		}
		averageAlpha *= 1.0 / (double)normals.size();

		double scaleFactor = 1.0 / averageAlpha;
		return averageNormal * scaleFactor;
	}
}





/****************************************************************************
 ****************************************************************************
						MDrawQuadsState
 ****************************************************************************
 ****************************************************************************/

MDrawQuadsState::MDrawQuadsState() : column( 0 ), row( 0 ), numColumns( 0 ), numRows( 0 ),
								bRowDirectionForward( true ), bCloseRows( false ), bCloseColumns( false ), bFlip( false ), bFlipFixed( false )
{
}



int MDrawQuadsState::getColumn() const
{
	return column;
}

int MDrawQuadsState::getRow() const
{
	return row;
}

int MDrawQuadsState::getNumColumns() const
{
	return numColumns;
}

int MDrawQuadsState::getNumRows() const
{
	return numRows;
}

bool MDrawQuadsState::isRowDirectionForward() const
{
	return bRowDirectionForward;
}

bool MDrawQuadsState::areRowsClosed() const
{
	return bCloseRows;
}

bool MDrawQuadsState::areColumnsClosed() const
{
	return bCloseColumns;
}

bool MDrawQuadsState::getQuadMeshVertexPosition(int column, int row, Point3 &pos) const
{
	int vertexIndex = column  +  row * numColumns;

	if ( vertexIndex < vertexPositions.size() )
	{
		pos = vertexPositions[vertexIndex];

		if ( numColumns < 2  ||  numRows < 2 )
		{
			return true;
		}
		else
		{
			return verticesValid[vertexIndex];
		}
	}
	else
	{
		pos = Point3();
		return false;
	}
}



bool MDrawQuadsState::checkVertex(MVertex *v)
{
	return verifyVertex( v, false );
}


void MDrawQuadsState::drawQuads(MMesh *mesh, const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlipRequested)
{
	this->numColumns = numColumns;
	this->bCloseRows = bCloseRows;
	this->bCloseColumns = bCloseColumns;
	bFlip = bFlipRequested;

	if ( points.size() > 0  &&  numColumns > 0 )
	{
		// Rows cannot be closed if there are <= 2 vertices per row
		if ( numColumns <= 2 )
		{
			bCloseRows = false;
		}


		// Compute the number of rows
		numRows = points.size() / numColumns;
		if ( points.size() % numColumns  >  0 )
		{
			numRows++;
		}

		// Columns cannot be closed if there are <= 2 vertices per column
		if ( numRows <= 2 )
		{
			bCloseColumns = false;
		}

		// Initialise member variables
		column = 0;
		row = 0;
		bRowDirectionForward = true;
		bFlipFixed = false;

		int numVertices = numColumns * numRows;

		// Initialise vertex arrays
		existingVertices.clear();
		existingVerticesUsed.clear();
		quadMeshVertices.clear();
		vertexPositions.clear();
		verticesValid.clear();
		existingVertices.fill( NULL, numVertices );
		quadMeshVertices.fill( NULL, numVertices );
		vertexPositions.resize( numVertices );
		verticesValid.fill( false, numVertices );


		// Process points
		for (int pointI = 0; pointI < points.size(); pointI++)
		{
			const MDrawQuadsPoint &point = points[pointI];

			// Compute the index of the vertex that corresponds to the point
			int vertexIndex = column  +  row * numColumns;


			// Generate a vertex and add it
			if ( point.target == MDrawQuadsPoint::EXISTINGVERTEX )
			{
				bool bUseV = false;

				MVertex *v = mesh->pickVertex( point.pick );

				if ( v != NULL )
				{
					bUseV = verifyVertex( v, true );
				}

				if ( bUseV )
				{
					quadMeshVertices[vertexIndex] = v;
					existingVertices[vertexIndex] = v;
					existingVerticesUsed.insert( v );
					vertexPositions[vertexIndex] = v->getPosition();
					verticesValid[vertexIndex] = true;
				}
				else
				{
					if ( numColumns >= 2  &&  numRows >= 2 )
					{
						quadMeshVertices[vertexIndex] = mesh->addVertex( point.position );
					}
					vertexPositions[vertexIndex] = point.position;
					verticesValid[vertexIndex] = true;
				}
			}
			else if ( point.target == MDrawQuadsPoint::NEWPOINT )
			{
				if ( numColumns >= 2  &&  numRows >= 2 )
				{
					quadMeshVertices[vertexIndex] = mesh->addVertex( point.position );
				}
				vertexPositions[vertexIndex] = point.position;
				verticesValid[vertexIndex] = true;
			}
			else
			{
				gs_assert_not_reached( "MDrawQuadsState::drawQuads(): target is invalid\n" );
			}


			if ( bRowDirectionForward )
			{
				column++;
			}
			else
			{
				column--;
			}


			// We are at the end of the row if:
			//	column == numColumns (moving forward, passed the end)
			//	column == -1 (moving backward, passed the beginning)
			if ( column == numColumns  ||  column == -1 )
			{
				// Starting a new row:

				// Compute the direction for the new row
				if ( point.direction == MDrawQuadsPoint::FORWARD )
				{
					// Forward
					bRowDirectionForward = true;
				}
				else if ( point.direction == MDrawQuadsPoint::PINGPONG )
				{
					// Ping-pong - the opposite of the current direction
					bRowDirectionForward = !bRowDirectionForward;
				}
				else
				{
					gs_assert_not_reached( "MDrawQuadsState::nextTarget(): point direction is invalid\n" );
				}

				// Initialise the column index
				if ( bRowDirectionForward )
				{
					// forward; start at 0
					column = 0;
				}
				else
				{
					// backward; start at numColumns - 1
					column = numColumns - 1;
				}

				// Next row
				row++;
			}
		}


		if ( numColumns > 1  &&  numRows > 1 )
		{
			// Create all the faces
			int maxRow = bCloseColumns  ?  numRows - 1  :  numRows - 2;
			int maxColumn = bCloseRows  ?  numColumns - 1  :  numColumns - 2;

			double v = 0.0;
			double deltaV = bCloseColumns  ?  ( 1.0 / (double)numRows )  :  ( 1.0 / (double)(numRows-1) );

			Array<MVertex*> vts;
			Array<Point2f> tex;

			vts.resize( 4 );
			tex.resize( 4 );

			for (int y = 0; y <= maxRow; y++)
			{
				int yNext = nextIndex( y, numRows );

				double u = 0.0;
				double deltaU = bCloseRows  ?  ( 1.0 / (double)numColumns )  :  ( 1.0 / (double)(numColumns-1) );

				for (int x = 0; x <= maxColumn; x++)
				{
					int xNext = nextIndex( x, numColumns );

					MVertex *v00 = quadMeshVertices[ y * numColumns  +  x ];
					MVertex *v01 = quadMeshVertices[ y * numColumns  +  xNext ];
					MVertex *v10 = quadMeshVertices[ yNext * numColumns  +  x ];
					MVertex *v11 = quadMeshVertices[ yNext * numColumns  +  xNext ];

					MEdge *e = NULL;


					if ( v00 != NULL  &&  v01 != NULL  &&  v10 != NULL  &&  v11 != NULL )
					{
						if ( v00->getFaceCapacityForEdgeTo( v01, e )  >= 1  &&
							v01->getFaceCapacityForEdgeTo( v11, e )  >= 1  &&
							v11->getFaceCapacityForEdgeTo( v10, e )  >= 1  &&
							v10->getFaceCapacityForEdgeTo( v00, e )  >= 1 )
						{
							Point2f t00( u, v );
							Point2f t01( u + deltaU, v );
							Point2f t10( u, v + deltaV );
							Point2f t11( u + deltaU, v + deltaV );

							if ( bFlip )
							{
								vts[0] = v01;
								vts[1] = v00;
								vts[2] = v10;
								vts[3] = v11;

								tex[0] = t01;
								tex[1] = t00;
								tex[2] = t10;
								tex[3] = t11;
							}
							else
							{
								vts[0] = v00;
								vts[1] = v01;
								vts[2] = v11;
								vts[3] = v10;

								tex[0] = t00;
								tex[1] = t01;
								tex[2] = t11;
								tex[3] = t10;
							}

							mesh->addFace( vts, tex, -1 );
						}
					}


					u += deltaU;
				}

				v += deltaV;
			}
		}



		for (int vertI = 0; vertI < quadMeshVertices.size(); vertI++)
		{
			MVertex *v = quadMeshVertices[vertI];
			if ( v != NULL )
			{
				if ( v->destroyIfIsolated() )
				{
					quadMeshVertices[vertI] = NULL;
				}
			}
		}
	}
	else
	{
		numRows = 1;
	}
}



bool MDrawQuadsState::verifyVertex(MVertex *v, bool bAllowChangeDirection)
{
	bool bFirstColumn = column == 0;
	bool bLastColumn = column == ( numColumns - 1 );
	bool bFirstRow = row == 0;
	bool bLastRow = row == ( numRows - 1 );

	// Cannot use a vertex more than once
	if ( existingVerticesUsed.find( v )  !=  existingVerticesUsed.end() )
	{
		return false;
	}

	// If columns are closed, then only allow existing vertices to be used for the first and last columns
	if ( bCloseColumns && !bFirstColumn && !bLastColumn )
	{
		return false;
	}

	// If rows are closed, then only allow existing vertices to be used for the first and last rows
	if ( bCloseRows && !bFirstRow  &&  !bLastRow )
	{
		return false;
	}


	// Compute the indices of surrounding vertices
	int vLeftIndex = vtxIndex( column - 1, row );
	int vRightIndex = vtxIndex( column + 1, row );
	int vAboveIndex = vtxIndex( column, row - 1 );
	int vBelowIndex = vtxIndex( column, row + 1 );


	// Get the surrounding vertices
	MVertex *vLeft = NULL, *vRight = NULL, *vAbove = NULL, *vBelow = NULL;

	if ( vLeftIndex != -1 )
	{
		vLeft = existingVertices[vLeftIndex];
	}
	if ( vRightIndex != -1 )
	{
		vRight = existingVertices[vRightIndex];
	}
	if ( vAboveIndex != -1 )
	{
		vAbove = existingVertices[vAboveIndex];
	}
	if ( vBelowIndex != -1 )
	{
		vBelow = existingVertices[vBelowIndex];
	}


	bool bUseV = true;


	if ( vLeft != NULL )
	{
		if ( bLastRow )
		{
			bUseV = verifyEdge( vLeft, v, ( bFirstRow || bLastRow )  &&  !bCloseColumns, bAllowChangeDirection );
		}
		else
		{
			bUseV = verifyEdge( v, vLeft, ( bFirstRow || bLastRow )  &&  !bCloseColumns, bAllowChangeDirection );
		}
	}

	if ( bUseV )
	{
		if ( vRight != NULL )
		{
			if ( bLastRow )
			{
				bUseV = verifyEdge( v, vRight, ( bFirstRow || bLastRow )  &&  !bCloseColumns, bAllowChangeDirection );
			}
			else
			{
				bUseV = verifyEdge( vRight, v, ( bFirstRow || bLastRow )  &&  !bCloseColumns, bAllowChangeDirection );
			}
		}
	}

	if ( bUseV )
	{
		if ( vAbove != NULL )
		{
			if ( bLastColumn )
			{
				bUseV = verifyEdge( v, vAbove, ( bFirstColumn || bLastColumn )  &&  !bCloseRows, bAllowChangeDirection );
			}
			else
			{
				bUseV = verifyEdge( vAbove, v, ( bFirstColumn || bLastColumn )  &&  !bCloseRows, bAllowChangeDirection );
			}
		}
	}

	if ( bUseV )
	{
		if ( vBelow != NULL )
		{
			if ( bLastColumn )
			{
				bUseV = verifyEdge( vBelow, v, ( bFirstColumn || bLastColumn )  &&  !bCloseRows, bAllowChangeDirection );
			}
			else
			{
				bUseV = verifyEdge( v, vBelow, ( bFirstColumn || bLastColumn )  &&  !bCloseRows, bAllowChangeDirection );
			}
		}
	}

	return bUseV;
}



int MDrawQuadsState::vtxIndex(int column, int row) const
{
	if ( column >= 0  &&  column < numColumns  &&  row >= 0  &&  row < numRows )
	{
		return column  +  row * numColumns;
	}
	else
	{
		return -1;
	}
}

bool MDrawQuadsState::verifyEdge(MVertex *v0, MVertex *v1, bool bOnBoundary, bool bAllowChangeDirection)
{
	// @v0 and @v1 are in the order in which they would be used in a prospective quad

	// Find an existing edge linking @v0 and @v1
	MEdge *e = v0->findEdgeTo( v1, MFINDPOLICY_RETURNNULL );
	if ( e != NULL )
	{
		// First, check if we have too many faces
		int numFacesInQuadMesh = bOnBoundary  ?  1  :  2;
		if ( e->getNumFaces() + numFacesInQuadMesh  > 2 )
		{
			// Too many faces, cannot use it
			return false;
		}
		else	if ( e->getNumFaces() == 1 )
		{
			// Check the direction
			bool bEdgeDirection = true;
			if ( e->getVertexA() == v0  &&  e->getVertexB() == v1 )
			{
				// @v0 and @v1 go in the same direction as @e
				bEdgeDirection = true;
			}
			else if ( e->getVertexA() == v1  &&  e->getVertexB() == v0 )
			{
				// @v0 and @v1 go in the opposite direction to @e
				bEdgeDirection = false;
			}
			else
			{
				gs_assert_not_reached( "MDrawQuadsState::verifyEdge(): both @v0 and @v1 are not incident to @e\n" );
			}

			if ( bFlip )
			{
				// Flipped
				if ( bEdgeDirection )
				{
					// Edge direction is consistent with the respective quad, this is a problem since flip is enabled
					if ( !bFlipFixed )
					{
						if ( bAllowChangeDirection )
						{
							// Flipping has not been fixed; invert it, and fix the flipping
							bFlip = !bFlip;
							bFlipFixed = true;
						}
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return true;
				}
			}
			else
			{
				// Not flipped
				if ( !bEdgeDirection )
				{
					// Edge direction is not consistent with the respective quad
					if ( !bFlipFixed )
					{
						if ( bAllowChangeDirection )
						{
							// Flipping has not been fixed; invert it, and fix the flipping
							bFlip = !bFlip;
							bFlipFixed = true;
						}
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return true;
				}
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		// No existing edge; ensure that there is not common face
		return v0->findCommonFaceWith( v1 ) == NULL;
	}
}







/****************************************************************************
 ****************************************************************************
						MDrawFaceState
 ****************************************************************************
 ****************************************************************************/

MDrawFaceState::MDrawFaceState() : numVertices( 0 ), bFlip( false ), bFlipFixed( false )
{
}



int MDrawFaceState::getNumVertices() const
{
	return numVertices;
}

const Point3 & MDrawFaceState::getFaceVertexPosition(int n) const
{
	return vertexPositions[n];
}



MDrawFaceVertexCheckResult MDrawFaceState::checkVertex(MVertex *v)
{
	return verifyVertex( numVertices, v, false, true );
}


void MDrawFaceState::drawFace(MMesh *mesh, const MDrawFacePointList &points, bool bFlipRequested)
{
	// Initialise member variables
	numVertices = points.size();
	bFlip = bFlipRequested;
	bFlipFixed = false;

	// Initialise vertex arrays
	existingVertices.clear();
	faceVertices.clear();
	vertexPositions.clear();
	existingVerticesUsed.clear();
	existingVertices.fill( NULL, numVertices );
	faceVertices.fill( NULL, numVertices );
	vertexPositions.resize( numVertices );


	// Process points
	for (int pointI = 0; pointI < points.size(); pointI++)
	{
		const MDrawFacePoint &point = points[pointI];

		// Generate a vertex and add it
		if ( point.target == MDrawFacePoint::EXISTINGVERTEX )
		{
			bool bUseV = false;

			MVertex *v = mesh->pickVertex( point.pick );

			if ( v != NULL )
			{
				bUseV = verifyVertex( pointI, v, true, pointI == points.size() - 1 )  ==  MDRAWFACEVERTEXCHECKRESULT_GOOD;
			}

			if ( bUseV )
			{
				faceVertices[pointI] = v;
				existingVertices[pointI] = v;
				existingVerticesUsed.insert( v );
				vertexPositions[pointI] = v->getPosition();
			}
			else
			{
				if ( numVertices >= 3 )
				{
					faceVertices[pointI] = mesh->addVertex( point.position );
				}
				vertexPositions[pointI] = point.position;
			}
		}
		else if ( point.target == MDrawFacePoint::NEWPOINT )
		{
			if ( numVertices >= 3 )
			{
				faceVertices[pointI] = mesh->addVertex( point.position );
			}
			vertexPositions[pointI] = point.position;
		}
		else
		{
			gs_assert_not_reached( "MDrawFaceState::drawFace(): target is invalid\n" );
		}
	}



	if ( numVertices >= 3 )
	{
		bool bCanAddFace = true;
		MEdge *e = NULL;

		for (int vertexI = 0; vertexI < faceVertices.size(); vertexI++)
		{
			if ( faceVertices[vertexI] == NULL )
			{
				bCanAddFace = false;
				break;
			}
		}

		if ( bCanAddFace )
		{
			// Check all edges to ensure that we would not over-populate any of them
			int vertexIPrev = faceVertices.size() - 1;
			for (int vertexI = 0; vertexI < faceVertices.size(); vertexI++)
			{
				if ( faceVertices[vertexIPrev]->getFaceCapacityForEdgeTo( faceVertices[vertexI], e )  <  1 )
				{
					bCanAddFace = false;
					break;
				}
			}
		}


		if ( bCanAddFace )
		{
			// Create the face
			mesh->addFaceAutoTexture( faceVertices, bFlip, -1 );
		}
	}


	for (int vertI = 0; vertI < faceVertices.size(); vertI++)
	{
		MVertex *v = faceVertices[vertI];
		if ( v != NULL )
		{
			if ( v->destroyIfIsolated() )
			{
				faceVertices[vertI] = NULL;
			}
		}
	}
}



MDrawFaceVertexCheckResult MDrawFaceState::verifyVertex(int vertexIndex, MVertex *v, bool bAllowChangeDirection, bool bCheckNextVertex)
{
	// Cannot use a vertex more than once
	if ( existingVerticesUsed.find( v )  !=  existingVerticesUsed.end() )
	{
		return MDRAWFACEVERTEXCHECKRESULT_INVALID;
	}
	if ( existingVertices.contains( v ) )
	{
 		return MDRAWFACEVERTEXCHECKRESULT_INVALID;
	}

	int vPrevIndex = vertexIndex == 0  ?  -1  :  vertexIndex - 1;
	int vNextIndex = numVertices < 2  ?  -1  :  0;

	// Get the surrounding vertices
	MVertex *vPrev = NULL, *vNext = NULL;

	if ( vPrevIndex != -1 )
	{
		vPrev = existingVertices[vPrevIndex];
	}
	if ( vNextIndex != -1  &&  bCheckNextVertex )
	{
		vNext = existingVertices[vNextIndex];
	}


	if ( vPrev != NULL )
	{
		if ( !verifyEdge( vPrev, v, bAllowChangeDirection ) )
		{
			return MDRAWFACEVERTEXCHECKRESULT_INVALID;
		}
	}

	if ( vNext != NULL  &&  bCheckNextVertex )
	{
		if ( !verifyEdge( v, vNext, bAllowChangeDirection ) )
		{
			return MDRAWFACEVERTEXCHECKRESULT_CLOSING_EDGE_INVALID;
		}
	}

	return MDRAWFACEVERTEXCHECKRESULT_GOOD;
}



bool MDrawFaceState::verifyEdge(MVertex *v0, MVertex *v1, bool bAllowChangeDirection)
{
	// @v0 and @v1 are in the order in which they would be used in a prospective face

	// Check the face capacity
	MEdge *e = v0->findEdgeTo( v1, MFINDPOLICY_RETURNNULL );
	if ( e != NULL )
	{
		if ( e->getNumFaces() == 0 )
		{
			return true;
		}
		else	if ( e->getNumFaces() == 1 )
		{
			// Check the direction
			bool bEdgeDirection = true;
			if ( e->getVertexA() == v1  &&  e->getVertexB() == v0 )
			{
				// @v0 and @v1 go in the same direction as @e
				bEdgeDirection = true;
			}
			else if ( e->getVertexA() == v0  &&  e->getVertexB() == v1 )
			{
				// @v0 and @v1 go in the opposite direction to @e
				bEdgeDirection = false;
			}
			else
			{
				gs_assert_not_reached( "MDrawFaceState::verifyEdge(): both @v0 and @v1 are not incident to @e\n" );
			}


			if ( bFlip )
			{
				// Flipped
				if ( bEdgeDirection )
				{
					// Edge direction is consistent with the respective quad, this is a problem since flip is enabled
					if ( !bFlipFixed )
					{
						if ( bAllowChangeDirection )
						{
							// Flipping has not been fixed; invert it, and fix the flipping
							bFlip = !bFlip;
							bFlipFixed = true;
						}
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return true;
				}
			}
			else
			{
				// Not flipped
				if ( !bEdgeDirection )
				{
					// Edge direction is not consistent with the respective quad
					if ( !bFlipFixed )
					{
						if ( bAllowChangeDirection )
						{
							// Flipping has not been fixed; invert it, and fix the flipping
							bFlip = !bFlip;
							bFlipFixed = true;
						}
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return true;
				}
			}
		}
		else
		{
			// Got 2 faces already; cannot add another
			return false;
		}
	}
	else
	{
		// No existing edge; ensure that there is not common face
		return v0->findCommonFaceWith( v1 ) == NULL;
	}
}







/****************************************************************************
 ****************************************************************************
						MProportionalAdjuster
 ****************************************************************************
 ****************************************************************************/

MProportionalAdjuster::MProportionalAdjuster()
{
}

MProportionalAdjuster::~MProportionalAdjuster()
{
}



void MProportionalAdjuster::save()
{
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		vertexWeightPairs[vertexI].first->savePosition();
	}
}

void MProportionalAdjuster::restore()
{
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		vertexWeightPairs[vertexI].first->restorePosition();
	}
}


void MProportionalAdjuster::move(const Vector3 &translation)
{
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		MVertex *v = vertexWeightPairs[vertexI].first;
		v->setPosition( v->getPosition()  +  translation * vertexWeightPairs[vertexI].second );
	}
}

void MProportionalAdjuster::moveNormal(double distance)
{
	moveNormalTranslations.resize( vertexWeightPairs.size() );
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		moveNormalTranslations[vertexI] = vertexWeightPairs[vertexI].first->computeAverageNormal()  *  distance  *  vertexWeightPairs[vertexI].second;
	}

	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		MVertex *v = vertexWeightPairs[vertexI].first;
		v->setPosition( v->getPosition()  +  moveNormalTranslations[vertexI] );
	}
}

void MProportionalAdjuster::scale(const Point3 &centre, double scaleFactor)
{
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		MVertex *v = vertexWeightPairs[vertexI].first;
		const Point3 &p = v->getPosition();
		double weightedScaleFactor = lerp( 1.0, scaleFactor, vertexWeightPairs[vertexI].second );
		v->setPosition( centre  +  ( ( p - centre )  *  weightedScaleFactor ) );
	}
}

void MProportionalAdjuster::rotate(const Point3 &centre, const Vector3 &rotationAxis, double rotation)
{
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		MVertex *v = vertexWeightPairs[vertexI].first;
		double w = vertexWeightPairs[vertexI].second;
		const Point3 &p = v->getPosition();
		Matrix4 m = Matrix4::rotate( rotationAxis, rotation * w );
		v->setPosition( centre  +  ( ( p - centre )  *  m ) );
	}
}

void MProportionalAdjuster::relax(double relaxAmount)
{
	// Initialise @relaxPositionWeightPairs
	relaxPositionWeightPairs.resize( vertexWeightPairs.size() );
	for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
	{
		relaxPositionWeightPairs[vertexI] = std::pair<Point3, double>( vertexWeightPairs[vertexI].first->computeRelaxedPosition(), relaxAmount * vertexWeightPairs[vertexI].second );
	}


	bool bWorkRemains = true;
	while ( bWorkRemains )
	{
		bWorkRemains = false;
		for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
		{
			// Get the weight
			double w = relaxPositionWeightPairs[vertexI].second;
			if ( w > 0.0 )
			{
				double relaxWeight = clamp( w, 0.0, 1.0 );
				MVertex *v = vertexWeightPairs[vertexI].first;
				v->setPosition( lerp( v->getPosition(), relaxPositionWeightPairs[vertexI].first, relaxWeight ) );
				w -= 1.0;
				relaxPositionWeightPairs[vertexI].second = w;
				if ( w > 0.0 )
				{
					bWorkRemains = true;
				}
			}
		}

		if ( bWorkRemains )
		{
			for (int vertexI = 0; vertexI < vertexWeightPairs.size(); vertexI++)
			{
				if ( relaxPositionWeightPairs[vertexI].second > 0.0 )
				{
					relaxPositionWeightPairs[vertexI].first = vertexWeightPairs[vertexI].first->computeRelaxedPosition();
				}
			}
		}
	}

}





#endif
