//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MFACE_CPP__
#define MFACE_CPP__

#include <list>
#include <algorithm>
#include <utility>

#include <Math/index.h>
#include <Math/Segment2.h>
#include <Math/PlanarEquation.h>

#include <CompGeometry/PolyTesselate.h>

#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>





/***************************************************************
							MFace
 ***************************************************************/


struct MFace::StoredCentre
{
	Point3 centre;
	MVertexAttrib attribCentre;
};



Pool<MFace> MFace::facePool;



void MFace::faceModified()
{
	// Face has been modified
	// Plane needs to be refreshed
	if ( !flags.planeRequiresRefresh )
	{
		// Notify all vertices that the face plane is now out of date
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].vertex->onFacePlaneModified();
		}

		flags.planeRequiresRefresh = true;
	}

	// This *MUST* be done irrespective of @flags.planeRequiresRefresh
	getMesh()->faceModified( this );
}




MFace::MFace(MMesh *m, int ndx)
			: materialID( -1 ), tesselation( NULL ), mesh( m ), index( ndx )
{
	vertices.clear();

	flags.faceMark = false;
	flags.destroyed = false;
	flags.secondaryMark = false;
	flags.planeRequiresRefresh = true;
	flags.incidentVerticesTransformed = false;
	flags.tesselationRequiresRefresh = true;
	flags.liveSubdVertexRequiresRefresh = false;

	faceOpData.insetFace.centre = NULL;
	faceOpData.insetFace.plane = NULL;
}

MFace::~MFace()
{
	destroyTesselation();
}



void * MFace::operator new(size_t sz)
{
	return static_cast<void *>( facePool.allocateRaw() );
}

void MFace::operator delete(void *data)
{
	facePool.freeRaw( static_cast<MFace *>( data ) );
}

void MFace::poolAllocReserve(int numItems)
{
	facePool.reserve( numItems );
}



void MFace::init(const Array<MVertex*> &vts, const Array<MVertexAttrib> &attribs, int matID)
{
	int i = vts.size() - 1;
	MVertex *vertexI = vts[i];

	flags.planeRequiresRefresh = true;

	vertices.resize( vts.size() );

	for (int j = 0; j < vts.size(); j++)
	{
		Vertex &v = vertices[i];
		MVertex *vertexJ = vts[j];

		v.vertex = vertexI;
		v.edge = vertexI->findEdgeTo( vertexJ, MFINDPOLICY_CREATE );
		v.edge->addFace( this, vertexI, i );
		v.attrib = vertexI->createVertexAttrib( attribs[i], MFINDPOLICY_CREATE );
		v.attrib->ref();
		v.normal = NULL;

		vertexI->addFace( this );


		i = j;
		vertexI = vertexJ;
	}

	changeMaterialID( matID );
}

void MFace::init(const Array<MVertex*> &vts, const Array<MVertexAttrib*> &attribs, int matID)
{
	int i = vts.size() - 1;
	MVertex *vertexI = vts[i];

	flags.planeRequiresRefresh = true;

	vertices.resize( vts.size() );

	for (int j = 0; j < vts.size(); j++)
	{
		Vertex &v = vertices[i];
		MVertex *vertexJ = vts[j];

		v.vertex = vertexI;
		v.edge = vertexI->findEdgeTo( vertexJ, MFINDPOLICY_CREATE );
		v.edge->addFace( this, vertexI, i );

		gs_assert( attribs[i]->getVertex() == vertexI, "MFace::init(): the vertex that owns the attribute does not match the corresponding vertex in the list\n" );

		v.attrib = attribs[i];
		v.attrib->ref();
		v.normal = NULL;

		vertexI->addFace( this );


		i = j;
		vertexI = vertexJ;
	}

	changeMaterialID( matID );
}

void MFace::init(const Array<MVertex*> &vts, const Array<MEdge*> &eds, const Array<MVertexAttrib*> &attribs, int matID)
{
	flags.planeRequiresRefresh = true;

	vertices.resize( vts.size() );

	for (int i = 0; i < vts.size(); i++)
	{
		MVertex *vertexI = vts[i];
		Vertex &v = vertices[i];

		v.vertex = vertexI;
		v.edge = eds[i];
		v.edge->addFace( this, vertexI, i );

		gs_assert( attribs[i]->getVertex() == vertexI, "MFace::init(): the vertex that owns the attribute does not match the corresponding vertex in the list\n" );

		v.attrib = attribs[i];
		v.attrib->ref();
		v.normal = NULL;

		vertexI->addFace( this );
	}

	changeMaterialID( matID );
}


void MFace::copyFrom(const MFace *face, MMesh *parentMesh, int verticesOffset, int edgesOffset)
{
	//duplicate the vertex structure
	vertices.resize( face->vertices.size() );

	for (int i = 0; i < face->vertices.size(); i++)
	{
		Vertex &v = vertices[i];
		const Vertex &src = face->vertices[i];

		//get pointer to the vertex and the edge
		v.vertex = parentMesh->vertices[ src.vertex->index + verticesOffset ];
		v.edge = parentMesh->edges[ src.edge->index + edgesOffset ];


		//get attrib pointer
		v.attrib = v.vertex->getVertexAttrib( src.attrib->index );

		gs_assert( v.attrib->getVertex() == v.vertex, "MFace::copyFrom(): the vertex that owns the attribute does not match the corresponding vertex in the list; v.attrib->getVertex()=%p, v.vertex=%p\n", v.attrib->getVertex(), v.vertex );

		//ref the attrib
		v.attrib->ref();


		//pointer to normal
		MVertexNormal *normal = NULL;

		if ( !face->flags.planeRequiresRefresh  &&  !src.vertex->flags.normalsRequireRefresh )
		{
			if ( src.normal != NULL )
			{
				normal = v.vertex->_getVertexNormalNoRefresh( src.normal->index );
			}
		}

		v.normal = normal;
	}

	//copy over the plane, material ID, and the flags
	plane = face->plane;
	changeMaterialID( face->materialID );
	flags = face->flags;

	if ( flags.faceMark )
	{
		getMesh()->incrementMarkedFaceCount();
	}

	faceOpData.insetFace.centre = NULL;
	faceOpData.insetFace.plane = NULL;

	destroyTesselation();
	if ( face->tesselation != NULL )
	{
		setTesselation( *( face->tesselation ) );
	}
	flags.tesselationRequiresRefresh = face->flags.tesselationRequiresRefresh;
}

void MFace::read(FILE *f, MMesh *parentMesh)
{
	int numVertices;
	fread( &numVertices, sizeof(int), 1, f );

	//read the vertices
	vertices.resize( numVertices );
	for (int i = 0; i < numVertices; i++)
	{
		Vertex &v = vertices[i];
		int vertexIndex, edgeIndex, attribIndex, normalIndex;

		fread( &vertexIndex, sizeof(int), 1, f );
		fread( &edgeIndex, sizeof(int), 1, f );
		fread( &attribIndex, sizeof(int), 1, f );
		fread( &normalIndex, sizeof(int), 1, f );

		v.vertex = parentMesh->vertices[vertexIndex];
		v.edge = parentMesh->edges[edgeIndex];
		v.edge->setFaceIndex( this, i );

		//get attrib pointer
		v.attrib = v.vertex->getVertexAttrib( attribIndex );

		gs_assert( v.attrib->getVertex() == v.vertex, "MFace::read(): the vertex that owns the attribute does not match the corresponding vertex in the list\n" );

		//ref the attrib
		v.attrib->ref();



		//pointer to normal
		MVertexNormal *normal;

		//get normal pointer
		if ( normalIndex != -1 )
		{
			normal = v.vertex->getVertexNormal( normalIndex );
		}
		else
		{
			normal = NULL;
		}
		v.normal = normal;
	}

	//read the plane
	plane.read( f );


	//read the tesselation
	bool tesselationPresent;
	fread( &tesselationPresent, sizeof(bool), 1, f );
	if ( tesselationPresent )
	{
		tesselation = new Tesselation();
		tesselation->read( f );
		flags.tesselationRequiresRefresh = false;
	}
	else
	{
		tesselation = NULL;
	}

	//read the material ID, and the flags
	int mat;
	fread( &mat, sizeof(int), 1, f );
	changeMaterialID( mat );
	fread( &flags, sizeof(flags), 1, f );

	if ( flags.faceMark )
	{
		getMesh()->incrementMarkedFaceCount();
	}

	faceOpData.insetFace.centre = NULL;
	faceOpData.insetFace.plane = NULL;
}

void MFace::write(FILE *f) const
{
	int numVertices = vertices.size();
	fwrite( &numVertices, sizeof(int), 1, f );

	for (int i = 0; i < vertices.size(); i++)
	{
		int indices[4];
		const Vertex &v = vertices[i];

		indices[0] = v.vertex->index;
		indices[1] = v.edge->index;
		indices[2] = v.attrib->index;
		if ( v.normal != NULL )
		{
			indices[3] = v.normal->index;
		}
		else
		{
			indices[3] = -1;
		}

		fwrite( indices, sizeof(int), 4, f );
	}

	//write the plane
	plane.write( f );

	//write the tesselation
	bool tesselationPresent = tesselation != NULL;
	fwrite( &tesselationPresent, sizeof(bool), 1, f );
	if ( tesselationPresent )
	{
		tesselation->write( f );
	}

	//write the material ID, and the flags
	fwrite( &materialID, sizeof(int), 1, f );
	fwrite( &flags, sizeof(flags), 1, f );
}


void MFace::read(SENode &node, MMesh *parentMesh)
{
	std::string header;
	node[0]  >>  header;

	if ( header == "f" )
	{
		//read the vertices
		SENode &vertsNode = node[1];
		int numVertices = 0;
		vertsNode[0]  >>  numVertices;
		vertices.resize( numVertices );
		int faceVertexIndex = 0;
		for (int i = 0; i < numVertices; i++)
		{
			SENode &vertexNode = vertsNode[i+2];

			Vertex &v = vertices[faceVertexIndex];
			int vertexIndex = -1, edgeIndex = -1, attribIndex = -1;

			vertexNode[0]  >>  vertexIndex;
			vertexNode[1]  >>  edgeIndex;
			vertexNode[2]  >>  attribIndex;

			if ( vertexIndex != -1  &&  edgeIndex != -1  &&  attribIndex != -1 )
			{
				v.vertex = parentMesh->vertices[vertexIndex];
				v.edge = parentMesh->edges[edgeIndex];
				v.edge->setFaceIndex( this, i );

				//get attrib pointer
				v.attrib = v.vertex->getVertexAttrib( attribIndex );

				gs_assert( v.attrib->getVertex() == v.vertex, "MFace::read(): the vertex that owns the attribute does not match the corresponding vertex in the list\n" );

				//ref the attrib
				v.attrib->ref();

				v.normal = NULL;

				faceVertexIndex++;
			}
		}


		//read the material ID, and the flags
		int mat = -1;
		node[2]  >>  mat;
		changeMaterialID( mat );

		bool bMarked = false;
		node[3]  >>  bMarked;
		setFaceMark( bMarked );
	}
}

void MFace::write(SENode &node) const
{
	node.addItem()  <<  "f";

	SENode &vertsNode = node.addList();
	vertsNode.addItem()  <<  vertices.size();
	vertsNode.addItem()  <<  ":";
	for (int i = 0; i < vertices.size(); i++)
	{
		SENode &vertNode = vertsNode.addList();
		const Vertex &v = vertices[i];

		vertNode.addItem()  <<  v.vertex->index;
		vertNode.addItem()  <<  v.edge->index;
		vertNode.addItem()  <<  v.attrib->index;
	}

	//write the material ID, and the flags
	node.addItem()  <<  getMaterialID();
	node.addItem()  <<  isFaceMarked();
}



void MFace::preComputeFull()
{
	// Refresh plane
	getPlane();

	// Refresh tesselation
	refreshTesselation();
}



MMesh * MFace::getMesh()
{
	return mesh;
}






// ******************* VERTEX OPS *******************

void MFace::getFacePoints(Array<Point3> &facePoints) const
{
	facePoints.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		facePoints.push_back( vertices[i].vertex->getPosition() );
	}
}

void MFace::getFaceNormals(Array<Vector3f> &faceNormals) const
{
	faceNormals.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		faceNormals.push_back( vertices[i].normal->getNormal() );
	}
}


MVertex* MFace::findClosestVertexTo(const Point3 &point)
{
	MVertex *bestVertex = vertices[0].vertex;
	double bestDistanceSquared = point.sqrDistanceTo( vertices[0].vertex->getPosition() );

	for (int i = 1; i < vertices.size(); i++)
	{
		double distSquared = point.sqrDistanceTo( vertices[i].vertex->getPosition() );

		if ( distSquared < bestDistanceSquared )
		{
			bestDistanceSquared = distSquared;
			bestVertex = vertices[i].vertex;
		}
	}

	return bestVertex;
}

MVertex* MFace::getFurthestVertexFromPlane(const Plane &p)
{
	double bestDistance = 0.0;
	MVertex *bestVertex = vertices[0].vertex;

	for (int i = 0; i < vertices.size(); i++)
	{
		double d = fabs( p.distanceTo( vertices[i].vertex->getPosition() ) );

		if ( d > bestDistance )
		{
			bestDistance = d;
			bestVertex = vertices[i].vertex;
		}
	}

	return bestVertex;
}



void MFace::replaceVertex(int vtxIndex, MVertex *vtxNew, MVertexAttrib *attrNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties)
{
	MVertex *vtxOld = vertices[vtxIndex].vertex;
	if ( vtxOld == vtxNew )
	{
		//replacing vOld with itself; nothing to do
		return;
	}

	//see if vNew is already used by this face
	int vNewPosition = findVertex( vtxNew );		//get the position

	//if the vertex is already present in this, simply remove vOld
	if ( vNewPosition != -1 )
	{
		removeVertex( vtxIndex, removeFaceFromVOld, false );
		return;
	}


	//get the previous and next vertices in the face
	int prev = prevIndex( vtxIndex, vertices.size() );
	int next = nextIndex( vtxIndex, vertices.size() );

	MVertex *va = vertices[prev].vertex;
	MVertex *vb = vtxNew;
	MVertex *vc = vertices[next].vertex;

	//get the two edges surrounding vOld
	MEdge *oldEdgeA = vertices[prev].edge;
	MEdge *oldEdgeB = vertices[vtxIndex].edge;

	//replace these edges with new edges linking to vNew rather than vOld
	vertices[prev].edge = va->findEdgeTo( vb, MFINDPOLICY_CREATE );
	vertices[vtxIndex].edge = vb->findEdgeTo( vc, MFINDPOLICY_CREATE );

	//keep edge properties if desired
	if ( keepEdgeProperties )
	{
		vertices[prev].edge->copyPropertiesFrom( oldEdgeA );
		vertices[vtxIndex].edge->copyPropertiesFrom( oldEdgeB );
	}

	//remove this face from the old edges
	oldEdgeA->removeFace( this, true, destroyIsolatedVertices );
	oldEdgeB->removeFace( this, true, destroyIsolatedVertices );

	//add this face to the new edges
	vertices[prev].edge->addFace( this, va, prev );
	vertices[vtxIndex].edge->addFace( this, vb, vtxIndex );

	//replace the vertex
	vertices[vtxIndex].vertex = vtxNew;
	//ref() the replacement attrib
	attrNew->ref();
	//unref() the old attrib
	vertices[vtxIndex].attrib->unref();
	//replace
	vertices[vtxIndex].attrib = attrNew;


	//add this face to vNew
	vtxNew->addFace( this );

	//if necessary, remove this face from vOld
	if ( removeFaceFromVOld )
	{
		vtxOld->removeFace( this );
	}

	//face has been modified
	faceModified();
	invalidateTesselation();
}

void MFace::replaceVertex(MVertex *vtxOld, MVertex *vtxNew, MVertexAttrib *attrNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties)
{
	int vtxIndex = findVertex( vtxOld );

	gs_assert( vtxIndex != -1, "MFace::replaceVertex(): vertex @vtxOld not found\n" );

	replaceVertex( vtxIndex, vtxNew, attrNew, removeFaceFromVOld, destroyIsolatedVertices, keepEdgeProperties );
}

void MFace::replaceVertex(int vtxIndex, MVertex *vtxNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties)
{
	MVertexAttrib *replacementAttrib = vtxNew->createVertexAttrib( *vertices[vtxIndex].attrib, MFINDPOLICY_CREATE );

	replaceVertex( vtxIndex, vtxNew, replacementAttrib, removeFaceFromVOld, destroyIsolatedVertices, keepEdgeProperties );

	//ref and unref it; this ensures that in case it wasn't used above, it gets
	//destroyed
	replacementAttrib->ref();
	replacementAttrib->unref();
}

void MFace::replaceVertex(MVertex *vtxOld, MVertex *vNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties)
{
	int vtxIndex = findVertex( vtxOld );

	gs_assert( vtxIndex != -1, "MFace::replaceVertex(): vertex @vtxOld not found\n" );

	replaceVertex( vtxIndex, vNew, removeFaceFromVOld, destroyIsolatedVertices, keepEdgeProperties );
}

void MFace::removeVertex(int vtxIndex, bool removeFaceFromV, bool destroyIsolatedVertices)
{
	//if this is a triangle, kill it.
	if ( getSize() == 3 )
	{
		destroy( true, true );
		return;
	}

	MVertex *v = vertices[vtxIndex].vertex;

	/*
		 prevVertex		 v			nextVertex
	------*------------*------------*------
				edgeA				edgeB
	becomes:

		 prevVertex		 			nextVertex
	------*-------------------------*------
						newEdge
	*/


	//get previous and next vertices
	int prev = prevIndex( vtxIndex, vertices.size() );
	int next = nextIndex( vtxIndex, vertices.size() );


	MVertex *prevVertex = vertices[prev].vertex;
	MVertex *nextVertex = vertices[next].vertex;

	//get edges surrounding v
	MEdge *oldEdgeA = vertices[prev].edge;
	MEdge *oldEdgeB = vertices[vtxIndex].edge;

	//if necessary, remove this face from v
	if ( removeFaceFromV )
	{
		v->removeFace( this );
	}
	//remove this face from both edges
	oldEdgeA->removeFace( this, true, destroyIsolatedVertices );
	oldEdgeB->removeFace( this, true, destroyIsolatedVertices );

	//edge to link the previous vertex to the next vertex
	MEdge *newEdge = prevVertex->findEdgeTo( nextVertex, MFINDPOLICY_CREATE );
	//add this face to it
	newEdge->addFace( this, prevVertex, prev );

	//put the new edge in the list
	vertices[prev].edge = newEdge;

	//remove v (need to unref() the attrib first)
	vertices[vtxIndex].attrib->unref();
	vertices.remove( vtxIndex );

	refreshEdgeIndices( vtxIndex );

	//face has been modified
	faceModified();
	invalidateTesselation();
}

void MFace::removeVertex(MVertex *v, bool removeFaceFromV, bool destroyIsolatedVertices)
{
	int vtxIndex = findVertex( v );

	gs_assert( vtxIndex != -1, "MFace::removeVertex(): vertex @v not found\n" );

	removeVertex( vtxIndex, removeFaceFromV, destroyIsolatedVertices );
}


bool MFace::checkRemoveVertex(int vtxIndex)
{
	//if this is a triangle, then it will be destroyed, so no conflict can occur
	if ( vertices.size() == 3 )
	{
		return true;
	}

	MVertex *prevVertex = vertices[ prevIndex( vtxIndex, vertices.size() ) ].vertex;
	MVertex *nextVertex = vertices[ nextIndex( vtxIndex, vertices.size() ) ].vertex;

	MEdge *newEdge = prevVertex->findEdgeTo( nextVertex, MFINDPOLICY_RETURNNULL );

	if ( newEdge != NULL )
	{
		return newEdge->getNumFaces() != 2;
	}

	return true;
}

bool MFace::checkRemoveVertex(const MVertex *v)
{
	return checkRemoveVertex( findVertex( v ) );
}



void MFace::insertVertex(int position, MVertex *newVertex, MVertexAttrib *attrib)
{
	/*
			prevIndex	position		nextIndex
		 prevVertex		vtx			nextVertex
	------*------------*------------*------
				edgeA				edgeB


	becomes:

			prevIndex	position		nextIndex
		 prevVertex		newVertex	 vtx			nextVertex
	------*-------------*-----------*-------------*------
				newEdgeA		 newEdgeB		edgeB
	*/


	int prevVtxIndex = prevIndex( position, vertices.size() );


	MVertex *prevVertex = vertices[prevVtxIndex].vertex;
	MVertex *vtx = vertices[position].vertex;


	MEdge *edgeA = vertices[prevVtxIndex].edge;

	//remove this from edgeA
	edgeA->removeFace( this, true, false );


	//get the new edges
	MEdge *newEdgeA = prevVertex->findEdgeTo( newVertex, MFINDPOLICY_CREATE );
	MEdge *newEdgeB = newVertex->findEdgeTo( vtx, MFINDPOLICY_CREATE );


	//put edgeA into the list, since its position is the previous position,
	//and this could change due to the insertion operation
	//(if prevIndex == size() - 1, and size() changes, then so does prevIndex:
	//use it while it is still valid)
	vertices[prevVtxIndex].edge = newEdgeA;


	//make space in vertex and edge lists
	vertices.insert( position, Vertex() );

	//put edgeB into the list
	vertices[position].edge = newEdgeB;


	//add this to the edge face lists
	newEdgeA->addFace( this, prevVertex, prevVtxIndex );
	newEdgeB->addFace( this, newVertex, position );

	//put the vertex into the vertex list, and add this to the vertex's face
	//list
	vertices[position].vertex = newVertex;
	vertices[position].attrib = attrib;
	vertices[position].normal = NULL;
	attrib->ref();
	newVertex->addFace( this );

	refreshEdgeIndices( position + 1 );

	//face has been modified
	faceModified();
	invalidateTesselation();
}

void MFace::insertVertex(MVertex *position, MVertex *newVertex, MVertexAttrib *attrib)
{
	int index = findVertex( position );

	gs_assert( index != -1, "MFace::insertVertex(): vertex @position not found\n" );

	insertVertex( index, newVertex, attrib );
}


int MFace::getSharedVertexCount(const MFace *f) const
{
	int count = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < f->vertices.size(); j++)
		{
			if ( vertices[i].vertex == f->vertices[j].vertex )
			{
				count++;
			}
		}
	}

	return count;
}





// ******************* VERTEX ATTRIBUTE OPS *******************

void MFace::setVertexAttrib(int index, MVertexAttrib *newAttrib)
{
	vertices[index].attrib = newAttrib;
}

void MFace::setVertexAttrib(MVertexAttrib *oldAttrib, MVertexAttrib *newAttrib)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].attrib == oldAttrib )
		{
			setVertexAttrib( i, newAttrib );
		}
	}
}





// ******************* VERTEX NORMAL OPS *******************

MVertexNormal * MFace::getVertexNormal(int i)
{
	vertices[i].vertex->getNumVertexNormals();
	gs_assert( vertices[i].normal != NULL, "MFace::getVertexNormal(): face %d, vertex %d(%d) normal is NULL\n", index, i, vertices[i].vertex->getIndex() );
	return vertices[i].normal;
}

const MVertexNormal * MFace::getVertexNormal(int i) const
{
	vertices[i].vertex->getNumVertexNormals();
	gs_assert( vertices[i].normal != NULL, "MFace::getVertexNormal(): face %d, vertex %d(%d) normal is NULL\n", index, i, vertices[i].vertex->getIndex() );
	return vertices[i].normal;
}


void MFace::refreshVertexNormals()
{
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI].vertex->refreshVertexNormals();
	}
}

void MFace::refreshVertexNormalsForVertex(int index)
{
	vertices[index].vertex->refreshVertexNormals();
}



// ******************* EDGE OPS *******************

void MFace::getAdjacentEdges(const MEdge *e, MEdge*& prevEdge, MEdge*& nextEdge)
{
	int edgeIndex = e->getFaceIndex( this );

	gs_assert( edgeIndex != -1, "MFace::getAdjacentEdges(): edge @e is not incident to @this\n" );

	int prevEdgeIndex = prevIndex( edgeIndex, vertices.size() );
	int nextEdgeIndex = nextIndex( edgeIndex, vertices.size() );

	prevEdge = vertices[prevEdgeIndex].edge;
	nextEdge = vertices[nextEdgeIndex].edge;
}


bool MFace::isEdgeInOrder(const MEdge *e, int index) const
{
	const MVertex *vb = e->getVertexB();

	return vertices[ nextIndex( index, vertices.size() ) ].vertex == vb;
}

bool MFace::isEdgeInOrder(const MEdge *e) const
{
	int eIndex = e->getFaceIndex( this );

	gs_assert( eIndex != -1, "MFace::isEdgeInOrder(): @e is not incident o @this\n" );

	return isEdgeInOrder( e, eIndex );
}

bool MFace::isEdgeInOrder_search(const MEdge *e) const
{
	const MVertex *va = e->getVertexA();

	int indexA = findVertex( va );

	gs_assert( indexA != -1, "MFace::isEdgeInOrder_search(): @e is not incident o @this\n" );

	return isEdgeInOrder( e, indexA );
}

MEdge * MFace::getNextEdge(const MEdge *currentEdge, const MVertex *pivotVertex)
{
	int edgeIndex = currentEdge->getFaceIndex( this );

	gs_assert( edgeIndex != -1, "MFace::getNextEdge(): could not find edge in this face\n" );

	int prev = prevIndex( edgeIndex, vertices.size() );
	int next = nextIndex( edgeIndex, vertices.size() );

	const MVertex *vtx = vertices[edgeIndex].vertex;
	const MVertex *nextVtx = vertices[next].vertex;

	if ( pivotVertex == vtx )
	{
		return vertices[prev].edge;
	}
	else if ( pivotVertex == nextVtx )
	{
		return vertices[next].edge;
	}
	else
	{
		gs_assert_not_reached( "MFace::getNextEdge(): could not find pivot vertex in this face\n" );
		return NULL;
	}
}

MEdge * MFace::getFaceMarkBoundaryEdge()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].edge->isFaceMarkBoundary() )
		{
			return vertices[i].edge;
		}
	}

	return NULL;
}


void MFace::refreshEdgeIndices(int start)
{
	for (int i = start; i < vertices.size(); i++)
	{
		vertices[i].edge->setFaceIndex( this, i );
	}
}


MEdge * MFace::findClosestEdgeTo(const Point3 &point)
{
	MEdge *bestEdge = vertices[0].edge;
	double bestDistanceSquared = vertices[0].edge->getSegment().sqrDistanceTo( point );

	for (int i = 1; i < vertices.size(); i++)
	{
		double distSquared = vertices[i].edge->getSegment().sqrDistanceTo( point );

		if ( distSquared < bestDistanceSquared )
		{
			bestDistanceSquared = distSquared;
			bestEdge = vertices[i].edge;
		}
	}

	return bestEdge;
}



MEdge * MFace::quadGetOppositeEdge(MEdge *e)
{
	if ( isQuad() )
	{
		int eIndex = e->getFaceIndex( this );

		//% 4  instead of % vertices.size(),  as vertices.size() == 4
		return vertices[ ( eIndex + 2 ) % 4 ].edge;
	}
	else
	{
		return NULL;
	}
}



//the edges that are returned, are in the same order as they appear
//in @this
void MFace::findCommonEdges(const MFace *f, MEdgeList &commonEdges)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < f->vertices.size(); j++)
		{
			if ( vertices[i].edge == f->vertices[j].edge )
			{
				commonEdges.push_back( vertices[i].edge );
			}
		}
	}
}

//the edge indices that are returned, are in increasing order,
//and index into the edge list in @this
void MFace::findCommonEdgeIndices(const MFace *f, Array<int> &commonEdgeIndices) const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < f->vertices.size(); j++)
		{
			if ( vertices[i].edge == f->vertices[j].edge )
			{
				commonEdgeIndices.push_back( i );
			}
		}
	}
}

int MFace::getSharedEdgeCount(MFace *withFace) const
{
	int count = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < withFace->vertices.size(); j++)
		{
			if ( vertices[i].edge  ==  withFace->vertices[j].edge )
			{
				count++;
			}
		}
	}

	return count;
}


bool MFace::isEdgeIndexListContinuous(Array<int> &edgeIndices) const
{
	//Check if the indices in @edgeIndices are sequential, in order,
	//and adjacent to one another.
	//Note that the indices in @edgeIndices may loop over;
	//e.g.: If @this has 10 edges, and the contents of @edgeIndices are
	//0,1,8,9 then this set of indices is continuous when rotated to:
	//8,9,0,1
	//
	//The break in the loop (0,1_,_8,9) will be detected as a
	//discontinuity; find it if it exists
	int discontinuityPosition = -1;
	for (int i = 1; i < edgeIndices.size(); i++)
	{
		int prevIndex = edgeIndices[ i - 1 ];
		int expectedIndex = nextIndex( prevIndex, vertices.size() );
		int index = edgeIndices[i];
		if ( expectedIndex != index )
		{
			discontinuityPosition = i;
			break;
		}
	}

	if ( discontinuityPosition== -1 )
	{
		//no discontinuity; continuous
		return true;
	}
	else
	{
		//a discontinuity was found.
		//scan from that position onwards
		//
		int j = discontinuityPosition;
		int prevIndex = edgeIndices[j];
		for (int i = 1; i < edgeIndices.size(); i++)
		{
			j = nextIndex( j, edgeIndices.size() );
			//compute the expected index
			int expectedIndex = nextIndex( prevIndex, vertices.size() );
			int index = edgeIndices[j];
			if ( index != expectedIndex )
			{
				//discontinuous
				return false;
			}
			prevIndex = index;
		}

		//continuous, but wrapped
		//rotate so that the index at @discontinuityPosition is first
		edgeIndices.rotateForward( discontinuityPosition );

		return true;
	}
}







// ******************* DESTROY OPS *******************

void MFace::destroy(bool destroyIsolatedEdges, bool destroyIsolatedVertices)
{
	//face has been modified
	faceModified();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].vertex->removeFace( this );
		vertices[i].edge->removeFace( this, destroyIsolatedEdges, destroyIsolatedVertices );
		vertices[i].attrib->unref();
	}

	//unref the material ID
	getMesh()->unrefMaterial( materialID );

	getMesh()->destroyFace( this );
}






// ******************* FACE MARK OPS *******************

void MFace::faceMark()
{
	if ( !flags.faceMark )
	{
		getMesh()->incrementMarkedFaceCount();
	}

	flags.faceMark = true;
}

void MFace::faceUnmark()
{
	if ( flags.faceMark )
	{
		getMesh()->decrementMarkedFaceCount();
	}

	flags.faceMark = false;
}

void MFace::faceMarkInvert()
{
	if ( flags.faceMark )
	{
		getMesh()->decrementMarkedFaceCount();
	}
	else
	{
		getMesh()->incrementMarkedFaceCount();
	}

	flags.faceMark = !flags.faceMark;
}

void MFace::setFaceMark(bool mark)
{
	if ( flags.faceMark != mark )
	{
		if ( mark )
		{
			getMesh()->incrementMarkedFaceCount();
		}
		else
		{
			getMesh()->decrementMarkedFaceCount();
		}
	}

	flags.faceMark = mark;
}



bool MFace::isOnFaceMarkBoundary() const
{
	if ( isFaceMarked() )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i].edge->isFaceMarkBoundary() )
			{
				return true;
			}
		}
	}

	return false;
}

bool MFace::isFaceMarkInternal() const
{
	if ( !isFaceMarked() )
	{
		return false;
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i].edge->isFaceMarkInternal() )
		{
			return false;
		}
	}

	return true;
}


bool MFace::hasNeighbouringMarkedFaces() const
{
	for (int edgeI = 0; edgeI < vertices.size(); edgeI++)
	{
		MFace *neighbourFace = vertices[edgeI].edge->getOppositeFace( this );

		if ( neighbourFace != NULL )
		{
			if ( neighbourFace->isFaceMarked() )
			{
				return true;
			}
		}
	}

	return false;
}

bool MFace::hasNeighbouringUnmarkedFaces() const
{
	for (int edgeI = 0; edgeI < vertices.size(); edgeI++)
	{
		MFace *neighbourFace = vertices[edgeI].edge->getOppositeFace( this );

		if ( neighbourFace != NULL )
		{
			if ( !neighbourFace->isFaceMarked() )
			{
				return true;
			}
		}
	}

	return false;
}


void MFace::visitFaceMarkedRegion()
{
	//search the mesh for adjacent marked faces, recording them as we go,
	//using a bredth first search algorithm
	std::list<MFace*> faceStack;

	faceStack.push_back( this );
	secondaryMark();


	while ( faceStack.size() > 0 )
	{
		MFace *f = faceStack.front();
		faceStack.pop_front();

		for (int i = 0; i < f->vertices.size(); i++)
		{
			MFace *oppositeFace = f->vertices[i].edge->getOppositeFace( f );

			if ( oppositeFace != NULL )
			{
				if ( !oppositeFace->isSecondaryMarked()  &&  oppositeFace->isFaceMarked() )
				{
					faceStack.push_back( oppositeFace );
					oppositeFace->secondaryMark();
				}
			}
		}
	}
}

void MFace::getFaceMarkedRegion(Array<MFace*> &region)
{
	//search the mesh for adjacent marked faces, recording them as we go,
	//using a bredth first search algorithm
	region.clear();

	std::list<MFace*> faceStack;

	faceStack.push_back( this );
	secondaryMark();


	while ( faceStack.size() > 0 )
	{
		MFace *f = faceStack.front();
		faceStack.pop_front();

		region.push_back( f );

		for (int i = 0; i < f->vertices.size(); i++)
		{
			MFace *oppositeFace = f->vertices[i].edge->getOppositeFace( f );

			if ( oppositeFace != NULL )
			{
				if ( !oppositeFace->isSecondaryMarked()  &&  oppositeFace->isFaceMarked() )
				{
					faceStack.push_back( oppositeFace );
					oppositeFace->secondaryMark();
				}
			}
		}
	}
}

void MFace::discoverFaceMarkedRegionVerticesPopulateStack(std::list<MVertex*> &vertexStack)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i].vertex;

		if ( !v->isSecondaryMarked() )
		{
			vertexStack.push_back( v );
			v->secondaryMark();
		}
	}
}




// ******************* TESSELATION OPS *******************

bool MFace::computeTesselation(Tesselation &tess) const
{
	/*
	- first, project the vertices onto the face plane
	- project these points onto an orthogonal plane which most closely resembles the
	  face plane
	- tesselate the resulting 2d polygon
	*/

	Array<Point2> poly2d;
	generate2DPointList( poly2d );

	PolyTesselationResult tessResult =  checkAndTesselatePoly( poly2d, tess );

	return tessResult == POLYTESSELATION_TESSELATED;
}

void MFace::tesselate() const
{
	destroyTesselation();

	if ( vertices.size() > 3 )
	{
		//reuse space between calls; static
		static Tesselation tess;
		tess.optimiseMemoryUsageFor( vertices.size() - 2 );
		bool tesselationResult = computeTesselation( tess );

		if ( tesselationResult )
		{
			setTesselation( tess );
		}
		else
		{
			tesselation = NULL;
		}
	}

	flags.tesselationRequiresRefresh = false;
}

void MFace::setTesselation(const Tesselation &t) const
{
	//destroy any existing tesselation
	destroyTesselation();

	tesselation = new Tesselation();
	*tesselation = t;
}

void MFace::destroyTesselation() const
{
	if ( tesselation != NULL )
	{
		delete tesselation;
		tesselation = NULL;
	}
}




// ******************* VERTEX MARK OPS *******************

bool MFace::isVertexMarked() const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].vertex->isVertexMarked() )
		{
			return true;
		}
	}

	return false;
}

bool MFace::isWhollyVertexMarked() const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i].vertex->isVertexMarked() )
		{
			return false;
		}
	}

	return true;
}




// ******************* EDGE MARK OPS *******************

bool MFace::isEdgeMarked() const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].edge->isEdgeMarked() )
		{
			return true;
		}
	}

	return false;
}

bool MFace::isWhollyEdgeMarked() const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i].edge->isEdgeMarked() )
		{
			return false;
		}
	}

	return true;
}

bool MFace::hasIncidentEdgeMarkedVertices() const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].vertex->isEdgeMarked() )
		{
			return true;
		}
	}

	return false;
}

MEdge * MFace::getOtherMarkedEdge(const MEdge *e)
{
	// Find a marked edge indicent to @this that is not @e
	// Return NULL if no edge found, or of more than two edges are marked
	MEdge *otherEdge = NULL;
	for (int edgeI = 0; edgeI < vertices.size(); edgeI++)
	{
		MEdge *eTest = vertices[edgeI].edge;
		if ( eTest->isEdgeMarked()  &&  eTest != e )
		{
			if ( otherEdge != NULL )
			{
				// More than two edges marked; return NULL
				return NULL;
			}
			else
			{
				otherEdge = eTest;
			}
		}
	}

	return otherEdge;
}






// ******************* MATERIAL OPS *******************

void MFace::changeMaterialID(int m)
{
	if ( m != materialID )
	{
		getMesh()->unrefMaterial( materialID );
		materialID = m;
		getMesh()->refMaterial( materialID );
	}
}





// ******************* SUBDIVISION OPS *******************

void MFace::createSubdivisionVertex()
{
	//add a vertex at the centre of the face
	faceOpData.subdivision.vertex = getMesh()->addVertex( computeCentroid() );
	faceOpData.subdivision.vertex->createVertexAttrib( computeAttribCentroid(), MFINDPOLICY_FORCEDCREATE );
}

void MFace::createLiveSubdivisionVertex(MMesh *liveMesh)
{
	MVertex *v = liveMesh->addVertex( computeCentroid() );
	v->createVertexAttrib( MVertexAttrib( Point2f(), -1 ), MFINDPOLICY_FORCEDCREATE );
}

void MFace::refreshLiveSubdivisionVertex(MMesh *liveMesh, int faceSubdVerticesOffset)
{
	if ( flags.liveSubdVertexRequiresRefresh )
	{
		MVertex *v = liveMesh->vertices[ index + faceSubdVerticesOffset ];
		v->setPosition( computeCentroid() );
	}

	liveSubdivisionRefreshed();
}

void MFace::liveSubdivisionIncidentVertexModified()
{
	if ( !flags.liveSubdVertexRequiresRefresh )
	{
		flags.liveSubdVertexRequiresRefresh = true;
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].vertex->liveSubdivisionIncidentFaceVertexModified();
			vertices[i].edge->liveSubdivisionIncidentFaceVertexModified();
		}
	}
}


void MFace::setSubdivisionFaceProperties(MFace *src, MEdge *ea, MEdge *eb, MVertex *vb, bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness)
{
	//preserve face mark
	setFaceMark( src->isFaceMarked() );


	//preserve edge mark
	if ( ea->isEdgeMarked() )
	{
		vertices[0].edge->edgeMark();
	}
	if ( eb->isEdgeMarked() )
	{
		vertices[1].edge->edgeMark();
	}



	//preserve sharpness
	if ( ea->isCreased()  ||  eb->isCreased() )
	{
		if ( catmullClark )
		{
			//compute the average edge sharpness for vertex vb
			float vbSharpness = vb->getSubdivideAverageEdgeSharpness();
			//average this with the sharpness for the source edges

			float edgeSharpness;

			edgeSharpness = ea->getSharpness();
			if ( ea->isCreased() )
			{
				vertices[0].edge->crease();
				if ( vbSharpness == -1.0 )
				{
					float s = edgeSharpness - 1.0;
					vertices[0].edge->setSharpness( clampLower( s, 0.0f ) );
				}
				else
				{
					float s = ( edgeSharpness + vbSharpness ) * 0.5  -  1.0;
					vertices[0].edge->setSharpness( clampLower( s, 0.0f ) );
				}
			}

			edgeSharpness = eb->getSharpness();
			if ( eb->isCreased() )
			{
				vertices[1].edge->crease();

				if ( vbSharpness == -1.0 )
				{
					float s = edgeSharpness - 1.0;
					vertices[1].edge->setSharpness( clampLower( s, 0.0f ) );
				}
				else
				{
					float s = ( edgeSharpness + vbSharpness ) * 0.5  -  1.0;
					vertices[1].edge->setSharpness( clampLower( s, 0.0f ) );
				}
			}
		}
		else
		{
			vertices[0].edge->setSharpness( ea->getSharpness() );
			vertices[0].edge->setCreased( ea->isCreased() );
			vertices[1].edge->setSharpness( eb->getSharpness() );
			vertices[1].edge->setCreased( eb->isCreased() );
		}
	}

	//preserve normal sharpness
	switch( preserveNormalSharpness )
	{
	case MPRESERVENORMALSHARPNESS_PRESERVE:
		if ( ea->isNormalSharp() )
		{
			vertices[0].edge->setNormalSharp();
		}
		if ( eb->isNormalSharp() )
		{
			vertices[1].edge->setNormalSharp();
		}
		break;
	case MPRESERVENORMALSHARPNESS_IFSHARP:
		if ( ea->isNormalSharp()  &&  ea->getSharpness() >= 1.0 )
		{
			vertices[0].edge->setNormalSharp();
		}
		if ( eb->isNormalSharp()  &&  ea->getSharpness() >= 1.0 )
		{
			vertices[1].edge->setNormalSharp();
		}
		break;
	case MPRESERVENORMALSHARPNESS_SMOOTH:
		break;
	default:
		break;
	}
}

void MFace::setLiveSubdivisionFaceProperties(MEdge *ea, MEdge *eb)
{
	//Preserve edge live subdivision odd/even status
	vertices[0].edge->setLiveSubdivisionOdd( ea->isLiveSubdivisionOdd() );
	vertices[1].edge->setLiveSubdivisionOdd( eb->isLiveSubdivisionOdd() );

	//Set remaining edges to odd
	for (int i = 2; i < vertices.size(); i++)
	{
		vertices[i].edge->liveSubdivisionOdd();
	}
}


void MFace::createSubdivisionFaces(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness)
{
	//CREATES THE NEW FACES WHICH REPLACE this.
	//SEE INFORMATION ON CATMULL CLARK SUBDIVISION SURFACES


	/*
	TRASFORMATION:

	o----+----o				o----+----o
	|			 |				|	  |	 |
	|			 |				|	  |	 |
	+	  x	 +   -->   	+----x----+
	|			 |				|	  |	 |
	|			 |				|	  |	 |
	o----+----o				o----+----o

	o = original vertex
	+ = edge vertex
	x = face vertex

	*/


	static Array<MVertex*> vts;
	static Array<MVertexAttrib*> attribs;
	static bool initialisedFlag = false;

	if ( !initialisedFlag )
	{
		vts.resize( 4 );
		attribs.resize( 4 );
		initialisedFlag = true;
	}


	int i = vertices.size() - 1;
	MEdge *ea = vertices[i].edge;

	//for each vertex
	for (int j = 0; j < vertices.size(); j++)
	{
		MEdge *eb = vertices[j].edge;
		MVertex *vb = vertices[j].vertex;



		//face vertices
		vts[0] = ea->getSubdivisionVertex();
		vts[1] = vb;
		vts[2] = eb->getSubdivisionVertex();
		vts[3] = faceOpData.subdivision.vertex;


		//and attributes
		attribs[0] = ea->getSubdivisionVertexAttrib( this );
		attribs[1] = vertices[j].attrib;
		attribs[2] = eb->getSubdivisionVertexAttrib( this );
		attribs[3] = vts[3]->getVertexAttrib( 0 );

		MFace *face = getMesh()->addFace( vts, attribs, materialID );

		face->setSubdivisionFaceProperties( this, ea, eb, vb, catmullClark, preserveNormalSharpness );


		ea = eb;
		i = j;
	}
}

void MFace::createLiveSubdivisionFaces(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness)
{
	static Array<MVertex*> vts;
	static Array<MVertexAttrib*> attribs;
	static bool initialisedFlag = false;

	if ( !initialisedFlag )
	{
		vts.resize( 4 );
		attribs.resize( 4 );
		initialisedFlag = true;
	}


	int i = vertices.size() - 1;
	MEdge *ea = vertices[i].edge;

	//for each vertex
	for (int j = 0; j < vertices.size(); j++)
	{
		MEdge *eb = vertices[j].edge;
		MVertex *vb = vertices[j].vertex;



		//face vertices
		vts[0] = liveMesh->vertices[ edgeSubdVerticesOffset + ea->index ];
		vts[1] = liveMesh->vertices[ vb->index ];
		vts[2] = liveMesh->vertices[ edgeSubdVerticesOffset + eb->index ];
		vts[3] = liveMesh->vertices[ faceSubdVerticesOffset + index ];

		//and attributes
		attribs[0] = vts[0]->getVertexAttrib0();
		attribs[1] = vts[1]->getVertexAttrib0();
		attribs[2] = vts[2]->getVertexAttrib0();
		attribs[3] = vts[3]->getVertexAttrib0();

		MFace *face = liveMesh->addFace( vts, attribs, -1 );

		face->setSubdivisionFaceProperties( this, ea, eb, vb, true, preserveNormalSharpness );
		face->setLiveSubdivisionFaceProperties( ea, eb );


		ea = eb;
		i = j;
	}
}



// ******************* EXTRUDE FACE OPS *******************

void MFace::extrudeFace()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *vertex = vertices[i].vertex;
		MVertexList *extruded = vertex->getExtrudeFaceExtrudedVertices( this );

		if ( extruded != NULL )
		{
			//boundary vertex (internal vertices have NULL)
			replaceVertex( i, extruded->back(), true, false, false );
		}
	}

	//face has been modified
	//faceModified();			//not required as it is called by replaceVertex()
}

void MFace::pathExtrudeSetMatrices(Array<Matrix4> *matrices)
{
	faceOpData.pathExtrude.matrices = matrices;
}

const Array<Matrix4>* MFace::getPathExtrudeMatrices() const
{
	return faceOpData.pathExtrude.matrices;
}



// ******************* EXTRUDE FACE SINGULAR OPS *******************

void MFace::extrudeSingular(MVertexAdjustList &vertexAdjusts, int numSegments,
						bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges, int sideMaterialID)
{
	Array<MVertexList> extrudeVertices;
	extrudeVertices.resize( numSegments + 1 );
	MVertexList &oldVertices = extrudeVertices.front();

	for (int i = 0; i <= numSegments; i++)
	{
		extrudeVertices[i].reserve( vertices.size() );
	}

	double totalLength = 0.0;


	for (int i = 0; i < vertices.size(); i++)
	{
		oldVertices.push_back( vertices[i].vertex );
		totalLength += vertices[i].edge->getLength();
	}


	double vInc = 1.0 / (double)numSegments;

	Vector3 normal = getPlane().n;

	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *vOld = oldVertices[i];
		vOld->setIndestructible();

		double v = vInc;
		for (int j = 1; j <= numSegments; j++)
		{
			MVertex *vNew = getMesh()->addVertex( vOld->getPosition() );
			extrudeVertices[j].push_back( vNew );
			vertexAdjusts.push_back( MVertexAdjust( vNew, vOld->getPosition(), normal * v, Vector3() ) );
			v += vInc;
		}

		//replace the vertices in *this with the vertices in the last list in
		//extrude vertices
		replaceVertex( i, extrudeVertices.back()[i], true, true, false );
	}

	if ( markBoundaryEdges || nsharpBoundaryEdges )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( markBoundaryEdges )
			{
				vertices[i].edge->edgeMark();
			}
			if ( nsharpBoundaryEdges )
			{
				vertices[i].edge->setNormalSharp();
			}
		}
	}


	Array<MVertex*> verts;
	Array<MVertexAttrib> attribs;
	verts.resize( 4 );
	attribs.resize( 4 );

	float uI = 0.0;
	float uJ = 0.0;

	double oneOverTotalLength = 1.0 / totalLength;
	double cumulativeLength = 0.0;

	int i = oldVertices.size() - 1;
	for (int j = 0; j < oldVertices.size(); j++)
	{
		cumulativeLength += vertices[i].edge->getLength();
		uJ = cumulativeLength * oneOverTotalLength;

		double v0 = 0.0, v1 = vInc;

		for (int x = 0; x < numSegments; x++)
		{
			verts[0] = extrudeVertices[x][i];
			verts[1] = extrudeVertices[x][j];
			verts[2] = extrudeVertices[x+1][j];
			verts[3] = extrudeVertices[x+1][i];
			attribs[0] = MVertexAttrib( Point2f( uI, v0 ), sideMaterialID );
			attribs[1] = MVertexAttrib( Point2f( uJ, v0 ), sideMaterialID );
			attribs[2] = MVertexAttrib( Point2f( uJ, v1 ), sideMaterialID );
			attribs[3] = MVertexAttrib( Point2f( uI, v1 ), sideMaterialID );

			MFace *quadFace = getMesh()->addFace( verts, attribs, sideMaterialID );

			if ( markExtrudedEdges )
			{
				//mark edge 1
				quadFace->vertices[1].edge->edgeMark();
			}
			if ( nsharpExtrudedEdges )
			{
				quadFace->vertices[1].edge->setNormalSharp();
			}

			v0 = v1;
			v1 += vInc;
		}

		uI = uJ;
		i = j;
	}

	for (int i = 0; i < oldVertices.size(); i++)
	{
		oldVertices[i]->setDestructible();
	}
}



// ******************* DUPLICATE FACE OPS *******************

MFace * MFace::createDuplicateFace()
{
	Array<MVertex*> dupVertices;
	Array<MVertexAttrib> attribs;

	dupVertices.resize( vertices.size() );
	attribs.resize( vertices.size() );

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		dupVertices[vertexI] = vertices[vertexI].vertex->getDuplicateFacesDuplicateVertex();
		attribs[vertexI] = *getVertexAttrib( vertexI );
	}

	// Create the face
	MFace *duplicateFace = getMesh()->addFace( dupVertices, attribs, getMaterialID() );
	duplicateFace->copyPropertiesFrom( this );
	return duplicateFace;
}





// ******************* INSET FACE SINGULAR OPS *******************

void MFace::insetSingular(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle,
					bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	MVertexList oldVertices, newVertices;
	MVertexAttribList oldAttribs;
	Array<Vector3> vertexInsets;
	Array<Vector2f> attribInsets;
	Array<double> uLower, uUpper;

	oldVertices.reserve( vertices.size() );
	newVertices.reserve( vertices.size() );
	oldAttribs.reserve( vertices.size() );
	vertexInsets.resize( vertices.size() );
	attribInsets.resize( vertices.size() );
	uLower.resize( vertices.size() );
	uUpper.resize( vertices.size() );


	Point3 centre = computeCentroid();
	Point2f attribCentre = computeAttribCentroid().getPoint();



	for (int i = 0; i < vertices.size(); i++)
	{
		if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
		{
			insetExpandFaceComputeToCentreInsetVectors( i, vertexInsets[i], attribInsets[i], centre, attribCentre, uLower[i], uUpper[i], true );
		}
		else if ( insetStyle == MINSETFACESTYLE_FROMEDGES )
		{
			computeFromEdgeInsetVectors( i, vertexInsets[i], attribInsets[i], true, uLower[i], uUpper[i], true );
		}

		oldVertices.push_back( vertices[i].vertex );
		oldAttribs.push_back( vertices[i].attrib );
		//ref all of the vertex attributes, so they don't get destroyed
		oldAttribs[i]->ref();
	}



	Vector3 normal = getPlane().n;
	for (int i = 0; i < oldVertices.size(); i++)
	{
		MVertex *vOld = oldVertices[i];
		MVertexAttrib *attrOld = oldAttribs[i];

		vOld->setIndestructible();

		MVertex *vNew = getMesh()->addVertex( vOld->getPosition() );

		MVertexAttrib *attrNew = vNew->createVertexAttrib( *attrOld, MFINDPOLICY_FORCEDCREATE );

		newVertices.push_back( vNew );


		vertexAdjusts.push_back( MVertexAdjust( vNew, vOld->getPosition(), vertexInsets[i], uLower[i], uUpper[i],
											normal, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() ) );
		vertexAttribAdjusts.push_back( MVertexAttribAdjust( attrNew, attrOld->getPoint(), attribInsets[i], uLower[i], uUpper[i] ) );

		//don't keep edge properties
		replaceVertex( i, vNew, true, true, false );
	}

	if ( markBoundaryEdges  ||  nsharpBoundaryEdges )
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( markBoundaryEdges )
			{
				vertices[i].edge->edgeMark();
			}
			if ( nsharpBoundaryEdges )
			{
				vertices[i].edge->setNormalSharp();
			}
		}
	}



	Array<MVertex*> verts;
	Array<MVertexAttrib*> attribs;
	verts.resize( 4 );
	attribs.resize( 4 );

	int i = oldVertices.size() - 1;
	for (int j = 0; j < oldVertices.size(); j++)
	{
		verts[0] = oldVertices[i];
		verts[1] = oldVertices[j];
		verts[2] = newVertices[j];
		verts[3] = newVertices[i];
		attribs[0] = oldAttribs[i];
		attribs[1] = oldAttribs[j];
		attribs[2] = newVertices[j]->getVertexAttrib( 0 );
		attribs[3] = newVertices[i]->getVertexAttrib( 0 );

		MFace *quadFace = getMesh()->addFace( verts, attribs, materialID );

		if ( markInsetEdges )
		{
			//mark edge 1
			quadFace->vertices[1].edge->edgeMark();
		}
		if ( nsharpInsetEdges )
		{
			quadFace->vertices[1].edge->setNormalSharp();
		}

		i = j;
	}

	for (int i = 0; i < oldVertices.size(); i++)
	{
		oldVertices[i]->setDestructible();
	}

	for (int i = 0; i < oldAttribs.size(); i++)
	{
		oldAttribs[i]->unref();
	}
}



// ******************* INSET FACE OPS *******************

bool MFace::insetExpandFaceStoreCentre()
{
	int i = vertices.size() - 1;

	for (int j = 0; j < vertices.size(); j++)
	{
		if ( vertices[i].edge->isFaceMarkBoundary()  &&  vertices[j].edge->isFaceMarkBoundary() )
		{
			faceOpData.insetFace.centre = new StoredCentre();
			faceOpData.insetFace.centre->centre = computeCentroid();
			faceOpData.insetFace.centre->attribCentre = computeAttribCentroid();
			return true;
		}

		i = j;
	}

	faceOpData.insetFace.centre = NULL;
	return false;
}

void MFace::insetExpandFaceDeleteCentre()
{
	if ( faceOpData.insetFace.centre != NULL )
	{
		delete faceOpData.insetFace.centre;
		faceOpData.insetFace.centre = NULL;
	}
}



bool MFace::insetExpandFaceStorePlane(MInsetExpandOperation insetExpandOp)
{
	bool bStorePlane = false;
	if ( insetExpandOp == MINSETEXPANDOP_INSET )
	{
		if ( isFaceMarked() )
		{
			bStorePlane = true;
		}
	}
	else if ( insetExpandOp == MINSETEXPANDOP_EXPAND )
	{
		if ( isFaceMarked() )
		{
			bStorePlane = true;
		}
		else
		{
			if ( hasNeighbouringMarkedFaces() )
			{
				bStorePlane = true;
			}
		}
	}

	if ( bStorePlane )
	{
		faceOpData.insetFace.plane = new Plane();
		*faceOpData.insetFace.plane = getPlane();
	}
	else
	{
		faceOpData.insetFace.plane = NULL;
	}
	return bStorePlane;
}

void MFace::insetExpandFaceDeletePlane()
{
	if ( faceOpData.insetFace.plane != NULL )
	{
		delete faceOpData.insetFace.plane;
		faceOpData.insetFace.plane = NULL;
	}
}

void MFace::insetExpandFaceCopyStoredPlaneFrom(const MFace *face)
{
	if ( face->faceOpData.insetFace.plane != NULL )
	{
		faceOpData.insetFace.plane = new Plane();
		*faceOpData.insetFace.plane = *face->faceOpData.insetFace.plane;
	}
	else
	{
		faceOpData.insetFace.plane = NULL;
	}
}

const Plane & MFace::getInsetExpandFaceStoredPlane() const
{
	gs_assert( faceOpData.insetFace.plane != NULL, "MFace::getInsetExpandFaceStoredPlane(): no plane stored\n" );
	return *faceOpData.insetFace.plane;
}




void MFace::insetExpandFaceComputeToCentreInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, const Point3 &centre, const Point2f &attribCentre,
												double &uLower, double &uUpper, bool allowNegativeInset)
{
	MVertex *vertex = vertices[vertexIndex].vertex;
	MVertexAttrib *attrib = vertices[vertexIndex].attrib;

	vertexInset = centre - vertex->getPosition();
	attribInset = attribCentre - attrib->getPoint();


	uLower = allowNegativeInset  ?  -std::numeric_limits<double>::max()  :  0.0;
	uUpper = 2.0;
}

void MFace::insetExpandFaceComputeToCentreInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, double &uLower, double &uUpper, bool allowNegativeInset)
{
	insetExpandFaceComputeToCentreInsetVectors( vertexIndex, vertexInset, attribInset, faceOpData.insetFace.centre->centre, faceOpData.insetFace.centre->attribCentre.getPoint(),
											uLower, uUpper, allowNegativeInset );
}

void MFace::insetExpandFaceComputeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
													bool allowNegativeInset)
{
	computeFromEdgeInsetVectors( vertexIndex, vertexInset, attribInset, projectOntoFacePlane, uLower, uUpper, allowNegativeInset, getInsetExpandFaceStoredPlane() );
}

void MFace::computeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
													bool allowNegativeInset)
{
	computeFromEdgeInsetVectors( vertexIndex, vertexInset, attribInset, projectOntoFacePlane, uLower, uUpper, allowNegativeInset, getPlane() );
}

void MFace::computeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
									bool allowNegativeInset, const Plane &facePlane)
{
	MVertex *vertex = vertices[vertexIndex].vertex;
	MVertexAttrib *attrib = vertices[vertexIndex].attrib;

	const Point3 &v0 = vertices[ prevIndex( vertexIndex, vertices.size() ) ].vertex->getPosition();
	const Point3 &v1 = vertex->getPosition();
	const Point3 &v2 = vertices[ nextIndex( vertexIndex, vertices.size() ) ].vertex->getPosition();

	const Point2f &a0 = vertices[ prevIndex( vertexIndex, vertices.size() ) ].attrib->getPoint();
	const Point2f &a1 = attrib->getPoint();
	const Point2f &a2 = vertices[ nextIndex( vertexIndex, vertices.size() ) ].attrib->getPoint();

	Vector3 planeNormal = facePlane.n;

	Vector3 vBack = v0 - v1;
	Vector3 vFor = v2 - v1;

	Vector2f aBack = a0 - a1;
	Vector2f aFor = a2 - a1;


	if ( projectOntoFacePlane )
	{
		vBack = vBack.projectOntoPlane( planeNormal );
		vFor = vFor.projectOntoPlane( planeNormal );
	}


	double vBackLength = 0.0, vForLength = 0.0;

	if ( vBack.sqrLength() > SQR_EPSILON )
	{
		vBackLength = vBack.length();
		double vBackLengthRecip = 1.0 / vBackLength;
		vBack *= vBackLengthRecip;
		aBack *= vBackLengthRecip;
	}

	if ( vFor.sqrLength() > SQR_EPSILON )
	{
		vForLength = vFor.length();
		double vForLengthRecip = 1.0 / vForLength;
		vFor *= vForLengthRecip;
		aFor *= vForLengthRecip;
	}

	double cosTheta = clamp( vBack.dot( vFor ), -1.0, 1.0 );

	//if the edge vectors do not oppose eachother
	if ( cosTheta >= -MMESH_ONEMINUSEPSILON )
	{
		double sinAlpha = sin(  acos( cosTheta ) * 0.5  );
		vertexInset = vBack + vFor;
		double scaleFactor = 1.0 / ( vertexInset.length() * sinAlpha );

		vertexInset *= scaleFactor;
		attribInset = ( aBack + aFor ) * scaleFactor;

		//IF THE VERTEX IS CONCAVE, then the direction will need to be reversed
		//no need to check if the edge vectors oppose eachother
		Vector3 n = vFor.cross( vBack );
		if ( n.dot( planeNormal ) < 0.0 )
		{
			vertexInset = -vertexInset;
			attribInset = -attribInset;
		}
	}
	else
	{
		//edge vectors oppose eachother
		if ( vBackLength > EPSILON )
		{
			vertexInset = vBack.cross( planeNormal );
		}
		else
		{
			vertexInset = -vFor.cross( planeNormal );
		}

		Vector2f attribInsetA = aBack.getRotated90CW();
		Vector2f attribInsetB = aFor.getRotated90CCW();
		attribInset = attribInsetA + attribInsetB;
	}


	// Set bounds
	uLower = allowNegativeInset  ?  -std::numeric_limits<double>::max()  :  0.0;


	// In order to compute the upper bound, get the leading vertex @leading w.r.t. @vertexInset, and compute the value of @u which would place the inset
	// vertex in the same plane as @leading where the plane is perpendicular to @vertexInset
	double anchor = vertex->getPosition().dot( vertexInset );
	uUpper = 0.0;
	double leadingVertexAlpha = -std::numeric_limits<double>::max();
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i].vertex;
		if ( v != vertex )
		{
			double alpha = v->getPosition().dot( vertexInset );
			leadingVertexAlpha = std::max( leadingVertexAlpha, alpha );
		}
	}
	leadingVertexAlpha -= anchor;

	uUpper = leadingVertexAlpha / vertexInset.sqrLength();
}



// ******************* INSET FACE TO POINT OPS *******************

void MFace::insetToPoint(MVertexAdjustList &vertexAdjusts, int numSegments)
{
	//make the central vertex which is the point
	MVertex *pointVertex = getMesh()->addVertex( computeCentroid() );
	MVertexAttrib *pointAttrib = pointVertex->createVertexAttrib( computeAttribCentroid(), MFINDPOLICY_FORCEDCREATE );

	Vector3 planeNormal = getPlane().n;

	//allow its position to be adjusted
	vertexAdjusts.push_back( MVertexAdjust( pointVertex, pointVertex->getPosition(), planeNormal, Vector3() ) );


	//take a copy of the vertex list and ref the vertex attributes;
	//if two or more consecutive edges indicent to *this are boundary edges,
	//then destroying this will unref the vertex attributes, resulting in their
	//automatic destruction. ref() them to prevent this
	MVertexList oldVertices;
	MVertexAttribList oldAttribs;
	oldVertices.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		oldVertices.push_back( vertices[i].vertex );
		oldAttribs.push_back( vertices[i].attrib );
		vertices[i].attrib->ref();
	}

	//store the mark state, so that the new faces can mirror it
	bool markState = isFaceMarked();


	//destroy *this, but not the edges and vertices
	destroy( false, false );


	if ( numSegments >= 2 )
	{
		Array<MVertexList> vts;
		Array<MVertexAttribList> attribs;

		vts.resize( numSegments );
		attribs.resize( numSegments );

		vts[0] = oldVertices;
		attribs[0] = oldAttribs;

		for (int i = 1; i < numSegments; i++)
		{
			vts[i].reserve( oldVertices.size() );
			attribs[i].reserve( oldVertices.size() );
		}

		double uInc = 1.0 / (double)numSegments, u = uInc;

		for (int i = 1; i < numSegments; i++)
		{
			for (int j = 0; j < oldVertices.size(); j++)
			{
				Point3 point = lerp( oldVertices[j]->getPosition(), pointVertex->getPosition(), u );
				Point2f attrPoint = lerp( oldAttribs[j]->getPoint(), pointAttrib->getPoint(), u );

				MVertex *v = getMesh()->addVertex( point );
				MVertexAttrib *a = v->createVertexAttrib( MVertexAttrib( attrPoint, materialID ), MFINDPOLICY_FORCEDCREATE );

				vertexAdjusts.push_back( MVertexAdjust( v, v->getPosition(), planeNormal * u, Vector3() ) );



				vts[i].push_back( v );
				attribs[i].push_back( a );
			}
			u += uInc;
		}


		Array<MVertex*> faceVts;
		Array<MVertexAttrib*> faceAttribs;

		faceVts.resize( 4 );
		faceAttribs.resize( 4 );

		//body faces (quads)
		for (int x = 0; x < numSegments - 1; x++)
		{
			int y = x + 1;

			int i = oldVertices.size() - 1;
			for (int j = 0; j < oldVertices.size(); j++)
			{
				faceVts[0] = vts[x][i];
				faceVts[1] = vts[x][j];
				faceVts[2] = vts[y][j];
				faceVts[3] = vts[y][i];

				faceAttribs[0] = attribs[x][i];
				faceAttribs[1] = attribs[x][j];
				faceAttribs[2] = attribs[y][j];
				faceAttribs[3] = attribs[y][i];

				MFace *face = getMesh()->addFace( faceVts, faceAttribs, materialID );
				if ( markState )
				{
					face->faceMark();
				}

				i = j;
			}
		}


		//cap faces (triangles)
		faceVts.resize( 3 );
		faceAttribs.resize( 3 );

		int i = oldVertices.size() - 1;
		for (int j = 0; j < oldVertices.size(); j++)
		{
			faceVts[0] = vts.back()[i];
			faceVts[1] = vts.back()[j];
			faceVts[2] = pointVertex;

			faceAttribs[0] = attribs.back()[i];
			faceAttribs[1] = attribs.back()[j];
			faceAttribs[2] = pointAttrib;

			MFace *face = getMesh()->addFace( faceVts, faceAttribs, materialID );
			if ( markState )
			{
				face->faceMark();
			}

			i = j;
		}
	}
	else
	{
		//build the triangles that replace *this
		Array<MVertex*> vts;
		Array<MVertexAttrib*> attribs;

		vts.resize( 3 );
		attribs.resize( 3 );

		int i = oldVertices.size() - 1;
		for (int j = 0; j < oldVertices.size(); j++)
		{
			vts[0] = oldVertices[i];
			vts[1] = oldVertices[j];
			vts[2] = pointVertex;

			attribs[0] = oldAttribs[i];
			attribs[1] = oldAttribs[j];
			attribs[2] = pointAttrib;

			MFace *face = getMesh()->addFace( vts, attribs, materialID );
			if ( markState )
			{
				face->faceMark();
			}

			i = j;
		}
	}
}



// ******************* EXTRUDE EDGE OPS *******************

void MFace::expandEdgeInsertCornerVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, Array<MEdge*> &edgesToCheck)
{
	int i = vertices.size() - 1;

	for (int j = 0; j < vertices.size(); j++)
	{
		//find cases where two adjacent edges of this are both marked
		if ( vertices[i].edge->isEdgeMarked()  &&  vertices[j].edge->isEdgeMarked() )
		{
			MVertex *vertex = vertices[j].vertex;
			const Point3 &vtxPos = vertex->getPosition();
			MVertexAttrib *vertexAttrib = vertices[j].attrib;


			Vector3 vertexVector;
			Vector2f attribVector;
			double uLower, uUpper;
			computeFromEdgeInsetVectors( j, vertexVector, attribVector, false, uLower, uUpper, false );



			/*
			TRANSFORMATION:
			vertex	edgeJ
			o----------				o ----------
			|							  \
			|							|\ \
			|							| \ \
			|							|  \ \
			| edgeI					|    x  centreVertex
			|							|
			*/


			MVertex *centreVertex = getMesh()->addVertex( vtxPos );
			MVertexAttrib *centreAttrib = centreVertex->createVertexAttrib( MVertexAttrib( vertexAttrib->getPoint(), materialID ), MFINDPOLICY_FORCEDCREATE );

			vertexAdjusts.push_back( MVertexAdjust( centreVertex, vtxPos, vertexVector, uLower, uUpper ) );

			//the insert actions can destroy one of the marked edges,
			//which is then recreated
			//prevent this by making them indestructible (temporarily)
			MEdge *edgeI = vertices[i].edge, *edgeJ = vertices[j].edge;
			edgeI->setIndestructible();
			edgeJ->setIndestructible();
			insertVertex( j, centreVertex, centreAttrib );
			insertVertex( j, vertex, vertexAttrib );
			edgeI->setDestructible();
			edgeJ->setDestructible();


			vertexAttribAdjusts.push_back( MVertexAttribAdjust( centreAttrib, vertexAttrib->getPoint(), attribVector, uLower, uUpper ) );

			MEdge *newEdge = vertex->findEdgeTo( centreVertex, MFINDPOLICY_RETURNNULL );

			gs_assert( newEdge != NULL, "MFace::extrudeEdgeCreateVertices(): could not find edge from @vertex to @newVertex\n" );

			edgesToCheck.push_back( newEdge );
		}

		i = j;
	}

	//face has been modified
	//faceModified();	//not needed as operations called here will call faceModified()
}




// ******************* BEVEL EDGE OPS *******************

MVertexAttrib* MFace::getBevelEdgeReplacementAttrib(MVertex *v, MEdge *e)
{
	int numAttribs = v->getNumVertexAttribs();

	if ( numAttribs == 1 )
	{
		//only one attrib
		return v->getVertexAttrib( 0 );
	}
	else if ( numAttribs == 2 )
	{
		//get the two attribs
		MVertexAttrib *a0 = v->getVertexAttrib( 0 );
		MVertexAttrib *a1 = v->getVertexAttrib( 1 );

		//if both have a ref count of 0, neither has been used yet
		if ( a0->getRefCount() == 0  &&  a1->getRefCount() == 0 )
		{
			if ( e->getFaceA() == this )
			{
				//this == faceA: use attrib 0
				return a0;
			}
			else if ( e->getFaceB() == this )
			{
				//this == faceA: use attrib 1
				return a1;
			}
			else
			{
				gs_assert_not_reached( "MFace::getBevelEdgeReplacementAttrib(): edge @e is not incident to @this\n" );
				return NULL;
			}
		}
		else if ( a0->getRefCount() == 1  &&  a1->getRefCount() == 0 )
		{
			//attrib1 is unused
			return a1;
		}
		else if ( a0->getRefCount() == 0  &&  a1->getRefCount() == 1 )
		{
			//attrib0 is unused
			return a0;
		}
		else
		{
			gs_assert_not_reached( "MFace::getBevelEdgeReplacementAttrib(): attribute ref counts do not conform to a known pattern\n" );
			return NULL;
		}
	}
	else
	{
		gs_assert_not_reached( "MFace::getBevelEdgeReplacementAttrib(): could not determine the correct vertex attribute\n" );
		return NULL;
	}
}

void MFace::bevelEdgeReplaceVerticesWithBoundaryVerts()
{
	MVertexList replacementVertices;
	MVertexAttribList replacementAttribs;

	replacementVertices.resize( vertices.size() * 2 );
	replacementAttribs.resize( vertices.size() * 2 );



	//generate the list of replacement vertices
	//there are two replacement vertices corresponding to each vertes incident to
	//*this; a pair each. Zero, one, or two of these may be used. Unused entries
	//are given the value of NULL
	//NOTE: if only one replacement is needed, the first of the pair should be
	//use, the second unused
	for (int i = 0, x = 0; i < vertices.size(); i++, x += 2)
	{
		if ( vertices[i].vertex->isInitialisedForBevelEdge() )
		{
			//get the replacement vertices for v
			MVertex *v = vertices[i].vertex;
			MVertex *repVertex0 = NULL, *repVertex1 = NULL;
			MVertexAttrib *repAttrib0 = NULL, *repAttrib1 = NULL;


			MEdge *e0 = vertices[ prevIndex( i, vertices.size() ) ].edge;
			MEdge *e1 = vertices[i].edge;


			if ( e0->isEdgeMarked()  &&  e1->isEdgeMarked() )
			{
				//both adjacent edges marked
				//get the vertex from the face
				repVertex0 = v->getBevelEdgeBoundaryVertexForFace( this );
				repVertex1 = NULL;
				//will only be one attrib at this vertex
				repAttrib0 = repVertex0->getVertexAttrib( 0 );
				repAttrib1 = NULL;
			}
			else if ( e0->isEdgeMarked()  &&  !e1->isEdgeMarked() )
			{
				if ( e0->isBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase( v ) )
				{
					// Linear neighbourhood with one marked edge special case.
					// @e0 is marked; a vertex will have been created that moves into the face @this
					repVertex0 = v->getBevelEdgeBoundaryVertexForFace( this );
					// @e1 is unmarked and will have the adjustable vertex
					repVertex1 = v->getBevelEdgeBoundaryVertexForEdge( e1 );
					repAttrib0 = repVertex0->getVertexAttrib( 0 );
					repAttrib1 = getBevelEdgeReplacementAttrib( repVertex1, e1 );
				}
				else
				{
					//e1 is unmarked and will have the adjustable vertex
					repVertex0 = v->getBevelEdgeBoundaryVertexForEdge( e1 );
					repVertex1 = NULL;
					repAttrib0 = getBevelEdgeReplacementAttrib( repVertex0, e1 );
					repAttrib1 = NULL;
				}
			}
			else if ( !e0->isEdgeMarked()  &&  e1->isEdgeMarked() )
			{
				if ( e1->isBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase( v ) )
				{
					// Linear neighbourhood with one marked edge special case.
					// @e0 is unmarked and will have the adjustable vertex
					repVertex0 = v->getBevelEdgeBoundaryVertexForEdge( e0 );
					// @e1 is marked; a vertex will have been created that moves into the face @this
					repVertex1 = v->getBevelEdgeBoundaryVertexForFace( this );
					repAttrib0 = getBevelEdgeReplacementAttrib( repVertex0, e0 );
					repAttrib1 = repVertex1->getVertexAttrib( 0 );
				}
				else
				{
					//e0 is unmarked and will have the adjustable vertex
					repVertex0 = v->getBevelEdgeBoundaryVertexForEdge( e0 );
					repVertex1 = NULL;
					repAttrib0 = getBevelEdgeReplacementAttrib( repVertex0, e0 );
					repAttrib1 = NULL;
				}
			}
			else if ( !e0->isEdgeMarked()  &&  !e1->isEdgeMarked() )
			{
				//e0 and e1 are unmarked, both will have adjustable vertices
				repVertex0 = v->getBevelEdgeBoundaryVertexForEdge( e0 );
				repVertex1 = v->getBevelEdgeBoundaryVertexForEdge( e1 );
				if ( repVertex0 != NULL )
				{
					repAttrib0 = getBevelEdgeReplacementAttrib( repVertex0, e0 );
				}
				if ( repVertex1 != NULL )
				{
					repAttrib1 = getBevelEdgeReplacementAttrib( repVertex1, e1 );
				}
			}

			replacementVertices[x] = repVertex0;
			replacementVertices[ x + 1 ] = repVertex1;
			replacementAttribs[x] = repAttrib0;
			replacementAttribs[ x + 1 ] = repAttrib1;
		}
		else
		{
			//this incident vertex is not involved in the bevel operation
			replacementVertices[x] = NULL;
			replacementVertices[ x + 1 ] = NULL;
			replacementAttribs[x] = NULL;
			replacementAttribs[ x + 1 ] = NULL;
		}
	}


	//perform the vertex replace operations
	for (int i = 0, x = 0; i < vertices.size(); i++, x += 2)
	{
		//get the two replacement vertices and attributes
		MVertex *repVertex0 = replacementVertices[x];
		MVertex *repVertex1 = replacementVertices[ x + 1 ];
		MVertexAttrib *repAttrib0 = replacementAttribs[x];
		MVertexAttrib *repAttrib1 = replacementAttribs[ x + 1 ];

		if ( repVertex0 != NULL  &&  repVertex1 != NULL )
		{
			//two replacements
			//replace the vertex with vertex and attrib #1
			replaceVertex( i, repVertex1, repAttrib1, true, true, true );
			//now insert vertex and attrib #0 before #1
			insertVertex( i, repVertex0, repAttrib0 );

			//vertex has been inserted
			i++;
		}
		else if ( repVertex0 != NULL  &&  repVertex1 == NULL )
		{
			//one replacement
			//replace the vertex with vertex and attrib #0
			replaceVertex( i, repVertex0, repAttrib0, true, true, true );
		}
	}
}




// ******************* KNIFE OPS *******************

MFace::KnifeScreenPoly::KnifeScreenPoly(const FaceVertexList &verts, const Matrix4 &m)
{
	poly.reserve( verts.size() );

	for (int i = 0; i < verts.size(); i++)
	{
		const Point3 &pos = verts[i].vertex->getPosition();
		poly.addVertex( m.transformHomogeneous2d( pos ) );
	}


	areax2 = poly.computeAreaX2();
}


const Point2 & MFace::KnifeScreenPoly::operator[](int index) const
{
	return poly[index];
}

bool MFace::KnifeScreenPoly::isSelfIntersecting() const
{
	return poly.isSelfIntersecting();
}

bool MFace::KnifeScreenPoly::isSplitAreaConsistent(int va, int vb) const
{
	//find the smallest sub-polygon (in terms of number of verts)
	//so the the amount of computation is minimised.
	int numEdges;
	computeSmallestSubPoly( va, vb, numEdges );

	//compute the area (*2) of the sub-polygon
	double splitAreax2 = 0.0;
	int iPrev = vb, i = va;
	for (int j = 0; j < numEdges; j++)
	{
		splitAreax2 += poly[iPrev].x * poly[i].y  -  poly[iPrev].y * poly[i].x;

		iPrev = i;
		i = nextIndex( i, poly.size() );
	}


	//the area is consistent if the area (*2) of the sub-polygon is
	//>= 0.0  and  <= the area (*2) of the polygon, if the area (*2) of
	//the polygon is positive,
	//or
	//>= the area (*2) of the polygon  and  <= 0.0, if the area (*2) of
	//the polygon is negative
	if ( areax2 > 0.0 )
	{
		return splitAreax2 >= 0.0  &&  splitAreax2 <= areax2;
	}
	else
	{
		return splitAreax2 >= areax2  &&  splitAreax2 <= 0.0;
	}
}

void MFace::KnifeScreenPoly::computeSmallestSubPoly(int &splitA, int &splitB, int &numEdges) const
{
	bool swapFlag = splitB < splitA;

	int min = std::min( splitA, splitB );
	int max = std::max( splitA, splitB );

	int distance = max - min;
	int flippedDistance = ( min + poly.size() )  -  max;
	if ( distance > flippedDistance )
	{
		swapFlag = !swapFlag;
		numEdges = flippedDistance + 1;
	}
	else
	{
		numEdges = distance + 1;
	}

	if ( swapFlag )
	{
		std::swap( splitA, splitB );
	}
}



bool MFace::knifeCanSplitPRD(KnifeCutVtxPRD a, KnifeCutVtxPRD b)
{
	if ( a != KNIFE_ON_ON  &&  b != KNIFE_ON_ON )
	{
		if ( a == KNIFE_SAME_SIDE  ||  b == KNIFE_SAME_SIDE )
		{
			return true;
		}
		else
		{
			return a != b;
		}
	}
	else
	{
		return true;
	}
}



void MFace::knifeInitialise()
{
	faceOpData.knife.cutVertices = NULL;
}

void MFace::knifeFinish()
{
	if ( faceOpData.knife.cutVertices != NULL )
	{
		delete faceOpData.knife.cutVertices;
	}
}

void MFace::knifeAddCutVertex(MVertex *cutVertex)
{
	if ( faceOpData.knife.cutVertices == NULL )
	{
		faceOpData.knife.cutVertices = new MVertexList();
	}

	faceOpData.knife.cutVertices->push_back( cutVertex );
}

void MFace::knife(const Plane &knifePlane, const Point2 &knifeStart, const Point2 &knifeEnd, const Matrix4 &localToScreen, bool markCutEdges)
{
	//If there are only two cut vertices then its easy; just connect them with
	//a split.
	//
	//=========
	//
	//If there are more than two, then things get a little more complicated,
	//as it is necessary to match the correct pairs of cut vertices with one
	//another in order to determine where to place the splits.
	//
	//To simplify the process, the vertices of the face area transformed into
	//screen space using the @localToScreen matrix. This matrix would be the
	//matrix used to transform the points into screen space when the model
	//is being edited by the user. Note, that the knife process can only
	//proceed if the resulting polygon is simple (not self-intersecting).
	//
	//
	// 	*------------------------------------------------------------*
	// 	|																				/
	// 	|																			  /
	// 	|   *---*			*-*	  *-*										 /  +ve side
	// 	|   |   |			|	\	 /  |				 						/	 of plane
	//   A|  B|	 C|	 D	  E|	F\	/	G|  H   I	J	K		L		M /
	// ...%===%...%====%====%....%....%===%---%---%==%=====%=====%............
	// 	|	 |   |	/ \	|			 |	  |		 | / \	/ \   /
	// 	|	 |   |  /   \  |			 |	  |		 |/	\ /	\ /		 -ve side
	// 	*---*   *-*	    *-*			 *---* 		 *		 *	    *			 of plane
	//
	// ..... : cut plane		* : vertex				POLYGON VERTEX ORDERING
	// ===== : split			% : split vertex		IS COUNTER-CLOCKWISE
	// ----- : edge
	//
	//FIGURE 1. Example knife polygon
	//
	//First, the criteria that determine whether or not a potential split is
	//valid, need to be defined.
	//
	//
	//Plane Relative Direction (PRD)
	//==============================
	//The plane relative direction of each vertex is the direction of the
	//vertices adjacent to it, relative to the cut plane.
	//There are nine different classifications of PRD:
	//
	//   ---> polygon vertex ordering
	//
	//				  NEG-POS	  POS-POS		  ON-POS		  POS-ON
	//		  *			 *			*   *					*			*
	//	+ve   \			/			 \ /				  /			 \						+ve
	//	   ....%......%.....%.....%...*--%...*--%.....%--*...%--*...*--%--*....
	//	-ve	  \	 /		 / \				 \				/								-ve
	//				*  *		*   *				  *		  *
	//		 POS-NEG 	  NEG-NEG		 ON-NEG		 NEG-ON				 ON-ON
	//
	//FIGURE 2. Polygon relative directions
	//
	//The PRDs of two cut vertices can be used to quickly determine whether or
	//not they can be matched:
	//1. Vertices with opposing PRDs can be matched; that is POS-NEG with NEG-POS,
	//   e.g. vertices A and B in figure 1
	//2. NEG-NEG and POS-POS can match with anything except ON-ON, thus they
	//   are treated as equivalent of one another. The code uses the category
	//   SAME-SIDE.
	//3. The same rules apply to ON-NEG, ON-POS, NEG-ON, and POS-ON, so they
	//   also fall under SAME-SIDE.
	//4. ON-ON cannot be matched with anything. An ON-ON vertex wculd
	//   be matched to a vertex that is adjacent to it, however, since
	//   they would already be linked by an edge, this would be pointless.
	//
	//
	//Area Sign Consistency (ASC)
	//===========================
	//Potential splits which pass the PRD test may not be valid, e.g. the
	//B-C potential split in Figure 1.
	//All splits that pass the PRD test are subjected to the area sign
	//consistency test:
	//
	//When a polygon is split, two sub-polygons result.
	//
	//a. VALID SPLIT:
	//			*---*				*---*
	//		  A|	B|				|   |
	//		   *===*		->		*---*		+		*---*
	//			|	 |				 -->				|   |
	//			*---*									*---*
	//			 -->									 -->
	//area:	+ve				+ve				+ve
	//
	//b. INVALID SPLIT
	//		   *---------*				*---------*
	//		   |			 |				|			 |
	//		   |  *---*	 |				|			 |				*---*
	//		   |  |   |  |		->		|			 |		+		|	 |
	//		  A| B|  C| D|				|  		 |				|	 |
	//		   %  %===%  %				*  *---*  *				*---*
	//		   |  |   |  |				|	|	 |  |				 <--
	//		   *--*   *--*				*--*	 *--*
	//			-->						-->
	//area:  +ve						+ve						-ve
	//
	//--> and <-- arrows indiciate the direction of the polygon; CW or CCW
	//(CCW polugons have a +ve area)
	//
	//FIGURE 3: Determining the validity of a potetial split using ASC
	//
	//The signs of the areas of the two sub-polygons resulting from the split
	//must be the same as the sign of the area of the original polygon
	//in order for the split to pass the ASC test.
	//Note that the sum of the areas of the sub-polygons will be equal to
	//the area of the original polygon. This means that the ASC can be
	//determined by computing the area of only one of the sub-polygons;
	//in case (b), the first sub-polygon has a positive area, like the
	//original polygon. Since the second sub-polygon has a negative area,
	//the area of the first sub-polygon will be greater than the area of
	//the original polygon.
	//So, to determine the ASC, compute the area of the sub-polygon that has
	//the smallest number of vertices (to save computation time).
	//The ASC test fails if:
	//1. The sign of the area of the sub-polygon is not the same as the sign
	//   of the area of the original polygon
	//2. The absolute value of the area of the sub-polygon is greater than the
	//   the absolute value of the area of the original polygon
	//
	//
	//Using the two criteria described above does not guarantee a correct result,
	//since the potential split J-M in figure 1 would pass both tests. Since the
	//polygon has been determined to be be simple, and the cut vertices lie on
	//the knife line (the knife plane projected into screen space), all valid
	//matches will match two cut vertices that are adjacent to one another if
	//they are sorted in order of distance along the knife line.
	//The dot product of the vertex position and the direction of the knife line
	//is used as a sorting key to sort the cut vertices. Adjacent vertices in
	//the sorted list are tested using the PRD and ASC tests. All pairs that
	//pass are used as a split.


	if ( faceOpData.knife.cutVertices != NULL )
	{
		MVertexList &cutVertices = *faceOpData.knife.cutVertices;

		if ( cutVertices.size() == 2 )
		{
			MEdge *newEdge;
			MFace *f = split( cutVertices[0], cutVertices[1], newEdge );

			if ( f != NULL )
			{
				f->secondaryUnmark();

				if ( markCutEdges  &&  newEdge != NULL )
				{
					newEdge->edgeMark();
				}
			}
		}
		else if ( cutVertices.size() > 2 )
		{
			Array< std::pair<MVertex*,MVertex*> > splits;
			//create a screen space polygon from @this
			KnifeScreenPoly screenPoly( vertices, localToScreen );

			if ( !screenPoly.isSelfIntersecting() )
			{
				Vector2 screenKnifeVector = knifeEnd - knifeStart;

				//get the indices of the split vertices
				Array<int> splitVtxToVtx;
				Array<KnifeCutVtxPRD> planeRelativeDir;
				splitVtxToVtx.resize( cutVertices.size() );
				planeRelativeDir.resize( cutVertices.size() );


				//Array for sorted list of knife cuts
				Array<KnifeCutEntry> knifeCuts;
				knifeCuts.reserve( cutVertices.size() );

				for (int i = 0; i < cutVertices.size(); i++)
				{
					//get the index of the cut vertex
					int vtxIndex = findVertex( cutVertices[i] );

					gs_assert( vtxIndex != -1, "MFace::knife(): cut vertex is not indicent to @this\n" );

					//record the index in the table
					splitVtxToVtx[i] = vtxIndex;

					//get the adjacent vertices
					int prevVtxIndex = prevIndex( vtxIndex, vertices.size() );
					int nextVtxIndex = nextIndex( vtxIndex, vertices.size() );
					MVertex *prevVtx = vertices[prevVtxIndex].vertex;
					MVertex *nextVtx = vertices[nextVtxIndex].vertex;

					//get the directions to the adjacent vertices
					const Point3 &pos = cutVertices[i]->getPosition();
					Vector3 prevDir = prevVtx->getPosition() - pos;
					Vector3 nextDir = nextVtx->getPosition() - pos;


					//get the sides of the planr
					Side prevSide = knifePlane.directionSide( prevDir );
					Side nextSide = knifePlane.directionSide( nextDir );


					//If a surrounding vertex is a cut vertex, then consider it to
					//be ON the plane
					if ( cutVertices.find( prevVtx )  !=  -1 )
					{
						prevSide = SIDE_ON;
					}
					if ( cutVertices.find( nextVtx )  !=  -1 )
					{
						nextSide = SIDE_ON;
					}


					//Determine the direction (relative to the cut plane) of the
					//locality of the face that surrounds the cut vertex
					if ( prevSide == SIDE_ON  &&  nextSide == SIDE_ON )
					{
						//Adjacent vertices also on the plane
						planeRelativeDir[i] = KNIFE_ON_ON;
					}
					else if ( prevSide == SIDE_POSITIVE  &&  nextSide == SIDE_NEGATIVE )
					{
						//Positive side to negative side
						planeRelativeDir[i] = KNIFE_POS_NEG;
					}
					else if ( prevSide == SIDE_NEGATIVE  &&  nextSide == SIDE_POSITIVE )
					{
						//Negative side to positive side
						planeRelativeDir[i] = KNIFE_NEG_POS;
					}
					else
					{
						//Doubles back on itself, like a v-shape
						planeRelativeDir[i] = KNIFE_SAME_SIDE;
					}


					//add to the list of knife cuts if the PRD is not ON-ON
					if ( planeRelativeDir[i] != KNIFE_ON_ON )
					{
						KnifeCutEntry &entry = knifeCuts.push_back();
						entry.index = i;
						//sorting key:
						entry.positionAlongKnifeVector = screenPoly[vtxIndex].dot( screenKnifeVector );
					}
				}


				//sort the list of knife cuts
				//KnifeCompareFunc compares the @positionAlongKnifeVector members
				//of the KnifeCutEntry structure
				std::sort( knifeCuts.begin(), knifeCuts.end(), KnifeCompareFunc() );


				//go through the list of knife cuts, and record each possible split
				for (int i = 1; i < knifeCuts.size(); i++)
				{
					int iPrev = i - 1;

					int cutIndexA = knifeCuts[iPrev].index;
					int cutIndexB = knifeCuts[i].index;

					//The direction of the edges adjacent to the cut vertices can
					//determine whether or not a split is possible
					if ( knifeCanSplitPRD( planeRelativeDir[cutIndexA], planeRelativeDir[cutIndexB] ) )
					{
						int vtxIndexA = splitVtxToVtx[cutIndexA];
						int vtxIndexB = splitVtxToVtx[cutIndexB];
						//Splitting the face will partition it into two sub-faces
						//Determine if the signs of the areas of the sub-faces are
						//consistent. If so, the split may proceed
						if ( screenPoly.isSplitAreaConsistent( vtxIndexA, vtxIndexB ) )
						{
							MVertex *va = cutVertices[cutIndexA];
							MVertex *vb = cutVertices[cutIndexB];
							//add split to list
							splits.push_back( std::make_pair( va, vb ) );
						}
					}
				}


				//perform the splits
				for (int i = 0; i < splits.size(); i++)
				{
					MVertex *va = splits[i].first;
					MVertex *vb = splits[i].second;

					MFace *fa, *fb;
					MEdge *newEdge;

					va->faceSplit( vb, fa, fb, newEdge );

					if ( markCutEdges  &&  newEdge != NULL )
					{
						newEdge->edgeMark();
					}
				}
			}
		}
	}
}



// ******************* CONNECT OPS *******************

void MFace::connectVertices(bool markCutEdges)
{
	//
	// 1. Generate a list of Arrays of vertices
	// Each Array contains a list of contiguous split vertices
	//


	// Find a starting vertex; a vertex at the beginning of a contiguous list of split vertices;
	// To find a vertex like this, it needs to be secondary marked, with the previous vertex
	// being unmarked.
	int vertexIndex = -1;
	MVertex *vPrev = vertices.back().vertex;
	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[i].vertex;

		if ( !vPrev->isSecondaryMarked()  &&  v->isSecondaryMarked() )
		{
			vertexIndex = i;
			break;
		}

		vPrev = v;
	}

	// None found; nothing to do
	if ( vertexIndex == -1 )
	{
		return;
	}


	//Generate lists of contiguous split vertices
	std::list<MVertex*> *currentList = NULL;
	std::list< std::list<MVertex*> > splitVertexLists;
	int numVertexLists = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		MVertex *v = vertices[vertexIndex].vertex;

		//secondary marked indicates that it was generated by an edge split
		if ( v->isSecondaryMarked() )
		{
			if ( currentList == NULL )
			{
				// Start a new list
				splitVertexLists.push_back( std::list<MVertex*>() );
				currentList = &splitVertexLists.back();
				numVertexLists++;
			}
			//add the the current list
			currentList->push_back( v );
		}
		else
		{
			//start a new list
			currentList = NULL;
		}

		vertexIndex = nextIndex( vertexIndex, vertices.size() );
	}




	//
	// 2. Split
	//
	if ( numVertexLists >= 2 )
	{
		//
		// 2a. Build the list of splits to perform
		//

		// This is done by matching 'adjacent' vertices from 'adjacent' lists, e.g. if there are four vertex lists A, B, C, and D, we link:
		// D.last -> A.first,  A.last -> B.first,  B.last -> C.first,  C.last -> D.first

		Array< std::pair<MVertex*, MVertex*> > splits;

		// Loop until vertex lists consumed
		while ( numVertexLists >= 2 )
		{
			std::list< std::list<MVertex*> >::iterator srcListIter, destListIter;


			// Loop over once, building the list of splits

			// @srcListIter and @dstListIter point to adjacent lists. Start with the last and first respectively,
			// so that we can iterate forward simply
			srcListIter = splitVertexLists.end();
			--srcListIter;
			destListIter = splitVertexLists.begin();

			// Determine if we only need a single split
			bool singleSplitFlag = numVertexLists == 2  &&  srcListIter->front() == srcListIter->back()  &&  destListIter->front() == destListIter->back();

			// Iterate the list iterators until we reach the end
			while ( destListIter != splitVertexLists.end() )
			{
				// Get the vertices to connect
				MVertex *va = srcListIter->back();
				MVertex *vb = destListIter->front();

				// Ensure that this split is suitable
				if ( va->findBestSplitFace( vb, false )  ==  this )
				{
					// Add the split to the split list
					std::pair<MVertex*, MVertex*> &splitPair = splits.push_back();

					splitPair.first = va;
					splitPair.second = vb;

					if ( singleSplitFlag )
					{
						// One split only, exit out
						break;
					}
				}

				srcListIter = destListIter;
				++destListIter;
			}


			// Remove 'consumed' vertices from the lists
			std::list< std::list<MVertex*> >::iterator listIter = splitVertexLists.begin();
			while ( listIter != splitVertexLists.end() )
			{
				// Get an iterator pointing to the next list
				std::list< std::list<MVertex*> >::iterator nextIter = listIter;
				++nextIter;

				// Get iterators pointing to the first and last vertices of @listIter
				std::list<MVertex*>::iterator frontIter = listIter->begin(), backIter = listIter->end();
				--backIter;

				// Remove the first and last vertices from the list pointed to by @listIter, avoiding
				// attempting to remove the vertex twice if they are the same vertex
				if ( backIter != frontIter )
				{
					listIter->erase( backIter );
				}

				listIter->erase( frontIter );

				// If the list is empty, remove it from the list lists, and decrease the list count
				if ( listIter->empty() )
				{
					splitVertexLists.erase( listIter );
					numVertexLists--;
				}

				listIter = nextIter;
			}
		}



		//
		// 2b. Perform the splits
		//
		MFace *faceA = this;
		for (int i = 0; i < splits.size(); i++)
		{
			MEdge *splitEdge;

			MFace *faceB = faceA->split( splits[i].first, splits[i].second, splitEdge );

			// The split can fail if an edge between @splits[i].first and @splits[i].second already exists, and already
			// has two incident faces.

			if ( faceB != NULL )
			{
				if ( markCutEdges )
				{
					splitEdge->edgeMark();
				}

				if ( i < ( splits.size() - 1 ) )
				{
					// We need to determine which of faceA and faceB we should split next
					MVertex *va = splits[i+1].first;
					MVertex *vb = splits[i+1].second;

					if ( faceA->isIncidentTo( va )  &&  faceA->isIncidentTo( vb ) )
					{
						//face A
					}
					else if ( faceB->isIncidentTo( vb )  &&  faceB->isIncidentTo( vb ) )
					{
						//face B
						faceA = faceB;
					}
					else
					{
						gs_assert_not_reached( "MFace::connectVertices(): could not determine which face to split next\n" );
					}
				}
			}
		}
	}
}



void MFace::cutSplitFace(MVertex *startVertex, MVertex *endVertex, bool hasFaceTargets, const MCutTargetList &targets, int targetStart, int numTargets)
{
	gs_assert( startVertex != NULL, "MFace::cutSplitFace(): @startVertex is NULL\n" );
	gs_assert( endVertex != NULL, "MFace::cutSplitFace(): @endVertex is NULL\n" );

	if ( numTargets > 0  &&  hasFaceTargets )
	{
		Array<MVertex*> internalVertices;
		Array<MVertexAttrib*> internalAttribs;

		Plane facePlane = getPlane();

		// Generate the vertices and corresponding texture co-ordinates that will be created by the
		// internal cut vertices

		// Generate a 2D-projected-world-space polygon representing @this
		Array<Point2> poly2d;
		generate2DPointList( poly2d );

		// Get the u and v texture co-ordinates of @this
		Array<Point3> uParams, vParams;
		uParams.resize( poly2d.size() );
		vParams.resize( poly2d.size() );

		for (int i = 0; i < poly2d.size(); i++)
		{
			uParams[i] = Point3( poly2d[i], vertices[i].attrib->getPoint().x );
			vParams[i] = Point3( poly2d[i], vertices[i].attrib->getPoint().y );
		}

		// Compute a least-sum-square error planar equation that transforms 2D-projected-world-space into texture-U and texture-V
		PlanarEquation uPlanar( uParams );
		PlanarEquation vPlanar( vParams );

		Axis orthoPlaneNormal = getPlane().n.getMajorAxis();

		// For each internal cut target
		for (int i = 0; i < numTargets; i++)
		{
			int targetIndex = targetStart + i;
			const MPick &pick = targets[targetIndex].pick;

			// Compute the position of the internal vertex
			Matrix4 screenToWorldMatrix = pick.projection.getWorldToScreenMatrix().inverse();
			Segment3 ray;
			ray.a = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 0.0 ) );
			ray.b = screenToWorldMatrix.transformHomogeneous( Point3( pick.screenPos, 1.0 ) );

			double t;
			Point3 intersectionPoint;
			if ( ray.intersect( facePlane, t, intersectionPoint ) )
			{
				// Transform the position into 2D-projected-world-space
				Point2 p2 = intersectionPoint.toPoint2Axis( orthoPlaneNormal );
				double u = uPlanar.evaluate( p2.x, p2.y );
				double v = vPlanar.evaluate( p2.x, p2.y );

				// Create the vertex
				MVertex *vertex = getMesh()->addVertex( intersectionPoint );

				// Create the texture co-ordinate
				MVertexAttrib attrib = MVertexAttrib( Point2f( (float)u, (float)v ), materialID );
				MVertexAttrib *vAttrib = vertex->createVertexAttrib( attrib, MFINDPOLICY_FORCEDCREATE );

				internalVertices.push_back( vertex );
				internalAttribs.push_back( vAttrib );
			}
		}


		MEdgeList splitEdges;
		splitWith( startVertex, endVertex, internalVertices, internalAttribs, splitEdges );

		for (int edgeI = 0; edgeI < splitEdges.size(); edgeI++)
		{
			splitEdges[edgeI]->edgeMark();
		}
	}
	else
	{
		MEdge *splitEdge;

		split( startVertex, endVertex, splitEdge );

		splitEdge->edgeMark();
	}
}


void MFace::nCutPerformSplits(MVertexList &src, MVertexList &dest, MEdgeList *splitEdges)
{
	gs_assert( src.size() == dest.size()  ||  src.size() == 1  ||  dest.size() == 1, "MFace::nCutPerformSplits(): vertex list sizes do not match\n" );

	if ( splitEdges != NULL )
	{
		splitEdges->reserveExtra( std::max( src.size(), dest.size() ) );
	}

	if ( src.size() == 1  &&  dest.size() == 1 )
	{
		// 1 vertex to 1 vertex
		MEdge *newEdge;
		MFace *f = split( src[0], dest[0], newEdge );

		gs_assert( f != NULL, "MFace::nCutPerformSplits(): split failed (1:1)\n" );

		newEdge->edgeMark();

		if ( splitEdges != NULL )
		{
			splitEdges->push_back( newEdge );
		}
	}
	else if ( src.size() == 1  ||  dest.size() == 1 )
	{
		// 1 vertex to N vertices
		MVertexList *listA = &src;
		MVertexList *listB = &dest;

		if ( listB->size() == 1 )
		{
			std::swap( listA, listB );
		}

		MVertex *va = listA->at( 0 );
		MFace *faceA = this;
		//@listA has 1 vertex, @listB has > 1
		for (int vertexIndex = 0; vertexIndex < listB->size(); vertexIndex++)
		{
			MVertex *vb = listB->at( vertexIndex );

			MEdge *newEdge;
			MFace *faceB = faceA->split( va, vb, newEdge );

			gs_assert( faceB != NULL, "MFace::nCutPerformSplits(): split failed (1:N)\n" );

			newEdge->edgeMark();

			if ( splitEdges != NULL )
			{
				splitEdges->push_back( newEdge );
			}

			// Not finished yet....
			if ( vertexIndex < ( listB->size() - 1 ) )
			{
				// Determine which face from @faceA and @faceB to work from next
				MVertex *vbNext = listB->at( vertexIndex + 1 );
				if ( faceA->isIncidentTo( vbNext ) )
				{
					// face A
				}
				else if ( faceB->isIncidentTo( vbNext ) )
				{
					// face B
					faceA = faceB;
				}
				else
				{
					gs_assert_not_reached( "MFace::nCutPerformSplits(): could not determine next face (1:N)\n" );
				}
			}
		}
	}
	else
	{
		// N vertices to N vertices
		int src0Index = findVertex( src[0] );
		int src1Index = findVertex( src[1] );
		int dest0Index = findVertex( dest[0] );
		int dest1Index = findVertex( dest[1] );

		gs_assert( src0Index != -1, "MFace::nCutPerformSplits(): src[0] is not incident to @this\n" );
		gs_assert( src1Index != -1, "MFace::nCutPerformSplits(): src[1] is not incident to @this\n" );
		gs_assert( dest0Index != -1, "MFace::nCutPerformSplits(): dest[0] is not incident to @this\n" );
		gs_assert( dest1Index != -1, "MFace::nCutPerformSplits(): dest[1] is not incident to @this\n" );

		bool srcOrder = src1Index == nextIndex( src0Index, vertices.size() );
		bool destOrder = dest1Index == nextIndex( dest0Index, vertices.size() );

		bool reverseMatching = srcOrder == destOrder;

		MFace *faceA = this;
		//@listA has 1 vertex, @listB has > 1
		for (int vertexIndex = 0; vertexIndex < dest.size(); vertexIndex++)
		{
			int vbIndex = reverseMatching  ?  ( dest.size() - 1 ) - vertexIndex  :  vertexIndex;

			MVertex *va = src[vertexIndex];
			MVertex *vb = dest[vbIndex];

			MEdge *newEdge;
			MFace *faceB = faceA->split( va, vb, newEdge );

			gs_assert( faceB != NULL, "MFace::nCutPerformSplits(): split failed (N:N)\n" );

			newEdge->edgeMark();

			if ( splitEdges != NULL )
			{
				splitEdges->push_back( newEdge );
			}

			// Not finished yet....
			if ( vertexIndex < ( dest.size() - 1 ) )
			{
				// Determine which face from @faceA and @faceB to work from next
				int vbNextIndex = reverseMatching  ?  ( dest.size() - 1 )  -  ( vertexIndex + 1 )   :   vertexIndex + 1;
				MVertex *vaNext = src[vertexIndex + 1];
				MVertex *vbNext = dest[vbNextIndex];
				if ( faceA->isIncidentTo( vaNext )  &&  faceA->isIncidentTo( vbNext ) )
				{
					// face A
				}
				else if ( faceB->isIncidentTo( vaNext )  &&  faceB->isIncidentTo( vbNext ) )
				{
					// face B
					faceA = faceB;
				}
				else
				{
					gs_assert_not_reached( "MFace::nCutPerformSplits(): could not determine next face (N:N)\n" );
				}
			}
		}
	}
}







// ******************* ROUND OPS *******************

void MFace::round()
{
	MVertexList splitOffVertices;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( !vertices[i].vertex->isSecondaryMarked() )
		{
			splitOffVertices.push_back( vertices[i].vertex );
		}
	}

	MEdge *newEdge;
	MFace *f = this, *f2;
	for (int i = 0; i < splitOffVertices.size(); i++)
	{
		f2 = f->splitOffTriangleIncidentToVertex( splitOffVertices[i], newEdge );

		if ( f->isIncidentTo( splitOffVertices[i] ) )
		{
			f->faceUnmark();
			f = f2;
		}
		else
		{
			f2->faceUnmark();
		}
	}
}



// ******************* EDGE SPLIT OPS *******************

MVertexAttrib * MFace::edgeSplit(int edgeIndex, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, double t, MVertexAttrib *useAttrib)
{
	MEdge *edge = vertices[edgeIndex].edge;

	//no need for nextIndex() as edgeBIndex is used as a position to insert
	//an edge; if edgeIndex == edges.size() - 1, then edgeBIndex == edges.size()
	//and the insert() method will insert a space at the end of the edge and
	//vertex lists
	int edgeBIndex = edgeIndex + 1;



	if ( edge != newEdgeA  &&  edge != newEdgeB )
	{
		edge->removeFace( this, false, false );
	}

	if ( edge != newEdgeA )
	{
		newEdgeA->addFace( this, vertices[edgeIndex].vertex, edgeIndex );
	}
	if ( edge != newEdgeB )
	{
		newEdgeB->addFace( this, vertices[edgeIndex].vertex, edgeBIndex );
	}

	splitVertex->addFace( this );



	vertices[edgeIndex].edge = newEdgeA;
	vertices.insert( edgeBIndex, Vertex() );
	vertices[edgeBIndex].vertex = splitVertex;
	vertices[edgeBIndex].edge = newEdgeB;

	int vertexCIndex = nextIndex( edgeBIndex, vertices.size() );

	if ( useAttrib == NULL )
	{
		MVertexAttrib splitVertexAttrib = MVertexAttrib( Point2f::weightedAverage( vertices[edgeIndex].attrib->getPoint(), 1.0 - t, vertices[vertexCIndex].attrib->getPoint(), t ), materialID );
		//force the creation of a vertex attribute, even if an equivalent one
		//already exists
		useAttrib = splitVertex->createVertexAttrib( splitVertexAttrib, MFINDPOLICY_FORCEDCREATE );
	}
	vertices[edgeBIndex].attrib = useAttrib;
	vertices[edgeBIndex].attrib->ref();
	vertices[edgeBIndex].normal = NULL;


	//if edge == newEdgeB, then edge's index to this will be incorrect, and
	//it will need refreshing, so refresh from edgeBIndex instead of
	//edgeBIndex + 1
	refreshEdgeIndices( edgeBIndex );


	//face has been modified
	faceModified();
	invalidateTesselation();

	return useAttrib;
}

MVertexAttrib * MFace::splitEdge(MEdge *edge, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, double t, MVertexAttrib *useAttrib)
{
	int edgeIndex = edge->getFaceIndex( this );

	gs_assert( edgeIndex != -1, "MFace::splitEdge(): could not find edge to split\n" );

	return edgeSplit( edgeIndex, newEdgeA, splitVertex, newEdgeB, t, useAttrib );
}

MVertexAttrib* MFace::splitEdge(MEdge *edge, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, MVertex *fromVertex, MVertexAttribAdjustList &vertexAttribAdjusts,
							double scaleFactor, MVertexAttrib *useAttrib)
{
	int edgeIndex = edge->getFaceIndex( this );

	gs_assert( edgeIndex != -1, "MFace::splitEdge(): could not find edge to split\n" );

	int nextVertexIndex = nextIndex( edgeIndex, vertices.size() );

	int startVertexIndex, endVertexIndex;

	double fraction;

	if ( vertices[edgeIndex].vertex == fromVertex )
	{
		startVertexIndex = edgeIndex;
		endVertexIndex = nextVertexIndex;
		fraction = 0.0;
	}
	else
	{
		startVertexIndex = nextVertexIndex;
		endVertexIndex = edgeIndex;
		fraction = 1.0;
	}



	Point2f attribAtZero;
	Vector2f attribVector;


	if ( useAttrib == NULL )
	{
		attribAtZero = vertices[startVertexIndex].attrib->getPoint();
		attribVector = vertices[endVertexIndex].attrib->getPoint() - attribAtZero;
		attribVector *= scaleFactor;
	}

	MVertexAttrib *splitVertexAttrib = edgeSplit( edgeIndex, newEdgeA, splitVertex, newEdgeB, fraction, useAttrib );

	if ( useAttrib == NULL )
	{
		vertexAttribAdjusts.push_back( MVertexAttribAdjust( splitVertexAttrib, attribAtZero, attribVector, 0.0, 1.0 / scaleFactor ) );
	}


	//face has been modified
	faceModified();

	return splitVertexAttrib;
}




// ******************* FACE SPLIT OPS *******************

MFace * MFace::split(int vaIndex, int vbIndex, MEdge*& newEdge)
{
	//NOTE: NO VERTICES OR EDGES ARE DESTROYED BY THIS OPERATION

	//initialise newEdge to NULL
	newEdge = NULL;

	//first, check to see if the vertices are adjacent to one another.
	//If they are, they are linked by an edge, and no face split can
	//be performed; return NULL
	if ( vbIndex  ==  prevIndex( vaIndex, vertices.size() ) )
	{
		return NULL;
	}
	if ( vbIndex  ==  nextIndex( vaIndex, vertices.size() ) )
	{
		return NULL;
	}


	if ( vaIndex > vbIndex )
	{
		std::swap( vaIndex, vbIndex );
	}

	MVertex *va = vertices[vaIndex].vertex;
	MVertex *vb = vertices[vbIndex].vertex;

	//vertex ordering in this will be:
	//this:		va, va+1, va+2, ...... vb
	//newFace:	vb, vb+1, vb+2, ...... last, 0, 1, ...... va
	//so edge order is vb va
	newEdge = vb->findEdgeTo( va, MFINDPOLICY_CREATE );


	//CHECK newEdge TO SEE IF IT ALREADY HAS INCIDENT FACES, IF SO, THE SPLIT
	//CANNOT PROCEED AS IT WOULD INVOLVE ADDING 2 FACES TO THIS EDGE, WHICH
	//COULD NOT BE DONE
	if ( newEdge->getNumFaces() > 0 )
	{
		return NULL;
	}


	vertices.rotateForward( vaIndex );


	vbIndex -= vaIndex;
	vaIndex = 0;


	MFace *newFace = getMesh()->newFace();
	newFace->plane = getPlane();


	int newFaceSize = vertices.size() - ( vbIndex - 1 );
	newFace->vertices.reserve( newFaceSize );


	//copy the vertices and edges over to the new face
	//note that the last one is missed off, this is added later
	newFace->vertices.extend( vertices, vbIndex, vbIndex + newFaceSize - 1 );



	//replace this face in all vertices moved over, except the first, since
	//it will be used by both faces
	//replace this face in all edges moved over
	newFace->vertices[0].edge->replaceFace( this, newFace, 0 );
	for (int i = 1; i < newFace->vertices.size(); i++)
	{
		newFace->vertices[i].edge->replaceFace( this, newFace, i );
		newFace->vertices[i].vertex->replaceFace( this, newFace );
	}

	//add vertex A to the new face
	//add the new edge to the new face
	Vertex v;
	v.vertex = va;
	v.edge = newEdge;
	v.attrib = vertices[vaIndex].attrib;
	newFace->vertices.push_back( v );

	//reference the vertex attributes that are shared by both faces
	newFace->vertices[0].attrib->ref();
	newFace->vertices.back().attrib->ref();


	//vertex A and vertex B are used by both this face and the new face
	//add the new face to their face lists
	va->addFace( newFace );
	vb->addFace( newFace );

	//remove the vertices and edges in the new face from this face
	vertices.resize( vbIndex + 1 );
	//add the new edge to this face's edge list
	vertices[vbIndex].edge = newEdge;

	//the new edge must have links to both faces
	newEdge->addFace( this, vb, vbIndex );
	newEdge->addFace( newFace, va, newFace->vertices.size() - 1 );	//last edge in newFace


	//copy properties to the new face
	newFace->copyPropertiesFrom( this );


	refreshEdgeIndices( 0 );
	//newFace->refreshEdgeIndices( 0 );	//no need; already done in replaceFace()
													//and newEdge->addFace() methods


	//face has been modified
	faceModified();
	newFace->faceModified();
	invalidateTesselation();
	newFace->invalidateTesselation();

	return newFace;
}

MFace * MFace::split(MVertex *va, MVertex *vb, MEdge*& newEdge)
{
	int vaIndex = findVertex( va );

	gs_assert( vaIndex != -1, "MFace::split(): vertex @va is not incident to @this\n" );

	int vbIndex = findVertex( vb );

	gs_assert( vbIndex != -1, "MFace::split(): vertex @vb is not incident to @this\n" );


	return split( vaIndex, vbIndex, newEdge );
}

MFace * MFace::splitOffTriangleIncidentToVertex(MVertex *v, MEdge*& newEdge)
{
	int index = findVertex( v );

	gs_assert( index != -1, "MFace::splitOffTriangleIncidentToVertex(): vertex @v is not incident to @this\n" );

	int prevVtxIndex = prevIndex( index, vertices.size() );
	int nextVtxIndex = nextIndex( index, vertices.size() );

	//no need to disable tesselation on the triangle, since this is done in
	//split()
	return split( prevVtxIndex, nextVtxIndex, newEdge );
}

MFace * MFace::splitOffQuadIncidentToEdge(MEdge *e, MEdge*& newEdge)
{
	int index = e->getFaceIndex( this );

	gs_assert( index != -1, "MFace::splitOffQuadIncidentToEdge(): edge @e is not incident to @this\n" );

	int prevVtxIndex = prevIndex( index, vertices.size() );
	int nextVtxIndex = nextIndex( index, vertices.size() );
	int next2VtxIndex = nextIndex( nextVtxIndex, vertices.size() );

	MFace *face = split( prevVtxIndex, next2VtxIndex, newEdge );


	return face;
}

double MFace::computeVertexVertexSplitSuitability(int vaIndex, int vbIndex, bool &degenerateSubFaces) const
{
	degenerateSubFaces = false;
	if ( vaIndex == vbIndex )
	{
		return 0.0;
	}

	Vector3 normalA = computeSubfaceTwiceAreaNormalProduct( vaIndex, vbIndex );
	Vector3 normalB = computeSubfaceTwiceAreaNormalProduct( vbIndex, vaIndex );

	if ( normalA.sqrLength() < EPSILON  ||  normalB.sqrLength() < EPSILON )
	{
		degenerateSubFaces = true;
		return 0.0;
	}

	normalA.normalise();
	normalB.normalise();

	return normalA.dot( normalB );
}

double MFace::computeVertexVertexSplitSuitability(MVertex *va, MVertex *vb, bool &degenerateSubFaces) const
{
	if ( va == vb )
	{
		return 0.0;
	}

	int vaIndex = findVertex( va );
	int vbIndex = findVertex( vb );

	gs_assert( vaIndex != -1, "MFace::computeVertexVertexSplitSuitability(): @va is not incident to @this\n" );
	gs_assert( vbIndex != -1, "MFace::computeVertexVertexSplitSuitability(): @vb is not incident to @this\n" );

	return computeVertexVertexSplitSuitability( vaIndex, vbIndex, degenerateSubFaces );
}

double MFace::computeVertexEdgeSplitSuitability(int vIndex, int eIndex) const
{
	if ( vIndex == eIndex  ||  vIndex == nextIndex( eIndex, vertices.size() ) )
	{
		return 0.0;
	}

	// Compute the weighted normal of the triangle formed by the vertex and the edge
	MVertex *v = vertices[vIndex].vertex;
	MVertex *edgeVertex = vertices[eIndex].vertex;
	Vector3 eToV = v->getPosition() - edgeVertex->getPosition();
	Vector3 halfVertexEdgeTriNormal = vertices[eIndex].edge->getEdgeVector( edgeVertex ).cross( eToV )  *  0.5;

	Vector3 normalA = computeSubfaceTwiceAreaNormalProduct( vIndex, eIndex )  +  halfVertexEdgeTriNormal;
	Vector3 normalB = computeSubfaceTwiceAreaNormalProduct( nextIndex( eIndex, vertices.size() ), vIndex )  +  halfVertexEdgeTriNormal;

	if ( normalA.sqrLength() < EPSILON  ||  normalB.sqrLength() < EPSILON )
	{
		return 0.0;
	}

	normalA.normalise();
	normalB.normalise();

	return normalA.dot( normalB );
}

double MFace::computeVertexEdgeSplitSuitability(MVertex *v, MEdge *e) const
{
	int vIndex = findVertex( v );
	int eIndex = e->getFaceIndex( this );

	gs_assert( vIndex != -1, "MFace::computeVertexEdgeSplitSuitability(): @v is not incident to @this\n" );
	gs_assert( eIndex != -1, "MFace::computeVertexEdgeSplitSuitability(): @e is not incident to @this\n" );

	return computeVertexEdgeSplitSuitability( vIndex, eIndex );
}

double MFace::computeEdgeEdgeSplitSuitability(int eaIndex, int ebIndex) const
{
	if ( eaIndex == ebIndex )
	{
		return 0.0;
	}

	// Compute the weighted normal of the quad formed by the vertex and the edge
	MVertex *va = vertices[eaIndex].vertex;
	MVertex *vb = vertices[ebIndex].vertex;
	Vector3 eaToEbV = vb->getPosition() - va->getPosition();
	Vector3 eaToEbVTriNormal = vertices[eaIndex].edge->getEdgeVector( va ).cross( eaToEbV );
	Vector3 eaVToEbTriNormal = vertices[ebIndex].edge->getEdgeVector( vb ).cross( -eaToEbV );
	Vector3 halfEdgeEdgeQuadNormal = ( eaToEbVTriNormal + eaVToEbTriNormal ) * 0.5;

	Vector3 normalA = computeSubfaceTwiceAreaNormalProduct( nextIndex( eaIndex, vertices.size() ), ebIndex )  +  halfEdgeEdgeQuadNormal;
	Vector3 normalB = computeSubfaceTwiceAreaNormalProduct( nextIndex( ebIndex, vertices.size() ), eaIndex )  +  halfEdgeEdgeQuadNormal;

	if ( normalA.sqrLength() < EPSILON  ||  normalB.sqrLength() < EPSILON )
	{
		return 0.0;
	}

	normalA.normalise();
	normalB.normalise();

	return normalA.dot( normalB );
}

double MFace::computeEdgeEdgeSplitSuitability(MEdge *ea, MEdge *eb) const
{
	int eaIndex = ea->getFaceIndex( this );
	int ebIndex = eb->getFaceIndex( this );

	gs_assert( eaIndex != -1, "MFace::computeVertexEdgeSplitSuitability(): @ea is not incident to @this\n" );
	gs_assert( ebIndex != -1, "MFace::computeVertexEdgeSplitSuitability(): @eb is not incident to @this\n" );

	return computeVertexEdgeSplitSuitability( eaIndex, ebIndex );
}

MFace * MFace::splitWith(int vaIndex, int vbIndex, const Array<MVertex*> &splitVerts, const Array<MVertexAttrib*> &splitAttribs, MEdgeList &splitEdges)
{
	//NOTE: NO VERTICES OR EDGES ARE DESTROYED BY THIS OPERATION

	MVertex *va = vertices[vaIndex].vertex;
	MVertex *vb = vertices[vbIndex].vertex;

	// Test the potential split edges; see if any of them already exist. If so, the operation cannot proceed if a split
	// edge has any incident faces
	MEdge *edgeToTest = NULL;
	edgeToTest = vertices[vaIndex].vertex->findEdgeTo( splitVerts.front(), MFINDPOLICY_RETURNNULL );
	if ( edgeToTest != NULL )
	{
		if ( edgeToTest->getNumFaces() > 0 )
		{
			return NULL;
		}
	}
	for (int splitVertexI = 1; splitVertexI < splitVerts.size(); splitVertexI++)
	{
		edgeToTest = splitVerts[splitVertexI - 1]->findEdgeTo( splitVerts[splitVertexI], MFINDPOLICY_RETURNNULL );
		if ( edgeToTest != NULL )
		{
			if ( edgeToTest->getNumFaces() > 0 )
			{
				return NULL;
			}
		}
	}
	edgeToTest = splitVerts.back()->findEdgeTo( vertices[vbIndex].vertex, MFINDPOLICY_RETURNNULL );
	if ( edgeToTest != NULL )
	{
		if ( edgeToTest->getNumFaces() > 0 )
		{
			return NULL;
		}
	}

	//Create the split edges
	splitEdges.resize( splitVerts.size() + 1 );
	splitEdges.front() = vertices[vaIndex].vertex->findEdgeTo( splitVerts.front(), MFINDPOLICY_CREATE );
	for (int splitVertexI = 1; splitVertexI < splitVerts.size(); splitVertexI++)
	{
		splitEdges[splitVertexI] = splitVerts[splitVertexI - 1]->findEdgeTo( splitVerts[splitVertexI], MFINDPOLICY_CREATE );
	}
	splitEdges.back() = splitVerts.back()->findEdgeTo( vertices[vbIndex].vertex, MFINDPOLICY_CREATE );



	bool reverseOrder = vaIndex > vbIndex;
	if ( reverseOrder )
	{
		std::swap( vaIndex, vbIndex );
		std::swap( va, vb );
	}

	// If x0, ...... xn-1 represents @splitVerts  (in reverse order if @reverseOrder is true)
	// The vertex ordering of the new faces will be:
	// @this:			va, va+1, va+2, ...... vb, xn-1, xn-2, ...... x0
	// @newFace: 		vb, vb+1, vb+2, ...... last, 0, 1, ...... va, x0, x1, ...... xn-1


	vertices.rotateForward( vaIndex );


	vbIndex -= vaIndex;
	vaIndex = 0;


	MFace *newFace = getMesh()->newFace();
	newFace->plane = getPlane();


	int numNewFaceVertsFromThis = vertices.size() - ( vbIndex - 1 );
	int newFaceSize = numNewFaceVertsFromThis  +  splitVerts.size();
	newFace->vertices.reserve( newFaceSize );


	// Copy the vertices and edges oin range [vb ... va] to @newFace
	// Note that @va is added later
	newFace->vertices.extend( vertices, vbIndex, vbIndex + numNewFaceVertsFromThis - 1 );
	// Link @vb to @newFace (@vb is currently at the front of @newFace->vertices)
	newFace->vertices.front().vertex->addFace( newFace );
	// Ref-count the vertex attrib corresponding to @vb, as it is shared by @this and @newFace
	newFace->vertices.front().attrib->ref();



	// Replace references to @this in all the vertices that were moved over, except @vb, since @vb is shared by @this and @newFace
	// Replace references to @this in all the edges that were moved over
	newFace->vertices[0].edge->replaceFace( this, newFace, 0 );
	for (int vertexI= 1; vertexI < newFace->vertices.size(); vertexI++)
	{
		newFace->vertices[vertexI].edge->replaceFace( this, newFace, vertexI );
		newFace->vertices[vertexI].vertex->replaceFace( this, newFace );
	}

	// Add @va to @newFace
	// Add the first edge in @splitEdges to @newFace
	Vertex faceVert;
	faceVert.vertex = va;
	faceVert.edge = reverseOrder  ?  splitEdges.back()  :  splitEdges.front();
	faceVert.attrib = vertices[vaIndex].attrib;
	// Link @va to @newFace
	faceVert.vertex->addFace( newFace );
	// Link the edge to @this
	faceVert.edge->addFace( newFace, faceVert.vertex, newFace->vertices.size() );
	// Ref-count the shared vertex attrib
	faceVert.attrib->ref();
	newFace->vertices.push_back( faceVert );

	// Add the split vertices to @newFace
	for (int vertexI = 0; vertexI < splitVerts.size(); vertexI++)
	{
		Vertex faceVert;
		faceVert.vertex = reverseOrder  ?  splitVerts.reverseAt( vertexI )  :  splitVerts[vertexI];
		faceVert.edge = reverseOrder  ?  splitEdges.reverseAt( vertexI + 1 )  :  splitEdges[vertexI+1];
		faceVert.attrib = reverseOrder  ?  splitAttribs.reverseAt( vertexI )  :  splitAttribs[vertexI];
		// Link the vertex to @newFace
		faceVert.vertex->addFace( newFace );
		// Link the edge to @newFace
		faceVert.edge->addFace( newFace, faceVert.vertex, newFace->vertices.size() );
		// Ref-count the vertex attrib
		faceVert.attrib->ref();
		newFace->vertices.push_back( faceVert );
	}


	// Remove the vertices and edges that were move from @this to @newFace, from @vertices
	vertices.resize( vbIndex + 1 );

	// Replace the last edge in @vertices with the last split edge
	vertices.back().edge = reverseOrder  ?  splitEdges.front()  :  splitEdges.back();
	// Link the edge to @this
	vertices.back().edge->addFace( this, vertices.back().vertex, vertices.size() - 1 );

	// Add the split vertices and corresponding split edges to @this, in the opposite order to that in which they were added to @newFace
	for (int vertexI = 0; vertexI < splitVerts.size(); vertexI++)
	{
		Vertex faceVert;
		faceVert.vertex = reverseOrder  ?  splitVerts[vertexI]  :  splitVerts.reverseAt( vertexI );
		faceVert.edge = reverseOrder  ?  splitEdges[vertexI+1]  :  splitEdges.reverseAt( vertexI + 1 );
		faceVert.attrib = reverseOrder  ?  splitAttribs[vertexI]  :  splitAttribs.reverseAt( vertexI );
		// Link the vertex to @this
		faceVert.vertex->addFace( this );
		// Link the edge to @this
		faceVert.edge->addFace( this, faceVert.vertex, vertices.size() );
		// Ref-count the vertex attrib
		faceVert.attrib->ref();
		vertices.push_back( faceVert );
	}


	//copy properties to the new face
	newFace->copyPropertiesFrom( this );


	refreshEdgeIndices( 0 );
	//newFace->refreshEdgeIndices( 0 );	//no need; already done in replaceFace()
													//and newEdge->addFace() methods


	//face has been modified
	faceModified();
	newFace->faceModified();
	invalidateTesselation();
	newFace->invalidateTesselation();

	return newFace;
}

MFace * MFace::splitWith(MVertex *va, MVertex *vb, const Array<MVertex*> &splitVerts, const Array<MVertexAttrib*> &splitAttribs, MEdgeList &splitEdges)
{
	int vaIndex = findVertex( va );

	gs_assert( vaIndex != -1, "MFace::splitWith(): vertex @va is not incident to @this\n" );

	int vbIndex = findVertex( vb );

	gs_assert( vbIndex != -1, "MFace::splitWith(): vertex @vb is not incident to @this\n" );


	return splitWith( vaIndex, vbIndex, splitVerts, splitAttribs, splitEdges );
}










// ******************* IMPORT MESH OS *******************

void MFace::importMeshSanityCheck(MFaceList &resultFaces)
{
	if ( !resultFaces.contains( this ) )
	{
		resultFaces.push_back( this );
	}

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		MVertex *a = vertices[vertexI].vertex;
		for (int vertexJ = vertexI + 1; vertexJ < vertices.size(); vertexJ++)
		{
			MVertex *b = vertices[vertexJ].vertex;

			if ( a == b )
			{
				MEdge *newEdge;
				MFace *f = split( prevIndex( vertexJ, vertices.size() ), nextIndex( vertexJ, vertices.size() ), newEdge );
				resultFaces.push_back( f );

				if ( f->getSize() > 3 )
				{
					f->importMeshSanityCheck( resultFaces );
				}
				if ( getSize() > 3 )
				{
					importMeshSanityCheck( resultFaces );
				}
				break;
			}
		}
	}
}



// ******************* EDGE COLLAPSE OPS *******************

void MFace::edgeCollapseAttribs(int edgeIndex, MCollapseEdgeTo collapseTo)
{
	int nextVtxIndex = nextIndex( edgeIndex, vertices.size() );


	if ( collapseTo == MCOLLAPSEEDGETO_MIDPOINT )
	{
		const Point2f &a = vertices[edgeIndex].attrib->getPoint();
		const Point2f &b = vertices[nextVtxIndex].attrib->getPoint();

		Point2f centrePoint = Point2f::average( a, b );
		MVertexAttrib centreAttrib( centrePoint, materialID );

		MVertexAttrib *attrib[2];
		attrib[0] = vertices[edgeIndex].vertex->createVertexAttrib( centreAttrib, MFINDPOLICY_CREATE );
		attrib[1] = vertices[nextVtxIndex].vertex->createVertexAttrib( centreAttrib, MFINDPOLICY_CREATE );

		vertices[edgeIndex].vertex->replaceVertexAttrib( vertices[edgeIndex].attrib, attrib[0] );
		vertices[nextVtxIndex].vertex->replaceVertexAttrib( vertices[nextVtxIndex].attrib, attrib[1] );
	}
	else if ( collapseTo == MCOLLAPSEEDGETO_B )
	{
		MVertexAttrib *attrib = vertices[edgeIndex].vertex->createVertexAttrib( *vertices[nextVtxIndex].attrib, MFINDPOLICY_CREATE );
		vertices[edgeIndex].vertex->replaceVertexAttrib( vertices[edgeIndex].attrib, attrib );
	}
	else if ( collapseTo == MCOLLAPSEEDGETO_A )
	{
		MVertexAttrib *attrib = vertices[nextVtxIndex].vertex->createVertexAttrib( *vertices[edgeIndex].attrib, MFINDPOLICY_CREATE );
		vertices[nextVtxIndex].vertex->replaceVertexAttrib( vertices[nextVtxIndex].attrib, attrib );
	}

	//face has been modified
	//faceModified();	//not needed; only vertex attributes are modified
}





// ******************* FACE COLLAPSE TO VERTEX OPS *******************

MVertex * MFace::collapseToVertex()
{
	Point3 vertexPosition = computeCentroid();
	Point2f attribCentroid = computeAttribCentroid().getPoint();

	// Reposition all the vertex attributes
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		vertices[vertexI].attrib->setPoint( attribCentroid );
	}

	//Attemp to collapse all incident edges, until this face is a triangle
	for (int i = 0; i < vertices.size(); i++)
	{
		//when this is a triangle, break out of the for loop
		if ( vertices.size() <= 3 )
		{
			break;
		}

		//collapse the edge
		MVertex *v = vertices[i].edge->collapse( vertexPosition, false, MCOLLAPSEEDGETO_MIDPOINT );

		//v != NULL if the collapse succeeded
		if ( v != NULL )
		{
			//edge list changed, start over
			i = 0;
		}
	}

	//if this is not a triangle, then the edge collapses were not
	//successful, so the collapse cannot proceed any further
	if ( vertices.size() > 3 )
	{
		return NULL;
	}


	//attempt to collapse each of the 3 edges, hopefully one will succeed
	int j = 2;
	MVertex *vertexOppositeEdge = NULL, *newVertex = NULL;
	for (int i = 0; i < vertices.size(); i++)
	{
		//take note of the vertex opposite the edge that is going to be
		//collapsed
		vertexOppositeEdge = vertices[j].vertex;
		MEdge *edge = vertices[i].edge;

		//attemp an edge collapse
		newVertex = edge->collapse( vertexPosition, false, MCOLLAPSEEDGETO_MIDPOINT );

		if ( newVertex != NULL )
		{
			//success, break out of the for loop
			break;
		}

		j = i;
	}

	//if newVertex == NULL, then the for loop terminated on its own,
	//and none of the edges could be collapsed
	if ( newVertex == NULL )
	{
		return NULL;
	}

	//face is now collapsed into an edge
	//find this edge
	MEdge *edge = vertexOppositeEdge->findEdgeTo( newVertex, MFINDPOLICY_RETURNNULL );

	gs_assert( edge != NULL, "MFace::collapse(): could not find edge of collapsed faces\n" );

	//try to collapse the remaining edge
	return edge->collapse( vertexPosition, false, MCOLLAPSEEDGETO_MIDPOINT );
}





// ******************* EDGE DISSOLVE OPS *******************

void MFace::mergeInto(MFace *destFace, const Array<int> &sharedEdgeIndices)
{
	//build a list of shared edges, and shared internal vertices (excludes
	//the vertices at the beginning and the end of the edge run)
	Array<MEdge*> sharedEdges;
	Array<MVertex*> sharedInternalVertices;
	sharedEdges.reserve( sharedEdgeIndices.size() );
	sharedInternalVertices.reserve( sharedEdgeIndices.size() - 1 );
	MVertexAttrib *boundaryAttribs[2];

	for (int i = 0; i < sharedEdgeIndices.size(); i++)
	{
		sharedEdges.push_back( vertices[ sharedEdgeIndices[i] ].edge );
	}

	for (int i = 1; i < sharedEdgeIndices.size(); i++)
	{
		MVertex *v = vertices[ sharedEdgeIndices[i] ].vertex;
		sharedInternalVertices.push_back( v );
	}



	//build a list of vertices and edges that can be moved from @this to
	//@destFace
	Array<MVertex*> verticesToMove;
	Array<MVertexAttrib*> attribsToMove;
	Array<MEdge*> edgesToMove;

	//compute the number of each item to move, and reserve space
	int numSharedVertices = sharedEdgeIndices.size() + 1;
	int numVerticesToMove = vertices.size()  -  numSharedVertices;
	int numEdgesToMove = vertices.size()  -  sharedEdgeIndices.size();
	verticesToMove.reserve( numVerticesToMove );
	attribsToMove.reserve( numVerticesToMove );
	edgesToMove.reserve( numEdgesToMove );


	//@sourceIndex is the index into the Array @vertices; the source of the
	//objects to move. Start at the position after the index of the last
	//shared edge, and move 1 space onward. This is the position of the first
	//edge that will be moved into @destFace.
	int sourceIndex = sharedEdgeIndices.back();
	sourceIndex = nextIndex( sourceIndex, vertices.size() );
	// Note the attributes at the start and the end of the run of shared edges
	boundaryAttribs[0] = vertices[sourceIndex].attrib;

	//put the objects to move into the Arrays
	//the first edge
	edgesToMove.push_back( vertices[sourceIndex].edge );
	//continue with the vertices, attribs, and the remaining edges
	//move onto the next index
	sourceIndex = nextIndex( sourceIndex, vertices.size() );
	for (int i = 0; i < numVerticesToMove; i++)
	{
		verticesToMove.push_back( vertices[sourceIndex].vertex );
		attribsToMove.push_back( vertices[sourceIndex].attrib );
		edgesToMove.push_back( vertices[sourceIndex].edge );
		sourceIndex = nextIndex( sourceIndex, vertices.size() );
	}
	boundaryAttribs[1] = vertices[sourceIndex].attrib;



	//MOVE THE VERTICES AND EDGES INTO @destFace

	//detach all edges and vertices from @this
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].vertex->removeFace( this );
		vertices[i].edge->removeFace( this, false, false );
		//do not bother unref()'ing the attribs, as their reference count
		//DOES NOT CHANGE as they are moved from one face to another
	}


	//We need to know where in @destFace the objects that we are moving in,
	//need to go. The merge operation replaces the shared edges in
	//@destFace, with the edges from @this that are NOT shared.
	//The first edge from @this will be put in the place of the first
	//shared edge in @destFace. Note that since the shared edges will
	//appear in @destFace in the opposite order to which they appear in
	//@this, start at the last shared edge, from the perspective of @this.
	int destEdgesOffset = sharedEdges.back()->getFaceIndex( destFace );


	//detach @destFace from the shared edges (this should result in the
	//destruction of the shared edges)
	for (int i = 0; i < sharedEdges.size(); i++)
	{
		sharedEdges[i]->removeFace( destFace, true, false );
	}
	//destroy the internal shared vertices if they are isolated
	for (int i = 0; i < sharedInternalVertices.size(); i++)
	{
		sharedInternalVertices[i]->destroyIfIsolated();
	}


	//The number of shared internal vertices may not equal the number of
	//vertices that replace them; determine the number of spaces that need
	//to be created in order to accomodate them. If the number is negative,
	//then spaces need to be removed
	int spaceRequired = verticesToMove.size() - sharedInternalVertices.size();

	int destVerticesOffset;

	//enlarge/shrink the vertex Array in @destFace
	if ( spaceRequired > 0 )
	{
		//no need to use nextIndex(), as the call to insert() will ensure
		//that the index of @destEdgesOffset + 1 is valid
		destVerticesOffset = destEdgesOffset + 1;
		//need extra space; insert it
		destFace->vertices.insert( destVerticesOffset, spaceRequired, Vertex() );
	}
	else if ( spaceRequired < 0 )
	{
		//initially compute @destVerticesOffset without wrapping
		destVerticesOffset = destEdgesOffset + 1;

		//need less spaces; remove some space.
		//note that the shared edges may wrap over the end, so use the
		//wrappedRemove() function
		int offset = destFace->vertices.wrappedRemove( destVerticesOffset, -spaceRequired );
		//the value in offset indicates the number of spaces that indices
		//need to be moved back in order to point at the correct location,
		//as wrappedRemove() may have move the contents forward in the Array
		//by @offset spaces
		destEdgesOffset -= offset;

		//in the case where @destVerticesOffset was given the value of
		//@destFace->vertices.size(), the wrapped remove will have succeeded,
		//however, subtracting @offset from @destVerticesOffset will
		//not be sufficient since it will still point to the end of the
		//(shorter) Array, so wrapping is required here
		destVerticesOffset = nextIndex( destEdgesOffset, destFace->vertices.size() );
	}
	else
	{
		//need wrapping in this case
		destVerticesOffset = nextIndex( destEdgesOffset, destFace->vertices.size() );
	}



	//move vertices and attribs into @destFace
	int destVertexIndex = destVerticesOffset;
	for (int i = 0; i < verticesToMove.size(); i++)
	{
		destFace->vertices[destVertexIndex].vertex = verticesToMove[i];
		destFace->vertices[destVertexIndex].attrib = attribsToMove[i];
		destFace->vertices[destVertexIndex].normal = NULL;
		//add @destFace to each vertex that is moved in
		verticesToMove[i]->addFace( destFace );
		destVertexIndex = nextIndex( destVertexIndex, destFace->vertices.size() );
	}
	//move edges info @destFace
	int destEdgeIndex = destEdgesOffset;
	for (int i = 0; i < edgesToMove.size(); i++)
	{
		destFace->vertices[destEdgeIndex].edge = edgesToMove[i];
		//add @destFace to each edge that is moved in
		edgesToMove[i]->addFace( destFace, destFace->vertices[destEdgeIndex].vertex, destEdgeIndex );
		destEdgeIndex = nextIndex( destEdgeIndex, destFace->vertices.size() );
	}

	//unref the boundary attribs, as they are now only used by one face
	boundaryAttribs[0]->unref();
	boundaryAttribs[1]->unref();


	//DESTROY @this
	//clear the Array @vertices (contains pointers to vertices, edges, attribs,
	//etc) first, as these have already been detached from @this earlier;
	//destroy() will try to detach anything it finds in the Array @vertices;
	//it will try to detach them for a SECOND time, so make sure destroy() can't
	//find them
	vertices.clear();
	destroy( true, true );


	//@destFace has been modified
	destFace->refreshEdgeIndices( destEdgeIndex );

	destFace->faceModified();
	destFace->invalidateTesselation();
}




// ******************* PLANE OPS *******************

void MFace::refreshPlane() const
{
	if ( flags.planeRequiresRefresh )
	{
		// Compute the average normal
		Vector3 n = computeTwiceAreaNormalProduct();
		n.normalise();

		// Now that we have an average normal, compute the average distance
		double d = computeAverageDistanceAlongNormalisedVector( n );

		// Set the plane
		plane = Plane( n, d );


		flags.planeRequiresRefresh = false;
	}
}


// ******************* MISC OPS *******************

Point3 MFace::computeCentroid() const
{
	//deal with common special cases to increase speed
	if ( vertices.size() == 3 )
	{
		return Point3::average( vertices[0].vertex->getPosition(), vertices[1].vertex->getPosition(), vertices[2].vertex->getPosition() );
	}
	else if ( vertices.size() == 4 )
	{
		return Point3::average( vertices[0].vertex->getPosition(), vertices[1].vertex->getPosition(), vertices[2].vertex->getPosition(), vertices[3].vertex->getPosition() );
	}
	else
	{
		Point3 average = Point3::sum( vertices[0].vertex->getPosition(), vertices[1].vertex->getPosition(), vertices[2].vertex->getPosition(), vertices[3].vertex->getPosition() );
		for (int i = 4; i < vertices.size(); i++)
		{
			average.cumulativeAdd( vertices[i].vertex->getPosition() );
		}

		return Point3::mul( average, 1.0 / (double)vertices.size() );
	}
}

MVertexAttrib MFace::computeAttribCentroid() const
{
	//deal with common special cases to increase speed
	if ( vertices.size() == 3 )
	{
		return MVertexAttrib( Point2f::average( vertices[0].attrib->getPoint(), vertices[1].attrib->getPoint(), vertices[2].attrib->getPoint() ), materialID );
	}
	else if ( vertices.size() == 4 )
	{
		return MVertexAttrib( Point2f::average( vertices[0].attrib->getPoint(), vertices[1].attrib->getPoint(), vertices[2].attrib->getPoint(), vertices[3].attrib->getPoint() ), materialID );
	}
	else
	{
		Point2f average = Point2f::sum( vertices[0].attrib->getPoint(), vertices[1].attrib->getPoint(), vertices[2].attrib->getPoint(), vertices[3].attrib->getPoint() );
		for (int i = 4; i < vertices.size(); i++)
		{
			average.cumulativeAdd( vertices[i].attrib->getPoint() );
		}

		average.scale( 1.0 / (double)vertices.size() );

		return MVertexAttrib( average, materialID );
	}
}

BBox3 MFace::computeBBox() const
{
	if ( vertices.size() > 0 )
	{
		BBox3 result( vertices[0].vertex->getPosition() );
		for (int vertexI = 1; vertexI < vertices.size(); vertexI++)
		{
			result.addPoint( vertices[vertexI].vertex->getPosition() );
		}
		return result;
	}
	else
	{
		return BBox3();
	}
}


Vector3 MFace::computeTwiceAreaNormalProduct() const
{
	Vector3 n;

	const Point3 &a = vertices[0].vertex->getPosition();
	const Point3 &b = vertices[1].vertex->getPosition();
	Vector3 edge0 = b - a;

	for (int k = 2; k < vertices.size(); k++)
	{
		const Point3 &c = vertices[k].vertex->getPosition();
		Vector3 edge1 = c - a;

		n += edge0.cross( edge1 );

		edge0 = edge1;
	}

	return n;
}

Vector3 MFace::computeSubfaceTwiceAreaNormalProduct(int vaIndex, int vbIndex) const
{
	Vector3 n;

	int numVerts;
	if ( vbIndex > vaIndex )
	{
		numVerts = ( vbIndex - vaIndex )  +  1;
	}
	else
	{
		numVerts = ( vertices.size() + vbIndex - vaIndex )  +  1;
	}

	int numTris = numVerts - 2;

	if ( numTris < 1 )
	{
		return Vector3();
	}

	int aIndex = vaIndex;
	int bIndex = nextIndex( aIndex, vertices.size() );
	int cIndex = nextIndex( bIndex, vertices.size() );

	const Point3 &a = vertices[aIndex].vertex->getPosition();
	const Point3 &b = vertices[bIndex].vertex->getPosition();
	Vector3 edge0 = b - a;

	for (int i = 0; i < numTris; i++)
	{
		const Point3 &c = vertices[cIndex].vertex->getPosition();
		Vector3 edge1 = c - a;

		n += edge0.cross( edge1 );

		edge0 = edge1;
		cIndex = nextIndex( cIndex, vertices.size() );
	}

	return n;
}


double MFace::computeAverageDistanceAlongNormalisedVector(const Vector3 &v) const
{
	double d = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		d += vertices[i].vertex->getPosition().dot( v );
	}

	return d  *  ( 1.0 / (double)vertices.size() );
}


int MFace::getNumTriangles()
{
	const Tesselation *tess = getTesselation();

	if ( tess != NULL )
	{
		return tess->size();
	}
	else
	{
		return getSize() - 2;
	}
}

Triangle3 MFace::getTriangle(int tri)
{
	const Tesselation *tess = getTesselation();

	if ( tess != NULL )
	{
		const IndexTriangle &t = tess->at( tri );
		return Triangle3( vertices[ t.a ].vertex->getPosition(), vertices[ t.b ].vertex->getPosition(), vertices[ t.c ].vertex->getPosition() );
	}
	else
	{
		return Triangle3( vertices[0].vertex->getPosition(), vertices[ tri + 1 ].vertex->getPosition(), vertices[ tri + 2 ].vertex->getPosition() );
	}
}

IndexTriangle MFace::getIndexTriangle(int tri)
{
	const Tesselation *tess = getTesselation();

	if ( tess != NULL )
	{
		return tess->at( tri );
	}
	else
	{
		return IndexTriangle( 0,  tri + 1,  tri + 2 );
	}
}

Triangle2 MFace::getAttribTriangle(int tri)
{
	const Tesselation *tess = getTesselation();

	if ( tess != NULL )
	{
		const IndexTriangle &t = tess->at( tri );
		return Triangle2( vertices[ t.a ].attrib->getPoint().toPoint2(), vertices[ t.b ].attrib->getPoint().toPoint2(), vertices[ t.c ].attrib->getPoint().toPoint2() );
	}
	else
	{
		return Triangle2( vertices[0].attrib->getPoint().toPoint2(), vertices[ tri + 1 ].attrib->getPoint().toPoint2(), vertices[ tri + 2 ].attrib->getPoint().toPoint2() );
	}
}

void MFace::generatePolygon(Polygon3 &poly) const
{
	poly.resize( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		poly[i] = vertices[i].vertex->getPosition();
	}
}

void MFace::generate2DPointList(Array<Point2> &points) const
{
	//- first, project the vertices onto the face plane
	//- project these points onto an orthogonal plane which most closely resembles the
	//  face plane

	//find the orthogonal plane which most closely resembles the face plane
	const Plane &p = getPlane();
	Axis orthoPlaneNormal = p.n.getMajorAxis();

	points.resize( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		//project onto the face plane
		Point3 pointOnPlane = p.projectPoint( vertices[i].vertex->getPosition() );
		//-> 2D
		points[i] = pointOnPlane.toPoint2Axis( orthoPlaneNormal );
	}
}






bool MFace::hitsTriangle(int tri, const Segment3 &ray, double &t, double &u, double &v)
{
	return getTriangle( tri ).raytrace( ray, t, u, v );
}


bool MFace::hits(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint, int &tri)
{
	if ( backfaceCullingFlag )
	{
		if ( ray.a < getPlane() )
		{
			return false;
		}
	}

	double u, v;

	int numTris = getNumTriangles();
	for (int triIndex = 0; triIndex < numTris; triIndex++)
	{
		if ( hitsTriangle( triIndex, ray, t, u, v ) )
		{
			intersectionPoint = ray.getPoint( t );
			tri = triIndex;
			return true;
		}
	}

	return false;
}

bool MFace::hits(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint)
{
	int tri;

	return hits( ray, backfaceCullingFlag, t, intersectionPoint, tri );
}


bool MFace::pick(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint)
{
	const Plane &facePlane = getPlane();

	if ( backfaceCullingFlag )
	{
		if ( ray.a < facePlane )
		{
			return false;
		}
	}

	Point3 intersection;

	if ( ray.intersect( facePlane, t, intersection ) )
	{
		Polygon3 poly;
		Polygon2 projectedPoly;

		generatePolygon( poly );
		projection.clipAndProjectPoly( projectedPoly, poly );

		if ( projectedPoly.side( p )  ==  SIDE_POSITIVE )
		{
			intersectionPoint = intersection;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

MEdge * MFace::pickEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, bool faceMarkedOnlyFlag, Point3 &closestPoint)
{
	Polygon3 projectedPoly3;
	Polygon2 projectedPoly2;
	Array<int> clippedIndices;

	generatePolygon( projectedPoly3 );
	projection.clipAndProjectPoly( projectedPoly3, clippedIndices );


	if ( projectedPoly3.size() == 0 )
	{
		return NULL;
	}


	projectedPoly3.generatePolygon2( projectedPoly2 );

	if ( projectedPoly2.side( p )  !=  SIDE_POSITIVE )
	{
		return NULL;
	}


	double bestSqrDist = -1.0;
	MEdge *bestEdge = NULL;
	Point3 bestClosest;

	int vPrevIndex = projectedPoly2.size() - 1;
	int prevFaceVertexIndex = clippedIndices.back();
	for (int vIndex = 0; vIndex < projectedPoly2.size(); vIndex++)
	{
		int faceVertexIndex = clippedIndices[vIndex];

		if ( prevFaceVertexIndex != -1  ||  faceVertexIndex != -1 )
		{
			MEdge *edge = NULL;

			if ( prevFaceVertexIndex != -1 )
			{
				edge = vertices[prevFaceVertexIndex].edge;
			}
			else if ( faceVertexIndex != -1 )
			{
				edge = vertices[ prevIndex( faceVertexIndex, vertices.size() ) ].edge;
			}

			gs_assert( edge != NULL, "MFace::pickEdge(): could not get edge\n" );

			if ( !faceMarkedOnlyFlag  ||  edge->isFaceMarked() )
			{
				Segment2 seg( projectedPoly2[vPrevIndex], projectedPoly2[vIndex] );

				double t;
				Point2 closest = seg.closestPointTo( p, t );

				double sqrDist = closest.sqrDistanceTo( p );
				if ( sqrDist < bestSqrDist  ||  bestSqrDist == -1.0 )
				{
					bestSqrDist = sqrDist;
					bestEdge = edge;
					bestClosest = lerp( projectedPoly3[vPrevIndex], projectedPoly3[vIndex], t );
				}
			}
		}

		vPrevIndex = vIndex;
		prevFaceVertexIndex = faceVertexIndex;
	}

	closestPoint = screenToWorldMatrix.transformHomogeneous( bestClosest );

	return bestEdge;
}

MVertex * MFace::pickVertex(const Point2 &p, const Projection &projection, bool faceMarkedOnlyFlag)
{
	Polygon3 poly;
	Polygon2 projectedPoly;
	Array<int> clippedIndices;

	generatePolygon( poly );
	projection.clipAndProjectPoly( projectedPoly, clippedIndices, poly );


	if ( projectedPoly.side( p )  !=  SIDE_POSITIVE )
	{
		return NULL;
	}


	double bestSqrDist = -1.0;
	MVertex *bestVertex = NULL;

	for (int vIndex = 0; vIndex < projectedPoly.size(); vIndex++)
	{
		int faceVertexIndex = clippedIndices[vIndex];
		if ( faceVertexIndex != -1 )
		{
			// The vertex in question did not arise from clipping
			if ( !faceMarkedOnlyFlag  ||  vertices[faceVertexIndex].vertex->isFaceMarked() )
			{
				double sqrDist = projectedPoly[vIndex].sqrDistanceTo( p );
				if ( sqrDist < bestSqrDist  ||  bestSqrDist == -1.0 )
				{
					bestSqrDist = sqrDist;
					bestVertex = vertices[faceVertexIndex].vertex;
				}
			}
		}
	}

	return bestVertex;
}

MVertex * MFace::pickVertexThruEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, bool faceMarkedOnlyFlag, MEdge *&thruEdge)
{
	Polygon3 projectedPoly3;
	Polygon2 projectedPoly2;
	Array<int> clippedIndices;

	generatePolygon( projectedPoly3 );
	projection.clipAndProjectPoly( projectedPoly3, clippedIndices );


	if ( projectedPoly3.size() == 0 )
	{
		return NULL;
	}


	projectedPoly3.generatePolygon2( projectedPoly2 );

	if ( projectedPoly2.side( p )  !=  SIDE_POSITIVE )
	{
		return NULL;
	}


	double bestSqrDist = std::numeric_limits<double>::max();
	MEdge *bestEdge = NULL;
	MVertex *bestVertex = NULL;

	int vPrevIndex = projectedPoly2.size() - 1;
	int prevFaceVertexIndex = clippedIndices.back();
	for (int vIndex = 0; vIndex < projectedPoly2.size(); vIndex++)
	{
		int faceVertexIndex = clippedIndices[vIndex];

		if ( prevFaceVertexIndex != -1  ||  faceVertexIndex != -1 )
		{
			MEdge *edge = NULL;

			if ( prevFaceVertexIndex != -1 )
			{
				edge = vertices[prevFaceVertexIndex].edge;
			}
			else if ( faceVertexIndex != -1 )
			{
				edge = vertices[ prevIndex( faceVertexIndex, vertices.size() ) ].edge;
			}

			gs_assert( edge != NULL, "MFace::pickVertexThruEdge(): could not get edge\n" );

			if ( !faceMarkedOnlyFlag  ||  edge->isFaceMarked() )
			{
				Segment2 seg( projectedPoly2[vPrevIndex], projectedPoly2[vIndex] );

				double t;
				Point2 closest = seg.closestPointTo( p, t );

				double sqrDist = closest.sqrDistanceTo( p );
				if ( sqrDist < bestSqrDist  ||  bestEdge == NULL )
				{
					bestSqrDist = sqrDist;
					bestEdge = edge;
					if ( t <= 0.5 )
					{
						if ( prevFaceVertexIndex != -1 )
						{
							bestVertex = vertices[prevFaceVertexIndex].vertex;
						}
						else
						{
							bestVertex = NULL;
						}
					}
					else
					{
						if ( faceVertexIndex != -1 )
						{
							bestVertex = vertices[faceVertexIndex].vertex;
						}
						else
						{
							bestVertex = NULL;
						}
					}
				}
			}
		}

		vPrevIndex = vIndex;
		prevFaceVertexIndex = faceVertexIndex;
	}

	thruEdge = bestEdge;

	return bestVertex;
}




void MFace::flip()
{
	//reverse order of vertices and vertex attributes and normals
	vertices.reverse();

	//to maintain correct edge order, the edge list must be rotated forward
	//by 1 after reversing
	MEdge *lastEdge = vertices.back().edge;
	int i = vertices.size() - 1;
	for (int j = 0; j < vertices.size() - 1; j++)
	{
		vertices[i].edge = vertices[j].edge;
		i = j;
	}
	vertices[ vertices.size() - 2 ].edge = lastEdge;


	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].edge->setFaceIndex( this, i );
	}

	faceModified();
	invalidateTesselation();
}

void MFace::triangulate()
{
	MFace *f = this;
	MEdge *newEdge;


	int a = 0, b = 2;

	while ( f->vertices.size() > 3 )
	{
		f = f->split( a, b, newEdge );

		if ( f == NULL )
		{
			//split failed, try splitting from 0 to the next vertex on
			b++;
			if ( b >= f->vertices.size() )
			{
				return;
			}
		}
		else
		{
			//reset b to 2, incase the previous split failed, in which case b would
			//have been changed
			b = 2;
		}
	}
}

void MFace::triangulate(const Tesselation &tess)
{
	Array<MVertex*> splits;

	splits.reserve( ( tess.size() - 1 )  *  2 );

	for (int i = 0; i < tess.size() - 1; i++)
	{
		MVertex *vertexA = vertices[ tess[i].a ].vertex;
		MVertex *vertexC = vertices[ tess[i].c ].vertex;

		splits.push_back( vertexA );
		splits.push_back( vertexC );
	}

	MFace *f = this, *f2;
	for (int i = 0; i < splits.size(); i += 2)
	{
		MVertex *vertexA = splits[i];
		MVertex *vertexB = splits[i];

		MEdge *newEdge;
		f2 = f->split( vertexA, vertexB, newEdge );

		if ( f2 != NULL )
		{
			//if f2->getSize() > 3  then f is the new triangle, so f2 needs to be split
			//next
			//else f2 is the triangle and f needs to be split
			if ( f2->getSize() > 3 )
			{
				f = f2;
			}
		}
	}
}


bool MFace::isPlanar()
{
	if ( vertices.size() <= 3 )
	{
		return true;
	}

	const Plane &p = getPlane();

	for (int i = 0; i < vertices.size(); i++)
	{
		const Point3 &v = vertices[i].vertex->getPosition();

		if ( p.side( v, false )  !=  SIDE_ON )
		{
			return false;
		}
	}

	return true;
}

bool MFace::isConvex()
{
	if ( vertices.size() <= 3 )
	{
		return true;
	}

	const Plane &p = getPlane();

	int i = vertices.size() - 1;
	for (int j = 0; j < vertices.size(); j++)
	{
		Vector3 u = vertices[j].vertex->getPosition()  -  vertices[i].vertex->getPosition();
		Plane edgePlane = Plane( vertices[i].vertex->getPosition(), p.n, u );

		for (int x = 0; x < vertices.size(); x++)
		{
			if ( x != i  &&  x != j )
			{
				if ( edgePlane.side( vertices[x].vertex->getPosition(), false )  ==
					  SIDE_NEGATIVE )
				{
					return false;
				}
			}
		}

		i = j;
	}

	return true;
}


void MFace::fix()
{
	bool requiresTriangulation = false;

	if ( !isPlanar() )
	{
		requiresTriangulation = true;
	}

	if ( !isConvex() )
	{
		requiresTriangulation = true;
	}

	if ( requiresTriangulation )
	{
		Tesselation tess;
		bool tessResult = computeTesselation( tess );

		if ( tessResult )
		{
			triangulate( tess );
		}
		else
		{
			triangulate();
		}
	}
}

Side MFace::side(const Plane &pl, bool strict) const
{
	if ( vertices.size() > 0 )
	{
		Point3 p = vertices[0].vertex->getPosition();

		Side result = pl.side( p, strict );

		if ( result == SIDE_BOTH )
		{
			return SIDE_BOTH;
		}


		for (int i = 1; i < vertices.size(); i++)
		{
			p = vertices[i].vertex->getPosition();

			Side thisSide = pl.side( p, strict );

			if ( thisSide == SIDE_BOTH )
			{
				return SIDE_BOTH;
			}


			if ( thisSide != result )
			{
				return SIDE_BOTH;
			}
		}

		return result;
	}
	else
	{
		return SIDE_POSITIVE;
	}
}

Side MFace::side(const BBox3 &box, bool strict) const
{
	Side result = SIDE_POSITIVE;	//start with +ve side (inside);
											//if all iterations of the for loop produce a
											//+ve result (inside), then the result will
											//remain +ve

	for (int i = 0; i < 6; i++)
	{
		Side cur = side( box.getPlane( i ), strict );

		if ( cur == SIDE_NEGATIVE )	//-ve side of plane? then this is outside of box
		{
			return SIDE_NEGATIVE;
		}
		else if ( cur == SIDE_BOTH )	//both? then so far, this is on both sides
		{
			result = SIDE_BOTH;
		}
	}

	return result;
}




// ******************* BOUNDARY / MARK OPS *******************

bool MFace::isWhollyInside(const Region3d *region)
{
	Polygon3 poly;
	generatePolygon( poly );
	return region->containsAllOf( poly );
}

bool MFace::isPartiallyInside(const Region3d *region)
{
	Polygon3 poly;
	generatePolygon( poly );
	return region->containsPartOf( poly );
}

bool MFace::isWhollyInside(const Region3d *region, const BackfaceCull *faceCull)
{
	if ( !isFrontFace( faceCull ) )
	{
		return false;
	}
	return isWhollyInside( region );
}

bool MFace::isPartiallyInside(const Region3d *region, const BackfaceCull *faceCull)
{
	if ( !isFrontFace( faceCull ) )
	{
		return false;
	}
	return isPartiallyInside( region );
}





// ******************* TRANSFORMATION OPS *******************

void MFace::vertexTransformFinalise()
{
	if ( flags.incidentVerticesTransformed )
	{
		bool transformed = vertices[0].vertex->isTransformed();

		for (int i = 1; i < vertices.size(); i++)
		{
			if ( vertices[i].vertex->isTransformed()  !=  transformed )
			{
				invalidateTesselation();
			}
		}

		flags.incidentVerticesTransformed = false;
	}
}



// ******************* MATERIAL OPS *******************

void MFace::setMaterial(int mat)
{
	//set the face material ID
	changeMaterialID( mat );

	//create replacement vertex attributes
	for (int i = 0; i < vertices.size(); i++)
	{
		//create a replacement attribute
		MVertexAttrib attrib( vertices[i].attrib->getPoint(), materialID );
		//unref the old one
		vertices[i].attrib->unref();
		//create a new one
		vertices[i].attrib = vertices[i].vertex->createVertexAttrib( attrib, MFINDPOLICY_CREATE );
		//ref the new one
		vertices[i].attrib->ref();
	}
}


void MFace::transformMaterial(const Matrix4 &xform, bool fixMarkedVertices)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].vertex->isOnFaceMarkBoundary()  &&  ( !fixMarkedVertices  ||  !vertices[i].vertex->isVertexMarked() ) )
		{
			//create a replacement attribute
			MVertexAttrib attrib( vertices[i].attrib->getPoint() * xform, materialID );
			//unref the old one
			vertices[i].attrib->unref();
			//create a new one
			vertices[i].attrib = vertices[i].vertex->createVertexAttrib( attrib, MFINDPOLICY_CREATE );
			//ref the new one
			vertices[i].attrib->ref();
		}
	}
}

void MFace::transformMaterialSave(bool fixMarkedVertices, MMaterialTransformBackup &b)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].vertex->isOnFaceMarkBoundary()  &&  ( !fixMarkedVertices  ||  !vertices[i].vertex->isVertexMarked() ) )
		{
			b.push_back( *( vertices[i].attrib ) );
		}
	}
}

void MFace::transformMaterialRestore(bool fixMarkedVertices, const MMaterialTransformBackup &b, int &position)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].vertex->isOnFaceMarkBoundary()  &&  ( !fixMarkedVertices  ||  !vertices[i].vertex->isVertexMarked() ) )
		{
			vertices[i].attrib->unref();
			vertices[i].attrib = vertices[i].vertex->createVertexAttrib( b[position++], MFINDPOLICY_CREATE );
			vertices[i].attrib->ref();
		}
	}
}


void MFace::setMaterialCoords(const Array<Point2f> &c)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		//create a replacement attribute
		MVertexAttrib attrib( c[i], materialID );
		//unref the old one
		vertices[i].attrib->unref();
		//create a new one
		vertices[i].attrib = vertices[i].vertex->createVertexAttrib( attrib, MFINDPOLICY_CREATE );
		//ref the new one
		vertices[i].attrib->ref();
	}
}

void MFace::setMaterialCoords(const Array<Point2f> &c, int mat)
{
	//set the material to mat
	changeMaterialID( mat );

	//do the work
	setMaterialCoords( c );
}

void MFace::setMaterialCoords(int index, const Point2f &uv)
{
	//create a replacement attribute
	MVertexAttrib attrib( uv, materialID );
	//unref the old one
	vertices[index].attrib->unref();
	//create a new one
	vertices[index].attrib = vertices[index].vertex->createVertexAttrib( attrib, MFINDPOLICY_CREATE );
	//ref the new one
	vertices[index].attrib->ref();
}

void MFace::setMaterialCoords(MVertex *vertex, const Point2f &uv)
{
	int index = findVertex( vertex );

	gs_assert( index != -1, "MFace::setMaterialCoords(): vertex @vertex is not incident to @this\n" );

	setMaterialCoords( index, uv );
}


BBox2 MFace::getMaterialBBox()
{
	BBox2 box( vertices[0].attrib->getPoint().toPoint2() );

	for (int i = 1; i < vertices.size(); i++)
	{
		box.addPoint( vertices[i].attrib->getPoint().toPoint2() );
	}

	return box;
}

void MFace::addToMaterialBBox(BBox2 &box)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		box.addPoint( vertices[i].attrib->getPoint().toPoint2() );
	}
}


void MFace::getUVMapNormals(Array<Vector3f> &uvmapNormals)
{
	uvmapNormals.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		uvmapNormals.push_back( vertices[i].vertex->getUVMapNormal() );
	}
}



// ******************* DEBUG OPS *******************

void MFace::display() const
{
	printf( "vertices (%d):", vertices.size() );
	for (int i = 0; i < vertices.size(); i++)
	{
		printf( "%p ", vertices[i].vertex );
	}

	printf( "edges (%d):", vertices.size() );
	for (int i = 0; i < vertices.size(); i++)
	{
		printf( "%p ", vertices[i].edge );
	}

	printf( "flags: " );
	if ( isFaceMarked() )
	{
		printf( "faceMark " );
	}
	if ( flags.secondaryMark )
	{
		printf( "secondaryMark " );
	}
	if ( flags.destroyed )
	{
		printf( "destroyed " );
	}
	if ( flags.planeRequiresRefresh )
	{
		printf( "planeRequiresRefresh " );
	}
	if ( flags.faceMarkSave )
	{
		printf( "faceMarkSave " );
	}
	if ( flags.tesselationRequiresRefresh )
	{
		printf( "tesselationRequiresRefresh " );
	}
	printf( "\n" );

	printf( "\n" );
}

bool MFace::checkIntegrity()
{
	bool passed = true;

	if ( !isDestroyed() )
	{
		// Check referential integrity
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( !vertices[i].vertex->isIncidentTo(this) )
			{
				printf("MFace %p: !vertices[%d]->isIncidentTo(this)\n", this, i);
				passed = false;
			}

			if ( vertices[i].attrib == NULL )
			{
				printf("MFace %p: vertices[%d].attrib is NULL\n", this, i);
				passed = false;
			}
			else
			{
				if ( vertices[i].attrib->getVertex() != vertices[i].vertex )
				{
					printf("MFace %p: vertices[%d].attrib != attribs[%d]\n", this, i, i);
					passed = false;
				}
			}

			if ( vertices[i].attrib->getMaterialID() != materialID )
			{
				printf("MFace %p: vertices[%d].attrib.materialID != materialID\n", this, i);
				passed = false;
			}
		}

		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i].edge->getFaceA() != this  &&  vertices[i].edge->getFaceB() != this )
			{
				printf("MFace %p: edges[%d]->faceA != this && edges[%d]->faceB != this\n", this, i, i);
				passed = false;
			}
		}

		if ( !flags.planeRequiresRefresh )
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				if ( vertices[i].normal != NULL )
				{
					if ( vertices[i].normal->index < 0 )
					{
						printf( "MFace %d(%p): normal %d index < 0 (%d) %d\n", index, this, i, vertices[i].normal->index, flags.planeRequiresRefresh );
						passed = false;
					}
				}
			}
		}


		// Ensure non-repetition of vertices
		for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
		{
			for (int checkI = vertexI + 1; checkI < vertices.size(); checkI++)
			{
				if ( vertices[vertexI].vertex  ==  vertices[checkI].vertex )
				{
					printf( "MFace %p: vertex %d and vertex %d are the same (%p)\n", this, vertexI, checkI, vertices[vertexI].vertex );
					passed = false;
				}
			}
		}


		MEdge *e = vertices[0].edge;
		MVertex *v = vertices[1].vertex;

		MEdge *previousEdge = e;
		MEdge *currentEdge = e->getNextEdge( this, v );


		while (currentEdge != e)
		{
			if ( currentEdge == NULL )
			{
				if ( !previousEdge->isBoundary() )
				{
					printf("MFace %p: edge circular list is invalid, previous = %p, v = %p\n", this, previousEdge, v);
					passed = false;
				}
				break;
			}
			v = currentEdge->getOppositeVertex(v);
			previousEdge = currentEdge;
			currentEdge = currentEdge->getNextEdge( this, v );
		}


		if ( flags.secondaryMark )
		{
			printf( "MFace %p is secondary marked\n", this );
			passed = false;
		}
	}

	return passed;
}



#endif
