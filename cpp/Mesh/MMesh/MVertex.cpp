//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MVERTEX_CPP__
#define MVERTEX_CPP__

#include <algorithm>
#include <sstream>

#include <Math/index.h>
#include <Math/clamp.h>
#include <Math/epsilon.h>

#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>

#include <Graphics/Drawing.h>
#include <PlatformSpecific/IncludeGL.h>


#define SIN_30				0.5
#define COS_30			0.8660254037844385965883020617





/****************************************************************************
 ****************************************************************************
											MVertexAttrib
 ****************************************************************************
 ****************************************************************************/

Pool<MVertexAttrib> MVertexAttrib::vertexAttribPool;



void * MVertexAttrib::operator new(size_t sz)
{
	return static_cast<void *>( vertexAttribPool.allocateRaw() );
}

void MVertexAttrib::operator delete(void *data)
{
	vertexAttribPool.freeRaw( static_cast<MVertexAttrib *>( data ) );
}

void MVertexAttrib::poolAllocReserve(int numItems)
{
	vertexAttribPool.reserve( numItems );
}



void MVertexAttrib::destroy()
{
	vertex->removeVertexAttrib( this );
}





/****************************************************************************
 ****************************************************************************
											MVertexNormal
 ****************************************************************************
 ****************************************************************************/

Pool<MVertexNormal> MVertexNormal::vertexNormalPool;



void * MVertexNormal::operator new(size_t sz)
{
	return static_cast<void *>( vertexNormalPool.allocateRaw() );
}

void MVertexNormal::operator delete(void *data)
{
	vertexNormalPool.freeRaw( static_cast<MVertexNormal *>( data ) );
}

void MVertexNormal::poolAllocReserve(int numItems)
{
	vertexNormalPool.reserve( numItems );
}




/****************************************************************************
 ****************************************************************************
												MVertexList
 ****************************************************************************
 ****************************************************************************/

MVertexList::MVertexList() : Array<MVertex*>()
{
	closedFlag = false;
}


void MVertexList::close()
{
	closedFlag = true;
}

void MVertexList::open()
{
	closedFlag = false;
}

void MVertexList::setClosed(bool c)
{
	closedFlag = c;
}

bool MVertexList::isClosed() const
{
	return closedFlag;
}


bool MVertexList::hasVertex(const MVertex *v) const
{
	for (int i = 0; i < size(); i++)
	{
		if ( at( i ) == v )
		{
			return true;
		}
	}

	return false;
}


BBox3 MVertexList::computeBBox() const
{
	BBox3 box;

	for (int i = 0; i < size(); i++)
	{
		box.addPoint( at( i )->getPosition() );
	}

	return box;
}

Point3 MVertexList::computeAverage() const
{
	Point3 average;

	for (int i = 0; i < size(); i++)
	{
		average.cumulativeAdd( at( i )->getPosition() );
	}

	average.scale( 1.0 / (double)size() );

	return average;
}

bool MVertexList::hasEdge(const MEdge *e) const
{
	int i = closedFlag ? size() - 1 : 0;
	int j = closedFlag ? 0 : 1;

	for (; j < size(); j++)
	{
		if (  e->links( at( i ), at( j ) )  )
		{
			return true;
		}

		i = j;
	}

	return false;
}

bool MVertexList::isEdgeInOrder(MEdge *e, int index) const
{
	return e->getVertexA()  ==  at( index );
}





/****************************************************************************
 ****************************************************************************
									MVertex::VertexNeighbourhood
 ****************************************************************************
 ****************************************************************************/

MVertex::VertexNeighbourhood::VertexNeighbourhood()
{
}

MVertex::VertexNeighbourhood::~VertexNeighbourhood()
{
}



bool MVertex::VertexNeighbourhood::hasMarkedEdges() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			return true;
		}
	}

	return false;
}

bool MVertex::VertexNeighbourhood::isOrderConsistent() const
{
	int edge0IndexInFace0 = edges[0]->getFaceIndex( faces[0] );
	int edge1IndexInFace0 = edges[1]->getFaceIndex( faces[0] );

	// order consistent if edge 0 comes before edge 1 in face 0
	return edge0IndexInFace0 == prevIndex( edge1IndexInFace0, faces[0]->getSize() );
}

bool MVertex::VertexNeighbourhood::isCounterClockwise() const
{
	int edge0IndexInFace0 = edges[0]->getFaceIndex( faces[0] );
	int edge1IndexInFace0 = edges[1]->getFaceIndex( faces[0] );

	// counter-clockwise if edge 0 comes after edge 1 in face 0
	return edge0IndexInFace0 == nextIndex( edge1IndexInFace0, faces[0]->getSize() );
}

void MVertex::VertexNeighbourhood::flip()
{
	//what is reversed depends upon whether the neighbourhood is closed or not
	//if it is open ( edges.size() == faces.size() + 1 ):
	//		reverse order of all faces and all edges
	//if it is closed ( edges.size() == faces.size() ):
	//		reverse order of all faces, and all edges except the first

	if ( isOpen() )
	{
		faces.reverse();
		edges.reverse();
	}
	else
	{
		faces.reverse();
		edges.reverse( 1, edges.size() );
	}
}





/****************************************************************************
 ****************************************************************************
									MVertex::NeighbourhoodIterator
 ****************************************************************************
 ****************************************************************************/

MVertex::NeighbourhoodIterator::NeighbourhoodIterator(MVertex *v, MEdge *e, MFace *f)
												: v( v ), e( e ), f( f )
{
	gs_assert( v != NULL, "MVertex::NeighbourhoodIterator::NeighbourhoodIterator(): v is NULL\n" );
	gs_assert( e != NULL, "MVertex::NeighbourhoodIterator::NeighbourhoodIterator(): e is NULL\n" );

	if ( this->f == NULL )
	{
		this->f = this->e->getFaceA();
	}
}


bool MVertex::NeighbourhoodIterator::next()
{
	if ( e != NULL  &&  f != NULL )
	{
		e = e->getNextEdge( f, v );
		f = e->getOppositeFace( f );
		return true;
	}

	return false;
}

bool MVertex::NeighbourhoodIterator::previous()
{
	if ( e != NULL )
	{
		MFace *f2 = e->getOppositeFace( f );
		if ( f2 != NULL )
		{
			e = e->getNextEdge( f2, v );
			f = f2;
			return true;
		}
	}

	return false;
}






/****************************************************************************
 ****************************************************************************
												MVertex
 ****************************************************************************
 ****************************************************************************/


Pool<MVertex> MVertex::vertexPool;



MVertex::MVertex(MMesh *m, int ndx)
				: attrib0( MVertexAttrib( 0 ) ), sharpness( 0.0f ), mesh( m ), index( ndx )
{
	vertexAttribs.reserve( 1 );
	vertexNormals.reserve( 1 );

	flags.vertexMark = false;
	flags.destroyed = false;
	flags.indestructible = false;
	flags.secondaryMark = false;
	flags.normalSharp = false;
	flags.positionChangedForMesh = true;
	flags.transformed = false;
	flags.normalsRequireRefresh = true;
	flags.topologyKnown = true;
	flags.topologySimple = true;
	flags.liveSubdRequiresRefresh = false;
	flags.liveSubdVertexRequiresRefresh = false;
	flags.liveSubdEdgeSharpnessRequiresRefresh = false;
	flags.liveSubdNormalSharpnessModified = false;
	flags.debug = false;

	vertexOpData.faceExtrude.multipleSectionsFlag = false;
	vertexOpData.faceExtrude.data.single = NULL;
}

MVertex::~MVertex()
{
	for (int i = 1; i < vertexAttribs.size(); i++)
	{
		getMesh()->destroyVertexAttrib( vertexAttribs[i] );
	}
	for (int i = 1; i < vertexNormals.size(); i++)
	{
		getMesh()->destroyVertexNormal( vertexNormals[i] );
	}
}



void * MVertex::operator new(size_t sz)
{
	return static_cast<void *>( vertexPool.allocateRaw() );
}

void MVertex::operator delete(void *data)
{
	vertexPool.freeRaw( static_cast<MVertex *>( data ) );
}

void MVertex::poolAllocReserve(int numItems)
{
	vertexPool.reserve( numItems );
}



void MVertex::copyFrom(const MVertex *vertex, MMesh *parentMesh, int edgesOffset, int facesOffset)
{
	//get pointers to the edges and faces
	edges.reserve( vertex->edges.size() );
	for (int i = 0; i < vertex->edges.size(); i++)
	{
		edges.push_back( parentMesh->edges[ vertex->edges[i]->index + edgesOffset ] );
	}

	faces.reserve( vertex->faces.size() );
	for (int i = 0; i < vertex->faces.size(); i++)
	{
		faces.push_back( parentMesh->faces[ vertex->faces[i]->index + facesOffset ] );
	}

	//duplicate the vertex attributes
	vertexAttribs.reserve( vertex->vertexAttribs.size() );
	vertexAttribs.push_back( &attrib0 );
	attrib0.copyFrom( &vertex->attrib0, this );
	for (int i = 1; i < vertex->vertexAttribs.size(); i++)
	{
		MVertexAttrib *attrib = parentMesh->newVertexAttrib( i );
		attrib->copyFrom( vertex->vertexAttribs[i], this );
		vertexAttribs.push_back( attrib );
	}

	//duplicate the vertex normals
	vertexNormals.reserve( vertex->vertexNormals.size() );
	vertexNormals.push_back( &normal0 );
	normal0.copyFrom( &vertex->normal0 );
	for (int i = 1; i < vertex->vertexNormals.size(); i++)
	{
		MVertexNormal *normal = parentMesh->newVertexNormal( i );
		normal->copyFrom( vertex->vertexNormals[i] );
		vertexNormals.push_back( normal );
	}

	//copy over the position, sharpness, and flags
	position = vertex->position;
	positionSave = vertex->positionSave;
	sharpness = vertex->sharpness;
	flags = vertex->flags;

	if ( flags.vertexMark )
	{
		getMesh()->incrementMarkedVertexCount();
	}

	vertexOpData.faceExtrude.multipleSectionsFlag = false;
	vertexOpData.faceExtrude.data.single = NULL;
}


void MVertex::read(FILE *f, MMesh *parentMesh)
{
	int numEdges, numFaces, numAttribs, numNormals;
	int index;
	//read the sizes of the lists
	fread( &numEdges, sizeof(int), 1, f );
	fread( &numFaces, sizeof(int), 1, f );
	fread( &numAttribs, sizeof(int), 1, f );
	fread( &numNormals, sizeof(int), 1, f );

	//read indices and compute pointers to the edges and faces
	edges.reserve( numEdges );
	for (int i = 0; i < numEdges; i++)
	{
		fread( &index, sizeof(int), 1, f );
		edges.push_back( parentMesh->edges[ index ] );
	}

	faces.reserve( numFaces );
	for (int i = 0; i < numFaces; i++)
	{
		fread( &index, sizeof(int), 1, f );
		faces.push_back( parentMesh->faces[ index ] );
	}

	//read the vertex attributes
	vertexAttribs.reserve( numAttribs );
	vertexAttribs.push_back( &attrib0 );
	attrib0.read( f, this );
	for (int i = 1; i < numAttribs; i++)
	{
		MVertexAttrib *attrib = parentMesh->newVertexAttrib( i );
		attrib->read( f, this );
		vertexAttribs.push_back( attrib );
	}

	//read the vertex normals
	vertexNormals.reserve( numNormals );
	vertexNormals.push_back( &normal0 );
	normal0.read( f );
	for (int i = 1; i < numNormals; i++)
	{
		MVertexNormal *normal = parentMesh->newVertexNormal( i );
		normal->read( f );
		vertexNormals.push_back( normal );
	}

	//read the position, sharpness, and flags
	position.read( f );
	positionSave = position;
	fread( &sharpness, sizeof(float), 1, f );
	fread( &flags, sizeof(flags), 1, f );

	if ( flags.vertexMark )
	{
		getMesh()->incrementMarkedVertexCount();
	}

	vertexOpData.faceExtrude.multipleSectionsFlag = false;
	vertexOpData.faceExtrude.data.single = NULL;
}

void MVertex::write(FILE *f)
{
	int numEdges, numFaces, numAttribs, numNormals;
	//write the sizes of the lists
	numEdges = edges.size();
	numFaces = faces.size();
	numAttribs = vertexAttribs.size();
	numNormals = vertexNormals.size();
	fwrite( &numEdges, sizeof(int), 1, f );
	fwrite( &numFaces, sizeof(int), 1, f );
	fwrite( &numAttribs, sizeof(int), 1, f );
	fwrite( &numNormals, sizeof(int), 1, f );

	//write the indices of the edges and faces
	for (int i = 0; i < edges.size(); i++)
	{
		fwrite( &edges[i]->index, sizeof(int), 1, f );
	}

	for (int i = 0; i < faces.size(); i++)
	{
		fwrite( &faces[i]->index, sizeof(int), 1, f );
	}

	//write the attribs and normals
	for (int i = 0; i < vertexAttribs.size(); i++)
	{
		vertexAttribs[i]->write( f );
	}

	for (int i = 0; i < vertexNormals.size(); i++)
	{
		vertexNormals[i]->write( f );
	}

	//compute topology info before writing
	refreshTopology();

	//write the position, sharpness, and flags
	position.write( f );
	fwrite( &sharpness, sizeof(float), 1, f );
	fwrite( &flags, sizeof(flags), 1, f );
}


void MVertex::read(SENode &node, MMesh *parentMesh)
{
	std::string header;
	node[0]  >>  header;
	if ( header == "v" )
	{
		int index;
		int numEdges = 0, numFaces = 0, numAttribs = 0;

		//read indices and compute pointers to the edges and faces
		SENode &edgesNode = node[1];
		edgesNode[0]  >>  numEdges;
		for (int i = 0; i < numEdges; i++)
		{
			edgesNode[i+2]  >>  index;
			edges.push_back( parentMesh->edges[ index ] );
		}

		SENode &facesNode = node[2];
		facesNode[0]  >>  numFaces;
		for (int i = 0; i < numFaces; i++)
		{
			facesNode[i+2]  >>  index;
			faces.push_back( parentMesh->faces[ index ] );
		}

		// Topology has been modified
		topologyModified();


		//read the vertex attributes
		SENode &attribsNode = node[3];
		attribsNode[0]  >>  numAttribs;
		vertexAttribs.push_back( &attrib0 );
		attrib0.read( attribsNode[2], this );
		for (int i = 1; i < numAttribs; i++)
		{
			MVertexAttrib *attrib = parentMesh->newVertexAttrib( i );
			attrib->read( attribsNode[i+2], this );
			vertexAttribs.push_back( attrib );
		}

		//read the position, sharpness, and flags
		Point3 pos;
		pos.read( node[4] );
		setPosition( pos );

		double sharp = 0.0;
		node[5]  >>  sharp;
		setSharpness( sharp );

		bool bVertexMarked = false, bNormalSharp = false;
		node[6]  >>  bVertexMarked;
		node[7]  >>  bNormalSharp;

		setVertexMark( bVertexMarked );
		setNormalSharpness( bNormalSharp );
	}
}

void MVertex::write(SENode &node)
{
	node.addItem()  <<  "v";

	SENode &edgesNode = node.addList();
	//write the indices of the edges and faces
	edgesNode.addItem()  <<  edges.size();
	edgesNode.addItem()  <<  ":";
	for (int i = 0; i < edges.size(); i++)
	{
		edgesNode.addItem()  <<  edges[i]->index;
	}

	SENode &facesNode = node.addList();
	facesNode.addItem()  <<  faces.size();
	facesNode.addItem()  <<  ":";
	for (int i = 0; i < faces.size(); i++)
	{
		facesNode.addItem()  <<  faces[i]->index;
	}

	//write the attribs
	SENode &attribsNode = node.addList();
	attribsNode.addItem()  <<  vertexAttribs.size();
	attribsNode.addItem()  <<  ":";
	for (int i = 0; i < vertexAttribs.size(); i++)
	{
		vertexAttribs[i]->write( attribsNode.addList() );
	}

	//write the position, sharpness, and flags
	getPosition().write( node.addList() );
	node.addItem()  <<  getSharpness();

	node.addItem()  <<  isVertexMarked();
	node.addItem()  <<  isNormalSharp();
}



void MVertex::preComputeFull()
{
	// Refresh vertex normals
	getNumVertexNormals();
}



void MVertex::optimiseMemoryUsage()
{
	edges.optimiseMemoryUsageLean();
	faces.optimiseMemoryUsageLean();
	vertexAttribs.optimiseMemoryUsageLean();
	vertexNormals.optimiseMemoryUsageLean();
}






// ******************* POSITION OPS *******************

void MVertex::restorePosition()
{
	setPosition( positionSave );
}

void MVertex::setPosition(const Point3 &p)
{
	position = p;

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->incidentVertexModified();
	}

	if ( !flags.positionChangedForMesh )
	{
		flags.positionChangedForMesh = true;

		getMesh()->vertexModified();
	}

	flags.liveSubdRequiresRefresh = true;
}

void MVertex::translate(const Vector3 &translation)
{
	setPosition( position + translation );
}

void MVertex::transformPosition(const Matrix4 &x)
{
	setPosition( position * x );
}

void MVertex::i_transformPosition(const Matrix4 &x, bool fromSaved)
{
	if ( fromSaved )
	{
		position = positionSave * x;
	}
	else
	{
		position *= x;
	}

	if ( !flags.transformed )
	{
		flags.transformed = true;

		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->incidentVertexTransformedLinear();
		}
	}

	if ( !flags.positionChangedForMesh )
	{
		flags.positionChangedForMesh = true;

		getMesh()->vertexModified();
	}

	flags.liveSubdRequiresRefresh = true;
}






// ******************* EDGE OPS *******************

void MVertex::addEdge(MEdge *e)
{
	edges.push_back( e );
	topologyModified();
}

void MVertex::removeEdge(MEdge *e)
{
	//find the edge and remove it
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i] == e )
		{
			edges.remove( i );
			topologyModified();
			return;
		}
	}
	gs_assert_not_reached( "MVertex::removeEdge(): %p: could not find edge %p to remove\n", this, e );
}


bool MVertex::isIncidentTo(const MEdge *e) const
{
	return e->isIncidentTo( this );
}

MEdge * MVertex::findEdgeTo(MVertex *v, MFindPolicy findPolicy)
{
	if ( findPolicy != MFINDPOLICY_FORCEDCREATE )
	{
		//return true if an edge from this to v can be found
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->getOppositeVertex( this ) == v )
			{
				return edges[i];
			}
		}
	}

	if ( findPolicy == MFINDPOLICY_CREATE  ||  findPolicy == MFINDPOLICY_FORCEDCREATE )
	{
		return mesh->addEdge( this, v );
	}
	else
	{
		return NULL;
	}
}

MEdge * MVertex::getBoundaryEdge() const
{
	//search for a boundary edge
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isBoundary() )
		{
			return edges[i];
		}
	}
	return NULL;
}

MEdge * MVertex::getOtherBoundaryEdge(MEdge *e)
{
	//search for a boundary edge
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i] != e )
		{
			if ( edges[i]->isBoundary() )
			{
				return edges[i];
			}
		}
	}
	return NULL;
}

MEdge * MVertex::getNextBoundaryEdge(MEdge *e)
{
	gs_assert( e->isBoundary(), "MVertex::getNextBoundaryEdge(): @e is not a boundary edge\n" );

	NeighbourhoodIterator iter = neighbourhoodIterator( e );

	// Iterate until we run out of faces
	while ( iter.face() != NULL )
	{
		iter.next();
	}

	gs_assert( iter.edge()->isBoundary(), "MVertex::getNextBoundaryEdge(): @iter.edge() is not a boundary edge\n" );

	return iter.edge();
}







// ******************* FACE OPS *******************

void MVertex::addFace(MFace *f)
{
	faces.push_back( f );
	topologyModified();
}

void MVertex::removeFace(MFace *f)
{
	//find the face and remove it
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i] == f )
		{
			faces.remove( i );
			topologyModified();
			return;
		}
	}
	gs_assert_not_reached( "MVertex::removeFace(): could not find face to remove\n" );
}


Vector3 MVertex::computeAverageNormal() const
{
	Vector3 normal;
	for (int i = 0; i < faces.size(); i++)
	{
		normal += faces[i]->getPlane().n;
	}

	return normal.getNormalised();
}


MFace * MVertex::findCommonFaceWithVertexNotIncidentToEdge(MVertex *v, MEdge *e)
{
	for (int i = 0; i < faces.size(); i++)
	{
		if ( v->isIncidentTo( faces[i] ) )
		{
			if ( !e->isIncidentTo( faces[i] ) )
			{
				return faces[i];
			}
		}
	}

	return NULL;
}


MFace * MVertex::findBestSplitFace(MVertex *splitTo, bool allowDegenerateSubfaces)
{
	MFace *bestFace = NULL;
	double bestSuitability = 0.0;

	if ( findEdgeTo( splitTo, MFINDPOLICY_RETURNNULL )  !=  NULL )
	{
		return NULL;
	}

	// Check each face that is incident to @this and @splitTo
	for (int i = 0; i < faces.size(); i++)
	{
		if ( splitTo->isIncidentTo( faces[i] ) )
		{
			MFace *f = faces[i];

			bool degenerateSubFaces;
			double suitability = f->computeVertexVertexSplitSuitability( this, splitTo, degenerateSubFaces );
			if ( !degenerateSubFaces  ||  allowDegenerateSubfaces )
			{
				if ( bestFace == NULL  ||  suitability > bestSuitability )
				{
					bestFace = f;
					bestSuitability = suitability;
				}
			}
		}
	}

	return bestFace;
}

MFace * MVertex::findBestSplitFace(const MVertexList &splitTargets)
{
	if ( splitTargets.size() >= 1 )
	{
		for (int i = 0; i < splitTargets.size(); i++)
		{
			gs_assert( splitTargets[i] != this, "MVertex::findBestSplitFace(): cannot connect @this to itself\n" );
		}

		// Generate a list of faces common to @this and the vertices in @splitTargets
		MFaceSet commonFaces;

		//get the faces common to @this and the first split target
		findCommonFacesWith( splitTargets[0], commonFaces );

		if ( commonFaces.size() > 0 )
		{
			//for each subsequent split target.....
			for (int i = 1; i < splitTargets.size(); i++)
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
				int indexInF = f->findVertex( this );
				double suitability = 0.0;
				bool degenerateSubFaces;

				for (int i = 0; i < splitTargets.size(); i++)
				{
					suitability += f->computeVertexVertexSplitSuitability( indexInF, f->findVertex( splitTargets[i] ), degenerateSubFaces );
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




bool MVertex::faceSplit(MVertex *vb, MFace *&faceA, MFace *&faceB, MEdge *&newEdge)
{
	//find the face to split
	faceA = findCommonFaceWith( vb );

	gs_assert( faceA != NULL, "MVertex::faceSplit(): could not find common face with @vb\n" );

	faceB = faceA->split( this, vb, newEdge );

	return faceB != NULL;
}


int MVertex::getFaceCapacityForEdgeTo(MVertex *vertex, MEdge *&edge)
{
	edge = findEdgeTo( vertex, MFINDPOLICY_RETURNNULL );

	if ( edge == NULL )
	{
		return 2;
	}
	else
	{
		return 2 - edge->getNumFaces();
	}
}






// ******************* VERTEX ATTRIBUTE OPS *******************

MVertexAttrib * MVertex::createVertexAttrib(const MVertexAttrib &attrib, MFindPolicy findPolicy)
{
	if ( findPolicy != MFINDPOLICY_FORCEDCREATE )
	{
		//try to match the attributes with an existing entry
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			if ( attrib == *vertexAttribs[i] )
			{
				return vertexAttribs[i];
			}
		}
	}

	//no existing entry will do:
	if ( findPolicy == MFINDPOLICY_CREATE  ||  findPolicy == MFINDPOLICY_FORCEDCREATE )
	{
		//create a new one
		MVertexAttrib *newAttrib;

		//0 vertex attribs, use attrib0
		if ( vertexAttribs.size() == 0 )
		{
			newAttrib = &attrib0;
			newAttrib->init( attrib );
		}
		else
		{
			//get a new attrib
			newAttrib = getMesh()->addVertexAttrib( attrib, vertexAttribs.size() );
		}

		newAttrib->setVertex( this );
		vertexAttribs.push_back( newAttrib );
		return newAttrib;
	}
	else
	{
		return NULL;
	}
}

void MVertex::removeVertexAttrib(MVertexAttrib *attrib)
{
	gs_assert( attrib->getVertex() == this, "MVertex::removeVertexAttrib(): attribute does not belong to @this\n" );
	if ( attrib == vertexAttribs[0] )
	{
		//removing attrib0;
		//replace it with attrib1
		if ( vertexAttribs.size() > 1 )
		{
			attrib0.init( *vertexAttribs[1] );
			attrib0.setVertex( this );
			replaceVertexAttrib( vertexAttribs[1], vertexAttribs[0] );
		}
	}
	else
	{
		for (int i = 1; i < vertexAttribs.size(); i++)
		{
			if ( attrib == vertexAttribs[i] )
			{
				vertexAttribs.remove( i );
				break;
			}
		}

		for (int i = 1; i < vertexAttribs.size(); i++)
		{
			vertexAttribs[i]->index = i;
		}

		getMesh()->destroyVertexAttrib( attrib );
	}
}

MVertexAttrib * MVertex::replaceVertexAttrib(MVertexAttrib *oldAttrib, MVertexAttrib *newAttrib)
{
	if ( oldAttrib != newAttrib )
	{
		//how this is handled depends upon whether or not oldAttrib is
		//attrib0, attrib0 is PART OF THE MVertex STRUCTURE, and this cannot be
		//simply removed
		if ( oldAttrib == vertexAttribs[0] )
		{
			//instead, of removing and replacing, copy the data from
			//@newAttrib to @attrib0, then replace newAttrib with attrib0 in the faces

			//copy data from @newAttrib to @attrib0
			attrib0.init( *newAttrib );
			//replace @newAttrib with @attrib0 in incident faces
			for (int i = 0; i < faces.size(); i++)
			{
				faces[i]->setVertexAttrib( newAttrib, &attrib0 );
			}
			//the faces referring to @newAttrib now refer to @attrib0,
			//so increase @attrib0's ref count accordingly
			attrib0.increaseRefCount( newAttrib->getRefCount() );
			//destroy @newAttrib as it is no longer used
			newAttrib->destroy();
			//the new attrib is @attrib0
			return &attrib0;
		}
		else
		{
			//is an ordinary attrib; replace it

			//find which attrib is being replaced
			for (int i = 1; i < vertexAttribs.size(); i++)
			{
				if ( oldAttrib == vertexAttribs[i] )
				{
					//found attrib to replace

					//replace @oldAttrib with @newAttrib in incident faces
					for (int i = 0; i < faces.size(); i++)
					{
						faces[i]->setVertexAttrib( oldAttrib, newAttrib );
					}

					//the faces referring to @oldAttrib now refer to @newAttrib,
					//so increase @newAttrib's ref count accordingly
					newAttrib->increaseRefCount( oldAttrib->getRefCount() );

					//destroy @oldAttrib
					oldAttrib->destroy();

					return newAttrib;
				}
			}

			//could not find attrib to replace
			return NULL;
		}
	}
	else
	{
		// Nothing to do
		return newAttrib;
	}
}


// ******************* VERTEX NORMAL OPS *******************

void MVertex::setNormalSharp()
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = true;

	if ( !wasSharp )
	{
		setNormalsRequireRefreshFlag();
		getMesh()->vertexNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}

void MVertex::setNormalSmooth()
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = false;

	if ( wasSharp )
	{
		setNormalsRequireRefreshFlag();
		getMesh()->vertexNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}

void MVertex::setNormalSharpness(bool s)
{
	bool wasSharp = isNormalSharp();
	flags.normalSharp = s;

	if ( wasSharp != s )
	{
		setNormalsRequireRefreshFlag();
		getMesh()->vertexNormalSharpnessChanged();
		flags.liveSubdNormalSharpnessModified = true;
	}
}

void MVertex::touchNormalSharpness()
{
	setNormalsRequireRefreshFlag();
	getMesh()->vertexNormalSharpnessChanged();
	flags.liveSubdNormalSharpnessModified = true;
}

MVertexNormal * MVertex::newVertexNormal(const Vector3f &n)
{
	MVertexNormal *normal;
	if ( vertexNormals.size() == 0 )
	{
		normal = &normal0;
	}
	else
	{
		normal = getMesh()->newVertexNormal( vertexNormals.size() );
	}

	normal->setNormal( n );

	vertexNormals.push_back( normal );

	return normal;
}

void MVertex::destroyVertexNormals()
{
	for (int i = 1; i < vertexNormals.size(); i++)
	{
		getMesh()->destroyVertexNormal( vertexNormals[i] );
	}

	vertexNormals.clear();
}

void MVertex::computeSectionNormals(MEdge *e)
{
	MFace *f = e->getFaceA();

	VertexNeighbourhood neighbourhood;

	discoverNeighbourhoodFromEdge( e, f, neighbourhood );

	//simply average the normals and apply to the faces
	MVertexNormal *vertexNormal = newVertexNormal();
	Vector3f &normal = vertexNormal->getNormal();

	for (int i = 0; i < neighbourhood.faces.size(); i++)
	{
		normal += neighbourhood.faces[i]->getPlane().n;
		neighbourhood.faces[i]->setVertexNormal( this, vertexNormal );
	}
	normal.normalise();

	for (int i = 0; i < neighbourhood.edges.size(); i++)
	{
		neighbourhood.edges[i]->secondaryMark();
	}
}

void MVertex::refreshVertexNormals()
{
	if ( flags.normalsRequireRefresh )
	{
		destroyVertexNormals();

		if ( flags.normalSharp )
		{
			//NORMAL SHARP: each face uses its own normal
			if ( faces.size() == 4 )
			{
				//deal with common special case
				faces[0]->setVertexNormal( this, newVertexNormal( faces[0]->getPlane().n ) );
				faces[1]->setVertexNormal( this, newVertexNormal( faces[1]->getPlane().n ) );
				faces[2]->setVertexNormal( this, newVertexNormal( faces[2]->getPlane().n ) );
				faces[3]->setVertexNormal( this, newVertexNormal( faces[3]->getPlane().n ) );
			}
			else
			{
				for (int i = 0; i < faces.size(); i++)
				{
					faces[i]->setVertexNormal( this, newVertexNormal( faces[i]->getPlane().n ) );
				}
			}
		}
		else
		{
			//discover the incident faces and edges, grouping together those faces
			//which share a normal at this vertex

			//start by counting the number of sharp edges
			int sharpEdgeCount = 0;

			for (int i = 0; i < edges.size(); i++)
			{
				if ( edges[i]->isNormalSharp()  ||  edges[i]->isBoundary() )
				{
					sharpEdgeCount++;
				}
			}


			//having 0 sharp edges is the most common case (generally), so
			//deal with this specially
			if ( sharpEdgeCount == 0 )
			{
				if ( isTopologySimple() )
				{
					//simply average the normals and apply to the faces
					MVertexNormal *vertexNormal = newVertexNormal();
					Vector3f &normal = vertexNormal->getNormal();

					//ordinary vertex
					if ( isOrdinary() )
					{
						//deal with common special case
						normal += faces[0]->getPlane().n;
						normal += faces[1]->getPlane().n;
						normal += faces[2]->getPlane().n;
						normal += faces[3]->getPlane().n;
						normal.normalise();

						faces[0]->setVertexNormal( this, vertexNormal );
						faces[1]->setVertexNormal( this, vertexNormal );
						faces[2]->setVertexNormal( this, vertexNormal );
						faces[3]->setVertexNormal( this, vertexNormal );
					}
					else
					{
						//extraordinary vertex
						for (int i = 0; i < faces.size(); i++)
						{
							normal += faces[i]->getPlane().n;
							faces[i]->setVertexNormal( this, vertexNormal );
						}
						normal.normalise();
					}
				}
				else
				{
					Array<VertexNeighbourhood> neighbourhoods;

					discoverAllNeighbourhoods( neighbourhoods );

					//for each neighbourhood
					for (int i = 0; i < neighbourhoods.size(); i++)
					{
						const VertexNeighbourhood &neighbourhood = neighbourhoods[i];

						//simply average the normals and apply to the faces
						MVertexNormal *vertexNormal = newVertexNormal();
						Vector3f &normal = vertexNormal->getNormal();

						for (int j = 0; j < neighbourhood.faces.size(); j++)
						{
							normal += neighbourhood.faces[j]->getPlane().n;
							neighbourhood.faces[j]->setVertexNormal( this, vertexNormal );
						}
						normal.normalise();
					}
				}
			}
			else
			{
				for (int i = 0; i < edges.size(); i++)
				{
					//NOTE:
					//an unusual scanning algorithm is used to ensure that the edge
					//list does not need to be scanned three times.
					//algo as follows:
					//- find an edge e to start from (boundary or normal sharp AND
					//  not visited)
					//- scan through faces, around *this from e, TRAVERSE in BOTH
					//  directions if possible
					//- only mark e as visited once scanning has been attempted in both
					//  directions (there will be no unvisited faces left, incident to e)
					//- do not mark e1 as visited, where e1 is the last edge encountered
					//  during the scanning, so that e1 may be used as a starting point
					//  for a subsequent scan
					//
					//find an edge to start from:
					//one that is normal sharp or is a boundary edge,
					//but is not secondary marked (already visited)
					if ( ( edges[i]->isNormalSharp()  ||  edges[i]->isBoundary() )  &&  !edges[i]->isSecondaryMarked() )
					{
						MEdge *e = edges[i];
						//get the incident face which is not secondary marked (already
						//visited)
						MFace *f = e->getSecondaryUnmarkedFace();


						//make sure we proceeed only if a face was found
						if ( f != NULL )
						{
							//compute the vertex normal for the group of faces
							//between two sharp edges


							//new normal for face group
							MVertexNormal *vertexNormal = newVertexNormal();
							Vector3f &normal = vertexNormal->getNormal();

							do
							{
								//add the face normal to the average normal
								normal += f->getPlane().n;
								//set the vertex normal
								f->setVertexNormal( this, vertexNormal );
								//visited this face
								f->secondaryMark();


								//rotating about this, get the next edge thru f
								e = e->getNextEdge( f, this );


								//if the new edge is sharp (or boundary) or is already
								//visited, this group of faces is done
								if ( e->isNormalSharp()  ||  e->isBoundary()  ||  e->isSecondaryMarked() )
								{
									break;
								}

								//visited this edge
								e->secondaryMark();
								//get the face on the opposite side of e
								f = e->getOppositeFace( f );


								//if e is a boundary edge, there wont be a face on the
								//opposite side of e, if this is true, f == NULL.
								//Since e should not be a boundary edge, something has
								//gone wrong if this point is reached
								gs_assert( f != NULL, "MVertex::computeVertexNormals(): could not get next face\n" );
							} while( !f->isSecondaryMarked() );	//loop while f is not visited

							//normalise the group normal
							normal.normalise();

							//need to test this edge again, the face on the opposite
							//side needs to be scanned
							i--;
						}
						else
						{
							//no incident faces that haven't been visited?
							//completely visited
							e->secondaryMark();
						}
					}
				}

				for (int i = 0; i < edges.size(); i++)
				{
					if ( !edges[i]->isSecondaryMarked() )
					{
						computeSectionNormals( edges[i] );
					}
				}

				//secondary unmark everything
				for (int i = 0; i < edges.size(); i++)
				{
					edges[i]->secondaryUnmark();
				}

				for (int i = 0; i < faces.size(); i++)
				{
					faces[i]->secondaryUnmark();
				}
			}
		}


		flags.normalsRequireRefresh = false;
	}

	for (int i = 0; i < faces.size(); i++)
	{
		gs_assert( faces[i]->getVertexNormal( faces[i]->findVertex( this ) ) != NULL, "MVertex::refreshVertexNormals(): vertex %d: normal for face %d is NULL\n", index, i );
	}
}





// ******************* TOPOLOGY OPS *******************

int MVertex::computeBoundaryCount() const
{
	int boundaryCount = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isBoundary() )
		{
			boundaryCount++;
		}
	}

	return boundaryCount;
}

bool MVertex::isOnBoundary() const
{
	return computeBoundaryCount() != 0;
}

bool MVertex::isOnOrdinaryBoundary() const
{
	return computeBoundaryCount() == 2  &&  edges.size() == 3  &&  faces.size() == 2;
}

bool MVertex::isOnOrdinaryCorner() const
{
	return computeBoundaryCount() == 2  &&  edges.size() == 2  &&  faces.size() == 1;
}


void MVertex::discoverNeighbourhoodFromEdge(MEdge *e, MFace *f, MEdgeList *edgeList, MFaceList *faceList)
{
	//use face A of e for f, if the parameter f is NULL
	if ( f == NULL )
	{
		f = e->getFaceA();
	}



	NeighbourhoodIterator iter = neighbourhoodIterator( e, f );
	//loop until e is encountered again (circular)
	do
	{
		//add e to the edge list
		if ( edgeList != NULL )
		{
			edgeList->push_back( iter.edge() );
		}

		//add f to the face list
		if ( faceList != NULL )
		{
			faceList->push_back( iter.face() );
		}

		//iterate
		bool iterationSuccess = iter.next();

		gs_assert( iterationSuccess, "MVertex::discoverNeighbourhoodFromEdge(): iteration failed\n" );

		//if the edge @iter.edge() is a boundary edge, then @iter.face() will be NULL
		if ( iter.face() == NULL )
		{
			//e will be a boundary edge
			break;
		}
	} while ( iter.edge() != e );


	//if this neighbourhood was open ended ( edge-face-edge-face-edge for example )
	//then the last edge will not yet be added
	if ( iter.edge() != e )
	{
		//open ended: The loop was termined by the break statement indicating
		//that it ran out of faces
		//add in the last edge
		if ( edgeList != NULL )
		{
			edgeList->push_back( iter.edge() );
		}
	}
}

void MVertex::discoverNeighbourhoodFromEdge(MEdge *e, MFace *f, VertexNeighbourhood &neighbourhood)
{
	discoverNeighbourhoodFromEdge( e, f, &neighbourhood.edges, &neighbourhood.faces );
}

void MVertex::computeNeighbourhoodSize(MEdge *e, MFace *f, int &numEdges, int &numFaces) const
{
	numEdges = numFaces = 0;

	MEdge *firstEdge = e;

	//use face A of @e for @f, if the parameter @f is NULL
	if ( f == NULL )
	{
		f = e->getFaceA();
	}



	//loop until @e is encountered again (circular)
	do
	{
		//count @e
		numEdges++;

		//count @f
		numFaces++;

		//get the next edge
		e = e->getNextEdge( f, this );
		//go through opposite face in preparation for next loop
		f = e->getOppositeFace( f );


		//wont bother checking @e to see if it is NULL,
		//as MEdge::getNextEdge() checks for this

		//note:
		//e->getOppositeFace() may return NULL, if @e is a boundary edge, so:
		if ( f == NULL )
		{
			//@e will be a boundary edge
			break;
		}
	} while ( e != firstEdge );


	//if this neighbourhood is open ended ( edge-face-edge-face-edge for example )
	//then the last edge will not yet be counted
	if ( e != firstEdge )
	{
		//open ended: The loop was termined by the break statement indicating
		//that it ran out of faces
		//count the last edge
		numEdges++;
	}
}

void MVertex::discoverAllNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods)
{
	while ( 1 )
	{
		MEdge *startEdge = NULL;

		//start by looking for a boundary, unvisited (secondary unmarked) edge
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isBoundary()  &&  !edges[i]->isSecondaryMarked() )
			{
				startEdge = edges[i];
			}
		}

		if ( startEdge == NULL )
		{
			//no start edge found yet, look for any unvisited edge
			for (int i = 0; i < edges.size(); i++)
			{
				if ( !edges[i]->isSecondaryMarked() )
				{
					startEdge = edges[i];
				}
			}
		}

		if ( startEdge == NULL )
		{
			//couldn't find an edge to start from, quit
			break;
		}


		MFace *startFace = startEdge->getSecondaryUnmarkedFace();

		gs_assert( startFace != NULL, "MVertex::discoverAllNeighbourhoods(): could not find starting face\n" );


		//create a new neighbourhood structure
		VertexNeighbourhood &neighbourhood = neighbourhoods.push_back();

		discoverNeighbourhoodFromEdge( startEdge, startFace, neighbourhood );

		//mark edges and faces as visited
		for (int i = 0; i < neighbourhood.edges.size(); i++)
		{
			neighbourhood.edges[i]->secondaryMark();
		}

		for (int i = 0; i < neighbourhood.faces.size(); i++)
		{
			neighbourhood.faces[i]->secondaryMark();
		}
	}

	//secondary unmark all edges and faces
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->secondaryUnmark();
	}

	for (int i = 0; i < faces.size(); i++)
	{
		faces[i]->secondaryUnmark();
	}
}


void MVertex::topologyModified()
{
	flags.topologyKnown = false;
	flags.topologySimple = false;
	flags.normalsRequireRefresh = true;
	getMesh()->topologyModified();
}

void MVertex::refreshTopology() const
{
	if ( !flags.topologyKnown )
	{
		//find an edge e to start with.
		//this is either a boundary edge, or edge 0 if no boundary edge can be found
		MEdge *startEdge;

		startEdge = getBoundaryEdge();
		if ( startEdge == NULL )
		{
			startEdge = edges[0];
		}

		//scan from e through e->faceA,
		//this will either:
		//		-return to e
		//		-terminate at a boundary edge
		//MEdgeList edgeList;
		int numEdgesInNeighbourhood, numFacesInNeighbourhood;
		computeNeighbourhoodSize( startEdge, startEdge->getFaceA(), numEdgesInNeighbourhood, numFacesInNeighbourhood );


		//now find out if this is the only segment incident to *this
		if ( numEdgesInNeighbourhood == edges.size()  &&  numFacesInNeighbourhood == faces.size() )
		{
			//yes, this is the only segment
			flags.topologySimple = true;
		}
		else
		{
			flags.topologySimple = false;
		}

		flags.topologyKnown = true;
	}
}



// ******************* DESTROY OPS *******************

bool MVertex::destroyIfIsolated()
{
	if ( isIsolated()  &&  !flags.indestructible )
	{
		mesh->destroyVertex( this );
		return true;
	}
	else
	{
		return false;
	}
}



void MVertex::setSharpness(double s)
{
	sharpness = clampLower( s, 0.0 );
	flags.liveSubdVertexRequiresRefresh = true;
	flags.liveSubdEdgeSharpnessRequiresRefresh = true;
	getMesh()->vertexSharpnessChanged();
}


// ******************* SHARPNESS OPS *******************

int MVertex::computeCreasedEdgeCount() const
{
	int creasedEdgeCount = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isCreased() )
		{
			creasedEdgeCount++;
		}
	}

	return creasedEdgeCount;
}




// ******************* VERTEX MARK OPS *******************

void MVertex::vertexMark()
{
	if ( !flags.vertexMark )
	{
		getMesh()->incrementMarkedVertexCount();
	}

	flags.vertexMark = true;
}

void MVertex::vertexUnmark()
{
	if ( flags.vertexMark )
	{
		getMesh()->decrementMarkedVertexCount();
	}

	flags.vertexMark = false;
}

void MVertex::vertexMarkInvert()
{
	if ( flags.vertexMark )
	{
		getMesh()->decrementMarkedVertexCount();
	}
	else
	{
		getMesh()->incrementMarkedVertexCount();
	}

	flags.vertexMark = !flags.vertexMark;
}

void MVertex::setVertexMark(bool mark)
{
	if ( flags.vertexMark != mark )
	{
		if ( mark )
		{
			getMesh()->incrementMarkedVertexCount();
		}
		else
		{
			getMesh()->decrementMarkedVertexCount();
		}
	}

	flags.vertexMark = mark;
}



bool MVertex::hasNeighbouringMarkedVertices() const
{
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MVertex *neighbourVertex = edges[edgeI]->getOppositeVertex( this );

		if ( neighbourVertex->isVertexMarked() )
		{
			return true;
		}
	}

	return false;
}

bool MVertex::hasNeighbouringUnmarkedVertices() const
{
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MVertex *neighbourVertex = edges[edgeI]->getOppositeVertex( this );

		if ( !neighbourVertex->isVertexMarked() )
		{
			return true;
		}
	}

	return false;
}


void MVertex::discoverVertexMarkedRegion(MVertexList &region)
{
	//search the mesh for adjacent marked vertices, recording them as we go,
	//using a bredth first search algorithm
	region.clear();

	std::list<MVertex*> vertexStack;

	vertexStack.push_back( this );
	secondaryMark();


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		region.push_back( v );

		for (int i = 0; i < v->edges.size(); i++)
		{
			MVertex *oppositeVertex = v->edges[i]->getOppositeVertex( v );

			if ( !oppositeVertex->isSecondaryMarked()  &&  oppositeVertex->isVertexMarked() )
			{
				vertexStack.push_back( oppositeVertex );
				oppositeVertex->secondaryMark();
			}
		}
	}
}



// ******************* EDGE MARK OPS *******************

bool MVertex::isEdgeMarked() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			return true;
		}
	}

	return false;
}

int MVertex::getMarkedEdgeCount() const
{
	int count = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			count++;
		}
	}

	return count;
}

MEdge * MVertex::getMarkedEdge() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getMarkedEdgeSecondaryUnmarked() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  !edges[i]->isSecondaryMarked() )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getOtherMarkedEdge(const MEdge *e) const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked()  &&  edges[i] != e )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getOtherUnmarkedEdge(const MEdge *e) const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( !edges[i]->isEdgeMarked()  &&  edges[i] != e )
		{
			return edges[i];
		}
	}

	return NULL;
}

void MVertex::getMarkedEdgeList(MEdgeList &edgeList) const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isEdgeMarked() )
		{
			edgeList.push_back( edges[i] );
		}
	}
}


void MVertex::discoverEdgeMarkedRegionVertices(MVertexList &region)
{
	//search the mesh for vertices by crossing incident marked edges, recording them as we go,
	//using a bredth first search algorithm
	region.clear();

	std::list<MVertex*> vertexStack;

	vertexStack.push_back( this );
	secondaryMark();


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		region.push_back( v );

		for (int i = 0; i < v->edges.size(); i++)
		{
			MEdge *e = v->edges[i];

			if ( e->isEdgeMarked() )
			{
				MVertex *oppositeVertex = e->getOppositeVertex( v );

				if ( !oppositeVertex->isSecondaryMarked() )
				{
					vertexStack.push_back( oppositeVertex );
					oppositeVertex->secondaryMark();
				}
			}
		}
	}
}

void MVertex::discoverEdgeMarkedRegionVertices(MVertexList &regionVertices, MEdgeList &regionEdges)
{
	//search the mesh for vertices by crossing incident marked edges, recording them as we go,
	//using a bredth first search algorithm
	regionVertices.clear();
	regionEdges.clear();

	std::list<MVertex*> vertexStack;

	vertexStack.push_back( this );
	secondaryMark();


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		regionVertices.push_back( v );

		for (int i = 0; i < v->edges.size(); i++)
		{
			MEdge *e = v->edges[i];

			if ( e->isEdgeMarked() )
			{
				if ( !e->isSecondaryMarked() )
				{
					regionEdges.push_back( e );
					e->secondaryMark();
				}

				MVertex *oppositeVertex = e->getOppositeVertex( v );

				if ( !oppositeVertex->isSecondaryMarked() )
				{
					vertexStack.push_back( oppositeVertex );
					oppositeVertex->secondaryMark();
				}
			}
		}
	}
}


bool MVertex::discoverNeighbourhoodBetweenMarkedEdges(MEdge *e, MFace *f, MEdgeList *edgeList, MFaceList *faceList, MEdge *boundaryEdges[2])
{
	gs_assert( e->isEdgeMarked(), "MVertex::discoverNeighbourhoodBetweenMarkedEdges(): @e is not edge marked\n" );

	//store the first edge; it is needed for a check at the end
	MEdge *firstEdge = e;

	if ( boundaryEdges != NULL )
	{
		boundaryEdges[0] = e;
	}

	if ( edgeList != NULL )
	{
		edgeList->push_back( e );
	}


	//loop until a marked edge is encountered
	do
	{
		//add f to the face list
		if ( faceList != NULL )
		{
			faceList->push_back( f );
		}

		//get the next edge
		e = e->getNextEdge( f, this );
		//go through opposite face in preparation for next loop
		f = e->getOppositeFace( f );


		//add e to the edge list
		if ( edgeList != NULL )
		{
			edgeList->push_back( e );
		}


		//wont bother checking e to see if it is NULL,
		//as MEdge::getNextEdge() checks for this

		//note:
		//e->getOppositeFace() may return NULL, if e is a boundary edge, so:
		if ( f == NULL )
		{
			//e will be a boundary edge
			break;
		}
	} while ( !e->isEdgeMarked() );


	//at the end of the loop, e will either be a boundary edge (terminated due
	//to f == NULL), or e will be edge marked

	if ( boundaryEdges != NULL )
	{
		boundaryEdges[1] = e;
	}


	return e->isEdgeMarked()  &&  e != firstEdge;
}

bool MVertex::discoverNeighbourhoodBetweenMarkedEdges(MEdge *e, MFace *f, VertexNeighbourhood &neighbourhood, MEdge *boundaryEdges[2])
{
	return discoverNeighbourhoodBetweenMarkedEdges( e, f, &neighbourhood.edges, &neighbourhood.faces, boundaryEdges );
}

MEdge * MVertex::traverseToOtherMarkedEdge(MEdge *e)
{
	MEdge *boundaryEdges[2];

	if ( discoverNeighbourhoodBetweenMarkedEdges( e, e->getFaceA(), NULL, NULL, boundaryEdges ) )
	{
		return boundaryEdges[1];
	}
	else
	{
		if ( e->getFaceB() != NULL )
		{
			if ( discoverNeighbourhoodBetweenMarkedEdges( e, e->getFaceB(), NULL, NULL, boundaryEdges ) )
			{
				return boundaryEdges[1];
			}
		}
	}

	return NULL;
}


// ******************* FACE MARK OPS *******************

bool MVertex::isFaceMarked() const
{
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			return true;
		}
	}

	return false;
}

int MVertex::getFaceMarkBoundaryCount() const
{
	int count = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceMarkBoundary() )
		{
			count++;
		}
	}

	return count;
}

bool MVertex::isOnFaceMarkBoundary() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceMarkBoundary() )
		{
			return true;
		}
	}

	return false;
}

bool MVertex::isFaceMarkInternal() const
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( !edges[i]->isFaceMarkInternal() )
		{
			return false;
		}
	}

	return true;
}



void MVertex::discoverFaceMarkNeighbourhood(MEdge *e, MEdgeList *edgeList, MFaceList *faceList, MEdge *boundaryEdges[2], bool markedFlag)
{
	gs_assert( e->isFaceMarkBoundary()  ||  ( !markedFlag  &&  e->isBoundary()  &&  !e->isFaceMarkBoundary() ),
			"MVertex::discoverFaceMarkNeighbourhood(): seed edge must be a face-mark boundary\n" );

	if ( boundaryEdges != NULL )
	{
		boundaryEdges[0] = e;
	}

	if ( edgeList != NULL )
	{
		edgeList->push_back( e );
	}

	//need to choose a face (from faceA and faceB) to go through
	MFace *f = NULL;
	if ( markedFlag )
	{
		// Discovering a marked neighbourhood; get the marked face
		f = e->getMarkedFace();
	}
	else
	{
		// Discovering an unmarked neighbourhood; get the unmarked face
		f = e->getUnmarkedFace();
	}


	//while the face that we are about to rotate through, about this,
	//remains targeted (marked if discovering a marked neighbourhood, unmarked otherwise), continue to rotate
	while ( f->isFaceMarked()  ==  markedFlag )
	{
		if ( faceList != NULL )
		{
			faceList->push_back( f );
		}

		//get the next edge
		e = e->getNextEdge( f, this );
		//go through opposite face in preparation for next loop
		f = e->getOppositeFace( f );


		if ( edgeList != NULL )
		{
			edgeList->push_back( e );
		}


		//wont bother checking e or f to see if they are NULL,
		//as MEdge::getNextEdge() and MEdge::getOppositeFace() check for this

		//note:
		//e->getOppositeFace() may return NULL, if e is a boundary edge, so:
		if ( f == NULL )
		{
			//e will be a boundary edge, and the previous face will be
			//face marked, making e a face mark boundary edge
			break;
		}
	}


	//at the end of the loop, the non-face-marked edge e will have caused the
	//loop to terminate, therefore, the previous edge (face-marked-boundary)
	//is the one we intended to find

	if ( boundaryEdges != NULL )
	{
		boundaryEdges[1] = e;
	}
}

void MVertex::discoverFaceMarkNeighbourhood(MEdge *e, VertexNeighbourhood &neighbourhood, bool markedFlag)
{
	discoverFaceMarkNeighbourhood( e, &neighbourhood.edges, &neighbourhood.faces, NULL, markedFlag );
}

void MVertex::discoverAllFaceMarkNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods, bool markedFlag)
{
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];

		// Start at an edge that is:
		//   - not visited (not secondary marked)
		//   - if discovering face marked neighbourhoods:
		//	- is face mark boundary
		//   - if discovering face unmarked neighbourhoods:
		//	- is face mark boundary  OR  boundary  (not both)
		if ( !e->isSecondaryMarked()  &&
			(	( markedFlag  &&  e->isFaceMarkBoundary() )  ||
				( !markedFlag  &&  ( e->isBoundary()  != e->isFaceMarkBoundary() ) ) ) )
		{
			VertexNeighbourhood &neighbourhood = neighbourhoods.push_back();

			discoverFaceMarkNeighbourhood( e, neighbourhood, markedFlag );
			neighbourhood.edges.front()->secondaryMark();
			neighbourhood.edges.back()->secondaryMark();
		}
	}

	for (int i = 0; i < neighbourhoods.size(); i++)
	{
		neighbourhoods[i].edges.front()->secondaryUnmark();
		neighbourhoods[i].edges.back()->secondaryUnmark();
	}

	if ( markedFlag )
	{
		//if *this is face mark internal, then there will be no face mark boundary
		//edges, in which case no face neighbourhoods will have been discovered
		//check for this:
		if ( isFaceMarkInternal() )
		{
			discoverAllNeighbourhoods( neighbourhoods );
		}
	}
	else
	{
		//opposite of above
		if ( !isFaceMarked() )
		{
			discoverAllNeighbourhoods( neighbourhoods );
		}
	}
}



MEdge * MVertex::getOtherFaceMarkBoundaryEdge(MEdge *e)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isFaceMarkBoundary()  &&  edges[i] != e )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getNextFaceMarkBoundaryEdge(MEdge *e)
{
	if ( getFaceMarkBoundaryCount() == 2 )
	{
		return getOtherFaceMarkBoundaryEdge( e );
	}
	else
	{
		gs_assert( e->isFaceMarkBoundary(), "MVertex::getNextFaceMarkedBoundaryEdge(): @e is not face mark boundary\n" );

		//need to choose a face (from faceA and faceB) to go through
		MFace *f = e->getMarkedFace();


		//while the face that we are about to rotate through, about this,
		//remains marked, continue to rotate
		while ( f->isFaceMarked() )
		{
			//get the next edge
			e = e->getNextEdge( f, this );
			//go through opposite face in preparation for next loop
			f = e->getOppositeFace( f );

			//wont bother checking e or f to see if they are NULL,
			//as MEdge::getNextEdge() and MEdge::getOppositeFace() check for this

			//note:
			//e->getOppositeFace() may return NULL, if e is a boundary edge, so:
			if ( f == NULL )
			{
				//e will be a boundary edge, and the previous face will be
				//face marked, making e a face mark boundary edge
				break;
			}
		}


		//at the end of the loop, the non-face-marked edge e will have caused the
		//loop to terminate, therefore, the previous edge (face-marked-boundary)
		//is the one we intended to find

		return e;
	}
}

MFace * MVertex::getFaceMarkedFace()
{
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			return faces[i];
		}
	}

	return NULL;
}

void MVertex::discoverFaceMarkedRegionVertices(MVertexList &region)
{
	//search the mesh for adjacent marked faces, recording them as we go,
	//using a bredth first search algorithm
	region.clear();

	std::list<MVertex*> vertexStack;

	vertexStack.push_back( this );
	secondaryMark();


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		region.push_back( v );

		for (int i = 0; i < v->faces.size(); i++)
		{
			MFace *f = v->faces[i];

			if ( !f->isSecondaryMarked()  &&  f->isFaceMarked() )
			{
				f->discoverFaceMarkedRegionVerticesPopulateStack( vertexStack );
				f->secondaryMark();
			}
		}
	}
}

void MVertex::discoverFaceMarkedRegionVertices(MVertexList &regionVertices, MFaceList &regionFaces)
{
	//search the mesh for adjacent marked faces, recording them as we go,
	//using a bredth first search algorithm
	regionVertices.clear();
	regionFaces.clear();

	std::list<MVertex*> vertexStack;

	vertexStack.push_back( this );
	secondaryMark();


	while ( vertexStack.size() > 0 )
	{
		MVertex *v = vertexStack.front();
		vertexStack.pop_front();

		regionVertices.push_back( v );

		for (int i = 0; i < v->faces.size(); i++)
		{
			MFace *f = v->faces[i];

			if ( !f->isSecondaryMarked()  &&  f->isFaceMarked() )
			{
				f->discoverFaceMarkedRegionVerticesPopulateStack( vertexStack );
				f->secondaryMark();
				regionFaces.push_back( f );
			}
		}
	}
}




// ******************* PATH DISCOVERY OPS *******************

bool MVertex::discoverEdgeLoopPathTo(MVertex *target, MEdgeRun &path)
{
	MDijkstraHeap<MVertex> heap;

	discoverPathSetCost( heap, 0.0, NULL, NULL );

	while ( !heap.isEmpty() )
	{
		MVertex *vertex = heap.removeMinCost();

		if ( vertex == target )
		{
			target->getDijkstraNode()->buildPath( target, path );
			return true;
		}
		else
		{
			double cumulativeCost = vertex->vertexOpData.dijkstra.node->cumulativeCost;

			// For each successor
			for (int i = 0; i < vertex->getValence(); i++)
			{
				MEdge *edge = vertex->getEdge( i );
				MVertex *succ = edge->getOppositeVertex( vertex );
				double cost = edge->getLength();
				double succCost = cumulativeCost + cost;
				succ->discoverPathSetCost( heap, succCost, vertex, edge );
			}
		}
	}

	return false;
}


void MVertex::discoverPathInit()
{
	vertexOpData.dijkstra.node = NULL;
}

void MVertex::discoverPathFinish()
{
	if ( vertexOpData.dijkstra.node != NULL )
	{
		delete vertexOpData.dijkstra.node;
	}
}


void MVertex::discoverPathSetCost(MDijkstraHeap<MVertex> &heap, double cumulativeCost, MVertex *previous, MEdge *edge)
{
	if ( vertexOpData.dijkstra.node == NULL )
	{
		MDijkstraNode<MVertex, MEdge> *node = new MDijkstraNode<MVertex, MEdge>( cumulativeCost, previous, edge );
		vertexOpData.dijkstra.node = node;
		heap.add( this );
	}
	else
	{
		MDijkstraNode<MVertex, MEdge> *node = vertexOpData.dijkstra.node;
		if ( node->cumulativeCost  >  cumulativeCost )
		{
			node->cumulativeCost = cumulativeCost;
			node->previous = previous;
			node->pathSeg = edge;
			heap.costChanged( this );
		}
	}
}








// ******************* SUBDIVISION OPS *******************

float MVertex::computeAverageEdgeSharpnessForSubdivision() const
{
	float edgeSharpness = 0.0;
	int count = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isCreased() )
		{
			edgeSharpness += edges[i]->getSharpness();
			count++;
		}
	}

	return count >= 3  ?  -1.0  :  edgeSharpness * 0.5;
}

float MVertex::catmullClarkComputeEdgeSharpnessForVertexPos(MEdge *sharpEdges[2])
{
	int count = 0;
	float edgeSharpness = 0.0;
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isCreased() )
		{
			if ( count < 2 )
			{
				sharpEdges[count] = edges[i];
			}
			//edgeSharpness += clampUpper( edges[i]->getSharpness(), 1.0 );
			edgeSharpness += edges[i]->getSharpness();
			count++;
		}
	}
	edgeSharpness *= 1.0 / (double)count;
	edgeSharpness = clampUpper( edgeSharpness, 1.0f );
	return edgeSharpness;
}



//uses the smooth subdivision rule to compute the new position of the
//point of attrib
Point2f MVertex::catmullClarkSmoothAttrib(MVertexAttrib *attrib)
{
	double n = 0.0;
	Point2f smooth;

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->getVertexAttrib( this ) == attrib )
		{
			smooth.cumulativeAdd( faces[i]->getSubdivisionVertex()->getVertexAttrib( 0 )->getPoint() );
			n += 1.0;
		}
	}

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->usesVertexAttrib( attrib ) )
		{
			smooth.cumulativeAdd( edges[i]->getAttribAtOppositeVertex( attrib )->getSavedPoint() );
		}
	}

	double oneOverN = 1.0 / n;
	smooth.scale( oneOverN * oneOverN );
	smooth.cumulativeAdd( Point2f::mul( attrib->getPoint(), ( n - 2 ) * oneOverN ) );

	return smooth;
}

//uses the crease subdivision rule to compute the new position of the
//point of attrib
Point2f MVertex::catmullClarkCreaseAttrib(MVertexAttrib *attrib)
{
	MEdge *sharpEdges[2];
	Point2f sharp;

	int count = 0;

	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->usesVertexAttrib( attrib )  &&  edges[i]->isCreased() )
		{
			sharpEdges[count++] = edges[i];
			if ( count >= 2 )
			{
				break;
			}
		}
	}

	sharp = Point2f::weightedAverage( attrib->getPoint(), 0.75,
								sharpEdges[0]->getAttribAtOppositeVertex( attrib )->getSavedPoint(), 0.125,
								sharpEdges[1]->getAttribAtOppositeVertex( attrib )->getSavedPoint(), 0.125 );

	return sharp;
}

//compute the new position of the point of attrib, using the boundary rule
Point2f MVertex::catmullClarkBoundaryAttrib(MVertexAttrib *attrib)
{
	int count = 0;

	Point2f av;

	//(equivalent to) average of the subdivision (odd) vertices of the incident edges
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isVertexAttribBoundary( attrib )  &&  edges[i]->isBoundary() )
		{
			av.cumulativeAdd( edges[i]->getAttribAtOppositeVertex( attrib )->getSavedPoint() );
			count++;
		}
	}

	av.scale( 1.0 / (double)count );

	//compute the mean of the average and the original position
	av = Point2f::weightedAverage( attrib->getPoint(), 0.75, av, 0.25 );

	return av;
}

//reposition the point of attrib accoriding to the catmull clark rules
void MVertex::catmullClarkAttrib(MVertexAttrib *attrib)
{
	int boundaryCount = 0;
	int meshBoundaryCount = 0;
	int sharpEdgeCount = 0;
	double edgeSharpness = 0.0;

	//compute the boundary and sharp edge counts, along with average
	//edge sharpness, for each incident edge which uses attrib
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->usesVertexAttrib( attrib ) )
		{
			if ( edges[i]->isVertexAttribBoundary() )
			{
				boundaryCount++;
				//also need to count the number of incident edges which are
				//ATTRIBUTE boundary edges *AND* MESH boundary edges
				if ( edges[i]->isBoundary() )
				{
					meshBoundaryCount++;
				}
			}

			if ( edges[i]->isCreased() )
			{
				sharpEdgeCount++;
				edgeSharpness += edges[i]->getSharpness();
			}
		}
	}


	if ( boundaryCount > 2  ||  ( boundaryCount == 2  &&  meshBoundaryCount != 2 ) )
	{
		//leave the attib point as it is
		return;
	}


	edgeSharpness *= 1.0 / (double)sharpEdgeCount;

	//clamp ceiling to 1.0, for weighted averaging
	edgeSharpness = clampUpper( edgeSharpness, 1.0 );




	if ( sharpness < 1.0 )
	{
		//determine which properties must be computed
		bool sharpIncidentEdgesFlag = sharpEdgeCount > 0;
		bool dartFlag = sharpEdgeCount == 1;
		bool creasedFlag = sharpEdgeCount == 2;
		bool cornerFlag = sharpEdgeCount >= 3;


		float edgeSharpness = 0.0;
		MEdge *sharpEdges[2];
		//creased: need to compute an average edge sharpness
		if ( creasedFlag  ||  cornerFlag )
		{
			edgeSharpness = catmullClarkComputeEdgeSharpnessForVertexPos( sharpEdges );
		}


		Point2f smooth, sharp, catm;


		bool needSmoothFlag = !sharpIncidentEdgesFlag  ||  dartFlag  ||  ( ( creasedFlag || cornerFlag )  &&  edgeSharpness < 1.0 );

		// ****** COMPUTE SMOOTH POINTS ******
		if ( needSmoothFlag  &&  boundaryCount < 2 )
		{
			smooth = catmullClarkSmoothAttrib( attrib );
		}
		else if ( boundaryCount == 2 )
		{
			smooth = catmullClarkBoundaryAttrib( attrib );
		}


		bool needSharpFlag = ( creasedFlag  &&  edgeSharpness > 0.0 )  ||  cornerFlag;

		// ****** COMPUTE SHARP POINTS ******
		if ( needSharpFlag  &&  boundaryCount < 2 )
		{
			if ( creasedFlag )
			{
				//use crease rules for sharp point
				sharp = catmullClarkCreaseAttrib( attrib );
			}
			else if ( cornerFlag )
			{
				//use corner rules for sharp point
				sharp = attrib->getPoint();
			}
		}
		else if ( needSharpFlag  &&  boundaryCount == 2 )
		{
			bool found_nonBoundary_sharpEdge_flag = false;

			for (int i = 0; i < edges.size(); i++)
			{
				bool edgeIsBoundary = edges[i]->isVertexAttribBoundary( attrib )  &&  edges[i]->isBoundary();

				if ( edges[i]->isCreased()  &&  !edgeIsBoundary )
				{
					found_nonBoundary_sharpEdge_flag = true;
					break;
				}
			}

			if ( found_nonBoundary_sharpEdge_flag )
			{
				//effectively 3 or more incident sharp edges:
				//2 boundary edges and some more sharp edges:
				//sharp position is current position
				sharp = attrib->getPoint();
			}
			else
			{
				sharp = smooth;
			}
		}


		// ****** COMPUTE CATMULL-CLARK POINTS ******
		if ( creasedFlag  ||  cornerFlag )
		{
			//creased or corner: weighted average of sharp and smooth
			catm = lerp( smooth, sharp, edgeSharpness );
		}
		else
		{
			//DART  or  NO SHARP EDGES  :  smooth
			catm = smooth;
		}


		// ****** REPOSITION ******
		if ( sharpness == 0.0 )
		{
			//vertex is smooth
			attrib->setPoint( catm );
		}
		else if ( sharpness < 1.0 )
		{
			//vertex is sharp: weighted average between catmull clark position
			//and current position
			attrib->setPoint( lerp( catm, attrib->getPoint(), sharpness ) );
		}
	}
}




/*
NOTE:
Current implementation:
If there are > 0 incident edges which are boundary edges, then the new position
is computed using only the current position and the subdivision vertices of these
edges. Should this change, then MMesh::subdivideMarked() will CRASH.
*/
void MVertex::computeNewPositionCatmullClark(MPreserveNormalSharpness preserveNormalSharpness)
{
	bool attribContinuousFlag = true;

	//determine whether the vertex attributes are continuous around this vertex
	//this is so if there is only one vertex attribute for this vertex
	//and there are no incident vertex attribute boundary edges
	if ( vertexAttribs.size() <= 1 )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isVertexAttribBoundary() )
			{
				attribContinuousFlag = false;
				break;
			}
		}
	}
	else
	{
		attribContinuousFlag = false;
	}



	if ( sharpness < 1.0 )
	{
		//boundary count is a multiple of 2 by definition
		int boundaryCount = computeBoundaryCount();

		//determine which properties must be computed
		int sharpEdgeCount = computeCreasedEdgeCount();
		bool sharpIncidentEdgesFlag = sharpEdgeCount > 0;
		bool dartFlag = sharpEdgeCount == 1;
		bool creasedFlag = sharpEdgeCount == 2;
		bool cornerFlag = sharpEdgeCount >= 3;


		float edgeSharpness = 0.0;
		MEdge *sharpEdges[2];
		//creased: need to compute an average edge sharpness
		if ( creasedFlag  ||  cornerFlag )
		{
			edgeSharpness = catmullClarkComputeEdgeSharpnessForVertexPos( sharpEdges );
		}


		Point3 smooth, sharp, catm;
		Point2f attribSmooth, attribSharp, attribCatm;

		bool needSmoothFlag = !sharpIncidentEdgesFlag  ||  dartFlag  ||  ( ( creasedFlag || cornerFlag )  &&  edgeSharpness < 1.0 );

		// ****** COMPUTE SMOOTH POINTS ******
		if ( needSmoothFlag  &&  boundaryCount == 0 )
		{
			//use smooth rule
			double n = (double)faces.size();

			//average of positions of face vertices
			for (int i = 0; i < faces.size(); i++)
			{
				smooth.cumulativeAdd( faces[i]->getSubdivisionVertex()->position );
				//don't bother with   if ( attribContinuousFlag )
				attribSmooth.cumulativeAdd( faces[i]->getSubdivisionVertex()->attrib0.getPoint() );
			}

			//average of opposite edge endpoints
			for (int i = 0; i < edges.size(); i++)
			{
				smooth.cumulativeAdd( edges[i]->getOppositeVertex( this )->positionSave );
				if ( attribContinuousFlag )
				{
					attribSmooth.cumulativeAdd( edges[i]->getAttribAtOppositeVertex( &attrib0 )->getSavedPoint() );
				}
			}

			//combine the edge and face average positions, compute above
			double oneOverN = 1.0 / n;
			double oneOverNSquared = oneOverN * oneOverN;
			double nMinus2TimesOneOverN = ( n - 2 ) * oneOverN;
			smooth.scale( oneOverNSquared );
			smooth.cumulativeAdd( Point3::mul( position, nMinus2TimesOneOverN ) );

			attribSmooth.scale( oneOverNSquared );
			attribSmooth.cumulativeAdd( Point2f::mul( attrib0.getPoint(), nMinus2TimesOneOverN ) );
		}
		else if ( boundaryCount == 2 )
		{
			//average of the subdivision (odd) vertices of the incident edges
			int count = 0;
			for (int i = 0; i < edges.size(); i++)
			{
				if ( edges[i]->isBoundary() )
				{
					smooth.cumulativeAdd( edges[i]->getSubdivisionVertex()->position );
					attribSmooth.cumulativeAdd( edges[i]->getSubdivisionVertex()->attrib0.getPoint() );
					count++;
				}
			}

			double oneOverCount = 1.0 / (double)count;
			smooth.scale( oneOverCount );
			attribSmooth.scale( oneOverCount );

			//compute the mean of the average and the original position
			smooth = Point3::average( position, smooth );
			attribSmooth = Point2f::average( attrib0.getPoint(), attribSmooth );
		}


		bool needSharpFlag = ( creasedFlag  &&  edgeSharpness > 0.0 )  ||  cornerFlag;

		// ****** COMPUTE SHARP POINTS ******
		if ( needSharpFlag  &&  boundaryCount == 0 )
		{
			if ( creasedFlag )
			{
				//sharp position uses crease rules
				sharp = Point3::weightedAverage( position, 0.75,
											sharpEdges[0]->getOppositeVertex( this )->positionSave, 0.125,
											sharpEdges[1]->getOppositeVertex( this )->positionSave, 0.125 );

				if ( attribContinuousFlag )
				{
					attribSharp = Point2f::weightedAverage( attrib0.getPoint(), 0.75,
													sharpEdges[0]->getAttribAtOppositeVertex( &attrib0 )->getSavedPoint(), 0.125,
													sharpEdges[1]->getAttribAtOppositeVertex( &attrib0 )->getSavedPoint(), 0.125 );
				}
			}
			else if ( cornerFlag )
			{
				//sharp position uses corner rules
				sharp = position;
				attribSharp = attrib0.getPoint();
			}
		}
		else if ( needSharpFlag  &&  boundaryCount == 2 )
		{
			bool found_nonBoundary_sharpEdge_flag = false;

			for (int i = 0; i < edges.size(); i++)
			{
				if ( edges[i]->isCreased()  &&  !edges[i]->isBoundary() )
				{
					found_nonBoundary_sharpEdge_flag = true;
					break;
				}
			}

			if ( found_nonBoundary_sharpEdge_flag )
			{
				//effectively 3 or more incident sharp edges:
				//2 boundary edges and some more sharp edges:
				//sharp position is current position
				sharp = position;
				attribSharp = attrib0.getPoint();
			}
			else
			{
				sharp = smooth;
				attribSharp = attribSmooth;
			}
		}


		// ****** COMPUTE CATMULL-CLARK POINTS ******
		if ( boundaryCount <= 2 )
		{
			if ( creasedFlag  ||  cornerFlag )
			{
				//creased or corner: weighted average of sharp and smooth
				catm = lerp( smooth, sharp, edgeSharpness );
				attribCatm = lerp( attribSmooth, attribSharp, edgeSharpness );
			}
			else
			{
				//DART  or  NO SHARP EDGES  :  smooth
				catm = smooth;
				attribCatm = attribSmooth;
			}
		}


		// ****** REPOSITION ******
		if ( boundaryCount <= 2 )
		{
			if ( sharpness == 0.0 )
			{
				//vertex is smooth
				position = catm;
				if ( attribContinuousFlag )
				{
					attrib0.setPoint( attribCatm );
				}
			}
			else if ( sharpness < 1.0 )
			{
				//vertex is sharp: weighted average between catmull clark position
				//and current position
				position = lerp( catm, position, sharpness );
				if ( attribContinuousFlag )
				{
					attrib0.setPoint( lerp( attribCatm, attrib0.getPoint(), sharpness ) );
				}
			}
		}
	}


	if ( !attribContinuousFlag )
	{
		//vertex attributes not continuous
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			catmullClarkAttrib( vertexAttribs[i] );
		}
	}

	//need this later (see MFace::createSubdivisionFaces())
	vertexOpData.subdivision.averageEdgeSharpness = computeAverageEdgeSharpnessForSubdivision();



	//preserve normal sharpness
	switch( preserveNormalSharpness )
	{
	case MPRESERVENORMALSHARPNESS_SMOOTH:
		setNormalSmooth();
		break;
	case MPRESERVENORMALSHARPNESS_IFSHARP:
		if ( getSharpness() < 1.0 )
		{
			setNormalSmooth();
		}
		break;
	case MPRESERVENORMALSHARPNESS_PRESERVE:
		break;
	default:
		break;
	}
}

void MVertex::liveSubdivideReposition(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness)
{
	if ( sharpness < 1.0 )
	{
		//boundary count is a multiple of 2 by definition
		int boundaryCount = computeBoundaryCount();

		//determine which properties must be computed
		int sharpEdgeCount = computeCreasedEdgeCount();
		bool sharpIncidentEdgesFlag = sharpEdgeCount > 0;
		bool dartFlag = sharpEdgeCount == 1;
		bool creasedFlag = sharpEdgeCount == 2;
		bool cornerFlag = sharpEdgeCount >= 3;


		float edgeSharpness = 0.0;
		MEdge *sharpEdges[2];
		//creased: need to compute an average edge sharpness
		if ( creasedFlag  ||  cornerFlag )
		{
			edgeSharpness = catmullClarkComputeEdgeSharpnessForVertexPos( sharpEdges );
		}


		Point3 smooth, sharp, catm;

		bool needSmoothFlag = !sharpIncidentEdgesFlag  ||  dartFlag  ||  ( ( creasedFlag || cornerFlag )  &&  edgeSharpness < 1.0 );

		// ****** COMPUTE SMOOTH POINTS ******
		if ( needSmoothFlag  &&  boundaryCount == 0 )
		{
			//use smooth rule
			double n = (double)faces.size();

			//average of positions of face vertices
			for (int i = 0; i < faces.size(); i++)
			{
				smooth.cumulativeAdd( liveMesh->vertices[ faces[i]->index + faceSubdVerticesOffset ]->getPosition() );
			}

			//average of opposite edge endpoints
			for (int i = 0; i < edges.size(); i++)
			{
				smooth.cumulativeAdd( edges[i]->getOppositeVertex( this )->position );
			}

			//combine the edge and face average positions, compute above
			double oneOverN = 1.0 / n;
			double oneOverNSquared = oneOverN * oneOverN;
			double nMinus2TimesOneOverN = ( n - 2 ) * oneOverN;
			smooth.scale( oneOverNSquared );
			smooth.cumulativeAdd( Point3::mul( position, nMinus2TimesOneOverN ) );
		}
		else if ( boundaryCount == 2 )
		{
			//average of the subdivision (odd) vertices of the incident edges
			int count = 0;
			for (int i = 0; i < edges.size(); i++)
			{
				if ( edges[i]->isBoundary() )
				{
					smooth.cumulativeAdd( liveMesh->vertices[ edges[i]->index + edgeSubdVerticesOffset]->getPosition() );
					count++;
				}
			}

			double oneOverCount = 1.0 / (double)count;
			smooth.scale( oneOverCount );

			//compute the mean of the average and the original position
			smooth = Point3::average( position, smooth );
		}


		bool needSharpFlag = ( creasedFlag  &&  edgeSharpness > 0.0 )  ||  cornerFlag;

		// ****** COMPUTE SHARP POINTS ******
		if ( needSharpFlag  &&  boundaryCount == 0 )
		{

			if ( creasedFlag )
			{
				//sharp position uses crease/sharp rule
				sharp = Point3::weightedAverage( position, 0.75,
											sharpEdges[0]->getOppositeVertex( this )->position, 0.125,
											sharpEdges[1]->getOppositeVertex( this )->position, 0.125 );
			}
			else if ( cornerFlag )
			{
				//sharp position uses corner rules
				sharp = position;
			}
		}
		else if ( needSharpFlag  &&  boundaryCount == 2 )
		{
			bool found_nonBoundary_sharpEdge_flag = false;

			for (int i = 0; i < edges.size(); i++)
			{
				if ( edges[i]->isCreased()  &&  !edges[i]->isBoundary() )
				{
					found_nonBoundary_sharpEdge_flag = true;
					break;
				}
			}

			if ( found_nonBoundary_sharpEdge_flag )
			{
				//effectively 3 or more incident sharp edges:
				//2 boundary edges and some more sharp edges:
				//sharp position is current position
				sharp = position;
			}
			else
			{
				sharp = smooth;
			}
		}


		// ****** COMPUTE CATMULL-CLARK POINTS ******
		if ( boundaryCount <= 2 )
		{
			if ( creasedFlag  ||  cornerFlag )
			{
				//creased or corner: weighted average of sharp and smooth
				catm = lerp( smooth, sharp, edgeSharpness );
			}
			else
			{
				//DART  or  NO SHARP EDGES  :  smooth
				catm = smooth;
			}
		}


		// ****** REPOSITION ******
		if ( boundaryCount <= 2 )
		{
			if ( sharpness == 0.0 )
			{
				//vertex is smooth
				liveMesh->vertices[index]->setPosition( catm );
			}
			else if ( sharpness < 1.0 )
			{
				//vertex is sharp: weighted average between catmull clark position
				//and current position
				liveMesh->vertices[index]->setPosition( lerp( catm, position, sharpness ) );
			}
		}
		else
		{
			liveMesh->vertices[index]->setPosition( position );
		}
	}
	else
	{
		liveMesh->vertices[index]->setPosition( position );
	}


	//need this later (see MFace::createSubdivisionFaces())
	vertexOpData.subdivision.averageEdgeSharpness = computeAverageEdgeSharpnessForSubdivision();



	//preserve normal sharpness
	switch( preserveNormalSharpness )
	{
	case MPRESERVENORMALSHARPNESS_SMOOTH:
		liveMesh->vertices[index]->setNormalSmooth();
		break;
	case MPRESERVENORMALSHARPNESS_IFSHARP:
		if ( getSharpness() < 1.0 )
		{
			liveMesh->vertices[index]->setNormalSmooth();
		}
		break;
	default:
		break;
	}
}

void MVertex::refreshLiveSubdivision(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness)
{
	if ( flags.liveSubdVertexRequiresRefresh )
	{
		liveSubdivideReposition( liveMesh, edgeSubdVerticesOffset, faceSubdVerticesOffset, preserveNormalSharpness );

		float s = getSharpness();
		s = s >= 1.0  ?  s - 1.0  :  0.0;
		liveMesh->vertices[index]->setSharpness( s );
	}

	if ( flags.liveSubdEdgeSharpnessRequiresRefresh )
	{
		float averageEdgeSharpness = vertexOpData.subdivision.averageEdgeSharpness;

		MVertex *liveVertex = liveMesh->vertices[index];

		for (int i = 0; i < edges.size(); i++)
		{
			MEdge *e = edges[i];
			MVertex *edgeVertex = liveMesh->vertices[ edges[i]->index + edgeSubdVerticesOffset ];

			MEdge *liveEdge = liveVertex->findEdgeTo( edgeVertex, MFINDPOLICY_RETURNNULL );

			if ( liveEdge != NULL )
			{
				if ( e->isCreased() )
				{
					liveEdge->crease();

					if ( averageEdgeSharpness == -1.0 )
					{
						float s = edges[i]->getSharpness() - 1.0;
						liveEdge->setSharpness( clampLower( s, 0.0f ) );
					}
					else
					{
						float edgeSharpness = edges[i]->getSharpness();
						float s = ( averageEdgeSharpness + edgeSharpness ) * 0.5  -  1.0;
						liveEdge->setSharpness( clampLower( s, 0.0f ) );
					}

					//may result in normal sharpness changing, if
					//preserveNormalSharpness is IFSHARP
					switch ( preserveNormalSharpness )
					{
					case MPRESERVENORMALSHARPNESS_PRESERVE:
						liveEdge->setNormalSharpness( e->isNormalSharp() );
						break;
					case MPRESERVENORMALSHARPNESS_IFSHARP:
						if ( preserveNormalSharpness == MPRESERVENORMALSHARPNESS_IFSHARP )
						{
							//use original sharpness
							if ( edges[i]->getSharpness() < 1.0 )
							{
								liveEdge->setNormalSmooth();
							}
						}
						break;
					case MPRESERVENORMALSHARPNESS_SMOOTH:
					default:
						break;
					}
				}
				else
				{
					//e is not sharp; reflect this in the live edge
					liveEdge->uncrease();
				}
			}
		}
	}

	if ( flags.liveSubdNormalSharpnessModified )
	{
		liveMesh->vertices[index]->touchNormalSharpness();
	}

	liveSubdivisionRefreshed();
}

void MVertex::liveSubdivisionRefreshInit()
{
	if ( flags.liveSubdRequiresRefresh )
	{
		flags.liveSubdVertexRequiresRefresh = true;

		for (int i = 0; i < faces.size(); i++)
		{
			faces[i]->liveSubdivisionIncidentVertexModified();
		}

		//no need to inform vertices too;
		//MFace::liveSubdivisionIncidentVertexModified() does that
	}
}



// ******************* EXTRUDE FACE OPS *******************

void MVertex::extrudeFaceCreateExtrudedVertices(MVertexList &verts, int numSegments)
{
	verts.reserve( numSegments + 1 );
	verts.push_back( this );

	for (int i = 0; i < numSegments; i++)
	{
		verts.push_back( mesh->addVertex( position ) );
	}
}

void MVertex::pathExtrudeFaceCreateExtrudedVertices(MVertexList &verts, MPathExtrudeFaceMode mode, const Array<Matrix4> &matrices)
{
	verts.reserve( matrices.size() );
	verts.push_back( this );

	for (int i = 1; i < matrices.size(); i++)
	{
		verts.push_back( mesh->addVertex( position * matrices[i] ) );
	}
}

void MVertex::extrudeFaceExtrudeVertexAdjust(MVertexList &verts, MVertexAdjustList &vertexAdjusts, const Vector3 extrusionVector, int numSegments)
{
	double uInc = 1.0 / (double)numSegments, u = uInc;

	for (int i = 0; i < numSegments; i++)
	{
		vertexAdjusts.push_back( MVertexAdjust( verts[i+1], position, extrusionVector * u, Vector3() ) );
		u += uInc;
	}
}

void MVertex::extrudeFaceExtrudeVertexAdjust(MVertexList &verts, MVertexVectorAdjustList &vertexAdjusts, int numSegments)
{
	double uInc = 1.0 / (double)numSegments, u = uInc;

	for (int i = 0; i < numSegments; i++)
	{
		vertexAdjusts.push_back( MVertexVectorAdjust( verts[i+1], position, u ) );
		u += uInc;
	}
}

void MVertex::fillInExtrudeFaceMultipleSectionsTable(VertexNeighbourhood &neighbourhood, MVertexList *extruded)
{
	//fill in the tables
	for (int j = 0; j < neighbourhood.edges.size(); j++)
	{
		MEdge *e = neighbourhood.edges[j];
		vertexOpData.faceExtrude.data.multiple->setVertexListForEdge( e, extruded );
	}
	for (int j = 0; j < neighbourhood.faces.size(); j++)
	{
		MFace *f = neighbourhood.faces[j];
		vertexOpData.faceExtrude.data.multiple->setVertexListForFace( f, extruded );
	}
}



void MVertex::extrudeFaceExtrudeVertex(MVertexAdjustList &vertexAdjusts, MExtrudeFaceDirection direction, int numSegments)
{
	bool internalFlag = isFaceMarkInternal();


	Array<VertexNeighbourhood> neighbourhoods;

	if ( !internalFlag )
	{
// 		//only discover the marked faces if *this is not face mark internal
		discoverAllFaceMarkedNeighbourhoods( neighbourhoods );
	}


	if ( neighbourhoods.size() > 1 )
	{
		//multiple sections
		vertexOpData.faceExtrude.multipleSectionsFlag = true;
		//create a new table
		vertexOpData.faceExtrude.data.multiple = new FaceExtrudedVertexTable();



		//for each neighbourhood
		for (int i = 0; i < neighbourhoods.size(); i++)
		{
			VertexNeighbourhood &neighbourhood = neighbourhoods[i];

			//CREATE THE VERTEX LIST FOR THIS SECTION
			MVertexList *extrudedVertices = &( vertexOpData.faceExtrude.data.multiple->vertices.push_back() );
			extrudeFaceCreateExtrudedVertices( *extrudedVertices, numSegments );



			//COMPUTE OR GET EXTRUSION VECTOR
			Vector3 extrusionVector;
			//if the direction is computed per vertex (expand)
			if ( direction == MEXTRUDEFACEDIRECTION_EXPAND )
			{
				//get a list of the normals of the faces that are part of this section
				Array<Vector3> normals;
				normals.reserve( neighbourhood.faces.size() );
				for (int j = 0; j < neighbourhood.faces.size(); j++)
				{
					normals.push_back( neighbourhood.faces[j]->getPlane().n );
				}

				//compute the extrusion vector
				extrusionVector = MMesh::computeExpansionVector( normals );
			}
			else if ( direction == MEXTRUDEFACEDIRECTION_PERGROUP )
			{
				//else get the group direction
				Vector3 *groupDirection = neighbourhood.faces.front()->extrudeFaceGetGroupDirection();
				extrusionVector = *groupDirection;
			}


			//FILL IN THE MULTIPLE SECTIONS TABLES
			fillInExtrudeFaceMultipleSectionsTable( neighbourhood, extrudedVertices );

			//CREATE VERTEX ADJUSTMENTS
			extrudeFaceExtrudeVertexAdjust( *extrudedVertices, vertexAdjusts, extrusionVector, numSegments );
		}
	}
	else if ( neighbourhoods.size() == 1  ||  internalFlag )
	{
		//ONLY ONE SECTION
		vertexOpData.faceExtrude.multipleSectionsFlag = false;


		//COMPUTE OR GET THE EXTRUSION VECTOR
		Vector3 extrusionVector;
		if ( direction == MEXTRUDEFACEDIRECTION_EXPAND )
		{
			Array<Vector3> normals;
			for (int i = 0; i < faces.size(); i++)
			{
				if ( faces[i]->isFaceMarked() )
				{
					normals.push_back( faces[i]->getPlane().n );
				}
			}

			//compute the extrusion vector
			extrusionVector = MMesh::computeExpansionVector( normals );
		}
		else if ( direction == MEXTRUDEFACEDIRECTION_PERGROUP )
		{
			//search for a indicent marked face
			for (int i = 0; i < faces.size(); i++)
			{
				if ( faces[i]->isFaceMarked() )
				{
					//get the per group extrude direction from this face
					MFace *f = faces[i];
					Vector3 *groupDirection = f->extrudeFaceGetGroupDirection();
					extrusionVector = *groupDirection;
				}
			}
		}


		//PERFORM THE VERTEX EXTRUSION
		if ( internalFlag )
		{
			//internal vertex
			vertexOpData.faceExtrude.data.single = NULL;
			//adjust *this
			vertexAdjusts.push_back( MVertexAdjust( this, position, extrusionVector, Vector3() ) );
		}
		else
		{
			//create new vertices
			MVertexList *extrudedVertices = new MVertexList();
			vertexOpData.faceExtrude.data.single = extrudedVertices;

			//create extruded vertices
			extrudeFaceCreateExtrudedVertices( *extrudedVertices, numSegments );
			//create vertex adjustments
			extrudeFaceExtrudeVertexAdjust( *extrudedVertices, vertexAdjusts, extrusionVector, numSegments );
		}
	}
	else
	{
		gs_assert_not_reached( "MVertex::extrudeFaceExtrudeVertex(): neighbourhoods.size() == 0\n" );
	}
}

void MVertex::extrudeFaceVectorExtrudeVertex(MVertexVectorAdjustList &vertexAdjusts, int numSegments)
{
	bool internalFlag = isFaceMarkInternal();


	Array<VertexNeighbourhood> neighbourhoods;

	if ( !internalFlag )
	{
		//only discover the marked faces if *this is not face mark internal
		discoverAllFaceMarkedNeighbourhoods( neighbourhoods );
	}


	if ( neighbourhoods.size() > 1 )
	{
		//multiple sections
		vertexOpData.faceExtrude.multipleSectionsFlag = true;
		//create a new table
		vertexOpData.faceExtrude.data.multiple = new FaceExtrudedVertexTable();



		//for each neighbourhood
		for (int i = 0; i < neighbourhoods.size(); i++)
		{
			VertexNeighbourhood &neighbourhood = neighbourhoods[i];

			//create a vertex list for this section
			MVertexList *extrudedVertices = &( vertexOpData.faceExtrude.data.multiple->vertices.push_back() );
			extrudeFaceCreateExtrudedVertices( *extrudedVertices, numSegments );

			//fill in the multiple section tables
			fillInExtrudeFaceMultipleSectionsTable( neighbourhood, extrudedVertices );

			//create vertex adjustments
			extrudeFaceExtrudeVertexAdjust( *extrudedVertices, vertexAdjusts, numSegments );
		}
	}
	else if ( neighbourhoods.size() == 1  ||  internalFlag )
	{
		//only one section
		vertexOpData.faceExtrude.multipleSectionsFlag = false;


		if ( internalFlag )
		{
			//internal vertex
			vertexOpData.faceExtrude.data.single = NULL;
			//adjust *this
			vertexAdjusts.push_back( MVertexVectorAdjust( this, position, 1.0 ) );
		}
		else
		{
			//create new vertices
			MVertexList *extrudedVertices = new MVertexList();
			vertexOpData.faceExtrude.data.single = extrudedVertices;

			//create extruded vertices
			extrudeFaceCreateExtrudedVertices( *extrudedVertices, numSegments );
			//create vertex adjustments
			extrudeFaceExtrudeVertexAdjust( *extrudedVertices, vertexAdjusts, numSegments );
		}
	}
	else
	{
		gs_assert_not_reached( "MVertex::extrudeFaceVectorExtrudeVertex(): neighbourhoods.size() == 0\n" );
	}
}

void MVertex::pathExtrudeFaceExtrudeVertices(MPathExtrudeFaceMode mode)
{
	bool internalFlag = isFaceMarkInternal();


	Array<VertexNeighbourhood> neighbourhoods;

	if ( !internalFlag )
	{
		//only discover the marked faces if *this is not face mark internal
		discoverAllFaceMarkedNeighbourhoods( neighbourhoods );
	}


	if ( neighbourhoods.size() > 1 )
	{
		//multiple sections
		vertexOpData.faceExtrude.multipleSectionsFlag = true;
		//create a new table
		vertexOpData.faceExtrude.data.multiple = new FaceExtrudedVertexTable();



		//for each neighbourhood
		for (int i = 0; i < neighbourhoods.size(); i++)
		{
			VertexNeighbourhood &neighbourhood = neighbourhoods[i];

			//create a vertex list for this section
			MVertexList *extrudedVertices = &( vertexOpData.faceExtrude.data.multiple->vertices.push_back() );
			MFace *f = neighbourhood.faces.front();
			const Array<Matrix4> *matrices = f->getPathExtrudeMatrices();
			pathExtrudeFaceCreateExtrudedVertices( *extrudedVertices, mode, *matrices );

			//fill in the tables
			fillInExtrudeFaceMultipleSectionsTable( neighbourhood, extrudedVertices );
		}
	}
	else if ( neighbourhoods.size() == 1  ||  internalFlag )
	{
		//only one section
		vertexOpData.faceExtrude.multipleSectionsFlag = false;


		if ( internalFlag )
		{
			//internal vertex
			MFace *f = faces.front();			//internal vertex: just use any face
			const Array<Matrix4> *matrices = f->getPathExtrudeMatrices();

			vertexOpData.faceExtrude.data.single = NULL;
			setPosition( position * matrices->back() );
		}
		else
		{
			MFace *f = neighbourhoods.front().faces.front();
			const Array<Matrix4> *matrices = f->getPathExtrudeMatrices();

			//vertex is on boundary
			//create new vertices
			MVertexList *extrudedVertices = new MVertexList();
			vertexOpData.faceExtrude.data.single = extrudedVertices;

			pathExtrudeFaceCreateExtrudedVertices( *extrudedVertices, mode, *matrices );
		}
	}
	else
	{
		gs_assert_not_reached( "MVertex::pathExtrudeFaceExtrudeVertices(): neighbourhoods.size() == 0\n" );
	}
}




// ******************* DUPLICATE FACES OPS *******************
void MVertex::duplicateFacesCreateDuplicateVertex(MVertexVectorAdjustList &vertexAdjusts)
{
	MVertex *dup = getMesh()->addVertex( getPosition() );
	vertexOpData.faceDuplicate.duplicateVertex = dup;
	vertexAdjusts.push_back( MVertexVectorAdjust( dup, getPosition(), 1.0 ) );
}



// ******************* INSET / EXPAND FACE OPS *******************

void MVertex::insetExpandFacesInsertCornerVertices(MEdgeList &edgesToCheck, MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
											MInsetFaceStyle insetStyle, bool flowThruMarkedEdges, MInsetExpandOperation insetExpandOp)
{
	//NOTE: ONLY CALLED FOR VERTEX WHICH LIES ON A FACE MARK BOUNDARY

	//check all the face marked incident faces
	for (int i = 0; i < faces.size(); i++)
	{
		MFace *f = faces[i];

		if ( f->isInsetExpandTarget( insetExpandOp ) )
		{
			MEdge *prevEdge, *nextEdge;

			//get the edges in f which are adjacent to this, put in prevEdge
			//and nextEdge
			f->getAdjacentEdges( this, prevEdge, nextEdge );

			if ( prevEdge != NULL  &&  nextEdge != NULL )
			{
				if ( prevEdge->isFaceMarkBoundary()  &&  !prevEdge->isInsetFaceFlowEdge( flowThruMarkedEdges)  &&
					nextEdge->isFaceMarkBoundary()  &&  !nextEdge->isInsetFaceFlowEdge( flowThruMarkedEdges ) )
				{
					//both are face mark boundary edges
					int vertexInFaceIndex = f->findVertex( this );

					/*
					TRANSFORMATION:
					this	 nextEdge	  this
					o----------				o ----------
					|							  \
					|							|\ \
					|							| \ \
					|							|  \ \
					| prevEdge				|    x  centreVertex
					|							|
					*/

					MVertexAttrib *vertexAttrib = f->getVertexAttrib( vertexInFaceIndex );



					//compute the average vertex position and create centreVertex
					MVertex *centreVertex = getMesh()->addVertex( position );
					MVertexAttrib *centreAttrib = centreVertex->createVertexAttrib( *vertexAttrib, MFINDPOLICY_FORCEDCREATE );


					Vector3 vertexVector;
					Vector2f attribVector;
					double uLower, uUpper;

					if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
					{
						f->insetExpandFaceComputeToCentreInsetVectors( vertexInFaceIndex, vertexVector, attribVector, uLower, uUpper, true );
					}
					else if ( insetStyle == MINSETFACESTYLE_FROMEDGES )
					{
						f->insetExpandFaceComputeFromEdgeInsetVectors( vertexInFaceIndex, vertexVector, attribVector, true, uLower, uUpper , true);
					}

					if ( insetExpandOp == MINSETEXPANDOP_INSET )
					{
						vertexAdjusts.push_back( MVertexAdjust( centreVertex, position, vertexVector, uLower, uUpper,
															f->getInsetExpandFaceStoredPlane().n, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() ) );
					}
					else
					{
						vertexAdjusts.push_back( MVertexAdjust( centreVertex, position, vertexVector, uLower, uUpper ) );
					}


					//the insert actions can destroy one of the marked edges,
					//which is then recreated
					//prevent this by making them indestructible (temporarily)
					prevEdge->setIndestructible();
					nextEdge->setIndestructible();
					f->insertVertex( vertexInFaceIndex, centreVertex, centreAttrib );
					f->insertVertex( vertexInFaceIndex, this, vertexAttrib );
					prevEdge->setDestructible();
					nextEdge->setDestructible();


					vertexAttribAdjusts.push_back( MVertexAttribAdjust( centreAttrib, vertexAttrib->getPoint(), attribVector, uLower, uUpper ) );


					MEdge *newEdge = findEdgeTo( centreVertex, MFINDPOLICY_RETURNNULL );
					gs_assert( newEdge != NULL, "MVertex::insetMarkedFaces(): could not find edge from this to centreVertex\n" );
					edgesToCheck.push_back( newEdge );
				}
			}
		}
	}
}

void MVertex::insetExpandFacesSplitIncidentInternalFaces(MInsetExpandOperation insetExpandOp, bool flowThruMarkedEdges, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	MFaceList facesToSplit;

	//the criteria for determining whether or not a face should be
	//split away is as follows:
	//-face is marked
	//-edges of face adjacent to *this are face internal marked

	//the splitting action may change the situation w.r.t. the above
	//criteria, so first, build a list of faces to split, then split them

	if ( flowThruMarkedEdges )
	{
		gs_assert( insetExpandOp == MINSETEXPANDOP_INSET, "MVertex::insetExpandFacesSplitIncidentInternalFaces(): 'flow through marked edges' is enabled, and the operation"
															"is not inset\n" );

		// Discover all face marked neighbourhoods
		Array<VertexNeighbourhood> neighbourhoods;
		discoverAllFaceMarkedNeighbourhoods( neighbourhoods );


		for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
		{
			VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

			MEdge *a = neighbourhood.edges.front(), *b = neighbourhood.edges.back();
			bool edgeAFlow = a->isInsetFaceFlowEdge( flowThruMarkedEdges );
			bool edgeBFlow = b->isInsetFaceFlowEdge( flowThruMarkedEdges );

			if ( !( edgeAFlow  &&  edgeBFlow ) )
			{
				for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
				{
					MFace *f = neighbourhood.faces[faceI];

					if ( f->isInsetExpandTarget( insetExpandOp ) )
					{
						//get the edges in f which are adjacent to this, put in prevEdge
						//and nextEdge
						MEdge *prevEdge = neighbourhood.edges[faceI];
						MEdge *nextEdge = neighbourhood.edges[faceI+1];

						if ( prevEdge != NULL  &&  nextEdge != NULL )
						{
							bool prevEdgeFlow = prevEdge == a  &&  edgeAFlow;
							bool nextEdgeFlow = nextEdge == b  &&  edgeBFlow;

							//if both incident edges are face mark internal
							if ( ( prevEdge->isInsetExpandFaceInternal( insetExpandOp )  ||  prevEdgeFlow )  &&
								( nextEdge->isInsetExpandFaceInternal( insetExpandOp )  ||  nextEdgeFlow ) )
							{
								facesToSplit.push_back( f );
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < faces.size(); i++)
		{
			MFace *f = faces[i];

			if ( f->isInsetExpandTarget( insetExpandOp ) )
			{
				MEdge *prevEdge, *nextEdge;

				//get the edges in f which are adjacent to this, put in prevEdge
				//and nextEdge
				f->getAdjacentEdges( this, prevEdge, nextEdge );

				if ( prevEdge != NULL  &&  nextEdge != NULL )
				{
					//if both incident edges are face mark internal
					if ( prevEdge->isInsetExpandFaceInternal( insetExpandOp )  &&  nextEdge->isInsetExpandFaceInternal( insetExpandOp ) )
					{
						facesToSplit.push_back( f );
					}
				}
			}
		}
	}

	//split the faces in the list
	for (int i = 0; i < facesToSplit.size(); i++)
	{
		MFace *f = facesToSplit[i];

		//split off the triangle adjacent to
		MEdge *newEdge;
		MFace *newFace = f->splitOffTriangleIncidentToVertex( this, newEdge );
		newFace->insetExpandFaceCopyStoredPlaneFrom( f );

		gs_assert( newFace != NULL, "MVertex::insetMarkedFacesSplitCoincidentInternalFaces(): face split failed\n" );

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
		else if ( f->isIncidentTo( this ) )
		{
			f->faceUnmark();
		}
		else
		{
			gs_assert_not_reached( "MVertex::insetMarkedFacesSplitCoincidentInternalFaces(): could not find this (MVertex) in either result face\n");
			return;
		}
	}
}

void MVertex::insetExpandFacesMarkedInternalVertexExtrude(MVertexAdjustList &vertexAdjusts)
{
	Array<Vector3> normals;
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFaceMarked() )
		{
			normals.push_back( faces[i]->getInsetExpandFaceStoredPlane().n );
		}
	}

	Vector3 extrusionVector = MMesh::computeExpansionVector( normals );

	vertexAdjusts.push_back( MVertexAdjust( this, position, Vector3(), extrusionVector ) );
}




// ******************* INSET FACE OPS *******************

void MVertex::insetFacesSplitInternalEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle, bool flowThruMarkedEdges,
									bool markInsetEdges, bool nsharpInsetEdges)
{
	//NOTE: ONLY CALLED FOR VERTEX WHICH LIES ON A FACE MARK BOUNDARY

	// Discover all face marked neighbourhoods
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllFaceMarkedNeighbourhoods( neighbourhoods );


	// If a face marked neighbourhood has two 'flow through' edges, then this vertex needs to have extrusion applied to it
	bool extrudeFlag = false;

	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

		//only bother processing if there are internal edges to work with,
		//ie edge count > 2, or 'flow through marked edges' is enabled
		if ( neighbourhood.edges.size() > 2  ||  flowThruMarkedEdges )
		{
			// Compute the extrusion vector
			Array<Vector3> normals;
			for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
			{
				normals.push_back( neighbourhood.faces[faceI]->getInsetExpandFaceStoredPlane().n );
			}

			Vector3 extrusionVector = MMesh::computeExpansionVector( normals );

			//get the boundary edges
			MEdge *a = neighbourhood.edges.front();
			MEdge *b = neighbourhood.edges.back();
			bool edgeAFlowFlag = a->isInsetFaceFlowEdge( flowThruMarkedEdges );
			bool edgeBFlowFlag = b->isInsetFaceFlowEdge( flowThruMarkedEdges );
			bool continuousFlowFlag = edgeAFlowFlag && edgeBFlowFlag;

			extrudeFlag = extrudeFlag || continuousFlowFlag;

			if ( !continuousFlowFlag )
			{
				// Compute the epsilon required by computeExpandEdgeScaleFactor()
				double adjustmentScaleSqrEpsilon = computeExpandEdgeScaleFactorEpsilonSquared( neighbourhood.edges );

				//SPLIT THE INTERNAL EDGES
				for (int edgeI = 1; edgeI < ( neighbourhood.edges.size() - 1 ); edgeI++)
				{
					MEdge *e = neighbourhood.edges[edgeI];

					double t = 1.0;

					if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
					{
						//to centre: reaches centre of edge at t=0.5
						t = 0.5;
					}
					else if ( insetStyle == MINSETFACESTYLE_FROMEDGES )
					{
						double ta = 0.0, tb = 0.0;

						ta = computeExpandEdgeScaleFactor( a, e, adjustmentScaleSqrEpsilon );
						tb = computeExpandEdgeScaleFactor( b, e, adjustmentScaleSqrEpsilon );

						t = std::max( ta, tb );
					}

					MEdge *newEdge0, *newEdge1;
					e->splitAdjustable( this, vertexAdjusts, extrusionVector, vertexAttribAdjusts, t, newEdge0, newEdge1, false, true );

					//mark the edge if desired
					if ( markInsetEdges )
					{
						newEdge0->edgeMark();
					}
					if ( nsharpInsetEdges )
					{
						newEdge0->setNormalSharp();
					}
				}


				// SPLIT EDGE @a
				if ( edgeAFlowFlag )
				{
					double t = 1.0;

					if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
					{
						//to centre: reaches centre of edge at t=0.5
						t = 0.5;
					}
					else if ( insetStyle == MINSETFACESTYLE_FROMEDGES )
					{
						t = 1.0 / a->getLength();
					}

					MEdge *newEdge0, *newEdge1;
					a->splitAdjustable( this, vertexAdjusts, extrusionVector, vertexAttribAdjusts, t, newEdge0, newEdge1, false, true );

					//mark the edge if desired
					if ( markInsetEdges )
					{
						newEdge0->edgeMark();
					}
					if ( nsharpInsetEdges )
					{
						newEdge0->setNormalSharp();
					}
				}

				// SPLIT EDGE @b
				if ( edgeBFlowFlag )
				{
					double t = 1.0;

					if ( insetStyle == MINSETFACESTYLE_TOCENTRE )
					{
						//to centre: reaches centre of edge at t=0.5
						t = 0.5;
					}
					else if ( insetStyle == MINSETFACESTYLE_FROMEDGES )
					{
						t = 1.0 / b->getLength();
					}

					MEdge *newEdge0, *newEdge1;
					b->splitAdjustable( this, vertexAdjusts, extrusionVector, vertexAttribAdjusts, t, newEdge0, newEdge1, false, true );

					//mark the edge if desired
					if ( markInsetEdges )
					{
						newEdge0->edgeMark();
					}
					if ( nsharpInsetEdges )
					{
						newEdge0->setNormalSharp();
					}
				}
			}
		}
	}

	if ( extrudeFlag )
	{
		insetExpandFacesMarkedInternalVertexExtrude( vertexAdjusts );
	}
}



// ******************* EXPAND FACE OPS *******************

void MVertex::expandFacesSplitInternalEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markExpandEdges, bool nsharpExpandEdges)
{
	//NOTE: ONLY CALLED FOR VERTEX WHICH LIES ON A FACE MARK BOUNDARY

	// Discover all unmarked face neighbourhoods
	Array<VertexNeighbourhood> neighbourhoods;

	discoverAllFaceUnmarkedNeighbourhoods( neighbourhoods );


	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

		//get the boundary edges
		MEdge *a = neighbourhood.edges.front();
		MEdge *b = neighbourhood.edges.back();

		if ( !a->isExpandFaceBoundary() )
		{
			a = NULL;
		}

		if ( !b->isExpandFaceBoundary() )
		{
			b = NULL;
		}


		// Compute the epsilon required by computeExpandEdgeScaleFactor()
		double adjustmentScaleSqrEpsilon = computeExpandEdgeScaleFactorEpsilonSquared( neighbourhood.edges );

		//SPLIT THE INTERNAL EDGES
		for (int edgeI = 0; edgeI < neighbourhood.edges.size(); edgeI++)
		{
			MEdge *e = neighbourhood.edges[edgeI];

			if ( !e->isExpandFaceBoundary() )
			{
				double t = 1.0;

				if ( a != NULL  &&  b != NULL )
				{
					double ta = computeExpandEdgeScaleFactor( a, e, adjustmentScaleSqrEpsilon );
					double tb = computeExpandEdgeScaleFactor( b, e, adjustmentScaleSqrEpsilon );
					t = std::max( ta, tb );
				}
				else if ( a != NULL )
				{
					t = computeExpandEdgeScaleFactor( a, e, adjustmentScaleSqrEpsilon );
				}
				else if ( b != NULL )
				{
					t = computeExpandEdgeScaleFactor( b, e, adjustmentScaleSqrEpsilon );
				}
				else
				{
					t = 1.0  /  e->getLength();
				}

				MEdge *newEdge0, *newEdge1;
				e->splitAdjustable( this, vertexAdjusts, Vector3(), vertexAttribAdjusts, t, newEdge0, newEdge1 );

				//mark the edge if desired
				if ( markExpandEdges )
				{
					newEdge0->edgeMark();
				}
				if ( nsharpExpandEdges )
				{
					newEdge0->setNormalSharp();
				}
			}
		}
	}
}






// ******************* EXPAND VERTEX OPS *******************

void MVertex::expand(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges, bool normalSharpenBoundaryEdges)
{
	Vector3 extrusionVector;

	for (int i = 0; i < faces.size(); i++)
	{
		extrusionVector += faces[i]->getPlane().n;
	}
	extrusionVector.normalise();


	//*this needs to be adjusted in the direction of the extrusion
	vertexAdjusts.push_back( MVertexAdjust( this, position, Vector3(), extrusionVector ) );



	//SPLIT ALL OF THE INCIDENT EDGES
	//splitting edges alters the edge list, so take a copy
	Array<MEdge*> edgeList = edges;

	for (int i = 0; i < edgeList.size(); i++)
	{
		//get the edge direction from this

		//compute the scale factor
		double scaleFactor = 1.0 / edgeList[i]->getLength();


		MEdge *newEdge0, *newEdge1;
		edgeList[i]->splitAdjustable( this, vertexAdjusts, Vector3(), vertexAttribAdjusts, scaleFactor, newEdge0, newEdge1 );
	}


	//SPLIT THE INCIDENT FACES
	for (int i = 0; i < faces.size(); i++)
	{
		MFace *face = faces[i];
		MEdge *edgeAtSplit;
		face->splitOffTriangleIncidentToVertex( this, edgeAtSplit );

		if ( markBoundaryEdges )
		{
			edgeAtSplit->edgeMark();
		}

		if ( normalSharpenBoundaryEdges )
		{
			edgeAtSplit->setNormalSharp();
		}
	}
}



// ******************* EXPAND VERTEX IN MARKED FACES OPS *******************

void MVertex::expandInMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges)
{
	//SPLIT THE INCIDENT EDGES THAT ARE FACE MARKED
	//splitting edges alters the edge list, so take a copy
	Array<MEdge*> edgeList = edges;

	for (int i = 0; i < edgeList.size(); i++)
	{
		MEdge *e = edgeList[i];

		if ( e->isFaceMarked() )
		{
			//compute the scale factor
			double scaleFactor = 1.0 / edgeList[i]->getLength();

			MEdge *newEdge0, *newEdge1;
			edgeList[i]->splitAdjustable( this, vertexAdjusts, Vector3(), vertexAttribAdjusts, scaleFactor, newEdge0, newEdge1 );
		}
	}


	//SPLIT MARKED INCIDENT FACES
	for (int i = 0; i < faces.size(); i++)
	{
		MFace *face = faces[i];

		if ( face->isFaceMarked() )
		{
			MEdge *edgeAtSplit;
			face->splitOffTriangleIncidentToVertex( this, edgeAtSplit );

			if ( markBoundaryEdges )
			{
				edgeAtSplit->edgeMark();
			}
		}
	}
}




// ******************* BEVEL VERTEX OPS *******************

Vector3 MVertex::computeBevelVector(const MEdgeList &edgeList) const
{
	Vector3 bevelVector;

	for (int i = 0; i < edgeList.size(); i++)
	{
		bevelVector += edgeList[i]->getEdgeVector( this ).getNormalised();
	}

	bevelVector *= 1.0 / (double)edgeList.size();

	return bevelVector;
}



void MVertex::bevel(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
				bool useExistingTexturing, bool markBoundaryEdges, bool normalSharpenBoundaryEdges)
{
	Array<VertexNeighbourhood> neighbourhoods;

	discoverAllNeighbourhoods( neighbourhoods );


	for (int i = 0; i < neighbourhoods.size(); i++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[i];


		bool validFace = neighbourhood.edges.size() >= 3;

		bool needFlip = neighbourhood.isOrderConsistent();

		if ( needFlip )
		{
			neighbourhood.flip();
		}



		//SPLIT THE INCIDENT EDGES IN THIS TRAVERSAL
		MVertexList splitVerts;
		if ( validFace )
		{
			splitVerts.reserve( neighbourhood.edges.size() );
		}

		//attrib adjustment points, if existing texturing is used.
		Array<Point2f> attribAdjustPoints;
		if ( useExistingTexturing  &&  validFace )
		{
			attribAdjustPoints.reserve( neighbourhood.edges.size() );
		}

		for (int j = 0; j < neighbourhood.edges.size(); j++)
		{
			MEdge *edge = neighbourhood.edges[j];
			double interpFactor = 1.0  /  edge->getLength();

			//acquire texturing info if needed
			if ( useExistingTexturing  &&  validFace )
			{
				MVertex *oppositeVertex = edge->getOppositeVertex( this );
				MVertexAttrib *attribA, *attribB;
				edge->getVertexAttribs( oppositeVertex, attribA, attribB );
				Point2f p;

				if ( attribB != NULL )
				{
					p = Point2f::average( attribA->getPoint(), attribB->getPoint() );
				}
				else
				{
					p = attribA->getPoint();
				}

				p = Point2f::mul( p, interpFactor );
				attribAdjustPoints.push_back( p );
			}

			MEdge *newEdge0, *newEdge1;
			MVertex *v = edge->splitAdjustable( this, vertexAdjusts, Vector3(), vertexAttribAdjusts, interpFactor, newEdge0, newEdge1 );
			if ( validFace )
			{
				splitVerts.push_back( v );
			}
		}



		//REMOVE @this FROM THE INCIDENT FACES
		for (int j = 0; j < neighbourhood.faces.size(); j++)
		{
			neighbourhood.faces[j]->removeVertex( this, true, true );
		}


		//CREATE THE BEVEL FACE

		if ( validFace )
		{
			MFace *f;

			if ( useExistingTexturing )
			{
				//compute the average tex coord
				Point2f texCentre;
				for (int j = 0; j < attribAdjustPoints.size(); j++)
				{
					texCentre.cumulativeAdd( attribAdjustPoints[j] );
				}
				texCentre.scale( 1.0 / (double)attribAdjustPoints.size() );

				//replicate into tex coord Array
				Array<Point2f> texCoords;
				texCoords.fill( texCentre, splitVerts.size() );

				//get material ID
				int materialID = neighbourhood.faces[0]->getMaterialID();

				//create the face
				f = getMesh()->addFace( splitVerts, texCoords, materialID );

				//create the attrib adjusts; they spread the texture
				//co-ordinate out from @texCentre
				for (int j = 0; j < splitVerts.size(); j++)
				{
					MVertexAttrib *attrib = f->getVertexAttrib( j );

					vertexAttribAdjusts.push_back( MVertexAttribAdjust( attrib, texCentre, attribAdjustPoints[j] - texCentre ) );
				}
			}
			else
			{
				f = getMesh()->addFaceAutoTexture( splitVerts, false, -1 );
			}


			//MARK / NORMAL SHARPEN BOUNDARY EDGES
			if ( markBoundaryEdges  ||  normalSharpenBoundaryEdges )
			{
				for (int j = 0; j < f->getSize(); j++)
				{
					MEdge *e = f->getEdge( j );

					if ( markBoundaryEdges )
					{
						e->edgeMark();
					}

					if ( normalSharpenBoundaryEdges )
					{
						e->setNormalSharp();
					}
				}
			}
		}
	}
}




// ******************* EXPAND EDGE OPS *******************

double MVertex::computeExpandEdgeScaleFactor(MEdge *expandedEdge, MEdge *edgeToSplit, double epsilonSquared)
{
	Ray3 expandedEdgeRay = expandedEdge->getRay( this );
	MVertex *oppositeVertex = edgeToSplit->getOppositeVertex( this );
	double sqrDistance = expandedEdgeRay.sqrDistanceTo( oppositeVertex->getPosition() );
	if ( sqrDistance >= epsilonSquared )
	{
		return 1.0  /  sqrt( sqrDistance );
	}
	else
	{
		return 1.0;
	}
}

double MVertex::computeExpandEdgeScaleFactorEpsilonSquared(const MEdgeList &edgeList)
{
	return MEdge::computeMaxLengthSquared( edgeList )  *  SQR_EPSILON;
}


void MVertex::expandEdgeSplitUnmarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MEdgeList &boundaryEdges)
{
	//SPLIT THE UNMARKED INCIDENT EDGES
	//splitting edges alters the edge list, so take a copy
	Array<MEdge*> edgeList, expandedEdges;

	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		MEdge *e = edges[edgeI];

		if ( e->isEdgeMarked() )
		{
			expandedEdges.push_back( e );
		}
		else
		{
			edgeList.push_back( e );
		}
	}

	// Compute the epsilon required by computeExpandEdgeScaleFactor()
	double adjustmentScaleSqrEpsilon = computeExpandEdgeScaleFactorEpsilonSquared( edges );

	for (int edgeI = 0; edgeI < edgeList.size(); edgeI++)
	{
		MEdge *e = edgeList[edgeI];
		double scaleFactor = std::numeric_limits<double>::max();

		for (int expandedEdgeI = 0; expandedEdgeI < expandedEdges.size(); expandedEdgeI++)
		{
			MEdge *expandedEdge = expandedEdges[expandedEdgeI];

			double expandedEdgeScaleFactor = computeExpandEdgeScaleFactor( expandedEdge, e, adjustmentScaleSqrEpsilon );
			scaleFactor = std::min( scaleFactor, expandedEdgeScaleFactor );
		}

		if ( expandedEdges.size() == 0 )
		{
			double eSqrLength = e->getLengthSquared();
			if ( eSqrLength >= adjustmentScaleSqrEpsilon )
			{
				scaleFactor = 1.0  /  sqrt( eSqrLength );
			}
			else
			{
				scaleFactor = 1.0;
			}
		}

		MEdge *newEdge0, *newEdge1;
		e->splitAdjustable( this, vertexAdjusts, Vector3(), vertexAttribAdjusts, scaleFactor, newEdge0, newEdge1 );
	}




	//SPLIT THE INCIDENT FACES
	//splitting faces alters the faces list, so take a copy and use that
	Array<MFace*> faceList = faces;

	for (int i = 0; i < faceList.size(); i++)
	{
		MFace *f = faceList[i];

		int index = f->findVertex( this );

		if ( index != -1 )
		{
			int prevVtxIndex = prevIndex( index, f->getSize() );
			int nextVtxIndex = nextIndex( index, f->getSize() );

			//only split if the edges that surround this in f are not marked
			if ( !f->getEdge( prevVtxIndex )->isEdgeMarked()  &&  !f->getEdge( index )->isEdgeMarked() )
			{
				MEdge *newEdge;
				f->split( prevVtxIndex, nextVtxIndex, newEdge );
				boundaryEdges.push_back( newEdge );
			}
		}
	}
}

void MVertex::expandEdgeMoveVertex(MVertexAdjustList &vertexAdjusts)
{
	//COMPUTE THE EXPANSION VECTOR

	Array<Vector3> normals;

	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isEdgeMarked() )			//only use the normals of edge marked
		{													//faces
			normals.push_back( faces[i]->getPlane().n );
		}
	}


	Vector3 extrusionVector = MMesh::computeExpansionVector( normals );


	vertexAdjusts.push_back( MVertexAdjust( this, position, Vector3(), extrusionVector ) );
}




// ******************* BEVEL EDGE OPS *******************

void MVertex::bevelEdgeInit(bool initialiseForBevel)
{
	if ( initialiseForBevel )
	{
		vertexOpData.bevelEdge.table = new BevelEdgeTable();
	}
	else
	{
		vertexOpData.bevelEdge.table = NULL;
	}
}

void MVertex::bevelEdgeFinish()
{
	delete vertexOpData.bevelEdge.table;
}

bool MVertex::isInitialisedForBevelEdge() const
{
	return vertexOpData.bevelEdge.table != NULL;
}


MVertex * MVertex::getBevelEdgeBoundaryVertexForEdge(MEdge *e)
{
	return vertexOpData.bevelEdge.table->getVertexForEdge( e );
}

MVertex * MVertex::getBevelEdgeBoundaryVertexForFace(MFace *f)
{
	return vertexOpData.bevelEdge.table->getVertexForFace( f );
}


void MVertex::bevelEdgeCreateBoundaryVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &attribAdjusts)
{
	BevelEdgeTable &bevelTable = *vertexOpData.bevelEdge.table;

	//discover all edges and faces around the vertex
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllNeighbourhoods( neighbourhoods );

	//for each neighbourhood
	for (int neighbourHoodI = 0; neighbourHoodI < neighbourhoods.size(); neighbourHoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourHoodI];

		//if there are no marked edges in this neighbourhood, skip it
		if ( !neighbourhood.hasMarkedEdges() )
		{
			//skip
			continue;
		}



		BevelEdgeTable::CreatedBoundaryVertices &createdBoundaryVertices = bevelTable.createdBoundaryVerticesByNeighbourhood.push_back();



		//if this vertex is to be bevelled into a face, then the face must use
		//the bevelled vertices in the opposite order to that which its neighbouring
		//faces do, so invert if the order in @neighbourhood is consistent
		createdBoundaryVertices.inverseOrderFlag = neighbourhood.isOrderConsistent();



		gs_assert( !neighbourhood.faces.isEmpty(), "MVertex::bevelEdgeCreateBoundaryVertices(): @neighbourhood has no faces\n" );

		createdBoundaryVertices.materialID = neighbourhood.faces[0]->getMaterialID();



		// Get the list of marked edges in this neighbourhood
		Array<MEdge*> neighbourhoodMarkedEdges;
		for (int edgeI = 0; edgeI < neighbourhood.edges.size(); edgeI++)
		{
			MEdge *e = neighbourhood.edges[edgeI];

			if ( e->isEdgeMarked() )
			{
				neighbourhoodMarkedEdges.push_back( e );
			}
		}

		// Compute the epsilon required by computeExpandEdgeScaleFactor()
		double adjustmentScaleSqrEpsilon = computeExpandEdgeScaleFactorEpsilonSquared( edges );
		bool linearNeighbourhoodWithOneMarkedEdgeSpecialCase = neighbourhood.edges.size() == 2  &&  neighbourhood.faces.size() == 2  &&  neighbourhoodMarkedEdges.size() == 1;

		// For each edge in the neighbourhood:
		for (int edgeI = 0; edgeI < neighbourhood.edges.size(); edgeI++)
		{
			MEdge *e = neighbourhood.edges[edgeI];

			// Get face pointer if the edge index is within the bounds of the face list, NULL otherwise
			MFace *f = edgeI < neighbourhood.faces.size()  ?  neighbourhood.faces[edgeI]  :  NULL;


			if ( !e->isEdgeMarked() )
			{
				//create a new vertex which moves down the edge
				MVertex *v = getMesh()->addVertex( getPosition() );
				bevelTable.setVertexForEdge( e, v );
				createdBoundaryVertices.vertices.push_back( v );

				//compute the adjustment scale factor
				double adjustmentScale = std::numeric_limits<double>::max();

				for (int markedEdgeI = 0; markedEdgeI < neighbourhoodMarkedEdges.size(); markedEdgeI++)
				{
					MEdge *markedEdge = neighbourhoodMarkedEdges[markedEdgeI];

					double markedEdgeScaleFactor = computeExpandEdgeScaleFactor( markedEdge, e, adjustmentScaleSqrEpsilon );
					adjustmentScale = std::min( adjustmentScale, markedEdgeScaleFactor );
				}

				if ( neighbourhoodMarkedEdges.size() == 0 )
				{
					// No marked edges in this neighbourhood; 1:1 ratio
					double eSqrLength = e->getLengthSquared();
					if ( eSqrLength > adjustmentScaleSqrEpsilon )
					{
						adjustmentScale = 1.0  /  sqrt( eSqrLength );
					}
					else
					{
						adjustmentScale = 1.0;
					}
				}

				//use MEdge::interpolatedAdjust() to move v along the edge e
				e->interpolatedAdjust( this, v, vertexAdjusts, Vector3(), attribAdjusts, adjustmentScale );

				//need the adjustment vector if a vertex face is to be created
				bevelTable.vertexFaceRelativePoints.push_back( e->getEdgeVector( this ) * adjustmentScale );
			}


			if ( f != NULL )
			{
				MEdge *e0 = e;
				MEdge *e1 = neighbourhood.edges[ nextIndex( edgeI, neighbourhood.edges.size() ) ];

				//insert a vertex between e0 and e1 if they are BOTH MARKED
				if ( ( e0->isEdgeMarked()  &&  e1->isEdgeMarked() )  ||  linearNeighbourhoodWithOneMarkedEdgeSpecialCase )
				{
					//create a new vertex which moves into the face
					MVertex *v = getMesh()->addVertex( getPosition() );
					bevelTable.setVertexForFace( f, v );
					createdBoundaryVertices.vertices.push_back( v );


					int vertexIndex = f->findVertex( this );
					//get the attribute currently associated with this vertex
					MVertexAttrib *attribFromF = f->getVertexAttrib( vertexIndex );
					//make a replacement attached to v
					MVertexAttrib *attrib = v->createVertexAttrib( *attribFromF, MFINDPOLICY_FORCEDCREATE );

					//compute the vertex and attrib adjustment vectors
					Vector3 vertexAdjustVec;
					Vector2f attribAdjustVec;
					double uLower, uUpper;
					//MFace::insetFaceComputeFromEdgeInsetVectors() uses the same
					//computations (don't project onto face plane though)
					f->computeFromEdgeInsetVectors( vertexIndex, vertexAdjustVec, attribAdjustVec, false, uLower, uUpper, false );

					//adjust
					vertexAdjusts.push_back( MVertexAdjust( v, getPosition(), vertexAdjustVec, uLower, uUpper ) );
					attribAdjusts.push_back( MVertexAttribAdjust( attrib, attribFromF->getPoint(), attribAdjustVec, uLower, uUpper ) );

					//need the adjustment vector if a vertex face is to be created
					bevelTable.vertexFaceRelativePoints.push_back( vertexAdjustVec );
				}
			}
		}


		//END POINT:
		// An end point is one where the run of edges terminates, or a vertex face is required
		//
		// Conditions:
		// - # of edges in neighbourhood > 4
		// - # of vertices gathered for a vertex face is >= 3
		// - # of marked edges != 2 (if it is 2, it might be part of a continuous edge run
		//
		// In the case of the linear neighbourhood with one marked edge special case (two edges, two faces, one edge marked, other not),
		// the conditions required for this will make this vertex (w.r.t. this neighbourhood) an end point.
		// The flag for this special case is also set here.
		bool isEndPoint = neighbourhood.edges.size() > 4  ||  createdBoundaryVertices.vertices.size() >= 3  ||  neighbourhoodMarkedEdges.size() != 2;

		if ( isEndPoint )
		{
			//at an end point
			for (int edgeI = 0; edgeI < neighbourhood.edges.size(); edgeI++)
			{
				//for each marked edge
				if ( neighbourhood.edges[edgeI]->isEdgeMarked() )
				{
					MEdge *e = neighbourhood.edges[edgeI];

					//mark e as a bevel strip end point
					e->setBevelStripEndPoint( this, true );
					e->setBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase( this, linearNeighbourhoodWithOneMarkedEdgeSpecialCase );
				}
			}
		}
	}
}


void MVertex::bevelEdgeCreateVertexFace(bool useExistingTexturing, MFaceList &createdFaces)
{
	BevelEdgeTable &bevelTable = *vertexOpData.bevelEdge.table;

	for (int i = 0; i < bevelTable.createdBoundaryVerticesByNeighbourhood.size(); i++)
	{
		BevelEdgeTable::CreatedBoundaryVertices &createdBoundaryVertices = bevelTable.createdBoundaryVerticesByNeighbourhood[i];
		MVertexList &createdVertices = createdBoundaryVertices.vertices;

		if ( createdVertices.size() >= 3 )
		{
			MFace *f = NULL;


			if ( useExistingTexturing )
			{
				Array<MVertexAttrib*> attribs;

				attribs.reserve( createdVertices.size() );
				for (int i = 0; i < createdVertices.size(); i++)
				{
					attribs.push_back( createdVertices[i]->getVertexAttrib( 0 ) );
				}

				int materialID = createdBoundaryVertices.materialID;

				//flip if necessary
				if ( createdBoundaryVertices.inverseOrderFlag )
				{
					createdVertices.reverse();
					attribs.reverse();
				}

				//add the face to the mesh
				f = getMesh()->addFace( createdVertices, attribs, materialID );
			}
			else
			{
				//NOTE: ***MUST*** use bevelTable.vertexFaceRelativePoints as a source
				//of face points, because the vertices in
				//bevelTable.createdBoundaryVertices are all in the same position, UNTIL
				//they are adjusted. This 'degenerate' set of points should not be used
				//to compute the texture-co-ordinates
				Array<Point2f> texCoords;


				Plane facePlane( bevelTable.vertexFaceRelativePoints );
				//get the axis that most closely represents the plane normal
				Axis normalAxis = facePlane.n.getMajorAxis();


				//boxes for computing a scale factor so that the texture co-ordinates
				//can be computed
				BBox3 box( bevelTable.vertexFaceRelativePoints );
				BBox2 box2 = box.toBBox2( normalAxis );

				double uScale = 1.0 / box2.getWidth();
				double vScale = 1.0 / box2.getHeight();

				//compute the texture co-ordinates for the points
				texCoords.reserve( createdVertices.size() );
				for (int j = 0; j < createdVertices.size(); j++)
				{
					const Point2 &vtx = bevelTable.vertexFaceRelativePoints[j].toPoint2Axis( normalAxis );
					Vector2 point = vtx - box2.getLower();
					point.x *= uScale;
					point.y *= vScale;
					texCoords.push_back( Point2f( point.x, point.y ) );
				}


				//flip if necessary
				if ( createdBoundaryVertices.inverseOrderFlag )
				{
					createdVertices.reverse();
					texCoords.reverse();
				}

				//add the face to the mesh
				f = getMesh()->addFace( createdVertices, texCoords, -1 );
			}

			createdFaces.push_back( f );
		}
	}
}


void MVertex::bevelEdgeExtractQuadstrip(_BevelledEdgeQuadStrip &strip, MEdge *startEdge, bool storeEdgeMaterials)
{
	MEdgeRun runEdges;
	MVertexList runVerts;
	MVertexList quadVerts;


	//initialise edge run
	runEdges.clear();
	runVerts.clear();

	MEdge *e = startEdge, *ePrev = e;
	MVertex *v = this;

	//assume to be open
	runEdges.open();

	//get the run of edges
	do
	{
		gs_assert( e != NULL, "MVertex::bevelEdgeExtractQuadstrip() edge run ran out of edges\n" );

		runEdges.push_back( e );
		runVerts.push_back( v );

		//visited
		e->secondaryMark();

		ePrev = e;
		v = e->getOppositeVertex( v );

		//get the next edge to pass through
		e = v->traverseToOtherMarkedEdge( e );

		if ( e != NULL )
		{
			//make sure e has not been already visited before looping again
			if ( e->isSecondaryMarked() )
			{
				runEdges.close();
				break;
			}
		}
	} while ( !ePrev->isBevelStripEndPoint( v ) );

	//add the last vertex
	runVerts.push_back( v );


	//CURRENT STATE:
	//if open:
	//runVerts contains all vertices including first and last vertices
	//runEdges contains all edges in between the vertices
	//runVerts.size() == runEdges.size() + 1
	//
	//if closed:
	//runVerts contains all vertices, the first vertex is repeated at the end
	//runEdges contains all edges in between the vertices
	//runVerts.size() == runEdges.size() + 1


	//generate texture-v co-ordinates
	runEdges.generateUnitCumulativeLengthArray( strip.textureV );


	//first extract 4 vertices for each quad and place in this Array
	//(clear it first)
	quadVerts.clear();

	//reserve space for edge material IDs if they are to be stored
	if ( storeEdgeMaterials )
	{
		strip.edgeMaterials.reserve( runEdges.size() );
	}

	for (int edgeI = 0; edgeI < runEdges.size(); edgeI++)
	{
		MEdge *runEdge = runEdges[edgeI];
		//extract the 4 vertices that make up the quad for this edge
		MEdge *thruFaceA[2], *thruFaceB[2];
		runEdge->getAdjacentEdges( runVerts[edgeI], thruFaceA[0], thruFaceB[0] );
		runEdge->getAdjacentEdges( runVerts[ edgeI + 1 ], thruFaceA[1], thruFaceB[1] );

		BevelEdgeTable *bevelTabs[2];
		MVertex *a[2], *b[2];

		bevelTabs[0] = runVerts[edgeI]->vertexOpData.bevelEdge.table;
		bevelTabs[1] = runVerts[ edgeI + 1 ]->vertexOpData.bevelEdge.table;


		for (int k = 0; k < 2; k++)
		{
			bool linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag = runEdge->isBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase( runVerts[ edgeI + k ] );

			if ( linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag )
			{
				// Linear neighbourhood with one marked edge special case; get vertices for quad from face
				a[k] = bevelTabs[k]->getVertexForFace( runEdge->getFaceA() );
				b[k] = bevelTabs[k]->getVertexForFace( runEdge->getFaceB() );
			}
			else
			{
				if ( thruFaceA[k] != NULL )
				{
					//query the bevel table
					if ( thruFaceA[k]->isEdgeMarked() )
					{
						//adjacent edge is marked; the face in between will have the
						//vertex associated with it
						a[k] = bevelTabs[k]->getVertexForFace( runEdge->getFaceA() );
					}
					else
					{
						//adjacent edge will have the vertex associated with it
						a[k] = bevelTabs[k]->getVertexForEdge( thruFaceA[k] );
					}
				}
				else
				{
					//just use the original vertex
					a[k] = runVerts[ edgeI + k ];
				}


				if ( thruFaceB[k] != NULL )
				{
					//query the bevel table
					if ( thruFaceB[k]->isEdgeMarked() )
					{
						//adjacent edge is marked; the face in between will have the
						//vertex associated with it
						b[k] = bevelTabs[k]->getVertexForFace( runEdge->getFaceB() );
					}
					else
					{
						//adjacent edge will have the vertex associated with it
						b[k] = bevelTabs[k]->getVertexForEdge( thruFaceB[k] );
					}
				}
				else
				{
					//just use the original vertex
					b[k] = runVerts[ edgeI + k ];
				}
			}
		}

		quadVerts.push_back( a[0] );
		quadVerts.push_back( b[0] );
		quadVerts.push_back( a[1] );
		quadVerts.push_back( b[1] );


		//get and store the edge material if requested
		if ( storeEdgeMaterials )
		{
			MFace *faceA = runEdge->getFaceA();

			gs_assert( faceA != NULL, "MVertex::bevelEdgeExtractQuadStrip(): could not get face A from @runEdge\n" );

			int edgeMaterialID = faceA->getMaterialID();

			strip.edgeMaterials.push_back( edgeMaterialID );
		}
	}


	//process the quads, ensuring that adjacent quads have the same vertex
	//ordering
	bool flipFlag = !runEdges.isEdge0InOrder();

	//copy first two vertices over, flipping if appropriate
	if ( flipFlag )
	{
		strip.vertices.push_back( quadVerts[1] );
		strip.vertices.push_back( quadVerts[0] );
	}
	else
	{
		strip.vertices.extend( quadVerts, 0, 2 );
	}

	//process intermediate vertex pairs
	for (int vertexI = 0; vertexI < quadVerts.size() - 4; vertexI += 4)
	{
		MVertex *currentQuadEnd[2], *nextQuadStart[2];

		//get the last two vertices of the current quad, and the first two
		//of the next quad
		//note: the two lists will contain the same pair of vertices, however
		//same order is not guaranteed
		currentQuadEnd[0] = quadVerts[ vertexI + 2 ];
		currentQuadEnd[1] = quadVerts[ vertexI + 3 ];
		nextQuadStart[0] = quadVerts[ vertexI + 4 ];
		nextQuadStart[1] = quadVerts[ vertexI + 5 ];

		//add in the last two vertices of the current quad
		if ( flipFlag )
		{
			strip.vertices.push_back( currentQuadEnd[1] );
			strip.vertices.push_back( currentQuadEnd[0] );
		}
		else
		{
			strip.vertices.push_back( currentQuadEnd[0] );
			strip.vertices.push_back( currentQuadEnd[1] );
		}

		//check if the next quad in in the same order
		if ( currentQuadEnd[0] == nextQuadStart[0]  ||  currentQuadEnd[1] == nextQuadStart[1] )
		{
			//same order, do nothing
		}
		else if ( currentQuadEnd[0] == nextQuadStart[1]  ||  currentQuadEnd[1] == nextQuadStart[0] )
		{
			//opposite order, flip the flip flag
			flipFlag = !flipFlag;
		}
		else
		{
			//the two vertex lists do not share any vertices,
			//something has gone horribly wrong....
			gs_assert_not_reached( "MVertex::bevelEdgeExtractQuadstrip(): adjacent quads do not share any vertices\n" );
			return;
		}
	}

	//copy last two vertices over, flipping if appropriate
	if ( flipFlag )
	{
		strip.vertices.push_back( quadVerts[ quadVerts.size() - 1 ] );
		strip.vertices.push_back( quadVerts[ quadVerts.size() - 2 ] );
	}
	else
	{
		strip.vertices.push_back( quadVerts[ quadVerts.size() - 2 ] );
		strip.vertices.push_back( quadVerts[ quadVerts.size() - 1 ] );
	}
}

void MVertex::bevelEdgeExtractOpenQuadstrips(Array<_BevelledEdgeQuadStrip> &strips, bool storeEdgeMaterials)
{
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		//find a marked, end point, unvisited edge to start at
		if ( edges[edgeI]->isEdgeMarked()  &&  edges[edgeI]->isBevelStripEndPoint( this )  &&  !edges[edgeI]->isSecondaryMarked() )
		{
			_BevelledEdgeQuadStrip &strip = strips.push_back();
			bevelEdgeExtractQuadstrip( strip, edges[edgeI], storeEdgeMaterials );
		}
	}
}

void MVertex::bevelEdgeExtractClosedQuadstrips(Array<_BevelledEdgeQuadStrip> &strips, bool storeEdgeMaterials)
{
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		//find a marked, unvisited edge to start at
		if ( edges[edgeI]->isEdgeMarked()  &&  !edges[edgeI]->isSecondaryMarked() )
		{
			_BevelledEdgeQuadStrip &strip = strips.push_back();
			bevelEdgeExtractQuadstrip( strip, edges[edgeI], storeEdgeMaterials );
		}
	}
}



// ******************* EXTRUDE EDGE OPS *******************

int MVertex::getExtrudeableEdgeCount() const
{
	int count = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isExtrudeable() )
		{
			count++;
		}
	}

	return count;
}

MEdge * MVertex::getExtrudeableEdge()
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isExtrudeable() )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getOtherExtrudeableEdge(MEdge *e)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if ( edges[i]->isExtrudeable()  &&  edges[i] != e )
		{
			return edges[i];
		}
	}

	return NULL;
}

MEdge * MVertex::getNextExtrudeableEdge(MEdge *e)
{
	if ( getExtrudeableEdgeCount() == 2 )
	{
		return getOtherExtrudeableEdge( e );
	}
	else
	{
		return NULL;
	}
}



// ******************* SLIDE EDGE OPS *******************

void MVertex::slideEdgeCreateAdjusts(MEdge *eNegative, MEdge *ePositive, MFace *fNegative, MFace *fPositive,
								MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts)
{
	adjusts.push_back( MVertexSlideAdjust( this, this->getPosition(), eNegative->getEdgeVector( this ), ePositive->getEdgeVector( this ) ) );
	MVertexAttrib *aNegative = eNegative->getVertexAttrib( this, fNegative );
	MVertexAttrib *aPositive = ePositive->getVertexAttrib( this, fPositive );
	if ( aNegative == aPositive )
	{
		attribAdjusts.push_back( MVertexAttribSlideAdjust( aPositive, aPositive->getPoint(),
													eNegative->getVertexAttribVector( fNegative, this ), ePositive->getVertexAttribVector( fPositive, this ) ) );
	}
}

void MVertex::slideEdgeCreateAdjusts(MEdge *eNegative, MEdge *ePositive, MFace *fNegativeA, MFace *fNegativeB, MFace *fPositiveA, MFace *fPositiveB,
								MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts)
{
	adjusts.push_back( MVertexSlideAdjust( this, this->getPosition(), eNegative->getEdgeVector( this ), ePositive->getEdgeVector( this ) ) );
	MVertexAttrib *aNegativeA = eNegative->getVertexAttrib( this, fNegativeA );
	MVertexAttrib *aNegativeB = eNegative->getVertexAttrib( this, fNegativeB );
	MVertexAttrib *aPositiveA = ePositive->getVertexAttrib( this, fPositiveA );
	MVertexAttrib *aPositiveB = ePositive->getVertexAttrib( this, fPositiveB );
	if ( aNegativeA == aPositiveA )
	{
		attribAdjusts.push_back( MVertexAttribSlideAdjust( aPositiveA, aPositiveA->getPoint(),
												eNegative->getVertexAttribVector( fNegativeA, this ),
												ePositive->getVertexAttribVector( fPositiveA, this ) ) );
	}
	if ( aNegativeB == aPositiveB  &&  ( aNegativeB != aNegativeA  ||  aPositiveB != aPositiveA ) )
	{
		attribAdjusts.push_back( MVertexAttribSlideAdjust( aPositiveB, aPositiveB->getPoint(),
												eNegative->getVertexAttribVector( fNegativeB, this ),
												ePositive->getVertexAttribVector( fPositiveB, this ) ) );
	}
}

void MVertex::slideEdgeCreateAdjusts(const MEdgeList &eNegative, const MEdgeList &ePositive, const MFaceList &fNegative, const MFaceList &fPositive,
							MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts)
{
	// The parameter pairs (eNegative, fNegative) and (ePositive, fPositive) can come in two configurations:
	// eNegative.size() == fNegative.size() + 1:
	//		1 more edge than there are faces. In this case the order is  [ e0, f0, e1, f1.... en-1, fn-1, en ]
	// OR
	// eNegative.size() == fNegative.size() - 1:
	//		1 more face than there are edges. In this case the order is  [ f0, e0, f1, e2.... fn-1, en-1, fn ]

	// Vertex adjustment
	Vector3 negativeVector, positiveVector;


	for (int edgeI = 0; edgeI < eNegative.size(); edgeI++)
	{
		negativeVector += eNegative[edgeI]->getEdgeVector( this );
	}

	for (int edgeI = 0; edgeI < ePositive.size(); edgeI++)
	{
		positiveVector += ePositive[edgeI]->getEdgeVector( this );
	}


	if ( eNegative.size() > 0 )
	{
		negativeVector *= ( 1.0 / (double)eNegative.size() );
	}

	if ( ePositive.size() > 0 )
	{
		positiveVector *= ( 1.0 / (double)ePositive.size() );
	}

	adjusts.push_back( MVertexSlideAdjust( this, this->getPosition(), negativeVector, positiveVector ) );


	// Vertex attribute adjustment
	for (int attribI = 0; attribI < getNumVertexAttribs(); attribI++)
	{
		MVertexAttrib *attrib = getVertexAttrib( attribI );

		Vector2f negativeAttribVector, positiveAttribVector;
		int negativeCount = 0, positiveCount = 0;

		bool bMoreNegativeFaces = fNegative.size() > eNegative.size();
		bool bMorePositiveFaces = fPositive.size() > ePositive.size();

		for (int edgeI = 0; edgeI < eNegative.size(); edgeI++)
		{
			int faceIPrev = bMoreNegativeFaces  ?  edgeI  :  edgeI - 1;
			int faceI = bMoreNegativeFaces  ?  edgeI + 1  :  edgeI;
			int validFaceI = -1;
			bool bFaceIPrevHasAttrib = false, bFaceIHasAttrib = false;
			if ( faceIPrev >= 0 )
			{
				bFaceIPrevHasAttrib = attrib == eNegative[edgeI]->getVertexAttrib( this, fNegative[faceIPrev] );
				validFaceI = faceIPrev;
			}
			if ( faceI < fNegative.size() )
			{
				bFaceIHasAttrib = attrib == eNegative[edgeI]->getVertexAttrib( this, fNegative[faceI] );
				validFaceI = faceI;
			}
			if ( bFaceIPrevHasAttrib  ||  bFaceIHasAttrib )
			{
				negativeAttribVector += eNegative[edgeI]->getVertexAttribVector( fNegative[validFaceI], this );
				negativeCount++;
			}
		}

		for (int edgeI = 0; edgeI < ePositive.size(); edgeI++)
		{
			int faceIPrev = bMorePositiveFaces  ?  edgeI  :  edgeI - 1;
			int faceI = bMorePositiveFaces  ?  edgeI + 1  :  edgeI;
			int validFaceI = -1;
			bool bFaceIPrevHasAttrib = false, bFaceIHasAttrib = false;
			if ( faceIPrev >= 0 )
			{
				bFaceIPrevHasAttrib = attrib == ePositive[edgeI]->getVertexAttrib( this, fPositive[faceIPrev] );
				validFaceI = faceIPrev;
			}
			if ( faceI < fPositive.size() )
			{
				bFaceIHasAttrib = attrib == ePositive[edgeI]->getVertexAttrib( this, fPositive[faceI] );
				validFaceI = faceI;
			}
			if ( bFaceIPrevHasAttrib  ||  bFaceIHasAttrib )
			{
				positiveAttribVector += ePositive[edgeI]->getVertexAttribVector( fPositive[validFaceI], this );
				positiveCount++;
			}
		}

		if ( negativeCount > 0  ||  positiveCount > 0 )
		{
			if ( negativeCount > 0 )
			{
				negativeAttribVector *= ( 1.0 / (double)negativeCount );
			}

			if ( positiveCount > 0 )
			{
				positiveAttribVector *= ( 1.0 / (double)positiveCount );
			}

			attribAdjusts.push_back( MVertexAttribSlideAdjust( attrib, attrib->getPoint(), negativeAttribVector, positiveAttribVector ) );
		}
	}
}

void MVertex::slideEdgeRunStartVertex(MEdge *e, MEdge *&eNegative, MEdge *&ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
							int slideVersion)
{
	eNegative = ePositive = NULL;

	if ( getMarkedEdgeCount() == 1 )
	{
		// There is only 1 marked edge incident to @this
		if ( isTopologySimple() )
		{
			// Simple topology (@this has 1 neighbourhood)

			if ( slideVersion == 0 )
			{
				// VERSION 0
				bool onBoundary = isOnBoundary();
				if ( ( onBoundary  &&  !e->isBoundary()  &&  edges.size() == 3 )   ||   ( !onBoundary  &&  edges.size() == 3 ) )
				{
					// On boundary: @e is not boundary, 3 edges; therefore the other two edges must be boundary
					// NOT on boundary: 3 edges

					// Get @ePositive and @eNegative, and set up the slide adjustments
					eNegative = e->getNextEdge( e->getFaceB(), this );
					ePositive = e->getNextEdge( e->getFaceA(), this );
					slideEdgeCreateAdjusts( eNegative, ePositive, e->getFaceB(), e->getFaceA(), adjusts, attribAdjusts );
				}
			}
			else
			{
				// VERSION > 0

				if ( !e->isBoundary()  &&  edges.size() >= 3 )
				{
					// @e is not boundary, >= 3 edges; setup the slide adjustments
					eNegative = e->getNextEdge( e->getFaceB(), this );
					ePositive = e->getNextEdge( e->getFaceA(), this );
					slideEdgeCreateAdjusts( eNegative, ePositive, e->getFaceB(), e->getFaceA(), adjusts, attribAdjusts );
				}
			}
		}
		else
		{
			// Complex topology (@this has >1 neighbourhoods)

			if ( !e->isBoundary() )
			{
				// @e is not boundary
				NeighbourhoodIterator iterNeg( this, e, e->getFaceB() );
				NeighbourhoodIterator iterPos( this, e, e->getFaceA() );
				iterNeg.next();
				iterPos.next();
				MEdge *ePos0 = iterPos.edge(), *eNeg0 = iterNeg.edge();

				if ( slideVersion == 0 )
				{
					// VERSION 0
					if ( ePos0->isBoundary()  &&  eNeg0->isBoundary() )
					{
						// Neighbouring edges are boundary; neighbourhood has 3 edges, 2 faces, @e is the centre edge
						// Setup slide adjustments
						eNegative = eNeg0;
						ePositive = ePos0;
						slideEdgeCreateAdjusts( eNegative, ePositive, e->getFaceB(), e->getFaceA(), adjusts, attribAdjusts );
					}
					else
					{
						// Compute the next edges in the neighbourhood
						iterNeg.next();
						iterPos.next();
						MEdge *ePos1 = iterPos.edge(), *eNeg1 = iterNeg.edge();

						if ( ePos0 == eNeg1  &&  eNeg0 == ePos1 )
						{
							// @ePos1 and @eNeg1 are @ePos0 and @eNeg0, just swapped over:
							// This means that the neighbourhood is closed, it has three edges, and three faces; slide
							eNegative = eNeg0;
							ePositive = ePos0;
							slideEdgeCreateAdjusts( eNegative, ePositive, e->getFaceB(), e->getFaceA(), adjusts, attribAdjusts );
						}
					}
				}
				else
				{
					// VERSION > 0
					// Setup the slide adjustments
					if ( ePos0 != eNeg0 )
					{
						// @ePos0 and @eNeg0 are not the same edge; slide
						eNegative = eNeg0;
						ePositive = ePos0;
						slideEdgeCreateAdjusts( eNegative, ePositive, e->getFaceB(), e->getFaceA(), adjusts, attribAdjusts );
					}
				}
			}
		}
	}
}

void MVertex::slideEdgeRunEndVertex(MEdge *e, MEdge *eNegative, MEdge *ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
							int slideVersion)
{
	if ( eNegative != NULL  &&  ePositive != NULL )
	{
		// @ePositive and @eNegative are used to keep the slide directions / sides constant
		// We can't do anything without them
		if ( getMarkedEdgeCount() == 1 )
		{
			// There is only 1 marked edge incident to @this

			if ( isTopologySimple() )
			{
				// Simple topology (@this has 1 neighbourhood)

				if ( slideVersion == 0 )
				{
					// VERSION 0
					bool onBoundary = isOnBoundary();
					if ( ( onBoundary  &&  !e->isBoundary()  &&  edges.size() == 3 )   ||   ( !onBoundary  &&  edges.size() == 3 ) )
					{
						// On boundary: @e is not boundary, 3 edges; therefore the other two edges must be boundary
						// NOT on boundary: 3 edges
						MFace *fNegative = eNegative->getSharedFace( e );
						MFace *fPositive = ePositive->getSharedFace( e );
						gs_assert( fNegative != NULL  &&  fPositive != NULL, "MVertex::slideEdgeRunEndVertex(): could not get shared faces\n" );
						eNegative = e->getNextEdge( fNegative, this );
						ePositive = e->getNextEdge( fPositive, this );
						slideEdgeCreateAdjusts( eNegative, ePositive, fNegative, fPositive, adjusts, attribAdjusts );
					}
				}
				else
				{
					// VERSION > 0
					if ( !e->isBoundary()  &&  edges.size() >= 3 )
					{
						// @e is not boundary, >= 3 edges
						MFace *fNegative = eNegative->getSharedFace( e );
						MFace *fPositive = ePositive->getSharedFace( e );
						gs_assert( fNegative != NULL  &&  fPositive != NULL, "MVertex::slideEdgeRunEndVertex(): could not get shared faces\n" );
						eNegative = e->getNextEdge( fNegative, this );
						ePositive = e->getNextEdge( fPositive, this );
						slideEdgeCreateAdjusts( eNegative, ePositive, fNegative, fPositive, adjusts, attribAdjusts );
					}
				}
			}
			else
			{
				// Complex topology (@this has >1 neighbourhoods)

				if ( !e->isBoundary() )
				{
					// @e is not boundary
					MFace *fNegative = eNegative->getSharedFace( e );
					MFace *fPositive = ePositive->getSharedFace( e );

					if ( slideVersion == 0 )
					{
						NeighbourhoodIterator iterNeg( this, e, fNegative );
						NeighbourhoodIterator iterPos( this, e, fPositive );
						iterNeg.next();
						iterPos.next();
						eNegative = iterNeg.edge();
						ePositive = iterPos.edge();

						if ( ePositive->isBoundary()  &&  eNegative->isBoundary() )
						{
							// Neighbouring edges are boundary; neighbourhood has 3 edges, 2 faces, @e is the centre edge
							// Setup slide adjustments
							slideEdgeCreateAdjusts( eNegative, ePositive, fNegative, fPositive, adjusts, attribAdjusts );
						}
						else
						{
							// Compute the next edges in the neighbourhood
							iterNeg.next();
							iterPos.next();
							MEdge *ePos1 = iterPos.edge(), *eNeg1 = iterNeg.edge();

							if ( ePositive == eNeg1  &&  eNegative == ePos1 )
							{
								// @ePos1 and @eNeg1 are @ePos0 and @eNeg0, just swapped over:
								// This means that the neighbourhood is closed, it has three edges, and three faces; slide
								slideEdgeCreateAdjusts( eNegative, ePositive, fNegative, fPositive, adjusts, attribAdjusts );
							}
						}
					}
					else
					{
						// VERSION > 0

						// Get the edges on either side of @e
						eNegative = e->getNextEdge( fNegative, this );
						ePositive = e->getNextEdge( fPositive, this );
						if ( ePositive != eNegative )
						{
							// @ePositive and @eNegative are not the same edge; slide
							slideEdgeCreateAdjusts( eNegative, ePositive, fNegative, fPositive, adjusts, attribAdjusts );
						}
					}
				}
			}
		}
	}
}

void MVertex::slideEdgeRunInternalVertex(MEdge *e, MEdge *&eNegative, MEdge *&ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
								int slideVersion)
{
	if ( getMarkedEdgeCount() == 2 )
	{
		// Two incident marked edges

		if ( edges.size() == 4  &&  faces.size() == 4  &&  isTopologySimple() )
		{
			// Simple topology, four incident edges, four incident faces

			// Since edges.size() == faces.size(), none of the edges can be boundary edges, as this would require edges.size() == faces.size() + 1
			// So we don' t have to check of @this is on a boundary

			// The edge opposite @e need must be the other marked edge

			// Check the two surrounding edges
			MFace *fNegative = e->getFaceB(), *fPositive = e->getFaceA();

			if ( eNegative != NULL )
			{
				fNegative = eNegative->getSharedFace( e );
			}

			if ( ePositive != NULL )
			{
				fPositive = ePositive->getSharedFace( e );
			}

			NeighbourhoodIterator iterNeg = neighbourhoodIterator( e, fNegative );
			NeighbourhoodIterator iterPos = neighbourhoodIterator( e, fPositive );
			iterNeg.next();
			iterPos.next();
			MEdge *eNeg0 = iterNeg.edge();
			MEdge *ePos0 = iterPos.edge();

			if ( !eNeg0->isEdgeMarked()  &&  !ePos0->isEdgeMarked() )
			{
				// Neither @eNeg0 or @ePos0 is marked, therefore the edge opposite @e must be the other marked edge
				// Setup the slide adjustments
				eNegative = eNeg0;
				ePositive = ePos0;
				slideEdgeCreateAdjusts( eNegative, ePositive,
									fNegative, eNegative->getOppositeFace( fNegative ),
									fPositive, ePositive->getOppositeFace( fPositive ),
									adjusts, attribAdjusts );
			}
			else
			{
				if ( eNeg0->isEdgeMarked() )
				{
					// @eNeg0 is the other marked edge
					if ( slideVersion == 0 )
					{
						// VERSION 0
						eNegative = e;
						MFace *f = eNeg0->getOppositeFace( fNegative );
						ePositive = eNeg0->getNextEdge( f, this );
						// Don't create adjustments; just acquite the negative and positive edges for the purpose of iteration
					}
					else
					{
						MEdgeList negEdges, posEdges;
						MFaceList negFaces, posFaces;

						// Edges on negative side: @e and @eNeg0
						negEdges.push_back( e );
						negEdges.push_back( eNeg0 );

						// Faces on negative side: @fNegative
						negFaces.push_back( fNegative );

						// Edges on positive side: @ePos0, and @ePos1
						// Faces on positive side: @fPositive, fPos0, fPos1
						posEdges.push_back( ePos0 );
						posFaces.push_back( fPositive );
						posFaces.push_back( iterPos.face() );		// fPos0
						iterPos.next();
						MEdge *ePos1 = iterPos.edge();
						posEdges.push_back( ePos1 );
						posFaces.push_back( iterPos.face() );		// fPos1


						// @eNegative and @ePositive are now @e and @ePos1
						eNegative = e;
						ePositive = ePos1;

						// Create adjustments
						slideEdgeCreateAdjusts( negEdges, posEdges, negFaces, posFaces, adjusts, attribAdjusts );
					}
				}
				else if ( ePos0->isEdgeMarked() )
				{
					// @ePos0 is the other marked edge
					if ( slideVersion == 0 )
					{
						// VERSION 0
						ePositive = e;
						MFace *f = ePos0->getOppositeFace( fPositive );
						eNegative = ePos0->getNextEdge( f, this );
						// Don't create adjustments; just acquite the negative and positive edges for the purpose of iteration
					}
					else
					{
						MEdgeList negEdges, posEdges;
						MFaceList negFaces, posFaces;

						// Edges on negative side: @eNeg0, and @eNeg1
						// Faces on negative side: @fNegative, fNeg0, fNeg1
						negEdges.push_back( eNeg0 );
						negFaces.push_back( fNegative );
						negFaces.push_back( iterNeg.face() );		// fNeg0
						iterNeg.next();
						MEdge *eNeg1 = iterNeg.edge();
						negEdges.push_back( eNeg1 );
						negFaces.push_back( iterNeg.face() );		// fNeg1

						// Edges on positive side: @e and @ePos0
						posEdges.push_back( e );
						posEdges.push_back( ePos0 );

						// Faces on positive side: @fPositive
						posFaces.push_back( fPositive );

						// @eNegative and @ePositive are now @eNeg1 and @e
						eNegative = eNeg1;
						ePositive = e;

						// Create adjustments
						slideEdgeCreateAdjusts( negEdges, posEdges, negFaces, posFaces, adjusts, attribAdjusts );
					}
				}
				else
				{
					gs_assert_not_reached( "Mvertex::slideEdgeRunInternalVertex(): cannot determine successor\n" );
				}
			}
		}
		else
		{
			// Has complex topology
			// OR
			// number of edges != 4  or  number of faces != 4
			// There may be boundary edges in the same neighbourhood as @e
			MFace *fNegative = NULL, *fPositive = NULL;

			if ( eNegative != NULL )
			{
				fNegative = eNegative->getSharedFace( e );
			}

			if ( ePositive != NULL )
			{
				fPositive = ePositive->getSharedFace( e );
			}


			if ( fNegative == NULL )
			{
				fNegative = e->getFaceB();
			}

			if ( fPositive == NULL )
			{
				fPositive = e->getFaceA();
			}



			// Iterators for iterating around each side of @this
			NeighbourhoodIterator iterNegative = neighbourhoodIterator( e, fNegative );
			NeighbourhoodIterator iterPositive = neighbourhoodIterator( e, fPositive );

			// Get the edge that we are trying to reach
			MEdge *targetEdge = getOtherMarkedEdge( e );
			// Lists to keep track of the edges and faces encountered
			MEdgeList posEdges, negEdges;
			MFaceList posFaces, negFaces;
			bool bReachedTargetViaNegative = true, bReachedTargetViaPositive = true;


			// Iterate round the negative side, to the target
			while ( iterNegative.edge() != targetEdge )
			{
				// Record the face
				negFaces.push_back( iterNegative.face() );
				// Iterate
				bool iteratedFlag = iterNegative.next();
				if ( !iteratedFlag )
				{
					// Encountered a boundary edge; could not iterate over it
					bReachedTargetViaNegative = false;
					break;
				}
				// Record the edge
				negEdges.push_back( iterNegative.edge() );
				if ( iterNegative.edge() == e )
				{
					// Reached the start, did not reach that target; this means that @e (the start edge) and @targetEdge are in different neighbourhoods
					bReachedTargetViaNegative = false;
					break;
				}
			}

			// Iterate round the positive side, to the target
			while ( iterPositive.edge() != targetEdge )
			{
				// Record the face
				posFaces.push_back( iterPositive.face() );
				// Iterate
				bool iteratedFlag = iterPositive.next();
				if ( !iteratedFlag )
				{
					// Encountered a boundary edge; could not iterate over it
					bReachedTargetViaPositive = false;
					break;
				}
				// Record the edge
				posEdges.push_back( iterPositive.edge() );
				if ( iterPositive.edge() == e )
				{
					// Reached the start, did not reach that target; this means that @e (the start edge) and @targetEdge are in different neighbourhoods
					bReachedTargetViaPositive = false;
					break;
				}
			}

			if ( bReachedTargetViaNegative  ||  bReachedTargetViaPositive )
			{
				// @negEdges will contain [ en1 ... enn ] where en0 is @e and enn is @targetEdge
				// @posEdges will contain [ ep1 ... epn ] where ep0 is @e and epn is @targetEdge

				// Remove @targetEdge from @negEdges and @posEdges
				if ( bReachedTargetViaNegative )
				{
					negEdges.pop_back();
				}
				else
				{
					negEdges.clear();
					negFaces.clear();
				}

				if ( bReachedTargetViaPositive )
				{
					posEdges.pop_back();
				}
				else
				{
					posEdges.clear();
					posFaces.clear();
				}

				// if @negEdges is empty:
				//		@negFaces will contain the one face that lies between @e and @targetEdge on the negative side
				// else:
				//		@negFaces will contain the faces that surround @negEdges. @negFaces.size == @negEdges.size + 1

				// The same as above, for @posFaces / @posEdges


				// The last edges in @negEdges and @posEdges can be used for the next stage/vertex
				if ( bReachedTargetViaNegative )
				{
					if ( negEdges.size() > 0 )
					{
						eNegative = negEdges.back();
					}
					else
					{
						eNegative = e;
					}
				}
				else
				{
					iterPositive.next();
					eNegative = iterPositive.edge();
				}


				if ( bReachedTargetViaPositive )
				{
					if ( posEdges.size() > 0 )
					{
						ePositive = posEdges.back();
					}
					else
					{
						ePositive = e;
					}
				}
				else
				{
					iterNegative.next();
					ePositive = iterNegative.edge();
				}

				if ( posEdges.size() == 1  &&  negEdges.size() == 1 )
				{
					// 4 edges, 4 faces in neighbourhood
					// @negEdges and @posEdges both contain the edge that lies between @e and @targetEdge on their respective sides
					slideEdgeCreateAdjusts( eNegative, ePositive, negFaces[0], negFaces[1], posFaces[0], posFaces[1], adjusts, attribAdjusts );
				}
				else if ( slideVersion > 0 )
				{
					// VERSION > 0

					if ( bReachedTargetViaNegative )
					{
						if ( negEdges.size() == 0 )
						{
							// @negEdges is empty
							// In order to slide @this, we need to slide along a combination of @e and @targetEdge; add them
							negEdges.push_back( e );
							negEdges.push_back( targetEdge );
							gs_assert( negFaces.size() == 1, "Mvertex::slideEdgeRunInternalVertex(): negative face list should have 1 face, it has %d\n", negFaces.size() );
						}
						else
						{
							gs_assert( negFaces.size() == negEdges.size() + 1, "Mvertex::slideEdgeRunInternalVertex(): negative face list size incorrect\n" );
						}
					}

					if ( bReachedTargetViaPositive )
					{
						if ( posEdges.size() == 0 )
						{
							// @posEdges is empty
							// In order to slide @this, we need to slide along a combination of @e and @targetEdge; add them
							posEdges.push_back( e );
							posEdges.push_back( targetEdge );
							gs_assert( posFaces.size() == 1, "Mvertex::slideEdgeRunInternalVertex(): positive face list should have 1 face, it has %d\n", posFaces.size() );
						}
						else
						{
							gs_assert( posFaces.size() == posEdges.size() + 1, "Mvertex::slideEdgeRunInternalVertex(): positive face list size incorrect\n" );
						}
					}

					// Slide
					if ( negEdges.size() > 0  ||  posEdges.size() > 0 )
					{
						slideEdgeCreateAdjusts( negEdges, posEdges, negFaces, posFaces, adjusts, attribAdjusts );
					}
				}
			}
			else
			{
				// Iteration did not reach the target edge. This was due to encountering boundary edges, or @e and @targetEdge being in different
				// neighbourhoods. Either way, we cannot proceed.
				ePositive = eNegative = NULL;
			}
		}
	}
}





// ******************* MARK EDGE LOOP OPS *******************

MEdge * MVertex::markEdgeLoopGetOppositeEdge(MEdge *e)
{
	gs_assert( e != NULL, "MVertex::markEdgeLoopGetOppositeEdge(): seed edge is NULL\n" );

	if ( isTopologySimple() )
	{
		if ( isOrdinary() )
		{
			//scan the neighbourhood of @this until we get the edge we want
			NeighbourhoodIterator iter = neighbourhoodIterator( e );

			gs_assert( iter.face() != NULL, "MVertex::markEdgeLoopGetOppositeEdge(): could not get face 0\n" );


			bool iterateSuccess = iter.next();
			gs_assert( iterateSuccess, "MVertex::markEdgeLoopGetOppositeEdge(): first iteration failed\n" );
			iterateSuccess = iter.next();
			gs_assert( iterateSuccess, "MVertex::markEdgeLoopGetOppositeEdge(): second iteration failed\n" );

			gs_assert( iter.edge() != NULL, "MVertex::markEdgeLoopGetOppositeEdge(): no edge\n" );

			return iter.edge();
		}
		else if ( isOnOrdinaryBoundary()  &&  e->isBoundary() )
		{
			//find the other boundary edge
			return getOtherBoundaryEdge( e );
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		VertexNeighbourhood neighbourhood;

		MFace *f0 = e->getFaceA();
		gs_assert( f0 != NULL, "MVertex::markEdgeLoopGetOppositeEdge(): could not get initial face for iteration\n" );

		discoverNeighbourhoodFromEdge( e, f0, neighbourhood );

		if ( e->isBoundary() )
		{
			if ( neighbourhood.faces.size() == 2  &&  neighbourhood.edges.size() == 3 )
			{
				return neighbourhood.edges[2];
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			if ( neighbourhood.faces.size() == 4  &&  neighbourhood.edges.size() == 4 )
			{
				return neighbourhood.edges[2];
			}
			else
			{
				return NULL;
			}
		}
	}
}




// ******************* KNIFE OPS *******************

void MVertex::knife()
{
	for (int i = 0; i < faces.size(); i++)
	{
		//for each indicent face that is marked for cutting
		if ( faces[i]->isSecondaryMarked() )
		{
			//add a split vertex
			faces[i]->knifeAddCutVertex( this );
		}
	}

	//mark all incident edges as visited
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->secondaryMark();
	}
}





// ******************* FACE SPLIT OPS *******************

void MVertex::replaceFace(MFace *face, MFace *newFace)
{
	//NOTE: NO VERTICES OR EDGES ARE DESTROYED BY THIS OPERATION
	int faceIndex = findFace( face );

	gs_assert( faceIndex != -1, "MVertex::replaceFace(): could not find @face\n" );

	faces[faceIndex] = newFace;
}




// ******************* REPLACE OPS *******************

void MVertex::replaceWith(MVertex *vNew)
{
	//create replacement edges
	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *edge = edges[i];
		MEdge *replacement;

		//make the replacement edge
		if ( edge->getVertexA() == this )
		{
			replacement = vNew->findEdgeTo( edge->getVertexB(), MFINDPOLICY_CREATE );
		}
		else
		{
			replacement = edge->getVertexA()->findEdgeTo( vNew, MFINDPOLICY_CREATE );
		}

		//copy properties over
		replacement->copyPropertiesFrom( edge );

		edge->destroyIfIsolated( true );
	}



	Array<MFace*> faceList = faces;

	//replace this in all the incident faces
	for (int i = 0; i < faceList.size(); i++)
	{
		faceList[i]->replaceVertex( this, vNew, true, false, true );
	}


	destroyIfIsolated();
}





// ******************* COLLAPSE / WELD OPS *******************

bool MVertex::checkEdgeCollapseValidity(MEdge *e, bool markMergedEdges)
{
	//check for a condition in which an edge collapse may not be performed.
	//an example is shown below

/*
		va						e						  vb
		#-------------------------------------#
		|\_											_/|
		 \ \_										 _/ /
		  \  \_			fa or fb			  _/	/
			\	 \_ 							_/	  /
			 \		\_						 _/	 /
			  \	  \_				  _/		/
				\		 \_			_/		  /
				 \			\_		 _/		 /
	  face 2   \		  \_ _/			/   face 3
					\ face 0  #	 face 1 /
      	 edgeA \			 | X		 / edgeB
      			  \		 |			/
      				\		 |		  /
      				 \		 |		 /
      				  \	 |		/
      					\	 |	  /
      					 \	 |	 /
      					  \ |	/
      						\|/
								 # oppositeVertexA == oppositeVertexB

	face 0, 1, 2, and 3 are distinct faces

	- va and vb are the vertices at either ends of e
	- fa and fb are the faces incident to e
	- edgeA is incident to va (incident faces are face 0 and face 2)
	- edgeB is incident to vb (incident faces are face 1 and face 3)
	- fa or fb are NOT incident to EITHER edgeA or edgeB
	- the vertex at the opposite end of edgeA is oppositeVertexA
	- the vertex at the opposite end of edgeB is oppositeVertexB
	- oppositeVertexA and oppositeVertexB are the same vertex

	This causes a problem because:
	- collapsing e merges va and vb, thus:
	- BOTH edges (edgeA and edgeB) now lead from va (==vb) to
	  oppositeVertexA (==oppositeVertexB); they are effectively the same edge,
	  and since they already have two incident faces each, this would
	  mean creating an edge with 4 incident faces, which is not permitted

	NOTE that edges which are merged into one another (edgeA and edgeB)
	are discovered here, so they are also marked (if desired) at this time
*/

	MVertex *va = e->getVertexA();
	MVertex *vb = e->getVertexB();

	MFace *fa = e->getFaceA();
	MFace *fb = e->getFaceB();


	for (int i = 0; i < va->edges.size(); i++)
	{
		MEdge *edgeA = va->edges[i];
		MVertex *oppositeVertexA = edgeA->getOppositeVertex( va );

		for (int j = 0; j < vb->edges.size(); j++)
		{
			MEdge *edgeB = vb->edges[j];
			MVertex *oppositeVertexB = edgeB->getOppositeVertex( vb );

			if ( oppositeVertexA == oppositeVertexB )
			{
				int totalFaces = edgeA->getNumFaces() + edgeB->getNumFaces();

				//only a potential problem if there are > 2 total faces

				if ( totalFaces > 2 )
				{
					if ( fa != NULL )
					{
						if ( edgeA->isIncidentTo( fa )  &&  edgeB->isIncidentTo( fa ) )
						{
							//fa is incident to e, so it doesn't count
							totalFaces -= 2;
						}
					}
					if ( fb != NULL )
					{
						if ( edgeA->isIncidentTo( fb )  &&  edgeB->isIncidentTo( fb ) )
						{
							//fa is incident to e, so it doesn't count
							totalFaces -= 2;
						}
					}

					//still > 2?
					if ( totalFaces > 2 )
					{
						//yes, cannot collapse
						return false;
					}
				}

				if ( markMergedEdges )
				{
					edgeA->edgeMark();
					edgeB->edgeMark();
				}
			}
		}
	}

	return true;
}

bool MVertex::checkEdgeCollapseValidity(MVertex *va, MVertex *vb, MFace *f)
{
	//check for a condition in which an edge collapse may not be performed.
	//an example is shown below

	// See diagram in MVertex::checkEdgeCollapseValidity() above


	for (int i = 0; i < va->edges.size(); i++)
	{
		MEdge *edgeA = va->edges[i];
		MVertex *oppositeVertexA = edgeA->getOppositeVertex( va );

		for (int j = 0; j < vb->edges.size(); j++)
		{
			MEdge *edgeB = vb->edges[j];
			MVertex *oppositeVertexB = edgeB->getOppositeVertex( vb );

			if ( oppositeVertexA == oppositeVertexB )
			{
				int totalFaces = edgeA->getNumFaces() + edgeB->getNumFaces();

				//only a potential problem if there are > 2 total faces

				if ( totalFaces > 2 )
				{
					if ( f != NULL )
					{
						if ( edgeA->isIncidentTo( f )  &&  edgeB->isIncidentTo( f ) )
						{
							//@f is incident to @e, so it doesn't count
							totalFaces -= 2;
						}
					}

					//still > 2?
					if ( totalFaces > 2 )
					{
						//yes, cannot collapse
						return false;
					}
				}
			}
		}
	}

	return true;
}


MVertex * MVertex::weldTo(MVertex *v, const Point3 &mergedPosition, const MVertex *positionTarget, bool markMergedEdges)
{
	//CHECK VERTICES FOR ISOLATION FIRST
	if ( v->isIsolated() )
	{
		//v is isolated:
		//reposition *this
		setPosition( mergedPosition );

		//destroy v
		v->destroyIfIsolated();

		//result is this
		return this;
	}
	else if ( isIsolated() )
	{
		//*this is isolated:
		//reposition v
		v->setPosition( mergedPosition );

		//destroy this
		destroyIfIsolated();

		//result is v
		return v;
	}

	//NO ISOLATED VERTICES: CONTINUE


	//IF AN EDGE ALREADY EXISTS FROM *this TO v, COLLAPSE IT
	MEdge *edge = findEdgeTo( v, MFINDPOLICY_RETURNNULL );
	if ( edge != NULL )
	{
		return edge->collapseTo( mergedPosition, markMergedEdges, positionTarget );
	}


	//IF *this AND v SHARE A COMMON FACE, SPLIT THE FACE,
	//AND COLLAPSE THE RESULTING EDGE
	MFace *commonFace = findCommonFaceWith( v );
	if ( commonFace != NULL )
	{
		MEdge *edge = NULL;

		//split the face
		commonFace->split( this, v, edge );

		if ( edge != NULL )
		{
			//then, collapse the edge
			return edge->collapseTo( mergedPosition, markMergedEdges, positionTarget );
		}
		else
		{
			return NULL;
		}
	}




	//Note that in the case of the two quads, face 0 and face 1 shown below,
	//*this and v cannot be merged. If face 0 and face 1 were triangles, or if
	//they shared all vertices except *this (for face 0) and v (for face 1),
	//*this and v can be merged if face 0 and face 1 cancel out
	//CANCEL FACES

	//note that *this and v do NOT share a common face, as case this is handled above
	for (int i = 0; i < faces.size(); i++)
	{
		MFace *f0 = faces[i];

		for (int j = 0; j < v->faces.size(); j++)
		{
			MFace *f1 = v->faces[j];

			bool cancelFaces = true;
			for (int k = 0; k < f0->getSize(); k++)
			{
				MVertex *v0 = f0->getVertex( k );

				//if v0 is this, then v0 cannot be shared between f0 and f1
				if ( v0 != this )
				{
					//is v0 incident to f1?
					if ( !f1->isIncidentTo( v0 ) )
					{
						//no, stop scanning, faces cannot be cancelled
						cancelFaces = false;
						break;
					}
				}
			}

			if ( cancelFaces )
			{
				//if cancelFaces is true, then secondary mark (mark the faces for
				//cancelling/destruction)
				f0->secondaryMark();
				f1->secondaryMark();
			}
		}
	}


	//make a list of faces to destroy
	MFaceList facesToDestroy;
	//delete secondary marked faces
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isSecondaryMarked() )
		{
			facesToDestroy.push_back( faces[i] );
		}
	}

	for (int i = 0; i < v->faces.size(); i++)
	{
		if ( v->faces[i]->isSecondaryMarked() )
		{
			facesToDestroy.push_back( v->faces[i] );
		}
	}

	//destroy faces
	for (int i = 0; i < facesToDestroy.size(); i++)
	{
		facesToDestroy[i]->destroy( true, true );
	}


	//make sure *this and v have not been destroyed
	if ( isDestroyed()  ||  v->isDestroyed() )
	{
		return NULL;
	}


	//check for a condition in which a vertex weld may not be performed.
	//an example is shown below

/*
		this											  v
		#                                     #
		|\__										  __/|
		 \	 \__								  __/  /
		  \	 \__						  __/		/
			\		 # 					 #		  /
			 \		  \					/		 /
			  \		\_				 _/		/
				\		  \			/		  /
				 \			\_		 _/		 /
	  face 2   \		  \_ _/			/   face 3
					\ face 0  #	 face 1 /
      	 edgeA \			 | X		 / edgeB
      			  \		 |			/
      				\		 |		  /
      				 \		 |		 /
      				  \	 |		/
      					\	 |	  /
      					 \	 |	 /
      					  \ |	/
      						\|/
								 # oppositeVertexA == oppositeVertexB

	face 0, 1, 2, and 3 are distinct faces

	VERY SIMILAR to checkEdgeCollapseValidity() above, except:

	- edgeA is incident to this (incident faces are face 0 and face 2)
	- edgeB is incident to v (incident faces are face 1 and face 3)
	- the vertex at the opposite end of edgeA is oppositeVertexA
	- the vertex at the opposite end of edgeB is oppositeVertexB
	- oppositeVertexA and oppositeVertexB are the same vertex
	- face 0 and face 1 MAY OR MAY NOT EXIST

	if edgeA or edgeB has TWO incident faces:
	This causes a problem because:
	- welding merges 'this' and v, thus:
	- BOTH edges (edgeA and edgeB) now lead from 'this' (==v) to
	  oppositeVertexA (==oppositeVertexB); they are effectively the same edge,
	  and since they already have two incident faces each, this causes a problem

	if edgeA and edgeB both have ONE incident face:
	- condition: face 0 and face 1 do NOT EXIST
	- this will cause edgeA and edgeB to be merged into one, so:
	- if the vertex ordering of face 2 and face 3 do not match, then
	  trying to merge edgeA and edgeB will cause problems, since the vertex
	  ordering of the edge will not be consistent with that of one of the faces

	NOTE that edges which are merged into one another (edgeA and edgeB)
	are discovered here, so they are also marked (if desired) at this time
*/

	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *edgeA = edges[i];
		MVertex *oppositeVertexA = edgeA->getOppositeVertex( this );

		for (int j = 0; j < v->edges.size(); j++)
		{
			MEdge *edgeB = v->edges[j];
			MVertex *oppositeVertexB = edgeB->getOppositeVertex( v );

			if ( oppositeVertexA == oppositeVertexB )
			{
				int faceCountA = edgeA->getNumFaces();
				int faceCountB = edgeB->getNumFaces();
				if ( faceCountA == 2  ||  faceCountB == 2 )
				{
					//already got 2 faces? if so, weld cannot succeed
					return NULL;
				}
				if ( faceCountA == 1  &&  faceCountB == 1 )
				{
					//face 2 is faceA of edgeA
					//face 3 is faceA of edgeB
					//therefore, for the weld to succeed, the ordering must
					//be consistent:
					//if edgeA->vertexA == oppositeVertexA then edgeB->vertexB == oppositeVertexA
					//or
					//if edgeA->vertexB == oppositeVertexA then edgeB->vertexA == oppositeVertexA
					//ANY OTHER WAY AND THE WELD CANNOT SUCCEED

					if (  edgeA->getVertexA() == oppositeVertexA  &&  edgeB->getVertexA() == oppositeVertexA  )
					{
						return NULL;
					}
					if (  edgeA->getVertexB() == oppositeVertexA  &&  edgeB->getVertexB() == oppositeVertexA  )
					{
						return NULL;
					}
				}

				if ( markMergedEdges )
				{
					edgeA->edgeMark();
					edgeB->edgeMark();
				}
			}
		}
	}

	setPosition( mergedPosition );

	copyPropertiesFrom( this, v );

	v->replaceWith( this );

	return this;
}

MVertex * MVertex::weldTo(MVertex *v, MWeldStyle style, bool markMergedEdges)
{
	switch( style )
	{
	case MWELDSTYLE_A:
		return weldTo( v, getPosition(), this, markMergedEdges );
		break;
	case MWELDSTYLE_B:
		return weldTo( v, v->getPosition(), v, markMergedEdges );
		break;
	case MWELDSTYLE_MIDPOINT:
		return weldTo( v, Point3::average( getPosition(), v->getPosition() ), NULL, markMergedEdges );
		break;
	default:
		return weldTo( v, getPosition(), this, markMergedEdges );
		break;
	}
	return NULL;
}

MVertex * MVertex::weldTo(MVertex *v, bool markMergedEdges)
{
	return weldTo( v, v->getPosition(), v, markMergedEdges );
}

MVertex * MVertex::weldToMidPointWith(MVertex *v, bool markMergedEdges)
{
	return weldTo( v, Point3::average( getPosition(), v->getPosition() ), NULL, markMergedEdges );
}

bool MVertex::canWeldTo(MVertex *v)
{
	// Weld can proceed if either vertex is isolated
	if ( v->isIsolated()  ||  isIsolated() )
	{
		return true;
	}


	// If an edge links @this to @v, then it would be collapsed, so call MEdge::canCollapse()
	MEdge *edge = findEdgeTo( v, MFINDPOLICY_RETURNNULL );
	if ( edge != NULL )
	{
		return edge->canCollapse();
	}


	// If @this and @v share a common face, the face will be split, and the resulting edge collapsed.
	MFace *commonFace = findCommonFaceWith( v );
	if ( commonFace != NULL )
	{
		// If there are any other common faces between @this and @v, then the weld cannot proceed
		int numCommonFaces = getNumCommonFacesWith( v );

		gs_assert( numCommonFaces != 0, "MVertex::canWeldTo(): @numCommonFaces is 0\n" );

		if ( numCommonFaces > 1 )
		{
			return false;
		}


		// Check the validity of collapsing an edge from @this to @v
		//NOTE:
		//depending upon the surrounding mesh, collapsing this may
		//result in an invalid mesh, in which case a collapse is not possible
		if ( !MVertex::checkEdgeCollapseValidity( this, v, commonFace ) )
		{
			return false;
		}


		//
		// CHECK TO SEE IF REMOVING VERTICES IS POSSIBLE.
		//
		// See diagram in MEdge::collapse()
		//

		int vtxAIndex = commonFace->findVertex( this );
		int vtxBIndex = commonFace->findVertex( v );
		int prevAVertexIndex = prevIndex( vtxAIndex, commonFace->getSize() );
		int prevBVertexIndex = prevIndex( vtxBIndex, commonFace->getSize() );
		int nextAVertexIndex = nextIndex( vtxAIndex, commonFace->getSize() );
		int nextBVertexIndex = nextIndex( vtxBIndex, commonFace->getSize() );

		MVertex *prevAVertex = commonFace->getVertex( prevAVertexIndex );
		MVertex *prevBVertex = commonFace->getVertex( prevBVertexIndex );
		MVertex *nextAVertex = commonFace->getVertex( nextAVertexIndex );
		MVertex *nextBVertex = commonFace->getVertex( nextBVertexIndex );

		bool canRemoveFromFace0 = true, canRemoveFromFace1 = true;

		if ( prevAVertex != nextBVertex )
		{
			// The portion of @commonFace starting at @this, passing through @prevAVertex and @nextBVertex to @v
			// is not a triangle
			MEdge *e0 = prevAVertex->findEdgeTo( v, MFINDPOLICY_RETURNNULL );
			MEdge *e1 = nextBVertex->findEdgeTo( this, MFINDPOLICY_RETURNNULL );

			if ( e0 != NULL )
			{
				if ( e0->getNumFaces() != 2 )
				{
					canRemoveFromFace0 = false;
				}
			}

			if ( e1 != NULL )
			{
				if ( e1->getNumFaces() != 2 )
				{
					canRemoveFromFace0 = false;
				}
			}
		}

		if ( prevBVertex != nextAVertex )
		{
			// The portion of @commonFace starting at @this, passing through @prevBVertex and @nextAVertex to @v
			// is not a triangle
			MEdge *e0 = prevBVertex->findEdgeTo( v, MFINDPOLICY_RETURNNULL );
			MEdge *e1 = nextAVertex->findEdgeTo( this, MFINDPOLICY_RETURNNULL );

			if ( e0 != NULL )
			{
				if ( e0->getNumFaces() != 2 )
				{
					canRemoveFromFace1 = false;
				}
			}

			if ( e1 != NULL )
			{
				if ( e1->getNumFaces() != 2 )
				{
					canRemoveFromFace1 = false;
				}
			}
		}


		return canRemoveFromFace0  ||  canRemoveFromFace1;
	}




	//check for a condition in which a vertex weld may not be performed.
	//an example is shown below

/*
		this											  v
		#                                     #
		|\__										  __/|
		 \	 \__								  __/  /
		  \	 \__						  __/		/
			\		 # 					 #		  /
			 \		  \					/		 /
			  \		\_				 _/		/
				\		  \			/		  /
				 \			\_		 _/		 /
	  face 2   \		  \_ _/			/   face 3
					\ face 0  #	 face 1 /
      	 edgeA \			 | X		 / edgeB
      			  \		 |			/
      				\		 |		  /
      				 \		 |		 /
      				  \	 |		/
      					\	 |	  /
      					 \	 |	 /
      					  \ |	/
      						\|/
								 # oppositeVertexA == oppositeVertexB

	face 0, 1, 2, and 3 are distinct faces

	VERY SIMILAR to checkEdgeCollapseValidity() above, except:

	- edgeA is incident to this (incident faces are face 0 and face 2)
	- edgeB is incident to v (incident faces are face 1 and face 3)
	- the vertex at the opposite end of edgeA is oppositeVertexA
	- the vertex at the opposite end of edgeB is oppositeVertexB
	- oppositeVertexA and oppositeVertexB are the same vertex
	- face 0 and face 1 MAY OR MAY NOT EXIST

	if edgeA or edgeB has TWO incident faces:
	This causes a problem because:
	- welding merges 'this' and v, thus:
	- BOTH edges (edgeA and edgeB) now lead from 'this' (==v) to
	  oppositeVertexA (==oppositeVertexB); they are effectively the same edge,
	  and since they already have two incident faces each, this causes a problem

	if edgeA and edgeB both have ONE incident face:
	- condition: face 0 and face 1 do NOT EXIST
	- this will cause edgeA and edgeB to be merged into one, so:
	- if the vertex ordering of face 2 and face 3 do not match, then
	  trying to merge edgeA and edgeB will cause problems, since the vertex
	  ordering of the edge will not be consistent with that of one of the faces

	NOTE that edges which are merged into one another (edgeA and edgeB)
	are discovered here, so they are also marked (if desired) at this time
*/

	for (int i = 0; i < edges.size(); i++)
	{
		MEdge *edgeA = edges[i];
		MVertex *oppositeVertexA = edgeA->getOppositeVertex( this );

		for (int j = 0; j < v->edges.size(); j++)
		{
			MEdge *edgeB = v->edges[j];
			MVertex *oppositeVertexB = edgeB->getOppositeVertex( v );

			if ( oppositeVertexA == oppositeVertexB )
			{
				int faceCountA = edgeA->getNumFaces();
				int faceCountB = edgeB->getNumFaces();
				if ( faceCountA == 2  ||  faceCountB == 2 )
				{
					//already got 2 faces? if so, weld cannot succeed
					return false;
				}
				if ( faceCountA == 1  &&  faceCountB == 1 )
				{
					//face 2 is faceA of edgeA
					//face 3 is faceA of edgeB
					//therefore, for the weld to succeed, the ordering must
					//be consistent:
					//if edgeA->vertexA == oppositeVertexA then
					//									edgeB->vertexB == oppositeVertexA
					//or
					//if edgeA->vertexB == oppositeVertexA then
					//									edgeB->vertexA == oppositeVertexA
					//ANY OTHER WAY AND THE WELD CANNOT SUCCEED

					if (  edgeA->getVertexA() == oppositeVertexA  &&  edgeB->getVertexA() == oppositeVertexA  )
					{
						return false;
					}
					if (  edgeA->getVertexB() == oppositeVertexA  &&  edgeB->getVertexB() == oppositeVertexA  )
					{
						return false;
					}
				}
			}
		}
	}


	return true;
}

bool MVertex::canWeldTo(MEdge *e)
{
	// Cannot weld if @this is incident to @e
	if ( e->isIncidentTo( this ) )
	{
		return false;
	}

	// Weld can proceed if this vertex is isolated or the edge @e is isolated
	if ( e->isIsolated()  ||  isIsolated() )
	{
		return true;
	}


	// If @this and @e share a common face, the face will be split, and the resulting edge collapsed.
	bool incidentToFaceA = false, incidentToFaceB = false;
	if ( e->getFaceA() != NULL )
	{
		incidentToFaceA = e->getFaceA()->isIncidentTo( this );
	}
	if ( e->getFaceB() != NULL )
	{
		incidentToFaceB = e->getFaceB()->isIncidentTo( this );
	}
	if ( incidentToFaceA || incidentToFaceB )
	{
		// If both face A and face B are incident to @this, then the weld cannot proceed
		if ( incidentToFaceA && incidentToFaceB )
		{
			return false;
		}
		else
		{
			// The edge collapse validity check performed by MVertex::checkEdgeCollapseValidity() is not necessary here, as
			// the second vertex / target vertex is a result of splitting the target edge. Its two incident edges are the
			// two sections of the target edge; therefore the situation where the vertices being merged are incident to edges
			// which link to a common vertex cannot arise unless the common vertex is incident to @commonFace, which would
			// be destroyed due to it being triangular, and having an edge collapsed.
			return true;
		}
	}



	// The normal vertex-to-vertex weld validity check is omitted for similar reasons as stated above in the comment explaining
	// the omission of an edge collapse validity check.

	return true;
}

bool MVertex::isLinkedTo(MVertex *v)
{
	if ( findEdgeTo( v, MFINDPOLICY_RETURNNULL )  != NULL )
	{
		return true;
	}

	return findCommonFaceWith( v ) != NULL;
}



// ******************* DISSOLVE OPS *******************

bool MVertex::dissolve(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces)
{
	//discover the neighbourhoods around @this and work on each one separately
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllNeighbourhoods( neighbourhoods );

	Array<MVertex*> neighbouringVertices;
	Array<MVertexAttrib> neighbouringAttribs;

	bool successFlag = true;

	for (int i = 0; i < neighbourhoods.size(); i++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[i];

		if ( twoEdgesOnly )
		{
			//only dissolve when there are 2 edges in this neighbourhood:
			//if this isn't the case, skip it:
			if ( neighbourhood.edges.size() != 2 )
			{
				continue;
			}
		}

		if ( twoFacesOnly )
		{
			//only dissolve when there are 2 faces in this neighbourhood:
			//if this isn't the case, skip it:
			if ( neighbourhood.faces.size() != 2 )
			{
				continue;
			}
		}


		/*
		The following situation will cause a problem:
					* va (@this)							* va (@this)
		FRONT   / \								BACK	  / \
				 / fa\									 /	fa\
				/	e1	\									/		\
		  vb *-------* vc						  vc * - - - * vb
			 / \		/ \							 /	\		/ \
			|   \	fb/	|							|	 \ fb/	|
		  / fc  \ /		 \						  / fc  \ /		 \
		 |	     	* vd    |					 |			* vd	  |
		/ ve				vf	\					/ vf				ve	\
		*-----------------*					*-----------------*

		DESCRIPTION:
		@fa is a triangular face with vertices @va,@vb,@vc
		@fb is a quadrilateral face with vertices @va,@vb,@vd,@vc
		@fc is a quadrilateral face with vertices @vb,@vc,@vf,@ve

		Edge @e1 is incident to @fa and @fc, but not @fb.

		PROBLEM:
		If vertex @va is dissolved then @va must be 'removed' from each of its
		currently incident faces. If it is removed from @fb first, then @fb
		attempts to link to @vb,@vc,@vd, and hence edge @e1. Since @e1 already
		has two incident faces, this fails

		SOLUTION:
		This problem will occur in two cases:
		1. Where a triangular face @f is incident to @this and @e1
		2. Where @e1 is incident to two faces that are not incident to
		   @this.
		To solve case 1, process all the triangular faces first; the
		face will be destroyed, this removing it from @e1, leaving it with
		only one incident face, allowing another to be added later.
		Case 2 cannot be solved, so quit if it is detected.
		*/

		//CHECK IF THIS TRAVERSAL CAN BE DISSOLVED
		//Compute the number of incident faces that would be attached
		//to each edge that is created (edges link the opposing ends of
		//two adjacent edges in the neighbourhood list)
		bool dissolvePossibleFlag = true;
		if ( neighbourhood.faces.size() == 2  &&  neighbourhood.isClosed() )
		{
			//two faces in a CLOSED neighbourhood is a special case

			//get the edge @e that will be created
			MVertex *va = neighbourhood.edges[0]->getOppositeVertex( this );
			MVertex *vb = neighbourhood.edges[1]->getOppositeVertex( this );
			MEdge *e = va->findEdgeTo( vb, MFINDPOLICY_RETURNNULL );
			int edgeFaceCount;
			if ( e == NULL )
			{
				edgeFaceCount = 0;
			}
			else
			{
				edgeFaceCount = e->getNumFaces();
			}

			//take faces incident to @this into account
			for (int j = 0; j < neighbourhood.faces.size(); j++)
			{
				MFace *f = neighbourhood.faces[j];
				if ( f->getSize() == 3 )
				{
					edgeFaceCount--;
				}
				if ( f->getSize() >= 4 )
				{
					edgeFaceCount++;
				}
			}

			//only two edges in neighbourhood; no face will be created in place
			//of @this

			//dissolve is impossible if @edgeFaceCount > 2
			if ( edgeFaceCount > 2 )
			{
				dissolvePossibleFlag = false;
			}
		}
		else
		{
			for (int j = 0; j < neighbourhood.faces.size(); j++)
			{
				int jNext = nextIndex( j, neighbourhood.edges.size() );
				MVertex *va = neighbourhood.edges[j]->getOppositeVertex( this );
				MVertex *vb = neighbourhood.edges[jNext]->getOppositeVertex( this );
				MEdge *e = va->findEdgeTo( vb, MFINDPOLICY_RETURNNULL );
				MFace *f = neighbourhood.faces[j];
				int edgeFaceCount;
				if ( e == NULL )
				{
					edgeFaceCount = 0;
				}
				else
				{
					edgeFaceCount = e->getNumFaces();
				}

				//now, decement the count if face @f is a triangle, since these
				//will be destroyed first
				if ( f->getSize() == 3 )
				{
					edgeFaceCount--;
				}

				//increment it if face @f has >= 4 vertices, since when @this is
				//removed from face @f, it will be attached to edge @e
				if ( f->getSize() >= 4 )
				{
					edgeFaceCount++;
				}

				//if the number of edges in the neighbourhood is >= 3 then a
				//face will be created in place of the dissolved vertex, so
				//increment @edgeFaceCount
				if ( neighbourhood.edges.size() >= 3 )
				{
					edgeFaceCount++;
				}

				//dissolve is impossible if @edgeFaceCount > 2
				if ( edgeFaceCount > 2 )
				{
					dissolvePossibleFlag = false;
					successFlag = false;
				}
			}
		}


		//proceed if dissolve is possible
		if ( dissolvePossibleFlag )
		{
			//the face created by dissolving @this must use its incident edges
			//in the opposing order to that of its neighbouring faces, hence
			//flip the vertices, if the order of @neighbourhood is consistent
			bool flipFlag = neighbourhood.isOrderConsistent();

			//get the material from the first face
			int materialID = neighbourhood.faces[0]->getMaterialID();

			//generate a list of neighbouring vertices (vertices on opposite ends
			//of edges incident to @this) and vertex attributes
			neighbouringVertices.clear();
			neighbouringAttribs.clear();
			neighbouringVertices.reserve( neighbourhood.edges.size() );
			neighbouringAttribs.reserve( neighbourhood.edges.size() );

			for (int j = 0; j < neighbourhood.edges.size(); j++)
			{
				MEdge *e = neighbourhood.edges[j];
				Point2f midPoint;

				MVertex *opposite = e->getOppositeVertex( this );
				//get the two vertex attribs that are used by the faces indicent
				//to @e, at the vertex @opposite
				MVertexAttrib *attribs[2];
				e->getVertexAttribs( opposite, attribs[0], attribs[1] );

				if ( e->isBoundary() )
				{
					//@e is boundary; only one attrib available
					midPoint = attribs[0]->getPoint();
				}
				else
				{
					//compute the average texture co-ordinate
					midPoint = Point2f::average( attribs[0]->getPoint(), attribs[1]->getPoint() );
				}

				neighbouringVertices.push_back( e->getOppositeVertex( this ) );
				neighbouringAttribs.push_back( MVertexAttrib( midPoint, materialID ) );
			}


			//destroy all incident triangular faces first
			for (int j = 0; j < neighbourhood.faces.size(); j++)
			{
				MFace *f = neighbourhood.faces[j];

				if ( f->getSize() == 3 )
				{
					f->destroy( true, true );
					//NULL the entry in the neighbourhood
					neighbourhood.faces[j] = NULL;
				}
			}

			//remove @this from each incident face
			for (int j = 0; j < neighbourhood.faces.size(); j++)
			{
				MFace *f = neighbourhood.faces[j];

				//if f was not NULLed by the triangle destroy phase
				if ( f != NULL )
				{
					//remove @this from @f, destroying any vertices or edges that
					//become unused as a result
					f->removeVertex( this, true, true );
				}
			}


			//create a face f to replace @this ONLY if the number of vertices >= 3
			if ( neighbouringVertices.size() >= 3 )
			{
				if ( flipFlag )
				{
					neighbouringVertices.reverse();
					neighbouringAttribs.reverse();
				}

				MFace *f = getMesh()->addFace( neighbouringVertices, neighbouringAttribs, materialID );

				if ( markCreatedFaces )
				{
					f->faceMark();
				}
			}
		}
	}

	return successFlag;
}


bool MVertex::dissolveSimpleValenceTwoVertex()
{
	if ( getValence() == 2 )
	{
		if ( getNumFaces() == 2 )
		{
			MFace *f0 = faces[0];
			MFace *f1 = faces[1];

			// Attempt to remove @this from @f0
			bool removeFromF0Flag = f0->checkRemoveVertex( this );
			if ( removeFromF0Flag )
			{
				f0->removeVertex( this, true, true );
			}

			// Attempt to remove @this from @f1
			bool removeFromF1Flag = f1->checkRemoveVertex( this );
			if ( removeFromF1Flag )
			{
				f1->removeVertex( this, true, true );
			}

			if ( !removeFromF0Flag  &&  removeFromF1Flag )
			{
				// If the attempt to remove @this from @f0 failed, and the attempt to remove @this from @f1 succeeded, then
				// that removal from @f1 may have changed the topology so that the removal of @this from @f0 is now possible.
				removeFromF0Flag = f0->checkRemoveVertex( this );
				if ( removeFromF0Flag )
				{
					f0->removeVertex( this, true, true );
				}
			}

			return removeFromF0Flag  &&  removeFromF1Flag;
		}
		else if ( getNumFaces() == 1  &&  edges[0]->isBoundary()  &&  edges[1]->isBoundary() )
		{
			MFace *f0 = faces[0];

			// Attempt to remove @this from @f0
			bool removeFromF0Flag = f0->checkRemoveVertex( this );
			if ( removeFromF0Flag )
			{
				f0->removeVertex( this, true, true );
			}

			return removeFromF0Flag;
		}
	}

	return false;
}


// ******************* TWEAK OPS *******************

void MVertex::vertexTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector)
{
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllNeighbourhoods( neighbourhoods );

	adjust = MVertexSurfaceTweakAdjust( this, neighbourhoods.size() );

	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];
		if ( !neighbourhood.isCounterClockwise() )
		{
			neighbourhood.flip();
		}

		adjust.initNeighbourhood( neighbourhoodI, neighbourhood.faces.size() );

		Vector3 u = neighbourhood.edges[0]->getEdgeVector( this );
		int numEdges = neighbourhood.edges.size();
		for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
		{
			Vector3 v = neighbourhood.edges[ nextIndex( faceI, numEdges ) ]->getEdgeVector( this );
			adjust.addFace( neighbourhoodI, faceI, neighbourhood.faces[faceI], u, v, true, viewVector );
			u = v;
		}
	}

	adjust.build( viewVector );
}

void MVertex::edgeTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector)
{
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllNeighbourhoods( neighbourhoods );

	adjust = MVertexSurfaceTweakAdjust( this, neighbourhoods.size() );

	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];
		if ( !neighbourhood.isCounterClockwise() )
		{
			neighbourhood.flip();
		}

		adjust.initNeighbourhood( neighbourhoodI, neighbourhood.faces.size() );

		for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
		{
			int nextI = nextIndex( faceI, neighbourhood.edges.size() );
			MFace *f = neighbourhood.faces[faceI];
			MEdge *e = neighbourhood.edges[faceI];
			adjust.addFace( neighbourhoodI, faceI, f,
						neighbourhood.edges[faceI]->getEdgeVector( this ),
						neighbourhood.edges[ nextIndex( faceI, neighbourhood.edges.size() ) ]->getEdgeVector( this ),
						e->isEdgeMarked()  ||  neighbourhood.edges[nextI]->isEdgeMarked(),
						viewVector );
		}
	}

	adjust.build( viewVector );
}

void MVertex::faceTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector)
{
	Array<VertexNeighbourhood> neighbourhoods;
	discoverAllNeighbourhoods( neighbourhoods );

	adjust = MVertexSurfaceTweakAdjust( this, neighbourhoods.size() );

	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		VertexNeighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];
		if ( !neighbourhood.isCounterClockwise() )
		{
			neighbourhood.flip();
		}

		adjust.initNeighbourhood( neighbourhoodI, neighbourhood.faces.size() );

		for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
		{
			MFace *f = neighbourhood.faces[faceI];
			adjust.addFace( neighbourhoodI, faceI, f,
						neighbourhood.edges[faceI]->getEdgeVector( this ),
						neighbourhood.edges[ nextIndex( faceI, neighbourhood.edges.size() ) ]->getEdgeVector( this ),
						f->isFaceMarked(),
						viewVector );
		}
	}

	adjust.build( viewVector );
}


void MVertex::vertexTweakAlongNormalAdjustable(MVertexAdjust &adjust)
{
	Vector3 tweakVector;
	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		tweakVector += faces[faceI]->getPlane().n;
	}
	if ( tweakVector.sqrLength()  >  SQR_EPSILON )
	{
		tweakVector.normalise();
	}
	adjust = MVertexAdjust( this, this->getPosition(), tweakVector );
}

void MVertex::edgeTweakAlongNormalAdjustable(MVertexAdjust &adjust)
{
	Vector3 tweakVector;
	for (int edgeI = 0; edgeI < edges.size(); edgeI++)
	{
		if ( edges[edgeI]->isEdgeMarked() )
		{
			tweakVector += edges[edgeI]->getNormal();
		}
	}
	if ( tweakVector.sqrLength()  >  SQR_EPSILON )
	{
		tweakVector.normalise();
	}
	adjust = MVertexAdjust( this, this->getPosition(), tweakVector );
}

void MVertex::faceTweakAlongNormalAdjustable(MVertexAdjust &adjust)
{
	Vector3 tweakVector;
	for (int faceI = 0; faceI < faces.size(); faceI++)
	{
		if ( faces[faceI]->isFaceMarked() )
		{
			tweakVector += faces[faceI]->getPlane().n;
		}
	}
	if ( tweakVector.sqrLength()  >  SQR_EPSILON )
	{
		tweakVector.normalise();
	}
	adjust = MVertexAdjust( this, this->getPosition(), tweakVector );
}





// ******************* BOUNDARY / MARK OPS *******************

bool MVertex::isFrontFacing(const BackfaceCull *faceCull) const
{
	for (int i = 0; i < faces.size(); i++)
	{
		if ( faces[i]->isFrontFace( faceCull ) )
		{
			return true;
		}
	}

	return false;
}

bool MVertex::isInside(const Region3d *region) const
{
	return region->contains( getPosition() );
}

bool MVertex::isInside(const Region3d *region, const BackfaceCull *faceCull) const
{
	if ( isFrontFacing( faceCull ) )
	{
		return isInside( region );
	}
	else
	{
		return false;
	}
}




// ******************* RELAX OPS *******************

Point3 MVertex::computeRelaxedPosition() const
{
	//boundary count is a multiple of 2 by definition
	int boundaryCount = computeBoundaryCount();

	if ( boundaryCount == 0 )
	{
		//use smooth rule
		Point3 smooth;
		double n = (double)faces.size();

		//average of positions of face vertices
		for (int i = 0; i < faces.size(); i++)
		{
			smooth.cumulativeAdd( faces[i]->computeCentroid() );
		}

		//average of opposite edge endpoints
		for (int i = 0; i < edges.size(); i++)
		{
			smooth.cumulativeAdd( edges[i]->getOppositeVertex( this )->getPosition() );
		}

		//combine the edge and face average positions, compute above
		double oneOverN = 1.0 / n;
		double oneOverNSquared = oneOverN * oneOverN;
		double nMinus2TimesOneOverN = ( n - 2 ) * oneOverN;
		smooth.scale( oneOverNSquared );
		smooth.cumulativeAdd( Point3::mul( position, nMinus2TimesOneOverN ) );
		return smooth;
	}
	else if ( boundaryCount == 2 )
	{
		Point3 smooth;
		//average of the subdivision (odd) vertices of the incident edges
		int count = 0;
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isBoundary() )
			{
				smooth.cumulativeAdd( Point3::average( position, edges[i]->getOppositeVertex( this )->getPosition() ) );
				count++;
			}
		}

		double oneOverCount = 1.0 / (double)count;
		smooth.scale( oneOverCount );

		//compute the mean of the average and the original position
		smooth = Point3::average( position, smooth );
		return smooth;
	}
	else
	{
		return position;
	}
}


// ******************* MATERIAL OPS *******************

void MVertex::uvmapInitialise()
{
	if ( isOnFaceMarkBoundary()  ||  vertexNormals.size() > 1 )
	{
		vertexOpData.uvmap.averageNormal = new Vector3f();

		for (int i = 0; i < faces.size(); i++)
		{
			if ( faces[i]->isFaceMarked() )
			{
				*vertexOpData.uvmap.averageNormal += Vector3f( faces[i]->getPlane().n );
			}
		}

		vertexOpData.uvmap.averageNormal->normalise();
	}
	else
	{
		vertexOpData.uvmap.averageNormal = NULL;
	}
}

void MVertex::uvmapShutdown()
{
	if ( vertexOpData.uvmap.averageNormal != NULL )
	{
		delete vertexOpData.uvmap.averageNormal;
		vertexOpData.uvmap.averageNormal = NULL;
	}
}

const Vector3f& MVertex::getUVMapNormal() const
{
	if ( vertexOpData.uvmap.averageNormal != NULL )
	{
		return *vertexOpData.uvmap.averageNormal;
	}
	else
	{
		gs_assert( !vertexNormals.isEmpty(), "MVertex::getUVMapNormal(): @vertexNormals is empty\n" );

		return vertexNormals[0]->getNormal();
	}
}





// ******************* DEBUG OPS *******************

void MVertex::display()
{
	printf( "edges (%d): ", edges.size() );
	for (int i = 0; i < edges.size(); i++)
	{
		printf( "%p ", edges[i] );
	}
	printf( "\n" );

	printf( "faces (%d): ", faces.size() );
	for (int i = 0; i < faces.size(); i++)
	{
		printf( "%p ", faces[i] );
	}
	printf( "\n" );
}

bool MVertex::checkIntegrity()
{
	bool passed = true;

	if ( !isDestroyed() )
	{
		for (int i = 0; i < edges.size(); i++)
		{
			MVertex *vb = edges[i]->getOppositeVertex( this );
			if ( vb == NULL )
			{
				printf("MVertex %p: edges[%d]->getOppositeVertex() == NULL\n", this, i );
				passed = false;
			}
			else
			{
				if ( !vb->_hasEdge( edges[i] ) )
				{
					printf( "MVertex %p: !edges[%d]->getOppositeVertex()->_hasEdge(edges[%d])\n", this, i, i );
					passed = false;
				}
				if ( edges[i]->getOppositeVertex(vb) != this )
				{
					printf( "MVertex %p: edges[%d]->getOppositeVertex(oppositeVertex) != this\n", this, i );
					passed = false;
				}
			}
		}

		bool attribError = false;
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			MVertexAttrib *attrib = vertexAttribs[i];

			int count = 0;

			for (int j = 0; j < faces.size(); j++)
			{
				if ( faces[j]->getVertexAttrib( this ) == attrib )
				{
					count++;
				}
			}

			if ( count != attrib->getRefCount() )
			{
				printf( "MVertex %p: vertex attrib %p has incorrect ref count, is %d, should be %d\n", this, attrib, attrib->getRefCount(), count );
				attribError = true;
				passed = false;
			}

			if ( attrib->getVertex() != this )
			{
				printf( "MVertex %p: vertexAttribs[%d]->getVertex() != this\n", this, i );
				attribError = true;
				passed = false;
			}
		}

		if ( attribError )
		{
			printf( "MVertex %p: attribs: ", this );
			for (int i = 0; i < vertexAttribs.size(); i++)
			{
				printf( "%p ", vertexAttribs[i] );
			}
			printf( "\n" );
		}

		if ( !flags.normalsRequireRefresh )
		{
			for (int i = 0; i < vertexNormals.size(); i++)
			{
				MVertexNormal *normal = vertexNormals[i];

				if ( normal->index < 0 )
				{
					printf( "MVertex %d(%p): vertex normal %d has index < 0 (%d)\n", this->index, this, i, normal->index );
					passed = false;
				}
			}
		}

		for (int i = 0; i < faces.size(); i++)
		{
			MFace *f = faces[i];
			if ( !f->isIncidentTo(this) )
			{
				printf( "MVertex %p: !faces[%d]->isIncidentTo(this)\n", this, i );
				passed = false;
			}
		}

		if ( faces.size() == 0 )
		{
			printf( "MVertex %p: has no faces\n", this );
			passed = false;
		}

		if ( edges.size() == 0 )
		{
			printf( "MVertex %p: has no edges\n", this );
			passed = false;
		}

		if ( flags.indestructible )
		{
			printf( "MVertex %p is indestructible\n", this );
			passed = false;
		}

		if ( flags.secondaryMark )
		{
			printf( "MVertex %p is secondary marked\n", this );
			passed = false;
		}
	}


	return passed;
}





/****************************************************************************
 ****************************************************************************
											MVertexAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexAdjustList::MVertexAdjustList()
{
}

MVertexAdjustList::~MVertexAdjustList()
{
}


void MVertexAdjustList::setParameters(double alpha, double beta)
{
	for (int i = 0; i < adjustments.size(); i++)
	{
		adjustments[i].setParameters( alpha, beta );
	}
}




/****************************************************************************
 ****************************************************************************
										MVertexVectorAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexVectorAdjustList::MVertexVectorAdjustList()
{
}

MVertexVectorAdjustList::~MVertexVectorAdjustList()
{
}


void MVertexVectorAdjustList::setParameters(const Vector3 &v)
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].setParameters( v );
	}
}




/****************************************************************************
 ****************************************************************************
							MVertexSlideAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexSlideAdjustList::MVertexSlideAdjustList()
{
}

MVertexSlideAdjustList::~MVertexSlideAdjustList()
{
}


void MVertexSlideAdjustList::setSlideFactor(double slide)
{
	for (int i = 0; i < adjustments.size(); i++)
	{
		adjustments[i].setSlideFactor( slide );
	}
}




/****************************************************************************
 ****************************************************************************
					MVertexSurfaceTweakAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexSurfaceTweakAdjust::InputFace::InputFace() : faceIndex( -1 )
{
}

MVertexSurfaceTweakAdjust::InputFace::InputFace(int faceIndex, const Vector3 &u, const Vector3 &v)
										: u( u ), v( v ), faceIndex( faceIndex )
{
	planeNormal = u.cross( v );

	gs_assert( planeNormal.sqrLength()  >=  u.sqrLength() * v.sqrLength() * SQR_EPSILON, "MVertexSurfaceTweakAdjust::InputFace::InputFace(): could not compute normal\n" );

	planeNormal.normalise();
}

MVertexSurfaceTweakAdjust::InputFace::InputFace(int faceIndex, const Vector3 &u, const Vector3 &v, const Vector3 &planeNormal)
										: u( u ), v( v ), planeNormal( planeNormal ), faceIndex( faceIndex )
{
}


bool MVertexSurfaceTweakAdjust::InputFace::isPreviousTo(const InputFace &face, int neighbourhoodSize) const
{
	return faceIndex  ==  prevIndex( face.faceIndex, neighbourhoodSize );
}

bool MVertexSurfaceTweakAdjust::InputFace::isFrontFacing(const Vector3 &viewVector) const
{
	return planeNormal.dot( viewVector )  <  0.0;
}




MVertexSurfaceTweakAdjust::FaceEntry::FaceEntry()
{
}

MVertexSurfaceTweakAdjust::FaceEntry::FaceEntry(const Vector3 &planeNormal, const Vector3 &u, const Vector3 &v, const Vector3 &viewVector)
										: planeNormal( planeNormal ), u( u ), v( v )
{
	inBoundaryNormal = u.cross( viewVector );
	outBoundaryNormal = viewVector.cross( v );
}



MVertexSurfaceTweakAdjust::MVertexSurfaceTweakAdjust() : vertex( NULL )
{
}

MVertexSurfaceTweakAdjust::MVertexSurfaceTweakAdjust(MVertex *vertex, int numNeighbourhoods)
											: initialPosition( vertex->getPosition() ), vertex( vertex )
{
	neighbourhoods.resize( numNeighbourhoods );
}


void MVertexSurfaceTweakAdjust::initNeighbourhood(int neighbourhoodIndex, int vertexNeighbourhoodSize)
{
	neighbourhoods[neighbourhoodIndex].vertexNeighbourhoodSize = vertexNeighbourhoodSize;
}


void MVertexSurfaceTweakAdjust::addInputFace(int neighbourhoodIndex, const InputFace &face, const Vector3 &viewVector)
{
	if ( face.isFrontFacing( viewVector ) )
	{
		neighbourhoods[neighbourhoodIndex].inputFaces.push_back( face );
	}
}

void MVertexSurfaceTweakAdjust::addFace(int neighbourhoodIndex, int faceIndex, const MFace *face, Vector3 u, Vector3 v,
								bool bUse, const Vector3 &viewVector)
{
	if ( bUse )
	{
		// Compute the error bounds for this face
		double faceSize = face->computeBBox().getMajorSize();
		double faceSqrEpsilon = faceSize * faceSize * SQR_EPSILON;

		// Ensure that the @u and @v vectors are long enough
		double uSqrLength = u.sqrLength();
		double vSqrLength = v.sqrLength();
		if ( uSqrLength  >=  faceSqrEpsilon  &&  vSqrLength  >=  faceSqrEpsilon )
		{
			// Normalise @u and @v
			u.normalise();
			v.normalise();

			// Compute the normal of the plane that lies on @u and @v
			Vector3 planeNormal = u.cross( v );

			if ( planeNormal.sqrLength()  >  uSqrLength * vSqrLength * SQR_EPSILON )
			{
				planeNormal.normalise();
				if ( planeNormal.dot( face->getPlane().n )  >  0.0 )
				{
					// Convex corner; add one face
					addInputFace( neighbourhoodIndex, InputFace( faceIndex, u, v, planeNormal ), viewVector );
				}
				else
				{
					// Concave corner; split into two faces
					Vector3 mid = -( u + v ).getNormalised();
					addInputFace( neighbourhoodIndex, InputFace( faceIndex, u, mid ), viewVector );
					addInputFace( neighbourhoodIndex, InputFace( faceIndex, mid, v ), viewVector );
				}
			}
			else
			{
				Vector3 p = face->getPlane().n.cross( u ).getNormalised();

				if ( u.dot( v )  <  0.0 )
				{
					// Straight corner; split into two faces
					addInputFace( neighbourhoodIndex, InputFace( faceIndex, u, p ), viewVector );
					addInputFace( neighbourhoodIndex, InputFace( faceIndex, p, v ), viewVector );
				}
			}
		}
	}
}

void MVertexSurfaceTweakAdjust::build(const Vector3 &viewVector)
{
	// STAGE ONE
	// Scan through the list filling any gaps
	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		Neighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

		if ( neighbourhood.inputFaces.size() >= 1 )
		{
			Vector3 startVector = neighbourhood.inputFaces[0].u.perpendicularTo( viewVector ).getNormalised();
			Vector3 endVector = startVector;



			for (int faceI = 0; faceI < neighbourhood.inputFaces.size(); faceI++)
			{
				int nextFaceI = nextIndex( faceI, neighbourhood.inputFaces.size() );
				const InputFace &face = neighbourhood.inputFaces[faceI];
				const InputFace &nextFace = neighbourhood.inputFaces[nextFaceI];

				// Skip these faces if they resulted from a split
				if ( face.faceIndex != nextFace.faceIndex  ||  faceI == nextFaceI )
				{
					bool bMakeNewFaces = !face.isPreviousTo( nextFace, neighbourhood.vertexNeighbourhoodSize );

					if ( bMakeNewFaces )
					{
						Vector3 n = face.v.cross( nextFace.u );

						double nDotView = n.dot( viewVector );
						if ( nDotView < -EPSILON )
						{
							// NOT INVERTED

							// Ensure that @face.v and @nextFace.u are sufficiently different to form a face
							if ( n.sqrLength()  >  SQR_EPSILON )
							{
								// Compute the mid line, that lies between the boundaries of @face and @nextFace
								Vector3 mid = face.v + nextFace.u;

								// Get their normals
								const Vector3 &n0 = face.planeNormal;
								const Vector3 &n1 = nextFace.planeNormal;

								// Compute the split line; the intersection of the planes of @face and @nextFace
								Vector3 splitLine = n0.cross( n1 );

								if ( splitLine.sqrLength() < SQR_EPSILON )
								{
									// @n0 and @n1 are identical; use the mid-line as the split-line
									splitLine = mid.getNormalised();
								}
								else
								{
									splitLine.normalise();

									// @splitLine may be pointing backwards, depending upon the orientation of the normals; if so correct it
									double splitLineDotMid = splitLine.dot( mid );
									if ( splitLineDotMid  <  -EPSILON )
									{
										splitLine = -splitLine;
									}
									else if ( splitLineDotMid  <=  EPSILON )
									{
										// @splitLine and @mid are perpendicular
										// Make perpendicular to the view vector and re-test
										Vector3 splitInView = splitLine.perpendicularTo( viewVector );
										Vector3 midInView = mid.perpendicularTo( viewVector );

										if ( splitInView.sqrLength()  <  SQR_EPSILON )
										{
											splitInView = splitLine;
										}

										if ( midInView.sqrLength()  <  SQR_EPSILON )
										{
											midInView = mid;
										}

										if ( splitInView.dot( midInView )  <  -EPSILON )
										{
											splitLine = -splitLine;
										}
									}
								}

								// We have now reached @nextFace.v
								endVector = nextFace.v;

								if ( fabs( splitLine.dot( face.v ) )  >  ( 1.0 - EPSILON )  ||  fabs( splitLine.dot( nextFace.u ) )  >  ( 1.0 - EPSILON ) )
								{
									// @splitLine is identical to @face.v or @nextFace.u; simple make a face linking @face.v and @nextFace.u
									neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, face.v, nextFace.u ) );
									faceI += 1;
								}
								else
								{
									// Ensure the @splitLine is between @face.v and @nextFace.u
									Vector3 p = face.v.cross( splitLine );
									Vector3 q = splitLine.cross( nextFace.u );
									if ( p.dot( viewVector )  <  -EPSILON   &&   q.dot( viewVector )  <  -EPSILON )
									{
										// Add two new faces, one extending the plane of @face and the other extending the plane of @nextFace
										Vector3 v = face.v;
										Vector3 nextU = nextFace.u;

										// Extend plane of @nextFace
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, splitLine, nextU ) );
										// Extend plane of @face
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, v, splitLine ) );
										faceI += 2;
									}
									else
									{
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, face.v, nextFace.u ) );
										faceI += 1;
									}
								}
							}
						}
						else if ( nDotView > EPSILON )
						{
							// INVERTED

							// Make @endVector perpendicular to @viewVector
							endVector = endVector.perpendicularTo( viewVector ).getNormalised();

							// First, ensure that @nextFace.u is counter-clockwise of @face.v
							// To do this, ensure that @nextFace.u is NOT between @startVector and @endVector
							Vector3 alpha = startVector.cross( nextFace.u );
							Vector3 beta = nextFace.u.cross( endVector );
							double alphaDotView = alpha.dot( viewVector );
							double betaDotView = beta.dot( viewVector );
							if ( !( alphaDotView < -EPSILON  &&  betaDotView < -EPSILON    ||    alphaDotView > EPSILON  &&  betaDotView > EPSILON ) )
							{
								// Compute the mid line, that lies between the boundaries of @face and @nextFace
								Vector3 mid;

								if ( face.v.dot( nextFace.u )  <  -( 1.0 - EPSILON ) )
								{
									// @face.v and @nextFace.u oppose one another; use @face.v.cross( normal ) as the mid-line
									Vector3 n = face.planeNormal + nextFace.planeNormal;

									if ( n.sqrLength()  <  SQR_EPSILON )
									{
										n = face.planeNormal;
									}

									mid = face.v.cross( n );
								}
								else
								{
									// average @face.v and @nextFace.u
									mid = -( face.v + nextFace.u );
								}

								// Get their normals
								const Vector3 &n0 = face.planeNormal;
								const Vector3 &n1 = nextFace.planeNormal;

								// Compute the split line; the intersection of the planes of @face and @nextFace
								Vector3 splitLine = n0.cross( n1 );

								if ( splitLine.sqrLength()  <  SQR_EPSILON )
								{
									// @n0 and @n1 are identical; use the mid-line
									splitLine = mid.getNormalised();
								}
								else
								{
									splitLine.normalise();

									// @splitLine may be pointing backwards, depending upon the orientation of the normals; if so correct it
									double splitLineDotMid = splitLine.dot( mid );
									if ( splitLineDotMid  <  -EPSILON )
									{
										splitLine = -splitLine;
									}
									else if ( splitLineDotMid  <=  EPSILON )
									{
										// @splitLine and @mid are perpendicular
										// Make perpendicular to the view vector and re-test
										Vector3 splitInView = splitLine.perpendicularTo( viewVector );
										Vector3 midInView = mid.perpendicularTo( viewVector );

										if ( splitInView.sqrLength()  <  SQR_EPSILON )
										{
											splitInView = splitLine;
										}

										if ( midInView.sqrLength()  <  SQR_EPSILON )
										{
											midInView = mid;
										}

										if ( splitInView.dot( midInView )  <  -EPSILON )
										{
											splitLine = -splitLine;
										}
									}
								}


								// We have now reached @nextFace.v
								endVector = nextFace.v;

								Vector3 v = face.v;
								Vector3 nextU = nextFace.u;

								if ( fabs( splitLine.dot( face.v ) )  >  ( 1.0 - EPSILON )  ||  fabs( splitLine.dot( nextFace.u ) )  >  ( 1.0 - EPSILON ) )
								{
									// @splitLine is identical to @face.v or @nextFace.u; use the mid-line as the split-line
									if ( fabs( mid.dot( face.v ) )  <  ( 1.0 - EPSILON )  &&  fabs( mid.dot( nextFace.u ) )  <  ( 1.0 - EPSILON ) )
									{
										mid.normalise();
										// Extend plane of @nextFace
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, mid, nextU ) );
										// Extend plane of @face
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, v, mid ) );
										faceI += 2;
									}
								}
								else
								{
									// Ensure the @splitLine is between @face.v and @nextFace.u
									Vector3 p = face.v.cross( splitLine );
									Vector3 q = splitLine.cross( nextFace.u );
									if ( p.dot( viewVector )  <  -EPSILON   &&   q.dot( viewVector )  <  -EPSILON )
									{
										// Add two new faces, one extending the plane of @face and the other extending the plane of @nextFace

										splitLine.normalise();

										// Extend plane of @nextFace
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, splitLine, nextU ) );
										// Extend plane of @face
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, v, splitLine ) );
										faceI += 2;
									}
									else
									{
										mid.normalise();
										// Extend plane of @nextFace
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, mid, nextU ) );
										// Extend plane of @face
										neighbourhood.inputFaces.insert( nextFaceI, InputFace( face.faceIndex, v, mid ) );
										faceI += 2;
									}
								}
							}
						}
					}
					else
					{
						endVector = nextFace.v;
					}
				}
			}
		}


		// Compute how many face entries will need to be generated; some input faces must be split into two
		int numFaceEntries = 0;
		for (int inputFaceI = 0; inputFaceI < neighbourhood.inputFaces.size(); inputFaceI++)
		{
			const InputFace &inputFace = neighbourhood.inputFaces[inputFaceI];

			if ( inputFace.isFrontFacing( viewVector ) )
			{
				numFaceEntries++;
			}
			else
			{
				numFaceEntries += 2;
			}
		}


		// Generate face entries
		neighbourhood.faces.resize( numFaceEntries );

		int faceI = 0;
		for (int inputFaceI = 0; inputFaceI < neighbourhood.inputFaces.size(); inputFaceI++)
		{
			const InputFace &inputFace = neighbourhood.inputFaces[inputFaceI];

			if ( inputFace.isFrontFacing( viewVector ) )
			{
				neighbourhood.faces[faceI++] = FaceEntry( inputFace.planeNormal, inputFace.u, inputFace.v, viewVector );
			}
			else
			{
				Vector3 mid = -( inputFace.u + inputFace.v );

				if ( mid.sqrLength()  <  SQR_EPSILON )
				{
					mid = inputFace.u.perpendicularVector();
				}

				mid.normalise();
				neighbourhood.faces[faceI++] = FaceEntry( -inputFace.planeNormal, inputFace.u, mid, viewVector );
				neighbourhood.faces[faceI++] = FaceEntry( -inputFace.planeNormal, mid, inputFace.v, viewVector );
			}
		}

		neighbourhood.inputFaces.clear();
	}
}


Vector3 MVertexSurfaceTweakAdjust::getConstrainedTweak(const Vector3 &v, const Vector3 &viewVector) const
{
	Vector3 bestPlaneNormal;
	double bestPlaneZ = 0.0;
	bool bGotBestPlane = false;

	double vSqrLen = v.sqrLength();

	if ( vSqrLen > SQR_EPSILON )
	{
		double vLength = v.length();


		// Attempt to find a face entry to use
		for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
		{
			const Neighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

			for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
			{
				const FaceEntry &face = neighbourhood.faces[faceI];

				if ( v.dot( face.inBoundaryNormal )  >=  0.0  &&  v.dot( face.outBoundaryNormal )  >=  0.0 )
				{
					Vector3 vp = v.projectOntoPlane( face.planeNormal ).getNormalisedToLength( vLength );
					double z = -vp.dot( viewVector );
					if ( z > bestPlaneZ  ||  !bGotBestPlane )
					{
						bestPlaneNormal = face.planeNormal;
						bestPlaneZ = z;
						bGotBestPlane = true;
					}
				}
			}
		}

		if ( !bGotBestPlane )
		{
			double bestPlaneDist = 0.0;
			for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
			{
				const Neighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

				for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
				{
					const FaceEntry &face = neighbourhood.faces[faceI];

					Vector3 vp = v.projectOntoPlane( face.planeNormal ).getNormalisedToLength( vLength );

					double inPlaneDist = vp.dot( face.inBoundaryNormal );
					double outPlaneDist = vp.dot( face.outBoundaryNormal );

					if ( inPlaneDist >= 0.0  ||  outPlaneDist >= 0.0 )
					{
						double z = -vp.dot( viewVector );

						double dist = 0.0;
						dist = -std::min( inPlaneDist, outPlaneDist );

						if ( dist < bestPlaneDist  ||  !bGotBestPlane )
						{
							bestPlaneNormal = face.planeNormal;
							bestPlaneZ = z;
							bestPlaneDist = dist;
							bGotBestPlane = true;
						}
					}
				}
			}

			if ( !bGotBestPlane )
			{
				return Vector3();
			}
		}

		Vector3 u = v.cross( viewVector );
		if ( u.sqrLength()  >  vSqrLen * viewVector.sqrLength() * SQR_EPSILON )
		{
			Vector3 tweakVector = u.cross( bestPlaneNormal );
			if ( tweakVector.dot( v )  <  0.0 )
			{
				tweakVector = -tweakVector;
			}
			return tweakVector.getNormalisedToLength( vLength );
		}

		return v.projectOntoPlane( bestPlaneNormal ).getNormalisedToLength( vLength );
	}
	else
	{
		return Vector3();
	}
}




void MVertexSurfaceTweakAdjust::tweak(const Vector3 &v, const Vector3 &viewVector)
{
	Vector3 translation = getConstrainedTweak( v, viewVector );

	vertex->setPosition( initialPosition + translation );
}

void MVertexSurfaceTweakAdjust::debugDraw()
{
	const Point3 &vertexPos = vertex->getPosition();

	glLineWidth( 2.0 );

	for (int neighbourhoodI = 0; neighbourhoodI < neighbourhoods.size(); neighbourhoodI++)
	{
		const Neighbourhood &neighbourhood = neighbourhoods[neighbourhoodI];

		for (int faceI = 0; faceI < neighbourhood.faces.size(); faceI++)
		{
			const FaceEntry &face = neighbourhood.faces[faceI];

			glBegin( GL_TRIANGLES );
 			glColor4d( 0.6, 0.6, 0.0, 0.5 );
 			glVertex3( vertexPos );
 			glColor4d( 0.6, 0.0, 0.0, 0.5 );
 			glVertex3( vertexPos + face.u * 5.0 );
 			glColor4d( 0.0, 0.6, 0.0, 0.5 );
 			glVertex3( vertexPos + face.v * 5.0 );
 			glEnd();

 			glBegin( GL_LINES );
 			glColor4d( 0.0, 0.0, 0.6, 0.5 );
 			glVertex3( vertexPos );
 			glVertex3( vertexPos + face.u * 5.0 );
 			glVertex3( vertexPos );
 			glVertex3( vertexPos + face.v * 5.0 );

 			glColor4d( 0.0, 0.0, 0.6, 0.5 );
 			glVertex3( vertexPos + face.u * 2.5 );
 			glColor4d( 0.6, 0.0, 0.6, 0.5 );
 			glVertex3( vertexPos + face.u * 2.5  +  face.inBoundaryNormal );

 			glColor4d( 0.0, 0.0, 0.6, 0.5 );
 			glVertex3( vertexPos + face.v * 2.5 );
 			glColor4d( 0.0, 0.6, 0.6, 0.5 );
 			glVertex3( vertexPos + face.v * 2.5  +  face.outBoundaryNormal );
 			glEnd();
		}
	}
}





/****************************************************************************
 ****************************************************************************
					MVertexSurfaceTweakAdjustList
 ****************************************************************************
 ****************************************************************************/

MVertexSurfaceTweakAdjustList::MVertexSurfaceTweakAdjustList()
{
}

MVertexSurfaceTweakAdjustList::~MVertexSurfaceTweakAdjustList()
{
}


void MVertexSurfaceTweakAdjustList::setViewVector(const Vector3 &viewVector)
{
	this->viewVector = viewVector;
}


void MVertexSurfaceTweakAdjustList::tweak(const Vector3 &v)
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].tweak( v, viewVector );
	}
}


void MVertexSurfaceTweakAdjustList::debugDraw()
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].debugDraw();
	}
}






/****************************************************************************
 ****************************************************************************
											MVertexAttribAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexAttribAdjustList::MVertexAttribAdjustList()
{
}

MVertexAttribAdjustList::~MVertexAttribAdjustList()
{
}


void MVertexAttribAdjustList::setParameters(float alpha)
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].setParameters( alpha );
	}
}





/****************************************************************************
 ****************************************************************************
									MVertexAttribVectorAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexAttribVectorAdjustList::MVertexAttribVectorAdjustList()
{
}

MVertexAttribVectorAdjustList::~MVertexAttribVectorAdjustList()
{
}


void MVertexAttribVectorAdjustList::setParameters(const Vector2f &v)
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].setParameters( v );
	}
}





/****************************************************************************
 ****************************************************************************
					MVertexAttribSlideAdjust
 ****************************************************************************
 ****************************************************************************/

MVertexAttribSlideAdjustList::MVertexAttribSlideAdjustList()
{
}

MVertexAttribSlideAdjustList::~MVertexAttribSlideAdjustList()
{
}


void MVertexAttribSlideAdjustList::setSlideFactor(double slide)
{
	for (int i = 0; i < size(); i++)
	{
		adjustments[i].setSlideFactor( slide );
	}
}





/****************************************************************************
 ****************************************************************************
									MTransformationTarget
 ****************************************************************************
 ****************************************************************************/

MTransformationTarget::MTransformationTarget()
{
	mesh = NULL;
}

MTransformationTarget::~MTransformationTarget()
{
}


const Point3 & MTransformationTarget::getCentre() const
{
	return centre;
}


int MTransformationTarget::size() const
{
	return groups.size();
}


void MTransformationTarget::translate(const Vector3 &translation)
{
	Matrix4 xlatMatrix = Matrix4::translate( translation );

	for (int i = 0; i < groups.size(); i++)
	{
		XformGroup &g = groups[i];

		for (int j = 0; j < g.vertices.size(); j++)
		{
			MVertex *v = g.vertices[j];
			v->i_transformPosition( xlatMatrix, true );
		}
	}

	mesh->vertexTransformFinalise();
}

void MTransformationTarget::centredTransform(const Matrix4 &xform)
{
	for (int i = 0; i < groups.size(); i++)
	{
		XformGroup &g = groups[i];

		Matrix4 m = Matrix4::translate( -g.centre.toVector3() ) * xform *
						Matrix4::translate( g.centre.toVector3() );

		for (int j = 0; j < g.vertices.size(); j++)
		{
			MVertex *v = g.vertices[j];
			v->i_transformPosition( m, true );
		}
	}

	mesh->vertexTransformFinalise();
}


void MTransformationTarget::finalise()
{
	centre = Point3();

	for (int i = 0; i < groups.size(); i++)
	{
		centre.cumulativeAdd( groups[i].centre );
	}

	centre.scale( 1.0 / (double)groups.size() );
}

void MTransformationTarget::clear()
{
	groups.clear();
	centre = Point3();
}

MTransformationTarget::XformGroup & MTransformationTarget::newGroup()
{
	return groups.push_back();
}

void MTransformationTarget::setMesh(MMesh *m)
{
	mesh = m;
}



#endif
