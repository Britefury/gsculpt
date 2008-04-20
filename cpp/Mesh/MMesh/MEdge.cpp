//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MEDGE_CPP__
#define MEDGE_CPP__

#include <limits>

#include <Math/index.h>

#include <Polyline/Polyline.h>

#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>






/****************************************************************************
 ****************************************************************************
						MEdge
 ****************************************************************************
 ****************************************************************************/

Pool<MEdge> MEdge::edgePool;



MEdge::MEdge(int ndx)
			: vertexA( NULL ), vertexB( NULL ), faceA( NULL ), faceB( NULL ), faceAIndex( -1 ), faceBIndex( -1 ), sharpness( 0.0f ), index( ndx )
{
	flags.edgeMark = false;
	flags.edgeMarkSave = false;
	flags.destroyed = false;
	flags.indestructible = false;
	flags.secondaryMark = false;
	flags.creased = false;
	flags.normalSharp = false;
	flags.liveSubdVertexRequiresRefresh = false;
	flags.liveSubdSharpnessModified = false;
	flags.liveSubdNormalSharpnessModified = false;
	flags.liveSubdOdd = false;

	edgeOpData.subdivision.vertex = NULL;
}

MEdge::~MEdge()
{
}



void * MEdge::operator new(size_t sz)
{
	return static_cast<void *>( edgePool.allocateRaw() );
}

void MEdge::operator delete(void *data)
{
	edgePool.freeRaw( static_cast<MEdge *>( data ) );
}

void MEdge::poolAllocReserve(int numItems)
{
	edgePool.reserve( numItems );
}



void MEdge::init(MVertex *a, MVertex *b)
{
	vertexA = a;
	vertexB = b;

	vertexA->addEdge( this );
	vertexB->addEdge( this );
}

void MEdge::copyFrom(const MEdge *edge, MMesh *parentMesh, int verticesOffset, int facesOffset)
{
	//get pointers to the vertices and faces
	vertexA = parentMesh->vertices[ edge->vertexA->index + verticesOffset ];
	vertexB = parentMesh->vertices[ edge->vertexB->index + verticesOffset ];
	if ( edge->faceA != NULL )
	{
		faceA = parentMesh->faces[ edge->faceA->index + facesOffset ];
	}
	if ( edge->faceB != NULL )
	{
		faceB = parentMesh->faces[ edge->faceB->index + facesOffset ];
	}

	//face indices, sharpness, and flags can be copied over
	faceAIndex = edge->faceAIndex;
	faceBIndex = edge->faceBIndex;
	sharpness = edge->sharpness;
	flags = edge->flags;

	if ( flags.edgeMark )
	{
		getMesh()->incrementMarkedEdgeCount();
	}

	edgeOpData.subdivision.vertex = NULL;
}

void MEdge::read(FILE *f, const MMesh *parentMesh)
{
	int vertexAIndex, vertexBIndex, faceAIndex, faceBIndex;

	fread( &vertexAIndex, sizeof(int), 1, f );
	fread( &vertexBIndex, sizeof(int), 1, f );
	fread( &faceAIndex, sizeof(int), 1, f );
	fread( &faceBIndex, sizeof(int), 1, f );

	vertexA = parentMesh->vertices[vertexAIndex];
	vertexB = parentMesh->vertices[vertexBIndex];
	if ( faceAIndex != -1 )
	{
		faceA = parentMesh->faces[faceAIndex];
	}
	else
	{
		faceA = NULL;
	}
	if ( faceBIndex != -1 )
	{
		faceB = parentMesh->faces[faceBIndex];
	}
	else
	{
		faceB = NULL;
	}

	fread( &sharpness, sizeof(float), 1, f );
	fread( &flags, sizeof(flags), 1, f );

	if ( flags.edgeMark )
	{
		getMesh()->incrementMarkedEdgeCount();
	}

	edgeOpData.subdivision.vertex = NULL;
}

void MEdge::write(FILE *f) const
{
	const int minusOne = -1;

	//get pointers to the vertices and faces
	fwrite( &vertexA->index, sizeof(int), 1, f );
	fwrite( &vertexB->index, sizeof(int), 1, f );
	if ( faceA != NULL )
	{
		fwrite( &faceA->index, sizeof(int), 1, f );
	}
	else
	{
		fwrite( &minusOne, sizeof(int), 1, f );
	}
	if ( faceB != NULL )
	{
		fwrite( &faceB->index, sizeof(int), 1, f );
	}
	else
	{
		fwrite( &minusOne, sizeof(int), 1, f );
	}

	fwrite( &sharpness, sizeof(float), 1, f );
	fwrite( &flags, sizeof(flags), 1, f );
}


void MEdge::read(SENode &node, const MMesh *parentMesh)
{
	std::string header;
	node[0]  >>  header;
	if ( header == "e" )
	{
		int vertexAIndex, vertexBIndex, faceAIndex, faceBIndex;

		node[1]  >>  vertexAIndex;
		node[2]  >>  vertexBIndex;
		node[3]  >>  faceAIndex;
		node[4]  >>  faceBIndex;

		vertexA = parentMesh->vertices[vertexAIndex];
		vertexB = parentMesh->vertices[vertexBIndex];
		if ( faceAIndex != -1 )
		{
			faceA = parentMesh->faces[faceAIndex];
		}
		else
		{
			faceA = NULL;
		}
		if ( faceBIndex != -1 )
		{
			faceB = parentMesh->faces[faceBIndex];
		}
		else
		{
			faceB = NULL;
		}

		double sharp;
		node[5]  >>  sharp;
		setSharpness( sharp );

		bool bEdgeMarked = false, bCreased = false, bNormalSharp = false;
		node[6]  >>  bEdgeMarked;
		node[7]  >>  bCreased;
		node[8]  >>  bNormalSharp;

		setEdgeMark( bEdgeMarked );
		setCreased( bCreased );
		setNormalSharpness( bNormalSharp );
	}
}

void MEdge::write(SENode &node) const
{
	node.addItem()  <<  "e";

	node.addItem()  <<  vertexA->index;
	node.addItem()  <<  vertexB->index;
	if ( faceA != NULL )
	{
		node.addItem()  <<  faceA->index;
	}
	else
	{
		node.addItem()  <<  -1;
	}
	if ( faceB != NULL )
	{
		node.addItem()  <<  faceB->index;
	}
	else
	{
		node.addItem()  <<  -1;
	}

	node.addItem()  <<  getSharpness();

	node.addItem()  <<  isEdgeMarked();
	node.addItem()  <<  isCreased();
	node.addItem()  <<  isNormalSharp();
}



void MEdge::preComputeFull()
{
}



MMesh * MEdge::getMesh()
{
	return vertexA->getMesh();
}




// ******************* VERTEX ATTRIBUTE OPS *******************

bool MEdge::usesVertexAttrib(const MVertexAttrib *attrib) const
{
	if ( faceA != NULL )
	{
		int index = faceAIndex;
		int nextVtxIndex = nextIndex( faceAIndex, faceA->getSize() );

		//check if faceA uses attrib
		if ( faceA->getVertexAttrib( index ) == attrib )
		{
			return true;
		}
		if ( faceA->getVertexAttrib( nextVtxIndex ) == attrib )
		{
			return true;
		}


		if ( faceB != NULL )
		{
			index = faceBIndex;
			nextVtxIndex = nextIndex( faceBIndex, faceB->getSize() );

			//check if faceB uses attrib
			if ( faceB->getVertexAttrib( index ) == attrib )
			{
				return true;
			}
			if ( faceB->getVertexAttrib( nextVtxIndex ) == attrib )
			{
				return true;
			}
		}
	}

	return false;
}

bool MEdge::isVertexAttribBoundary() const
{
	if ( faceA == NULL )
	{
		return false;
	}

	if ( faceB == NULL )
	{
		return true;
	}

	if ( faceA->getMaterialID() != faceB->getMaterialID() )
	{
		return true;
	}


	if ( vertexA->getNumVertexAttribs() == 1  &&  vertexB->getNumVertexAttribs() == 1 )
	{
		return false;
	}


	int faceAEdgeIndex = faceAIndex;
	int faceBEdgeIndex = faceBIndex;



	MVertexAttrib *faceAAttribs[2], *faceBAttribs[2];
	faceAAttribs[0] = faceA->getVertexAttrib( faceAEdgeIndex );
	faceAAttribs[1] = faceA->getVertexAttrib( nextIndex( faceAEdgeIndex, faceA->getSize() ) );

	faceBAttribs[0] = faceB->getVertexAttrib( nextIndex( faceBEdgeIndex, faceB->getSize() ) );
	faceBAttribs[1] = faceB->getVertexAttrib( faceBEdgeIndex );

	return faceAAttribs[0] != faceBAttribs[0]  ||  faceAAttribs[1] != faceBAttribs[1];
}

bool MEdge::isVertexAttribBoundary(const MVertexAttrib *attrib) const
{
	if ( usesVertexAttrib( attrib ) )
	{
		return isVertexAttribBoundary();
	}
	else
	{
		return false;
	}
}

MVertexAttrib * MEdge::getAttribAtOppositeVertex(const MVertexAttrib *attrib)
{
	int index, nextVtxIndex;
	if ( faceA != NULL )
	{
		index = faceAIndex;
		nextVtxIndex = nextIndex( faceAIndex, faceA->getSize() );

		//check if faceA uses attrib
		if ( faceA->getVertexAttrib( index ) == attrib )
		{
			return faceA->getVertexAttrib( nextVtxIndex );
		}
		if ( faceA->getVertexAttrib( nextVtxIndex ) == attrib )
		{
			return faceA->getVertexAttrib( index );
		}


		if ( faceB != NULL )
		{
			index = faceBIndex;
			nextVtxIndex = nextIndex( faceBIndex, faceB->getSize() );

			//check if faceB uses attrib
			if ( faceB->getVertexAttrib( index ) == attrib )
			{
				return faceB->getVertexAttrib( nextVtxIndex );
			}
			if ( faceB->getVertexAttrib( nextVtxIndex ) == attrib )
			{
				return faceB->getVertexAttrib( index );
			}
		}
	}

	gs_assert_not_reached( "MEdge::getAttribAtOppositeVertex() : could not find attribute for opposite vertex\n" );

	return NULL;
}

MVertexAttrib * MEdge::getVertexAttrib(MVertex *v, MFace *f)
{
	if ( f == faceA )
	{
		//f is face A
		if ( v == vertexA )
		{
			return faceA->getVertexAttrib( faceAIndex );
		}
		else if ( v == vertexB )
		{
			return faceA->getVertexAttrib( nextIndex( faceAIndex, faceA->getSize() ) );
		}
		else
		{
			gs_assert_not_reached( "MEdge::getVertexAttrib(): vertex @v is not incident to @this\n" );
			return NULL;
		}
	}
	else if ( f == faceB )
	{
		//f is face A
		if ( v == vertexA )
		{
			return faceB->getVertexAttrib( nextIndex( faceBIndex, faceB->getSize() ) );
		}
		else if ( v == vertexB )
		{
			return faceB->getVertexAttrib( faceBIndex );
		}
		else
		{
			gs_assert_not_reached( "MEdge::getVertexAttrib(): vertex @v is not incident to @this\n" );
			return NULL;
		}
	}
	else
	{
		gs_assert_not_reached( "MEdge::getVertexAttrib(): face @f is not incident to @this\n" );
		return NULL;
	}
}

void MEdge::getVertexAttribs(MVertex *v, MVertexAttrib *&attribA, MVertexAttrib *&attribB)
{
	if ( v == vertexA )
	{
		attribA = faceA->getVertexAttrib( faceAIndex );

		if ( faceB != NULL )
		{
			attribB = faceB->getVertexAttrib( nextIndex( faceBIndex, faceB->getSize() ) );
		}
		else
		{
			attribB = NULL;
		}
	}
	else if ( v == vertexB )
	{
		attribA = faceA->getVertexAttrib( nextIndex( faceAIndex, faceA->getSize() ) );

		if ( faceB != NULL )
		{
			attribB = faceB->getVertexAttrib( faceBIndex );
		}
		else
		{
			attribB = NULL;
		}
	}
	else
	{
		gs_assert_not_reached( "MEdge::getVertexAttribs(): vertex @v is not incident to @this\n" );
	}
}

void MEdge::getVertexAttribs(MFace *f, MVertexAttrib *&attribA, MVertexAttrib *&attribB)
{
	if ( f == faceA )
	{
		attribA = faceA->getVertexAttrib( faceAIndex );
		attribB = faceA->getVertexAttrib( nextIndex( faceAIndex, faceA->getSize() ) );
	}
	else if ( f == faceB )
	{
		attribA = faceB->getVertexAttrib( nextIndex( faceBIndex, faceB->getSize() ) );
		attribB = faceB->getVertexAttrib( faceBIndex );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getVertexAttribs(): face @f [%p] is not incident to @this [%p: %p,%p]", f, this, getFaceA(), getFaceB() );
	}
}

Vector2f MEdge::getVertexAttribVector(MFace *incidentTo, const MVertex *fromVertex)
{
	MVertexAttrib *attribA, *attribB;
	getVertexAttribs( incidentTo, attribA, attribB );

	if ( fromVertex == vertexA )
	{
		return attribB->getPoint() - attribA->getPoint();
	}
	else if ( fromVertex == vertexB )
	{
		return attribA->getPoint() - attribB->getPoint();
	}
	else
	{
		gs_assert_not_reached( "MEdge::getVertexAttribVector(): the vertex fromVertex is not incident to this edge\n" );
		return Vector2f();
	}
}





// ******************* VERTEX NORMAL OPS *******************

void MEdge::setNormalSharp()
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = true;

	if ( !wasSharp )
	{
		vertexA->setNormalsRequireRefreshFlag();
		vertexB->setNormalsRequireRefreshFlag();
		getMesh()->edgeNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}

void MEdge::setNormalSmooth()
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = false;

	if ( wasSharp )
	{
		vertexA->setNormalsRequireRefreshFlag();
		vertexB->setNormalsRequireRefreshFlag();
		getMesh()->edgeNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}

void MEdge::setNormalSharpness(bool s)
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = s;

	if ( wasSharp != s )
	{
		vertexA->setNormalsRequireRefreshFlag();
		vertexB->setNormalsRequireRefreshFlag();
		getMesh()->edgeNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}




// ******************* FACE OPS *******************

MFace * MEdge::getQuadFace()
{
	if ( faceA->isQuad() )
	{
		return faceA;
	}

	if ( faceB != NULL )
	{
		if ( faceB->isQuad() )
		{
			return faceB;
		}
	}

	return NULL;
}

MFace * MEdge::getSharedFace(const MVertex *v)
{
	//try face A
	if ( v->isIncidentTo( faceA ) )
	{
		return faceA;
	}

	//try face B
	if ( faceB != NULL )
	{
		if ( v->isIncidentTo( faceB ) )
		{
			return faceB;
		}
	}

	//couldn't find either faceA or faceB in
	return NULL;
}

MFace * MEdge::getMarkedFace()
{
	if ( faceA->isFaceMarked() )
	{
		return faceA;
	}
	else if ( faceB != NULL )
	{
		if ( faceB->isFaceMarked() )
		{
			return faceB;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

MFace * MEdge::getUnmarkedFace()
{
	if ( !faceA->isFaceMarked() )
	{
		return faceA;
	}
	else if ( faceB != NULL )
	{
		if ( !faceB->isFaceMarked() )
		{
			return faceB;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

MFace * MEdge::getSecondaryUnmarkedFace()
{
	if ( !faceA->isSecondaryMarked() )
	{
		return faceA;
	}
	else if ( faceB != NULL )
	{
		if ( !faceB->isSecondaryMarked() )
		{
			return faceB;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}


void MEdge::addFace(MFace *f, MVertex *v, int index)
{
	if ( faceA == NULL )
	{
		// Isolated edge; no existing faces

		// This edge MUST point in the same direction as the boundary of face A
		// See if the vertices are in the correct order
		if ( vertexB == v )
		{
			//incorrect order, flip the edge
			flip();
		}

		// Set face A
		faceA = f;
		faceAIndex = index;
	}
	else if ( faceB == NULL )
	{
		// Boundary edge; one face

		// Ensure consistent face directions
		gs_assert( v == vertexB, "MEdge::addFace(): (%p) could not add face; face directions are inconsistent\n", this );

		// Set face B
		faceB = f;
		faceBIndex = index;
	}
	else
	{
		gs_assert_not_reached( "MEdge::addFace(): (%p) could not add face; already got 2 faces\n", this );
	}
}

void MEdge::removeFace(MFace *f, bool destroyIsolatedEdges, bool destroyIsolatedVertices)
{
	if ( faceB == NULL )
	{
		//edge is a boundary edge; only one face
		if ( f == faceA )
		{
			//remove face A
			faceA = NULL;
			faceAIndex = -1;

			if ( destroyIsolatedEdges )
			{
				//destroy if isolated
				destroyIfIsolated( destroyIsolatedVertices );
			}
		}
	}
	else if ( faceA != NULL )
	{
		//two faces adjacent to this edge
		if ( f == faceA )
		{
			//flip so that face B can be removed
			flip();
		}
		if ( f == faceB )
		{
			//if it was face A, it was flipped, becoming face B

			//remove face B
			faceB = NULL;
			faceBIndex = -1;
		}
	}
	else
	{
		gs_assert_not_reached( "MEdge::removeFace(): could not remove face; no faces to remove\n" );
	}
}


int MEdge::getFaceAVertexIndex(const MVertex *v) const
{
	if ( v == vertexA )
	{
		return faceAIndex;
	}
	else if ( v == vertexB )
	{
		return nextIndex( faceAIndex, faceA->getSize() );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getFaceAVertexIndex(): vertex @v is not incident to @this\n" );
		return -1;
	}
}

int MEdge::getFaceBVertexIndex(const MVertex *v) const
{
	if ( v == vertexB )
	{
		return faceBIndex;
	}
	else if ( v == vertexA )
	{
		return nextIndex( faceBIndex, faceB->getSize() );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getFaceBVertexIndex(): vertex @v is not incident to @this\n" );
		return -1;
	}
}

int MEdge::getFaceVertexAIndex(const MFace *f) const
{
	if ( f == faceA )
	{
		return faceAIndex;
	}
	else if ( f == faceB )
	{
		return nextIndex( faceBIndex, faceB->getSize() );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getFaceVertexAIndex(): face @f is not incident to @this\n" );
		return -1;
	}
}

int MEdge::getFaceVertexBIndex(const MFace *f) const
{
	if ( f == faceB )
	{
		return faceBIndex;
	}
	else if ( f == faceA )
	{
		return nextIndex( faceAIndex, faceA->getSize() );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getFaceVertexBIndex(): face @f is not incident to @this\n" );
		return -1;
	}
}

int MEdge::getFaceVertexIndex(const MFace *f, const MVertex *v) const
{
	if ( f == faceA )
	{
		return getFaceAVertexIndex( v );
	}
	else if ( f == faceB )
	{
		return getFaceBVertexIndex( v );
	}
	else
	{
		gs_assert_not_reached( "MEdge::getFaceVertexIndex(): face @f is not incident to @this\n" );
		return -1;
	}
}


MFace * MEdge::findCommonFaceWith(const MVertex *v) const
{
	if ( faceA != NULL )
	{
		if ( faceA->isIncidentTo( v ) )
		{
			return faceA;
		}
	}

	if ( faceB != NULL )
	{
		if ( faceB->isIncidentTo( v ) )
		{
			return faceB;
		}
	}

	return NULL;
}


MFace * MEdge::findBestSplitFace(MVertex *splitTo)
{
	gs_assert( !isIncidentTo( splitTo ), "MEdge::findBestSplitFace(): @splitTo is incident to @this\n" );

	MFace *bestFace = NULL;

	if ( splitTo->isIncidentTo( faceA ) )
	{
		bestFace = faceA;
	}

	if ( faceB != NULL )
	{
		if ( splitTo->isIncidentTo( faceB ) )
		{
			if ( bestFace == NULL )
			{
				bestFace = faceB;
			}
			else
			{
				double faceASuitability = faceA->computeVertexEdgeSplitSuitability( faceA->findVertex( splitTo ), faceAIndex );
				double faceBSuitability = faceB->computeVertexEdgeSplitSuitability( faceB->findVertex( splitTo ), faceBIndex );

				return faceASuitability >= faceBSuitability  ?  faceA  :  faceB;
			}
		}
	}

	return bestFace;
}

MFace * MEdge::findBestSplitFace(const MVertexList &splitTargets)
{
	if ( splitTargets.size() >= 1 )
	{
		for (int i = 0; i < splitTargets.size(); i++)
		{
			gs_assert( !isIncidentTo( splitTargets[i] ), "MEdge::findBestSplitFace(): cannot connect @this a vertex that is incident to @this\n" );
		}

		// Generate a list of faces common to @this and the vertices in @splitTargets
		MFaceSet commonFaces;

		commonFaces.insert( faceA );

		if ( faceB != NULL )
		{
			commonFaces.insert( faceB );
		}

		if ( commonFaces.size() > 0 )
		{
			//for each split target.....
			for (int i = 0; i < splitTargets.size(); i++)
			{
				MVertex *target = splitTargets[i];

				//remove any faces from @commonFaces that are not incident to @target
				for (MFaceSet::iterator iter = commonFaces.begin(); iter != commonFaces.end(); ++iter)
				{
					if ( !target->isIncidentTo( *iter ) )
					{
						MFaceSet::iterator iterToRemove = iter;
						--iter;
						commonFaces.remove( iterToRemove );

						if ( commonFaces.size() == 0 )
						{
							break;
						}
					}
				}

				if ( commonFaces.size() == 0 )
				{
					break;
				}
			}
		}

		// Determine the best face
		if ( commonFaces.size() == 0 )
		{
			return NULL;
		}
		else if ( commonFaces.size() == 1 )
		{
			return *( commonFaces.begin() );
		}
		else
		{
			MFace *bestFace = NULL;
			double bestSuitability = 0.0;

			for (MFaceSet::iterator iter = commonFaces.begin(); iter != commonFaces.end(); ++iter)
			{
				MFace *f = *iter;
				int indexInF = getFaceIndex( f );
				double suitability = 0.0;

				for (int i = 0; i < splitTargets.size(); i++)
				{
					suitability += f->computeVertexEdgeSplitSuitability( f->findVertex( splitTargets[i] ), indexInF );
				}

				if ( suitability > bestSuitability  ||  bestFace == NULL )
				{
					bestFace = f;
					bestSuitability = suitability;
				}
			}

			return bestFace;
		}
	}

	return NULL;
}

MFace * MEdge::findBestSplitFace(MEdge *splitTo)
{
	gs_assert( splitTo != this, "MEdge::findBestSplitFace(): @splitTo == @this\n" );

	MFace *bestFace = NULL;

	if ( faceA == splitTo->faceA  ||  faceA == splitTo->faceB )
	{
		bestFace = faceA;
	}

	if ( faceB == splitTo->faceB  ||  faceB == splitTo->faceB )
	{
		if ( bestFace == NULL )
		{
			bestFace = faceB;
		}
		else
		{
			double faceASuitability = faceA->computeEdgeEdgeSplitSuitability( faceAIndex, splitTo->getFaceIndex( faceA ) );
			double faceBSuitability = faceB->computeEdgeEdgeSplitSuitability( faceBIndex, splitTo->getFaceIndex( faceB ) );

			return faceASuitability >= faceBSuitability  ?  faceA  :  faceB;
		}
	}

	return bestFace;
}





// ******************* EDGE LINK OPS *******************

MEdge * MEdge::getNextEdge(MFace *f, const MVertex *v) const
{
	gs_assert( f == faceA  ||  f == faceB, "MEdge::getNextEdge(): face @f is not incident to @this\n" );

	return f->getNextEdge( this, v );
}

void MEdge::getAdjacentEdges(MVertex *v, MEdge *&thruFaceA, MEdge *&thruFaceB)
{
	if ( faceA != NULL )
	{
		thruFaceA = getNextEdge( faceA, v );
	}
	else
	{
		thruFaceA = NULL;
	}

	if ( faceB != NULL )
	{
		thruFaceB = getNextEdge( faceB, v );
	}
	else
	{
		thruFaceB = NULL;
	}
}




// ******************* DESTROY OPS *******************

bool MEdge::destroyIfIsolated(bool destroyIsolatedVertices)
{
	if ( flags.indestructible )
	{
		return false;
	}


	//keep the value of the isolated flag; need to return it
	bool isolatedFlag = isIsolated();

	if ( isolatedFlag )
	{
		//edge is isolated

		//remove edge from vertices
		vertexA->removeEdge( this );
		vertexB->removeEdge( this );

		if ( destroyIsolatedVertices )
		{
			//destroy vertices if they are isolated
			vertexA->destroyIfIsolated();
			vertexB->destroyIfIsolated();
		}

		//remove from mesh
		getMesh()->destroyEdge( this );
	}

	return isolatedFlag;
}



// ******************* SHARPNESS OPS *******************

void MEdge::setSharpness(float s)
{
	sharpness = clampLower( s, 0.0f );
	getMesh()->edgeSharpnessChanged();
	flags.liveSubdSharpnessModified = true;
}



// ******************* MISC OPS *******************

double MEdge::getLength() const
{
	return getEdgeVector().length();
}

double MEdge::getLengthSquared() const
{
	return getEdgeVector().sqrLength();
}

double MEdge::computeMaxLengthSquared(const MEdgeList &edgeList)
{
	double maxSqrLength = 0.0;

	for (int edgeI = 0; edgeI < edgeList.size(); edgeI++)
	{
		maxSqrLength = std::max( maxSqrLength, edgeList[edgeI]->getLengthSquared() );
	}

	return maxSqrLength;
}

double MEdge::computeMaxLength(const MEdgeList &edgeList)
{
	return sqrt( computeMaxLengthSquared( edgeList ) );
}

Segment3 MEdge::getSegment() const
{
	return Segment3( vertexA->getPosition(), vertexB->getPosition() );
}

Segment3 MEdge::getSegment(const MVertex *fromVertex) const
{
	gs_assert( isIncidentTo( fromVertex ), "MEdge::getSegment(): @this is not incident to @fromVertex\n" );

	const MVertex *opposite = getOppositeVertex( fromVertex );

	return Segment3( fromVertex->getPosition(), opposite->getPosition() );
}

Ray3 MEdge::getRay() const
{
	return Ray3( vertexA->getPosition(), vertexB->getPosition() );
}

Ray3 MEdge::getRay(const MVertex *fromVertex) const
{
	if ( !isIncidentTo( fromVertex ) )
	{
		int *x = (int*)0;
		*x = 1;
	}

	gs_assert( isIncidentTo( fromVertex ), "MEdge::getRay(): @this is not incident to @fromVertex\n" );

	const MVertex *opposite = getOppositeVertex( fromVertex );

	return Ray3( fromVertex->getPosition(), opposite->getPosition() );
}

Line3 MEdge::getLine() const
{
	return Line3( vertexA->getPosition(), vertexB->getPosition() );
}

Vector3 MEdge::getEdgeVector() const
{
	return vertexB->getPosition() - vertexA->getPosition();
}

Vector3 MEdge::getEdgeVector(const MVertex *fromVertex) const
{
	if ( fromVertex == vertexA )
	{
		return vertexB->getPosition() - vertexA->getPosition();
	}
	else if ( fromVertex == vertexB )
	{
		return vertexA->getPosition() - vertexB->getPosition();
	}
	else
	{
		return Vector3();
	}
}

Vector3 MEdge::getNormal() const
{
	if ( faceA != NULL )
	{
		if ( faceB != NULL )
		{
			return ( faceA->getPlane().n + faceB->getPlane().n ).getNormalised();
		}
		else
		{
			return faceA->getPlane().n;
		}
	}
	else
	{
		return Vector3();
	}
}

Vector3 MEdge::getEdgeAndNormalPlaneNormal() const
{
	return getEdgeVector().cross( getNormal() ).getNormalised();
}

Vector3 MEdge::getEdgeAndNormalPlaneNormal(const MVertex *fromVertex) const
{
	return getEdgeVector( fromVertex ).cross( getNormal() ).getNormalised();
}

bool MEdge::intersect(const Plane &plane, double &t) const
{
	return plane.intersect( vertexA->getPosition(), vertexB->getPosition(), t );
}

double MEdge::computeIntersection(const Plane &plane) const
{
	double t;

	plane.intersect( vertexA->getPosition(), getEdgeVector(), t );
	return t;
}

bool MEdge::intersectStrict(const Plane &plane, double &t) const
{
	return plane.intersectStrict( vertexA->getPosition(), vertexB->getPosition(), t );
}

bool MEdge::intersects(const Plane &plane) const
{
	return plane.separates( vertexA->getPosition(), vertexB->getPosition() );
}

Point3 MEdge::getPoint(double t) const
{
	return Point3::weightedAverage( vertexA->getPosition(), 1.0 - t, vertexB->getPosition(), t );
}

Point3 MEdge::getMidPoint() const
{
	return Point3::average( vertexA->getPosition(), vertexB->getPosition() );
}





// ******************* EDGE MARK OPS *******************

void MEdge::edgeMark()
{
	if ( !flags.edgeMark )
	{
		getMesh()->incrementMarkedEdgeCount();
	}

	flags.edgeMark = true;
}

void MEdge::edgeUnmark()
{
	if ( flags.edgeMark )
	{
		getMesh()->decrementMarkedEdgeCount();
	}

	flags.edgeMark = false;
}

void MEdge::edgeMarkInvert()
{
	if ( flags.edgeMark )
	{
		getMesh()->decrementMarkedEdgeCount();
	}
	else
	{
		getMesh()->incrementMarkedEdgeCount();
	}

	flags.edgeMark = !flags.edgeMark;
}

void MEdge::setEdgeMark(bool mark)
{
	if ( flags.edgeMark != mark )
	{
		if ( mark )
		{
			getMesh()->incrementMarkedEdgeCount();
		}
		else
		{
			getMesh()->decrementMarkedEdgeCount();
		}
	}

	flags.edgeMark = mark;
}



bool MEdge::hasNeighbouringMarkedEdges() const
{
	if ( vertexA->getOtherMarkedEdge( this )  !=  NULL )
	{
		return true;
	}
	else if ( vertexB->getOtherMarkedEdge( this )  !=  NULL )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool MEdge::hasNeighbouringUnmarkedEdges() const
{
	if ( vertexA->getOtherUnmarkedEdge( this )  !=  NULL )
	{
		return true;
	}
	else if ( vertexB->getOtherUnmarkedEdge( this )  !=  NULL )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void MEdge::generateMarkedEdgeRun(MVertex *v, MEdgeRun &run, MVertex *&lastVertex)
{
	MEdge *e = this;

	do
	{
		e->secondaryMark();
		run.push_back( e );

		v = e->getOppositeVertex( v );

		int edgeCount = v->getMarkedEdgeCount();

		if ( edgeCount != 2 )
		{
			lastVertex = v;
			run.open();
			return;
		}
		else
		{
			e = v->getMarkedEdgeSecondaryUnmarked();
		}
	} while ( e != NULL );

	lastVertex = v;
	run.close();
	return;
}

void MEdge::getEdgeMarkedRegion(MEdgeList &region)
{
	//search through incident vertices for incident marked edges, recording
	//them as we go, using a bredth first search algorithm
	region.clear();

	std::list<MVertex*> vertexStack;

	//add a vertex indicent to @this to the vertex stack
	vertexStack.push_back( getVertexA() );


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		for (int i = 0; i < v->getValence(); i++)
		{
			MEdge *e = v->getEdge( i );

			//if @e is marked and has not been visited
			if ( !e->isSecondaryMarked()  &&  e->isEdgeMarked() )
			{
				MVertex *oppositeVertex = e->getOppositeVertex( v );

				//visit @e
				e->secondaryMark();
				//add to region
				region.push_back( e );

				gs_assert( oppositeVertex != NULL, "MEdge::getEdgeMarkedRegion(): could not get opposite vertex of @v from edge @e\n" );

				vertexStack.push_back( oppositeVertex );
			}
		}
	}
}



// ******************* VERTEX MARK OPS *******************

bool MEdge::isVertexMarked() const
{
	return vertexA->isVertexMarked() || vertexB->isVertexMarked();
}

bool MEdge::isWhollyVertexMarked() const
{
	return vertexA->isVertexMarked() && vertexB->isVertexMarked();
}




// ******************* FACE MARK OPS *******************

int MEdge::getFaceMarkCount() const
{
	int count = 0;

	if ( faceA != NULL )
	{
		if ( faceA->isFaceMarked() )
		{
			count++;
		}
	}

	if ( faceB != NULL )
	{
		if ( faceB->isFaceMarked() )
		{
			count++;
		}
	}

	return count;
}

bool MEdge::isFaceMarkInternal() const
{
	return getFaceMarkCount() == 2;
}

bool MEdge::isFaceMarkBoundary() const
{
	return getFaceMarkCount() == 1;
}

bool MEdge::isFaceMarked() const
{
	return getFaceMarkCount() != 0;
}

bool MEdge::hasFaceMarkedVertices() const
{
	return vertexA->isFaceMarked()  ||  vertexB->isFaceMarked();
}


void MEdge::getFaceMarkBoundaryEdges(MEdgeRun &boundary)
{
	boundary.clear();

	gs_assert( isFaceMarkBoundary(), "MEdge::getFaceMarkBoundaryEdges(): @this is not face mark boundary\n" );

	MEdge *e = this;
	MVertex *v = e->getVertexA();

	do
	{
		e->secondaryMark();
		boundary.push_back( e );

		e = v->getNextFaceMarkBoundaryEdge( e );

		gs_assert( e != NULL, "MEdge::getFaceMarkBoundaryEdges(): could not get next boundary edge\n" );

		v = e->getOppositeVertex( v );

		gs_assert( v != NULL, "MEdge::getFaceMarkBoundaryEdges(): could not get vertex at opposite end of boundary edge\n" );
	}
	while ( e != this );

	boundary.close();
}



int MEdge::getFaceSecondaryMarkCount() const
{
	int count = 0;

	if ( faceA != NULL )
	{
		if ( faceA->isSecondaryMarked() )
		{
			count++;
		}
	}

	if ( faceB != NULL )
	{
		if ( faceB->isSecondaryMarked() )
		{
			count++;
		}
	}

	return count;
}

bool MEdge::isFaceSecondaryMarkInternal() const
{
	return getFaceSecondaryMarkCount() == 2;
}

bool MEdge::isFaceSecondaryMarkBoundary() const
{
	return getFaceSecondaryMarkCount() == 1;
}

bool MEdge::isFaceSecondaryMarked() const
{
	return getFaceSecondaryMarkCount() != 0;
}




// ******************* PATH DISCOVERY OPS *******************

bool MEdge::discoverEdgeRingPathTo(MEdge *target, MEdgeList &path)
{
	MDijkstraHeap<MEdge> heap;

	discoverPathSetCost( heap, 0.0, NULL );

	while ( !heap.isEmpty() )
	{
		MEdge *edge = heap.removeMinCost();

		if ( edge == target )
		{
			target->getDijkstraNode()->buildPath( target, path );
			return true;
		}
		else
		{
			double cumulativeCost = edge->edgeOpData.dijkstra.node->cumulativeCost;

			// For each successor
			MFace *a = edge->getFaceA();
			MFace *b = edge->getFaceB();
			if ( a != NULL )
			{
				Point3 aCentroid = a->computeCentroid();
				for (int i = 0; i < a->getSize(); i++)
				{
					MEdge *succ = a->getEdge( i );
					if ( succ != edge )
					{
						double cost = edge->getMidPoint().distanceTo( aCentroid )  +  aCentroid.distanceTo( succ->getMidPoint() );
						double succCost = cumulativeCost + cost;
						succ->discoverPathSetCost( heap, succCost, edge );
					}
				}
			}

			if ( b != NULL )
			{
				Point3 bCentroid = b->computeCentroid();
				for (int i = 0; i < b->getSize(); i++)
				{
					MEdge *succ = b->getEdge( i );
					if ( succ != edge )
					{
						double cost = edge->getMidPoint().distanceTo( bCentroid )  +  bCentroid.distanceTo( succ->getMidPoint() );
						double succCost = cumulativeCost + cost;
						succ->discoverPathSetCost( heap, succCost, edge );
					}
				}
			}
		}
	}

	return false;
}


void MEdge::discoverPathInit()
{
	edgeOpData.dijkstra.node = NULL;
}

void MEdge::discoverPathFinish()
{
	if ( edgeOpData.dijkstra.node != NULL )
	{
		delete edgeOpData.dijkstra.node;
	}
}


void MEdge::discoverPathSetCost(MDijkstraHeap<MEdge> &heap, double cumulativeCost, MEdge *previous)
{
	if ( edgeOpData.dijkstra.node == NULL )
	{
		MDijkstraNode<MEdge, MEdge> *node = new MDijkstraNode<MEdge, MEdge>( cumulativeCost, previous, this );
		edgeOpData.dijkstra.node = node;
		heap.add( this );
	}
	else
	{
		MDijkstraNode<MEdge, MEdge> *node = edgeOpData.dijkstra.node;
		if ( node->cumulativeCost  >  cumulativeCost )
		{
			node->cumulativeCost = cumulativeCost;
			node->previous = previous;
			node->pathSeg = this;
			heap.costChanged( this );
		}
	}
}






// ******************* SUBDIVISION OPS *******************

void MEdge::destroySubdivisionVertex()
{
	edgeOpData.subdivision.vertex->destroyIfIsolated();
	edgeOpData.subdivision.vertex = NULL;
}

void MEdge::liveSubdivisionRefreshInit()
{
	if ( flags.liveSubdSharpnessModified )
	{
		flags.liveSubdVertexRequiresRefresh = true;

		vertexA->liveSubdivisionIncidentEdgeSharpnessModified();
		vertexB->liveSubdivisionIncidentEdgeSharpnessModified();
	}

	if ( flags.liveSubdNormalSharpnessModified )
	{
		vertexA->liveSubdivisionIncidentEdgeNormalSharpnessModified();
		vertexB->liveSubdivisionIncidentEdgeNormalSharpnessModified();
	}
}


Point3 MEdge::computeLiveSubdivisionVertexPosition(MMesh *liveMesh, int faceSubdVerticesOffset)
{
	Point3 catm, smooth, sharp;

	bool boundaryFlag = isIsolated()  ||  isBoundary();
	bool needSharp = boundaryFlag  ||  sharpness > 0.0;
	bool needSmooth = !boundaryFlag  &&  sharpness < 1.0;



	if ( needSharp )
	{
		sharp = Point3::average( vertexA->getPosition(), vertexB->getPosition() );
	}
	if ( needSmooth )
	{
		smooth = Point3::average( vertexA->getPosition(), vertexB->getPosition(),
							liveMesh->vertices[ faceA->index + faceSubdVerticesOffset ]->getPosition(),
							liveMesh->vertices[ faceB->index + faceSubdVerticesOffset ]->getPosition() );
	}


	if ( boundaryFlag )
	{
		catm = sharp;
	}
	else
	{
		if ( sharpness >= 1.0 )
		{
			catm = sharp;
		}
		else if ( sharpness == 0.0 )
		{
			catm = smooth;
		}
		else
		{
			catm = lerp( smooth, sharp, sharpness );
		}
	}
	return catm;
}

void MEdge::createLiveSubdivisionVertex(MMesh *liveMesh, int faceSubdVerticesOffset)
{
	Point3 catm = computeLiveSubdivisionVertexPosition( liveMesh, faceSubdVerticesOffset );
	MVertex *newVertex = liveMesh->addVertex( catm );
	newVertex->createVertexAttrib( MVertexAttrib( Point2f(), -1 ), MFINDPOLICY_FORCEDCREATE );
}

void MEdge::refreshLiveSubdivisionVertex(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset)
{
	if ( flags.liveSubdVertexRequiresRefresh )
	{
		Point3 catm = computeLiveSubdivisionVertexPosition( liveMesh, faceSubdVerticesOffset );
		liveMesh->vertices[ index + edgeSubdVerticesOffset ]->setPosition( catm );
	}

	liveSubdivisionRefreshed();
}



void MEdge::createCatmullClarkSubdivisionVertex()
{
	Point3 catm, smooth, sharp;
	Point2f attribCatm, attribSmooth, attribSharp;

	bool boundaryFlag = isIsolated()  ||  isBoundary();
	bool needSharp = boundaryFlag  ||  sharpness > 0.0;
	bool needSmooth = !boundaryFlag  &&  sharpness < 1.0;




	if ( needSharp )
	{
		sharp = Point3::average( vertexA->getPosition(), vertexB->getPosition() );
	}
	if ( needSmooth )
	{
		smooth = Point3::average( vertexA->getPosition(), vertexB->getPosition(),
							faceA->getSubdivisionVertex()->getPosition(), faceB->getSubdivisionVertex()->getPosition() );
	}



	if ( boundaryFlag )
	{
		catm = sharp;
	}
	else
	{
		if ( sharpness >= 1.0 )
		{
			catm = sharp;
		}
		else if ( sharpness == 0.0 )
		{
			catm = smooth;
		}
		else
		{
			catm = lerp( smooth, sharp, sharpness );
		}
	}

	MVertex *newVertex = getMesh()->addVertex( catm );
	edgeOpData.subdivision.vertex = newVertex;



	if ( boundaryFlag  ||  isVertexAttribBoundary() )
	{
		if ( faceA != NULL )
		{
			subdivisionLinearSubdivideAttrib( faceA, newVertex );

			if ( faceB != NULL )
			{
				subdivisionLinearSubdivideAttrib( faceB, newVertex );
			}
		}
	}
	else
	{
		int index = faceAIndex;
		int nextVtxIndex = nextIndex( index, faceA->getSize() );

		MVertexAttrib *attrib[2];
		attrib[0] = faceA->getVertexAttrib( index );
		attrib[1] = faceA->getVertexAttrib( nextVtxIndex );


		if ( needSharp )
		{
			attribSharp = Point2f::average( attrib[0]->getPoint(), attrib[1]->getPoint() );
		}
		if ( needSmooth )
		{
			attribSmooth = Point2f::average( attrib[0]->getPoint(), attrib[1]->getPoint(),
										faceA->getSubdivisionVertex()->getVertexAttrib( 0 )->getPoint(),
										faceB->getSubdivisionVertex()->getVertexAttrib( 0 )->getPoint() );
		}

		if ( sharpness >= 1.0 )
		{
			attribCatm = attribSharp;
		}
		else if ( sharpness == 0.0 )
		{
			attribCatm = attribSmooth;
		}
		else
		{
			attribCatm = lerp( attribSmooth, attribSharp, sharpness );
		}


		newVertex->createVertexAttrib( MVertexAttrib( attribCatm, attrib[0]->getMaterialID() ), MFINDPOLICY_FORCEDCREATE );
	}
}

void MEdge::createLinearSubdivisionVertex()
{
	Point3 vtx = Point3::average( vertexA->getPosition(), vertexB->getPosition() );
	edgeOpData.subdivision.vertex = getMesh()->addVertex( vtx );

	if ( faceA != NULL )
	{
		subdivisionLinearSubdivideAttrib( faceA, edgeOpData.subdivision.vertex );

		if ( faceB != NULL )
		{
			if ( isVertexAttribBoundary() )
			{
				subdivisionLinearSubdivideAttrib( faceB, edgeOpData.subdivision.vertex );
			}
		}
	}
}

MVertexAttrib * MEdge::getSubdivisionVertexAttrib(const MFace *f)
{
	MVertex *vertex = edgeOpData.subdivision.vertex;

	if ( vertex->getNumVertexAttribs() == 2 )
	{
		if ( f == faceA )
		{
			return vertex->getVertexAttrib( 0 );
		}
		else if ( f == faceB )
		{
			return vertex->getVertexAttrib( 1 );
		}
		else
		{
			gs_assert_not_reached( "MEdge::getSubdivisionVertexAttrib(): face @f is not indicent to @this\n" );
			return NULL;
		}
	}
	else
	{
		return vertex->getVertexAttrib( 0 );
	}
}


void MEdge::subdivideMarkedBoundary()
{
	gs_assert( faceB == NULL, "MEdge::subdivideMarkedBoundary(): @this is not a boundary edge\n" );

	MVertex *subdVertex = edgeOpData.subdivision.vertex;
	MVertexAttrib *subdVertexAttrib = NULL;


	if ( subdVertex->getNumVertexAttribs() == 2 )
	{
		if ( subdVertex->getVertexAttrib( 0 )->getRefCount() == 0 )
		{
			subdVertexAttrib = subdVertex->getVertexAttrib( 0 );
		}
		else if ( subdVertex->getVertexAttrib( 1 )->getRefCount() == 0 )
		{
			subdVertexAttrib = subdVertex->getVertexAttrib( 1 );
		}
		else
		{
			gs_assert_not_reached( "MEdge::subdivideMarkedBoundary(): neither vertex attrib is unused\n" );
		}
	}
	else
	{
		subdVertexAttrib = subdVertex->getVertexAttrib( 0 );
	}


	int position = nextIndex( faceAIndex, faceA->getSize() );
	faceA->insertVertex( position, subdVertex, subdVertexAttrib );
}



MVertexAttrib * MEdge::subdivisionLinearSubdivideAttrib(MFace *f, MVertex *newVertex)
{
	int index = getFaceIndex( f );
	int nextVtxIndex = nextIndex( index, f->getSize() );

	const MVertexAttrib *attr = f->getVertexAttrib( index );
	const MVertexAttrib *nextAttr = f->getVertexAttrib( nextVtxIndex );

	Point2f newPos = Point2f::average( attr->getPoint(), nextAttr->getPoint() );

	MVertexAttrib *newAttrib = newVertex->createVertexAttrib( MVertexAttrib( newPos, attr->getMaterialID() ), MFINDPOLICY_FORCEDCREATE );

	return newAttrib;
}



// ******************* EXTRUDE FACE OPS *******************

void MEdge::extrudeFaceCreateExtrudedDuplicates(bool markBoundaryEdges, bool nsharpBoundaryEdges, const Polyline *extrusionPath)
{
	MVertexList *aExtruded = vertexA->getExtrudeFaceExtrudedVertices( this );
	MVertexList *bExtruded = vertexB->getExtrudeFaceExtrudedVertices( this );

	if ( extrusionPath != NULL )
	{
		MEdge *e = NULL;

		//handle edge 0: this
		if ( extrusionPath->at( 0 ).isNormalSharp() )
		{
			setNormalSharp();
		}

		//miss out edge 0
		for (int i = 1; i < extrusionPath->size(); i++)
		{
			if ( extrusionPath->at( i ).isNormalSharp() )
			{
				e = getMesh()->addEdge( aExtruded->at( i ), bExtruded->at( i ) );
				e->setNormalSharp();
			}
			else
			{
				e = NULL;
			}
		}

		//e contains the value of the last loop
		if ( e == NULL )
		{
			e = getMesh()->addEdge( aExtruded->back(), bExtruded->back() );
		}
		edgeOpData.faceExtrude.edge = e;
	}
	else
	{
		edgeOpData.faceExtrude.edge = getMesh()->addEdge( aExtruded->back(), bExtruded->back() );
	}

	if ( markBoundaryEdges )
	{
		edgeOpData.faceExtrude.edge->edgeMark();
	}
	if ( nsharpBoundaryEdges )
	{
		edgeOpData.faceExtrude.edge->setNormalSharp();
	}
}

void MEdge::extrudeFaceExtrudeToQuads(int numSegments, int materialID, float u[2],
									bool markExtrudedEdges, bool nsharpExtrudedEdges, const PolylineEdgeTextureTable &ptex)
{
	Array<MVertex*> vts;
	vts.resize( 4 );
	Array<MVertexAttrib> attribs;
	attribs.resize( 4 );

	MEdge *extrudedEdge = edgeOpData.faceExtrude.edge;

	//extrudedEdge may have been flipped around
	MVertexList *vertexAExtruded = vertexA->getExtrudeFaceExtrudedVertices( this );
	MVertexList *vertexBExtruded = vertexB->getExtrudeFaceExtrudedVertices( this );

	bool flipped = false;

	if ( extrudedEdge->vertexA == vertexAExtruded->back() )
	{
		flipped = false;
	}
	else if ( extrudedEdge->vertexB == vertexAExtruded->back() )
	{
		flipped = true;
	}
	else
	{
		gs_assert_not_reached( "MEdge::createFaceExtrudedEdgeQuadFace(): could not determine orientation of extruded edge\n" );
	}


	for (int i = 0; i < numSegments; i++)
	{
		if ( flipped )
		{
			vts[0] = vertexAExtruded->at( i + 1 );
			vts[1] = vertexBExtruded->at( i + 1 );
			vts[2] = vertexBExtruded->at( i );
			vts[3] = vertexAExtruded->at( i );
			attribs[0] = MVertexAttrib( Point2f( u[0], ptex[i].getU1() ), materialID );
			attribs[1] = MVertexAttrib( Point2f( u[1], ptex[i].getU1() ), materialID );
			attribs[2] = MVertexAttrib( Point2f( u[1], ptex[i].getU0() ), materialID );
			attribs[3] = MVertexAttrib( Point2f( u[0], ptex[i].getU0() ), materialID );
		}
		else
		{
			vts[0] = vertexBExtruded->at( i + 1 );
			vts[1] = vertexAExtruded->at( i + 1 );
			vts[2] = vertexAExtruded->at( i );
			vts[3] = vertexBExtruded->at( i );
			attribs[0] = MVertexAttrib( Point2f( u[1], ptex[i].getU1() ), materialID );
			attribs[1] = MVertexAttrib( Point2f( u[0], ptex[i].getU1() ), materialID );
			attribs[2] = MVertexAttrib( Point2f( u[0], ptex[i].getU0() ), materialID );
			attribs[3] = MVertexAttrib( Point2f( u[1], ptex[i].getU0() ), materialID );
		}

		MFace *quadFace = getMesh()->addFace( vts, attribs, materialID );

		if ( markExtrudedEdges )
		{
			//mark edges 1 and 3
			quadFace->getEdge( 1 )->edgeMark();

			quadFace->getEdge( 3 )->edgeMark();
		}
		if ( nsharpExtrudedEdges )
		{
			//normal sharpen edges 1 and 3
			quadFace->getEdge( 1 )->setNormalSharp();

			quadFace->getEdge( 3 )->setNormalSharp();
		}
	}
}





// ******************* INSET FACE OPS *******************

void MEdge::insetExpandFaceSplitOffIncidentQuad(MFace *fromFace, MInsetExpandOperation insetExpandOp, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	if ( fromFace != NULL )
	{
		if ( fromFace->isInsetExpandTarget( insetExpandOp ) )
		{
			MEdge *newEdge;
			MFace *newFace = fromFace->splitOffQuadIncidentToEdge( this, newEdge );
			newFace->insetExpandFaceCopyStoredPlaneFrom( fromFace );

			if ( newFace != NULL )
			{
				if ( markBoundaryEdges )
				{
					newEdge->edgeMark();
				}
				if ( nsharpBoundaryEdges )
				{
					newEdge->setNormalSharp();
				}

				if ( newFace->isIncidentTo( this ) )
				{
					newFace->faceUnmark();
				}
				else if ( fromFace->isIncidentTo( this ) )
				{
					fromFace->faceUnmark();
				}
				else
				{
					gs_assert_not_reached( "MEdge::insetExpandFaceSplitOffIncidentQuad(): @this is not incident to either result face\n");
				}
			}
		}
	}
}

void MEdge::insetExpandFaceSplitOffIncidentQuads(MInsetExpandOperation insetExpandOp, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	//faces get modified: use copies
	MFace *fa = faceA, *fb = faceB;

	insetExpandFaceSplitOffIncidentQuad( fa, insetExpandOp, markBoundaryEdges, nsharpBoundaryEdges );
	insetExpandFaceSplitOffIncidentQuad( fb, insetExpandOp, markBoundaryEdges, nsharpBoundaryEdges );
}



// ******************* EXPAND EDGE OPS *******************

void MEdge::expandEdgeSplitOffIncidentQuads(MEdgeList &boundaryEdges)
{
	MFace *fa = faceA, *fb = faceB;
	MEdge *newEdge;


	if ( fa != NULL )
	{
		fa->splitOffQuadIncidentToEdge( this, newEdge );
		boundaryEdges.push_back( newEdge );
	}

	if ( fb != NULL )
	{
		fb->splitOffQuadIncidentToEdge( this, newEdge );
		boundaryEdges.push_back( newEdge );
	}
}




// ******************* BEVEL EDGE OPS *******************

void MEdge::bevelInit()
{
	// Only allocate memory for the BevelEdgeTable structure if this edge is involved in bevelling (is edge marked, or incident to a vertex that is incident to marked edges)
	if ( isEdgeMarked()  ||  getVertexA()->isEdgeMarked()  ||  getVertexB()->isEdgeMarked() )
	{
		edgeOpData.bevel.bevelEdgeTable = new BevelEdgeTable();
		edgeOpData.bevel.bevelEdgeTable->insert( getVertexA(), BevelEdgeTableVertexFlags() );
		edgeOpData.bevel.bevelEdgeTable->insert( getVertexB(), BevelEdgeTableVertexFlags() );
	}
	else
	{
		edgeOpData.bevel.bevelEdgeTable = NULL;
	}
}

void MEdge::bevelShutdown()
{
	if ( edgeOpData.bevel.bevelEdgeTable != NULL )
	{
		delete edgeOpData.bevel.bevelEdgeTable;
		edgeOpData.bevel.bevelEdgeTable = NULL;
	}
}



// ******************* EXTRUDE EDGE OPS *******************

void MEdge::generateExtrudeableEdgeRun(MEdgeRun &run, MVertex *startAt)
{
	run.clear();

	gs_assert( isExtrudeable(), "MEdge::generateExtrudeableEdgeRun(): @this is not extrudeable\n" );

	MEdge *e = this, *nextEdge;
	//first vertex: used to detect if we have looped back on ourselves
	MVertex *firstVertex = startAt == NULL  ?  e->getVertexA() : startAt;
	//get the next vertex:
	MVertex *v = startAt == NULL  ?  e->getVertexB()  :  e->getOppositeVertex( startAt );


	do
	{
		//add to the edge run
		run.push_back( e );

		//get the next edge
		nextEdge = v->getNextExtrudeableEdge( e );

		//secondary mark AFTER getting next edge, as secondary marking makes
		//an edge unextrudeable, thus altering the extrudeable edge count
		e->secondaryMark();

		//assign e to the next edge
		e = nextEdge;

		//if there is no next edge, then return as we are done
		if ( e == NULL )
		{
			return;
		}

		v = e->getOppositeVertex( v );

		gs_assert( v != NULL, "MEdge::generateExtrudeableEdgeRun(): could not get vertex at opposite end of extrudeable edge\n" );
	} while ( v != firstVertex );		//break out if we loop back on ourselves

	//add the last edge
	run.push_back( e );

	//there are two ways in which the above do-while loop exits:
	//1. e == NULL; no extrudeable edge found, the method returns
	//2. v == startAt; we loop back on ourselves, and the loop terminates
	//if we reach this point, condition 2 is true, so:

	//the edge run is closed
	run.close();
}



// ******************* KNIFE OPS *******************

void MEdge::knifeSplit(double t)
{
	MEdge *newEdge;

	//split the edge at t
	MVertex *splitVertex = split( t, newEdge );

	//for each incident face marked for cutting,
	//add a split vertex
	if ( faceA != NULL )
	{
		if ( faceA->isSecondaryMarked() )
		{
			faceA->knifeAddCutVertex( splitVertex );
		}
	}

	if ( faceB != NULL )
	{
		if ( faceB->isSecondaryMarked() )
		{
			faceB->knifeAddCutVertex( splitVertex );
		}
	}
}




// ******************* EDGE SPLIT OPS *******************

//do the edge splitting
void MEdge::splitEdge(double fraction, MVertex *&splitVertex, MEdge *&newEdgeB)
{
	//NOTE:
	//instead of creating two new edges, and destroying this,
	//the splitEdge() method creates one new edge and modifies this

	//compute the position of the vertex being inserted
	Point3 position = getPoint( fraction );


	//create the vertex
	splitVertex = getMesh()->addVertex( position );

	//create an edge connecting the split vertex to vertex B
	newEdgeB = getMesh()->addEdge( splitVertex, vertexB );
	newEdgeB->copyPropertiesFrom( this );

	//change vertex B of this to the split vertex
	vertexB->removeEdge( this );
	//deal with the boundary and sharp edge counts
	vertexB = splitVertex;
	splitVertex->addEdge( this );
}

MVertex * MEdge::split(double fraction, MEdge *&newEdgeB)
{
	MVertex *splitVertex;

	//split the edge, getting the newly created edge and the split vertex
	splitEdge( fraction, splitVertex, newEdgeB );


	//get and store pointers to faceA and faceB, since the
	//MFace::splitEdge method alters the faceA and faceB pointers of this edge
	MFace *fa = faceA, *fb = faceB;

	bool attribBoundaryFlag = isVertexAttribBoundary();

	if ( fa != NULL )
	{
		//same order as faceA, so t is fraction
		MVertexAttrib *resultAttrib = fa->splitEdge( this, this, splitVertex, newEdgeB, fraction, NULL );

		resultAttrib = attribBoundaryFlag  ?  NULL : resultAttrib;

		if ( fb != NULL )
		{
			//opposite order as faceB, so t is 1.0 - fraction
			fb->splitEdge( this, newEdgeB, splitVertex, this, 1.0 - fraction, resultAttrib );
		}
	}


	return splitVertex;
}

MVertex * MEdge::split(MVertex *fromVertex, double fraction, MEdge *&newEdge0, MEdge *&newEdge1)
{
	if ( fromVertex == vertexA )
	{
		newEdge0 = this;
		return split( fraction, newEdge1 );
	}
	else
	{
		newEdge1 = this;
		return split( 1.0 - fraction, newEdge0 );
	}
}

MVertex * MEdge::splitAdjustable(MVertex *fromVertex, MVertexAdjustList &vertexAdjusts, const Vector3 &adjustVectorV, MVertexAttribAdjustList &vertexAttribAdjusts,
							double scaleFactor, MEdge *&newEdge0, MEdge *&newEdge1, bool constrainLowerAdjustment, bool constrainUpperAdjustment)
{
	double fraction = fromVertex == vertexA  ?  0.0 : 1.0;


	MVertex *endVertex = getOppositeVertex( fromVertex );
	gs_assert( endVertex != NULL, "MEdge::splitAdjustable(): could not get end vertex\n" );





	MVertex *splitVertex;
	MEdge *newEdgeB;

	//split the edge, getting the newly created edge and the split vertex
	splitEdge( fraction, splitVertex, newEdgeB );


	double uLower = constrainLowerAdjustment  ?  0.0  :  -std::numeric_limits<double>::max();
	double uUpper = constrainUpperAdjustment  ?  1.0 / scaleFactor  :  std::numeric_limits<double>::max();

	vertexAdjusts.push_back( MVertexAdjust( splitVertex, fromVertex->getPosition(),
						( endVertex->getPosition() - fromVertex->getPosition() ) * scaleFactor, uLower, uUpper,
						adjustVectorV, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() ) );


	//get and store pointers to faceA and faceB, since the
	//MFace::splitEdge method alters the faceA and faceB pointers of this edge
	MFace *fa = faceA, *fb = faceB;

	bool attribBoundaryFlag = isVertexAttribBoundary();

	if ( fa != NULL )
	{
		MVertexAttrib *resultAttrib = fa->splitEdge( this, this, splitVertex, newEdgeB, fromVertex, vertexAttribAdjusts, scaleFactor, NULL );

		resultAttrib = attribBoundaryFlag  ?  NULL : resultAttrib;

		if ( fb != NULL )
		{
			fb->splitEdge( this, newEdgeB, splitVertex, this, fromVertex, vertexAttribAdjusts, scaleFactor, resultAttrib );
		}
	}


	if ( fromVertex == vertexA )
	{
		newEdge0 = this;
		newEdge1 = newEdgeB;
	}
	else
	{
		newEdge0 = newEdgeB;
		newEdge1 = this;
	}


	return splitVertex;
}

void MEdge::splitIntoSegments(MVertex *from, int numSegments, MVertexList &newVertices, MEdgeList &splitEdges)
{
	MEdge *newEdge0;
	MEdge *e = this;

	newVertices.open();
	newVertices.reserveExtra( numSegments - 1 );

	for (int i = 0; i < numSegments - 1; i++)
	{
		from = e->split( from, 1.0 / (double)( numSegments - i ), newEdge0, e );
		newVertices.push_back( from );
		splitEdges.push_back( newEdge0 );
	}
	splitEdges.push_back( e );
}

void MEdge::splitIntoSegments(int numSegments, MVertexList &newVertices, MEdgeList &splitEdges)
{
	splitIntoSegments( vertexA, numSegments, newVertices, splitEdges );
}

void MEdge::splitIntoSegments(const std::set<double> &parameters, bool reverseFlag, MVertexList &newVertices, MEdgeList &splitEdges)
{
	MEdge *newEdge0;
	MEdge *e = this;

	newVertices.open();
	newVertices.reserveExtra( parameters.size() );

	MVertex *from = reverseFlag  ?  getVertexB()  :  getVertexA();
	double prevT = 0.0;
	for (std::set<double>::const_iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
	{
		double t = *iter;
		double scaledT = ( t - prevT )  /  ( 1.0 - prevT );
		from = e->split( from, scaledT, newEdge0, e );
		newVertices.push_back( from );
		splitEdges.push_back( newEdge0 );
		prevT = t;
	}
	splitEdges.push_back( e );
}




// ******************* INTEROPOLATED ADJUSTMENT OPS *******************

void MEdge::interpolatedAdjust(MVertex *fromVertex, MVertex *vertexToAdjust, MVertexAdjustList &vertexAdjusts, const Vector3 &adjustVectorV, MVertexAttribAdjustList &vertexAttribAdjusts,
						double scaleFactor)
{
	bool vertexToAdjustIsA = vertexToAdjust == vertexA;
	bool vertexToAdjustIsB = vertexToAdjust == vertexB;

	MVertex *endVertex = getOppositeVertex( fromVertex );
	gs_assert( endVertex != NULL, "MEdge::interpolatedAdjust(): could not get end vertex\n" );



	//create an adjustment for the vertex position
	vertexAdjusts.push_back( MVertexAdjust( vertexToAdjust, fromVertex->getPosition(),
						( endVertex->getPosition() - fromVertex->getPosition() ) * scaleFactor, 0.0, 1.0 / scaleFactor,
						adjustVectorV, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() ) );


	//ADJUST THE VERTEX ATTRIBUTES

	if ( isVertexAttribBoundary()  &&  !isBoundary() )
	{
		MVertexAttrib *fromFaceA, *fromFaceB, *toFaceA, *toFaceB, *attribA, *attribB;

		//get all four vertex attributes
		//fromFaceA : fromVertex, faceA   etc
		getVertexAttribs( fromVertex, fromFaceA, fromFaceB );
		getVertexAttribs( endVertex, toFaceA, toFaceB );

		if ( vertexToAdjustIsA  ||  vertexToAdjustIsB )
		{
			//vertexToAdjust is incident to *this
			if ( fromFaceB == fromFaceA )
			{
				//despite being an attrib boundary, at fromVertex, both faces
				//share the same vertex attribute.
				//When interpolating, they will need to separate

				fromFaceB = fromVertex->createVertexAttrib( *fromFaceA, MFINDPOLICY_FORCEDCREATE );
				faceB->setVertexAttrib( vertexToAdjustIsA  ?  nextIndex( faceBIndex, faceB->getSize() )  :  faceBIndex,   fromFaceB );
				//unref the old attrib
				fromFaceA->unref();
				//ref the new one
				fromFaceB->ref();
			}

			//adjusting one of the attributes used by *this
			attribA = fromFaceA;
			attribB = fromFaceB;
		}
		else
		{
			//vertexToAdjust is NOT incident to *this
			//create the vertex attributes that will be adjusted
			attribA = vertexToAdjust->createVertexAttrib( *fromFaceA, MFINDPOLICY_FORCEDCREATE );
			attribB = vertexToAdjust->createVertexAttrib( *fromFaceB, MFINDPOLICY_FORCEDCREATE );
		}


		//create the adjustment objects
		vertexAttribAdjusts.push_back( MVertexAttribAdjust( attribA, fromFaceA->getPoint(), ( toFaceA->getPoint() - fromFaceA->getPoint() ) * scaleFactor ) );
		vertexAttribAdjusts.push_back( MVertexAttribAdjust( attribB, fromFaceB->getPoint(), ( toFaceB->getPoint() - fromFaceB->getPoint() ) * scaleFactor ) );
	}
	else
	{
		//attribute continuous:
		//get the attributes, only two of them
		MVertexAttrib *from = getVertexAttrib( fromVertex, faceA );
		MVertexAttrib *to = getVertexAttrib( endVertex, faceA );
		MVertexAttrib *attrib;

		if ( vertexToAdjustIsA  ||  vertexToAdjustIsB )
		{
			//vertexToAdjust is incident to *this
			//adjust an existing attribute
			attrib = from;
		}
		else
		{
			//vertexToAdjust is NOT incident to *this
			//create the vertex attributes to be adjusted
			attrib = vertexToAdjust->createVertexAttrib( *from, MFINDPOLICY_FORCEDCREATE );
		}

		//create the adjustment object
		vertexAttribAdjusts.push_back( MVertexAttribAdjust( attrib, from->getPoint(), ( to->getPoint() - from->getPoint() ) * scaleFactor ) );
	}
}




// ******************* FACE SPLIT OPS *******************

void MEdge::replaceFace(MFace *fOld, MFace *fNew, int index)
{
	//NOTE: NO VERTICES OR EDGES ARE DESTROYED BY THIS OPERATION
	if ( faceA == fOld )
	{
		faceA = fNew;
		faceAIndex = index;
	}
	else if ( faceB == fOld )
	{
		faceB = fNew;
		faceBIndex = index;
	}
	else
	{
		gs_assert_not_reached( "MEdge::replaceFace(): @this is not incident to the old face (@fOld)\n" );
	}
}





// ******************* EDGE COLLAPSE OPS *******************

MVertex * MEdge::collapse(const Point3 &vertexPosition, bool markMergedEdges, MCollapseEdgeTo collapseTo)
{
	MVertex *va = vertexA, *vb = vertexB;



	//if vertexA and vertexB share a common face f, which is NOT incident to
	//*this, then collapsing *this will result in f becoming butterfly/bow-tie
	//shaped, so do not permit a collapse in this case
	MFace *shared = va->findCommonFaceWithVertexNotIncidentToEdge( vb, this );

	if ( shared != NULL )
	{
		return NULL;
	}



	//NOTE:
	//depending upon the surrounding mesh, collapsing this may
	//result in an invalid mesh, in which case it should not be attempted
	if ( !MVertex::checkEdgeCollapseValidity( this, markMergedEdges ) )
	{
		return NULL;
	}


/*
CHECK TO SEE IF REMOVING VERTICES IS POSSIBLE:

NOTE FOLLOWING EXAMPLE:

                       v2
                     __#_
                  __/  /|\_
               __/    / |  \_e5
          e0__/      /  |    \_
         __/    ^e2^/   |      \_
      __/          /    |        \_
   __/            /     |          \
v0#__    f0    v1#  f1  |e4  f2     #v4
     \__          \     |         _/
        \__        \    |       _/
           \__    e3\   |     _/
	       e1  \__    \  |   _/				# = vertex
                 \__  \ | _/ e6
                    \__\_/
		                 #
				           v3


- edge e2 is being collapsed
- vertex v1 is vertex A of edge e2

When e2 is collapsed the following procedure is used:
   CODE												TRANSLATION
1	faceA->removeVertex( va );					f0->removeVertex( v1 );
2	faceB->removeVertex( va );					f1->removeVertex( v1 );
3	va->replaceWith( newVertex );				v1->replaceWith( newVertex );
4	vb->replaceWith( newVertex );				v2->replaceWith( newVertex );

A problem occurs with line 1:
removing v1 from f0, results in f0 attempting to attach to an
edge from v2 to v3, namely e4. Since e4 already has 2 faces (f1 & f2)
this will fail.

To fix this problem, line 2 should be called before line1 in this case.

The checkRemoveVertex() method checks too see if removing a vertex from a
face will cause this problem.
*/

	bool removeVAFromFaceA, removeVAFromFaceB;

	removeVAFromFaceA = removeVAFromFaceB = true;

	if ( faceA != NULL )
	{
		removeVAFromFaceA = faceA->checkRemoveVertex( va );

		if ( faceB != NULL )
		{
			removeVAFromFaceB = faceB->checkRemoveVertex( va );
		}
	}




	//if neither removal is possible, don't collapse the edge
	if ( !removeVAFromFaceA  &&  !removeVAFromFaceB )
	{
		return NULL;
	}


	bool attribBoundary = isVertexAttribBoundary();



	//COMPUTE VERTEX ATTRIBUTES
	if ( faceA != NULL )
	{
		faceA->edgeCollapseAttribs( faceAIndex, collapseTo );
		if ( faceB != NULL )
		{
			//if this edge does not lie on an attributes boundary, then
			//calling edgeCollapseAttribs() for faceB is unnecessary
			if ( attribBoundary )
			{
				MCollapseEdgeTo collapseTo_faceB;

				//order is inverted for face B
				switch ( collapseTo )
				{
				case MCOLLAPSEEDGETO_A:
					collapseTo_faceB = MCOLLAPSEEDGETO_B;
					break;
				case MCOLLAPSEEDGETO_B:
					collapseTo_faceB = MCOLLAPSEEDGETO_A;
					break;
				case MCOLLAPSEEDGETO_MIDPOINT:
				default:
					collapseTo_faceB = MCOLLAPSEEDGETO_MIDPOINT;
					break;
				}

				faceB->edgeCollapseAttribs( faceBIndex, collapseTo_faceB );
			}
		}
	}



	//PERFORM COLLAPSE
	MFace *fa = faceA, *fb = faceB;

	if ( !removeVAFromFaceA )
	{
		//cannot remove vertexA from faceA; remove from faceB first
		if ( fb != NULL )
		{
			fb->removeVertex( va, true, false );

			//the remove vertex operation will alter the situation, so
			//now check if vertexA can be removed from faceA
			if ( fa->checkRemoveVertex( va ) )	//now see if it can be removed from faceA
			{
				fa->removeVertex( va, true, false );
			}
		}
	}
	else
	{
		//can remove vertexA from faceA
		if ( removeVAFromFaceB )
		{
			//can remove from either:
			if ( fa != NULL )
			{
				fa->removeVertex( va, true, false );
				if ( fb != NULL )
				{
					fb->removeVertex( va, true, false );
				}
			}
		}
		else
		{
			//can remove from faceA:
			if ( fa != NULL )
			{
				//remove from faceA
				fa->removeVertex( va, true, false );

				//situation may have changed
				if ( fb != NULL )
				{
					//situation may have changed
					if ( fb->checkRemoveVertex( va ) )
					{
						fb->removeVertex( va, true, false );
					}
				}
			}
		}
	}


	MVertex *resultVertex = getMesh()->addVertex( vertexPosition );

	resultVertex->copyPropertiesFrom( va, vb );

	va->replaceWith( resultVertex );
	vb->replaceWith( resultVertex );


	//there are some cases where resultVertex may end up isolated
	bool destroyed = resultVertex->destroyIfIsolated();


	return destroyed  ?  NULL : resultVertex;
}

MVertex * MEdge::collapse(bool markMergedEdges, bool lockMarkedVertices)
{
	MCollapseEdgeTo collapseTo;

	if ( lockMarkedVertices )
	{
		if ( vertexA->isVertexMarked()  &&  vertexB->isVertexMarked() )
		{
			//both marked, collapse to mid point
			collapseTo = MCOLLAPSEEDGETO_MIDPOINT;
		}
		else if ( vertexA->isVertexMarked()  &&  !vertexB->isVertexMarked() )
		{
			//vertex A marked, collapse to vertex A
			collapseTo = MCOLLAPSEEDGETO_A;
		}
		else if ( !vertexA->isVertexMarked()  &&  vertexB->isVertexMarked() )
		{
			//vertex B marked, collapse to vertex B
			collapseTo = MCOLLAPSEEDGETO_B;
		}
		else
		{
			//neither vertex marked; collapse to mid point
			collapseTo = MCOLLAPSEEDGETO_MIDPOINT;
		}
	}
	else
	{
		//collapse to mid point
		collapseTo = MCOLLAPSEEDGETO_MIDPOINT;
	}


	//compute the position of the resulting vertex
	Point3 vertexPosition;
	switch ( collapseTo )
	{
	case MCOLLAPSEEDGETO_A:
		vertexPosition = vertexA->getPosition();
		break;
	case MCOLLAPSEEDGETO_B:
		vertexPosition = vertexB->getPosition();
		break;
	case MCOLLAPSEEDGETO_MIDPOINT:
	default:
		vertexPosition = Point3::average( vertexA->getPosition(), vertexB->getPosition() );
		break;
	}


	return collapse( vertexPosition, markMergedEdges, collapseTo );
}

MVertex * MEdge::collapseTo(const Point3 &vertexPosition, bool markMergedEdges, const MVertex *v)
{
	if ( v == NULL )
	{
		return collapse( vertexPosition, markMergedEdges, MCOLLAPSEEDGETO_MIDPOINT );
	}
	else if ( v == vertexA )
	{
		return collapse( vertexPosition, markMergedEdges, MCOLLAPSEEDGETO_A );
	}
	else if ( v == vertexB )
	{
		return collapse( vertexPosition, markMergedEdges, MCOLLAPSEEDGETO_B );
	}
	else
	{
		gs_assert_not_reached( "MEdge::collapseTo(): @v is not incident to @this and is not NULL\n" );
		return NULL;
	}
}


bool MEdge::canCollapse()
{
	MVertex *va = vertexA, *vb = vertexB;



	//if vertexA and vertexB share a common face f, which is NOT incident to
	//*this, then collapsing *this will result in f becoming butterfly/bow-tie
	//shaped; a collapse is not possible in this case
	MFace *shared = va->findCommonFaceWithVertexNotIncidentToEdge( vb, this );

	if ( shared != NULL )
	{
		return false;
	}



	//NOTE:
	//depending upon the surrounding mesh, collapsing this may
	//result in an invalid mesh, in which case a collapse is not possible
	if ( !MVertex::checkEdgeCollapseValidity( this, false ) )
	{
		return false;
	}


	//
	// CHECK TO SEE IF REMOVING VERTICES IS POSSIBLE.
	//
	// See diagram in MEdge::collapse()
	//

	bool removeVAFromFaceA, removeVAFromFaceB;

	removeVAFromFaceA = removeVAFromFaceB = true;

	if ( faceA != NULL )
	{
		removeVAFromFaceA = faceA->checkRemoveVertex( va );

		if ( faceB != NULL )
		{
			removeVAFromFaceB = faceB->checkRemoveVertex( va );
		}
	}


	return removeVAFromFaceA || removeVAFromFaceB;
}




// ******************* EDGE DISSOLVE OPS *******************

void MEdge::dissolveAttribs(MVertex *vertex, MFace *destFace)
{
	MVertexAttrib *a[2];

	//get pointers to the attribs at @vertex
	getVertexAttribs( vertex, a[0], a[1] );

	//compute mid point
	Point2f midPoint = Point2f::average( a[0]->getPoint(), a[1]->getPoint() );

	//generate vertex attrib
	MVertexAttrib newAttrib( midPoint, destFace->getMaterialID() );
	MVertexAttrib *attrib = vertex->createVertexAttrib( newAttrib, MFINDPOLICY_CREATE );

	//index of @vertex in @destFace
	int vertexIndex = getFaceVertexIndex( destFace, vertex );

	//unref the old attrib
	destFace->getVertexAttrib( vertexIndex )->unref();
	//set the attrib
	destFace->setVertexAttrib( vertexIndex, attrib );
	//ref the new one
	destFace->getVertexAttrib( vertexIndex )->ref();
}

void MEdge::dissolve(MFace *from, MFace *into, bool dissolveAttribs)
{
	Array<int> sharedEdgeIndices;
	from->findCommonEdgeIndices( into, sharedEdgeIndices );

	gs_assert( from->isEdgeIndexListContinuous( sharedEdgeIndices), "MEdge::dissolve(): shared edge index list is continuous\n" );

	if ( dissolveAttribs)
	{
		if ( sharedEdgeIndices.size() == 1 )
		{
			MEdge *e = from->getEdge( sharedEdgeIndices[0] );
			e->dissolveAttribs( e->getVertexA(), into );
			e->dissolveAttribs( e->getVertexB(), into );
		}
		else
		{
			//only the attribs at the first and last shared vertices
			//need to be dissolved; find these vertices
			int firstIndex = sharedEdgeIndices.front();
			int lastIndex = sharedEdgeIndices.back();

			//the first shared vertex is at the same index in @from as the
			//first shared edge
			MVertex *v0 = from->getVertex( firstIndex );
			//the last shared vertex is at the position after the position
			//of the last shared edge
			int lastSharedVertexIndex = nextIndex( lastIndex, from->getSize() );
			MVertex *vLast = from->getVertex( lastSharedVertexIndex );

			//need first and last edges too
			MEdge *e0 = from->getEdge( firstIndex );
			MEdge *eLast = from->getEdge( lastIndex );

			//dissolve the attribs
			e0->dissolveAttribs( v0, into );
			eLast->dissolveAttribs( vLast, into );
		}
	}

	from->mergeInto( into, sharedEdgeIndices );
}


MFace * MEdge::dissolve(MEdgeDissolveStyle style)
{
	if ( canDissolve() )
	{
		MFace *mergedFace = NULL;
		switch ( style )
		{
		case MEDGEDISSOLVESTYLE_A:
			mergedFace = faceA;
			dissolve( faceB, faceA, false );
			break;
		case MEDGEDISSOLVESTYLE_B:
			mergedFace = faceB;
			dissolve( faceA, faceB, false );
			break;
		case MEDGEDISSOLVESTYLE_MIDPOINT_MAT_A:
			mergedFace = faceA;
			dissolve( faceB, faceA, true );
			break;
		case MEDGEDISSOLVESTYLE_MIDPOINT_MAT_B:
			mergedFace = faceB;
			dissolve( faceA, faceB, true );
			break;
		default:
			break;
		}

		return mergedFace;
	}
	else
	{
		return NULL;
	}
}


bool MEdge::canDissolve() const
{
	if ( getNumFaces() == 2 )
	{
		//get the indices of the edges that are shared by @faceA and @faceB
		int numSharedEdges = faceA->getSharedEdgeCount( faceB );

		gs_assert( numSharedEdges != 0, "MEdge::canDissolve(): @numSharedEdges == 0\n" );

		//get the number of shared vertices
		int numSharedVertices = faceA->getSharedVertexCount( faceB );

		//if the shared edges form a continuous run, then the number of
		//vertices shared between @faceA and @faceB will be
		//@numSharedEdges + 1.
		//If this is not the case, then:
		//1. the edge run is discontinuous;
		//	  ( @numSharedVertices > @numSharedEdges + 1 )
		//2. other vertices that are not incident to the edge run are
		//   shared by the two faces,
		//	  ( @numSharedVertices > @numSharedEdges + 1 )
		//3. the edge run is circular; the only way this can happen
		//   is if @faceA and @faceB are identical (in terms of
		//   vertices and edges), except for opposite ordering
		//	  ( @numSharedVertices == @numSharedEdges )
		//In any of these cases the face merging cannot proceed.
		if ( numSharedVertices  !=  ( numSharedEdges + 1 ) )
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

// ******************* EDGE SPIN OPS *******************

bool MEdge::spin(int turns)
{
	if ( getNumFaces() == 2 )
	{
		//get the indices of the edges that are shared by @faceA and @faceB
		int numSharedEdges = faceA->getSharedEdgeCount( faceB );

		gs_assert( numSharedEdges != 0, "MEdge::dissolve(): @numSharedEdges == 0\n" );

		//get the number of shared vertices
		int numSharedVertices = faceA->getSharedVertexCount( faceB );

		// Can only spin @this if 1 edge and 2 vertices are shared
		if ( numSharedEdges == 1  &&  numSharedVertices == 2 )
		{
			// Get the vertices incident to @this
			MVertex *a = vertexA;
			MVertex *b = vertexB;
			MFace *mergedFace = faceA;
			EdgeProperties props = getProperties();

			// Merge @faceB into @faceA
			dissolve( faceB, faceA, true );

			// Get the indices of @a and @b in @mergedFace
			int indexA = mergedFace->findVertex( a );
			int indexB = mergedFace->findVertex( b );
			int faceSize = mergedFace->getSize();

			// Turn the indices
			indexA += turns;
			indexB += turns;

			if ( indexA < 0 )
			{
				indexA = faceSize  -  ( -indexA  %  faceSize );
			}
			if ( indexA >= faceSize )
			{
				indexA = indexA  %  faceSize;
			}

			if ( indexB < 0 )
			{
				indexB = faceSize  -  ( -indexB  %  faceSize );
			}
			if ( indexB >= faceSize )
			{
				indexB = indexB  %  faceSize;
			}

			gs_assert( indexA >= 0  &&  indexA < faceSize, "MEdge::spin(): @indexA is invalid\n" );
			gs_assert( indexB >= 0  &&  indexB < faceSize, "MEdge::spin(): @indexB is invalid\n" );

			// Create the turned edge
			MEdge *turnedEdge = NULL;
			MFace *f = mergedFace->split( indexA, indexB, turnedEdge );

			if ( f != NULL  &&  turnedEdge != NULL )
			{
				turnedEdge->setProperties( props );

				return true;
			}
		}
	}

	return false;
}


// ******************* REWIRE EDGE OPS *******************

bool MEdge::canRewire() const
{
	return canDissolve();
}






// ******************* UV-MAP OPS *******************

void MEdge::uvmapEdge(const Point2f &uvVertexA, const Point2f &uvVertexB)
{
	if ( faceA->isFaceMarked() )
	{
		faceA->setMaterialCoords( faceAIndex, uvVertexA );
		faceA->setMaterialCoords( nextIndex( faceAIndex, faceA->getSize() ), uvVertexB );
	}
	if ( faceB->isFaceMarked() )
	{
		faceB->setMaterialCoords( faceBIndex, uvVertexB );
		faceB->setMaterialCoords( nextIndex( faceBIndex, faceB->getSize() ), uvVertexA );
	}
}




// ******************* BOUNDARY / MARK OPS *******************

bool MEdge::isFrontFacing(const BackfaceCull *faceCull) const
{
	if ( faceA->isFrontFace( faceCull ) )
	{
		//@faceA is front facing, so @this is front facing
		return true;
	}

	//if we have already determined that faceA is front facing, then *this
	//is front facing, so no need to check faceB
	if ( faceB != NULL )
	{
		if ( faceB->isFrontFace( faceCull ) )
		{
			return true;
		}
	}


	return false;
}

bool MEdge::isWhollyInside(const Region3d *region) const
{
	return region->containsAllOf( getSegment() );
}

bool MEdge::isPartiallyInside(const Region3d *region) const
{
	return region->containsPartOf( getSegment() );
}

bool MEdge::isWhollyInside(const Region3d *region, const BackfaceCull *faceCull) const
{
	if ( !isFrontFacing( faceCull ) )
	{
		return false;
	}

	return isWhollyInside( region );
}

bool MEdge::isPartiallyInside(const Region3d *region, const BackfaceCull *faceCull) const
{
	if ( !isFrontFacing( faceCull ) )
	{
		return false;
	}

	return isPartiallyInside( region );
}






// ******************* DEBUG METHODS *******************

void MEdge::display()
{
	printf( "MEdge %p: %p -> %p   %p -> %p\n", this, vertexA, vertexB, faceA, faceB );
}

bool MEdge::checkIntegrity()
{
	bool passed = true;

	if ( !isDestroyed() )
	{
		if ( !vertexA->_hasEdge(this) )
		{
			printf( "MEdge %p: !vertexA->_hasEdge(this)\n", this );
			passed = false;
		}
		if ( !vertexB->_hasEdge(this) )
		{
			printf( "MEdge %p: !vertexB->_hasEdge(this)\n", this );
			passed = false;
		}
		if ( faceA != NULL )
		{
			if ( !faceA->isIncidentTo(this) )
			{
				printf( "MEdge %p: !faceA->isIncidentTo(this)\n", this );
				passed = false;
			}
			if ( !faceA->isEdgeInOrder_search(this) )
			{
				printf( "MEdge %p: should have vertices in same order as face A\n", this );
				passed = false;
			}
			if ( faceA->findEdge( this ) != faceAIndex )
			{
				printf( "MEdge %p: faceAIndex (%d) != faceA->findEdge( this ) (%d)\n", this, faceAIndex, faceA->findEdge( this ) );
				passed = false;
			}
		}
		else
		{
			if ( faceAIndex != -1 )
			{
				printf( "MEdge %p: faceA == NULL, faceAIndex should be -1\n", this );
				passed = false;
			}
		}
		if ( faceB != NULL )
		{
			if ( !faceB->isIncidentTo(this) )
			{
				printf( "MEdge %p: !faceB->isIncidentTo(this)\n", this );
				passed = false;
			}
			if ( faceB->isEdgeInOrder_search(this) )
			{
				printf( "MEdge %p: should NOT have vertices in same order as face B\n", this );
				passed = false;
			}
			if ( faceB->findEdge( this ) != faceBIndex )
			{
				printf( "MEdge %p: faceBIndex (%d) != faceB->findEdge( this ) (%d)\n", this, faceBIndex, faceB->findEdge( this ) );
				passed = false;
			}
		}
		else
		{
			if ( faceBIndex != -1 )
			{
				printf( "MEdge %p: faceB == NULL, faceBIndex should be -1\n", this );
				passed = false;
			}
		}
		if ( isIsolated() )
		{
			printf( "MEdge %p is isolated\n", this );
			passed = false;
		}

		if ( flags.indestructible )
		{
			printf( "MEdge %p is indestructible\n", this );
			passed = false;
		}
		if ( flags.secondaryMark )
		{
			printf( "MEdge %p is secondary marked\n", this );
			passed = false;
		}
	}


	return passed;
}







/****************************************************************************
 ****************************************************************************
										MEdgeRun::VertexIterator
 ****************************************************************************
 ****************************************************************************/

MEdgeRun::VertexIterator& MEdgeRun::VertexIterator::operator++()
{
	current = edgeRun->at( index )->getOppositeVertex( current );
	index++;
	return *this;
}

MEdgeRun::VertexIterator MEdgeRun::VertexIterator::operator++(int)
{
	VertexIterator iter = *this;
	current = edgeRun->at( index )->getOppositeVertex( current );
	index++;
	return iter;
}

MEdgeRun::VertexIterator& MEdgeRun::VertexIterator::operator--()
{
	index--;
	current = edgeRun->at( index )->getOppositeVertex( current );
	return *this;
}

MEdgeRun::VertexIterator MEdgeRun::VertexIterator::operator--(int)
{
	VertexIterator iter = *this;
	index--;
	current = edgeRun->at( index )->getOppositeVertex( current );
	return iter;
}


/****************************************************************************
												MEdgeRun

 ****************************************************************************/

MEdgeRun::MEdgeRun() : Array<MEdge*>()
{
	closedFlag = false;
	flippedFlag = false;
}

MEdgeRun::MEdgeRun(bool c) : Array<MEdge*>()
{
	closedFlag = c;
	flippedFlag = false;
}


bool MEdgeRun::isClosed() const
{
	return closedFlag;
}

void MEdgeRun::close()
{
	closedFlag = true;
}

void MEdgeRun::open()
{
	closedFlag = false;
}


int MEdgeRun::getNumVertices() const
{
	return closedFlag  ?  size() : size() + 1;
}


void MEdgeRun::flip()
{
	reverse();

	if ( size() == 1 )
	{
		flippedFlag = !flippedFlag;
	}
}


void MEdgeRun::extractVertices(MVertexList &vertices)
{
	vertices.setClosed( closedFlag );

	if ( size() == 0 )
	{
		return;
	}
	else if ( size() == 1 )
	{
		MEdge *e0 = at( 0 );
		vertices.clear();
		vertices.resize( 2 );

		if ( flippedFlag )
		{
			vertices[0] = e0->getVertexB();
			vertices[1] = e0->getVertexA();
		}
		else
		{
			vertices[0] = e0->getVertexA();
			vertices[1] = e0->getVertexB();
		}
		return;
	}

	vertices.clear();

	MEdge *e0 = at( 0 ), *e1 = at( 1 );

	MVertex *v = e0->getSharedVertex( e1 );

	gs_assert( v != NULL, "MEdgeRun::getVerties(): edges 0 and 1 do not share a vertex\n" );

	vertices.push_back( e0->getOppositeVertex( v ) );
	vertices.push_back( v );

	for (int i = 1; i < size() - 1; i++)
	{
		e1 = at( i );

		v = e1->getOppositeVertex( v );

		vertices.push_back( v );
	}

	if ( !closedFlag )
	{
		e1 = back();
		v = e1->getOppositeVertex( v );
		vertices.push_back( v );
	}
}

void MEdgeRun::extractVerticesAndAttribs(MVertexList &vertices, MVertexAttribList &attribs)
{
	vertices.setClosed( closedFlag );

	if ( size() == 0 )
	{
		return;
	}
	else if ( size() == 1 )
	{
		MEdge *e0 = at( 0 );
		vertices.clear();
		attribs.clear();
		if ( flippedFlag )
		{
			vertices.push_back( e0->getVertexB() );
			vertices.push_back( e0->getVertexA() );
			attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[1] ) );
			attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[0] ) );
		}
		else
		{
			vertices.push_back( e0->getVertexA() );
			vertices.push_back( e0->getVertexB() );
			attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[0] ) );
			attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[1] ) );
		}
		return;
	}

	vertices.clear();
	attribs.clear();

	MEdge *e0 = at( 0 ), *e1 = at( 1 );

	MVertex *v = e0->getSharedVertex( e1 );

	gs_assert( v != NULL, "MEdgeRun::getVerticesAndAttribs(): edges 0 and 1 do not share a vertex\n" );


	vertices.push_back( e0->getOppositeVertex( v ) );
	vertices.push_back( v );

	attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[0] ) );
	attribs.push_back( e0->getFaceA()->getVertexAttrib( vertices[1] ) );

	for (int i = 1; i < size() - 1; i++)
	{
		e1 = at( i );

		attribs.push_back( e1->getFaceA()->getVertexAttrib( v ) );

		v = e1->getOppositeVertex( v );

		vertices.push_back( v );
		attribs.push_back( e1->getFaceA()->getVertexAttrib( v ) );
	}

	e1 = back();
	attribs.push_back( e1->getFaceA()->getVertexAttrib( v ) );
	v = e1->getOppositeVertex( v );
	attribs.push_back( e1->getFaceA()->getVertexAttrib( v ) );

	if ( !closedFlag )
	{
		vertices.push_back( v );
	}
}

MVertex * MEdgeRun::getFirstVertex()
{
	if ( size() == 1 )
	{
		return flippedFlag  ?  at( 0 )->getVertexB()  :  at( 0 )->getVertexA();
	}
	else if ( size() >= 2 )
	{
		MVertex *sharedVertex = at( 0 )->getSharedVertex( at( 1 ) );
		return at( 0 )->getOppositeVertex( sharedVertex );
	}
	else
	{
		return NULL;
	}
}

double MEdgeRun::computeLength() const
{
	double cumulativeLength = 0.0;

	for (int i = 0; i < size(); i++)
	{
		cumulativeLength += at( i )->getLength();
	}

	return cumulativeLength;
}

void MEdgeRun::generateCumulativeLengthArray(Array<double> &cumulativeLengthArray)
{
	double cumulativeLength = 0.0;

	cumulativeLengthArray.reserve( size() + 1 );

	cumulativeLengthArray.clear();
	cumulativeLengthArray.push_back( 0.0 );

	for (int i = 0; i < size(); i++)
	{
		cumulativeLength += at( i )->getLength();
		cumulativeLengthArray.push_back( cumulativeLength );
	}
}

void MEdgeRun::generateUnitCumulativeLengthArray(Array<double> &cumulativeLengthArray)
{
	generateCumulativeLengthArray( cumulativeLengthArray );

	double totalLength = cumulativeLengthArray.back();
	double oneOverLength = totalLength == 0.0  ?  1.0  :  1.0 / totalLength;

	for (int i = 0; i < cumulativeLengthArray.size(); i++)
	{
		cumulativeLengthArray[i] *= oneOverLength;
	}
}

bool MEdgeRun::isEdge0InOrder() const
{
	if ( size() < 2 )
	{
		return true;
	}

	MVertex *sharedVertex = at( 0 )->getSharedVertex( at( 1 ) );

	gs_assert( sharedVertex != NULL, "MEdgeRun::isEdge0InOrder(): edges 0 and 1 do not share a vertex\n" );

	return sharedVertex == at( 0 )->getVertexB();
}

bool MEdgeRun::isEdgeInOrder(int index) const
{
	MVertex *sharedVertex, *compareVertex;

	if ( closedFlag )
	{
		int iPrev = prevIndex( index, size() );
		sharedVertex = at( iPrev )->getSharedVertex( at( index ) );
		compareVertex = at( index )->getVertexA();
	}
	else
	{
		if ( index == 0 )
		{
			int iNext = index + 1;
			sharedVertex = at( index )->getSharedVertex( at( iNext ) );
			compareVertex = at( index )->getVertexB();
		}
		else
		{
			int iPrev = index - 1;
			sharedVertex = at( iPrev )->getSharedVertex( at( index ) );
			compareVertex = at( index )->getVertexA();
		}
	}

	gs_assert( sharedVertex != NULL, "MEdgeRun::isEdgeInOrder(): adjacent edges do not share a vertex\n" );

	return sharedVertex == compareVertex;
}




MEdgeRun::VertexIterator MEdgeRun::verticesBegin()
{
	return VertexIterator( this, getFirstVertex(), 0 );
}



#endif
