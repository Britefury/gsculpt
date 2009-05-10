//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKGROUNDMESH_CPP__
#define BACKGROUNDMESH_CPP__

#include <float.h>

#include <PlatformSpecific/IncludeGL.h>
#include <PlatformSpecific/GLExtensions.h>

#include <Model/ModelDraw.h>


#include <Background/BackgroundMesh/BackgroundMesh.h>



#define NODE_SIZE_THRESHOLD 32
#define FACE_BOX_EPSILON 1.0e-6





/***************************************************************
				KDTree::KDTreeNode
 ***************************************************************/

BackgroundMesh::KDTree::KDTreeNode::KDTreeNode()
{
}


void BackgroundMesh::KDTree::KDTreeNode::build(KDTree *tree, int nodeIndex, int start, int end, ProgressMonitor *kdtreeMonitor)
{
	startIndex = start;
	endIndex = end;
	negativeIndex = positiveIndex = 0;

	int numFacesInNode = end - start;
	
	box = BBox3f();

	if ( numFacesInNode < NODE_SIZE_THRESHOLD )
	{
		// LEAF NODE
		// Compute the bounding box for the faces in this node
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3f &faceBox = tree->faceBoxes[ tree->faceOrder[faceI] ];
			box.addBox( faceBox );
		}
		box.enlargeForIntersection();
	
		if ( kdtreeMonitor != NULL )
		{
			/*if ( nodeIndex % 1000 == 0 )
			{
				kdtreeMonitor->updateProgress( (float)nodeIndex / (float)tree->numNodesEstimate );
			}*/
			if ( start % 10000 == 0  ||  end % 10000 == 0  ||  ( start % 10000 ) > ( end % 10000 ) )
			{
				kdtreeMonitor->updateProgress( (float)end / (float)tree->mesh->tris.size() );
			}
		}
	}
	else
	{
// 		// BRANCH NODE
		// Compute the bounding box for the faces in this node, and compute the average
		// of the centroids of the faces
		Point3f centroid;
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3f &faceBox = tree->faceBoxes[ tree->faceOrder[faceI] ];
			box.addBox( faceBox );
			centroid.cumulativeAdd( tree->faceCentroids[ tree->faceOrder[faceI] ] );
		}
		box.enlargeForIntersection();
		centroid.scale( 1.0 / (double)( end - start ) );

		// Using the average centroid as a point on the split plane, compute the number of faces that would
		// end up on the negative, positive, or both sides of the plane with a given axis (X, Y, or Z)
		// used as the split plane normal.
		int negativeCount[3] = { 0, 0, 0 };
		int crossCount[3] = { 0, 0, 0 };
		int positiveCount[3] = { 0, 0, 0 };
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3f &faceBox = tree->faceBoxes[ tree->faceOrder[faceI] ];

			for (int axisI = 0; axisI < 3; axisI++)
			{
				if ( faceBox.getLower().v[axisI] > centroid.v[axisI] )
				{
					positiveCount[axisI]++;
				}
				else if ( faceBox.getUpper().v[axisI] < centroid.v[axisI] )
				{
					negativeCount[axisI]++;
				}
				else
				{
					crossCount[axisI]++;
				}
			}
		}

		double minimumAllowedSize = box.getMajorSize()  *  FACE_BOX_EPSILON;

		// Compute the score of each axis
		int score[3] = { 0, 0, 0 };
		for (int axisI = 0; axisI < 3; axisI++)
		{
			// Compute the difference between the negative and positive counts (the amount of imbalance);
			// imbalance should be minimised
			int diff = positiveCount[axisI] - negativeCount[axisI];
			diff = std::max( diff, -diff );

			// The score is cross-count * 2  +  difference
			// the cross-count should be minimised, as this reduces the amount of overlap
			score[axisI] = crossCount[axisI] * 2  +  diff;

			// If either @negativeCount or @positiveCount are 0, set the score to -1, indicating an
			// invalid choice
			if ( negativeCount[axisI] == 0  ||  positiveCount[axisI] == 0 )
			{
				score[axisI] = -1;
			}

			// If the size of the box in this axis is less than @minimumAllowedSize, set the score to -1, indicating an
			// invalid choice
			if ( box.getSize( (Axis)axisI ) < minimumAllowedSize )
			{
				score[axisI] = -1;
			}
		}

		// Choose the axis with the minimum score as the split plane axis
		int splitAxis = 0;
		for (int axisI = 1; axisI < 3; axisI++)
		{
			if ( score[axisI] < score[splitAxis]  ||  score[splitAxis] == -1 )
			{
				splitAxis = axisI;
			}
		}

		// Ensure that we only proceed if the score is 'valid'; an invalid score indicates that splitting
		// cannot improve matters, so don't bother.
		if ( score[splitAxis] != -1 )
		{
			setSplitAxis( splitAxis );

			// Re-order the face index list so that the faces the lie on the negative side of the split plane come
			// before those that lie on the positive side
			// The algorithm works from each end in towards the centre of the list, swapping elements if they
			// are on the wrong side of the 'anchor' point.
			float planeD = centroid.v[splitAxis];												// @planeD is the anchor value
			int faceI = start, faceJ = end - 1;													// @faceI is the lower index, @faceJ the upper index
			float faceID = tree->faceCentroids[ tree->faceOrder[faceI] ].v[splitAxis];						// @faceID and @faceJD are the values for @faceI and @faceJ
			float faceJD = tree->faceCentroids[ tree->faceOrder[faceJ] ].v[splitAxis];
			int anchorIndex = 0;																	// @anchorIndex will be the anchor position
			while ( faceI < faceJ )
			{
				if ( faceID <= planeD )
				{
					// @faceI indexes a face that is on the correct side of the anchor, move on to the next
					faceI++;
					faceID = tree->faceCentroids[ tree->faceOrder[faceI] ].v[splitAxis];
				}
				if ( faceJD >= planeD )
				{
					// @faceJ indexes a face that is on the correct side of the anchor, move on to the next
					anchorIndex = faceJ;
					faceJ--;
					faceJD = tree->faceCentroids[ tree->faceOrder[faceJ] ].v[splitAxis];
				}
				if ( faceID > planeD  &&  faceJD < planeD )
				{
					// @faceI and @faceJ index faces that are on the wrong side of the anchor; swap them
					std::swap( tree->faceOrder[faceI], tree->faceOrder[faceJ] );
					std::swap( faceID, faceJD );
					anchorIndex = faceJ;
				}
				if ( faceI == faceJ )
				{
					// @faceI and @faceJ index the same face; this would be the last iteration of the loop
					// and is a special case; compute @anchorIndex
					if ( faceID < planeD )
					{
						// should go on negative side; anchor is @faceI + 1
						anchorIndex = faceI + 1;
					}
					else
					{
						// should go on negative side; anchor is @faceI
						anchorIndex = faceI;
					}
				}
			}


			/*if ( kdtreeMonitor != NULL )
			{
				if ( nodeIndex % 1000 == 0 )
				{
					kdtreeMonitor->updateProgress( (float)nodeIndex / (float)tree->numNodesEstimate );
				}
			}*/
			
			// Only create subtrees if the split does result in splitting the face list into two parts, otherwise
			// you end up with infinite recursion
			if ( start != anchorIndex  &&  anchorIndex != end )
			{
				// Create subtrees
				
				// DO NOT REFERENCE *this
				// The tree->createNode() call modifies the node array. This can result in *this being copied into a new node
				// In this case we want to refer to the node in the array, which may be a new copy of *this, not *this, so we use
				// tree->nodes[nodeIndex]
				tree->nodes[nodeIndex].setNegativeIndex( tree->createNode( start, anchorIndex, kdtreeMonitor ) );
				tree->nodes[nodeIndex].setPositiveIndex( tree->createNode( anchorIndex, end, kdtreeMonitor ) );
			}
		}
	}
}



bool BackgroundMesh::KDTree::KDTreeNode::intersects(const KDSegment &seg) const
{
	// Segment->box intersection code based on the algorithm used by David Eberly,
	// http://www.geometrictools.com

	float segExtents[3], diffExtents[3], absDirCrossDelta[3], rhs;

	Vector3f delta = seg.getMidPoint() - box.getCentre();
	Vector3f segDir = seg.getDirection() * 0.5f;
	Vector3f boxExtents = box.getSize() * 0.5f;


	segExtents[0] = fabs( segDir.x );
	diffExtents[0] = fabs( delta.x );
	rhs = boxExtents.x + segExtents[0];
	if ( diffExtents[0] > rhs )
	{
		return false;
	}

	segExtents[1] = fabs( segDir.y );
	diffExtents[1] = fabs( delta.y );
	rhs = boxExtents.y + segExtents[1];
	if ( diffExtents[1] > rhs )
	{
		return false;
	}

	segExtents[2] = fabs( segDir.z );
	diffExtents[2] = fabs( delta.z );
	rhs = boxExtents.z + segExtents[2];
	if ( diffExtents[2] > rhs )
	{
		return false;
	}


	Vector3f dirCrossDelta = segDir.cross( delta );

	absDirCrossDelta[0] = fabs( dirCrossDelta.x );
	rhs = boxExtents.y * segExtents[2]  +  boxExtents.z * segExtents[1];
	if ( absDirCrossDelta[0]  >  rhs )
	{
		return false;
	}

	absDirCrossDelta[1] = fabs( dirCrossDelta.y );
	rhs = boxExtents.x * segExtents[2]  +  boxExtents.z * segExtents[0];
	if ( absDirCrossDelta[1]  >  rhs )
	{
		return false;
	}

	absDirCrossDelta[2] = fabs( dirCrossDelta.z );
	rhs = boxExtents.x * segExtents[1]  +  boxExtents.y * segExtents[0];
	if ( absDirCrossDelta[2]  >  rhs )
	{
		return false;
	}

	return true;
}


bool BackgroundMesh::KDTree::KDTreeNode::isLeaf() const
{
	return getNegativeIndex() == 0  &&  getPositiveIndex() == 0;
}





/***************************************************************
				KDTree
 ***************************************************************/

BackgroundMesh::KDTree::KDTree()
{
	mesh = NULL;
	numNodesEstimate = 1;
}

void BackgroundMesh::KDTree::readFromFile(BackgroundMesh *mesh, FILE *file)
{
	this->mesh = mesh;
	
	int numNodes = 0, numFaces = 0;
	
	// Nodes
	nodes.clear();
	fread( &numNodes, sizeof(int), 1, file );
	nodes.resize( numNodes );
	fread( nodes.begin(), sizeof(KDTreeNode), numNodes, file );
	
	// Face data
	fread( &numFaces, sizeof(int), 1, file );
	faceBoxes.clear();
	faceCentroids.clear();
	faceOrder.clear();
	faceOrder.resize( numFaces );
	fread( faceOrder.begin(), sizeof(int), numFaces, file );
}

void BackgroundMesh::KDTree::writeToFile(FILE *file)
{
	// Nodes
	int numNodes = nodes.size();
	fwrite( &numNodes, sizeof(int), 1, file );
	fwrite( nodes.begin(), sizeof(KDTreeNode), nodes.size(), file );
	
	// Face data
	int numFaces = faceOrder.size();
	fwrite( &numFaces, sizeof(int), 1, file );
	fwrite( faceOrder.begin(), sizeof(int), faceOrder.size(), file );
}



void BackgroundMesh::KDTree::initialise(BackgroundMesh *mesh, ProgressMonitor *kdtreeMonitor)
{
	this->mesh = mesh;
	
	if ( kdtreeMonitor != NULL )
	{
		kdtreeMonitor->updateProgress( 0.0f );
	}

	// Initialise the lists of face bounding boxes and indices
	int numFaces = mesh->tris.size();

	//faces.resize( numFaces );
	faceBoxes.resize( numFaces );
	faceCentroids.resize( numFaces );
	faceOrder.resize( numFaces );
	nodes.clear();
	
	numNodesEstimate = (int)( (float)numFaces * 2.0f / (float)NODE_SIZE_THRESHOLD );


	for (int faceI = 0; faceI < numFaces; faceI++)
	{
		const IndexTri &t = mesh->tris[faceI];
		const Point3f &a = mesh->vPosition[t.i[0]];
		const Point3f &b = mesh->vPosition[t.i[1]];
		const Point3f &c = mesh->vPosition[t.i[2]];
		BBox3f &faceBox = faceBoxes[faceI];
		faceBox.l = Point3f::min( Point3f::min( a, b ), c );
		faceBox.u = Point3f::max( Point3f::max( a, b ), c );
		faceCentroids[faceI] = Point3f::average( a, b, c );
		faceOrder[faceI] = faceI;
	}

	if ( numFaces > 0 )
	{
		createNode( 0, numFaces, kdtreeMonitor );	
	}

	faceBoxes.clear();
	faceBoxes.setCapacity( 0 );
	faceCentroids.clear();
	faceCentroids.setCapacity( 0 );
}


int BackgroundMesh::KDTree::createNode(int startIndex, int endIndex, ProgressMonitor *kdtreeMonitor)
{
	int nodeIndex = nodes.size();

	KDTreeNode &node = nodes.push_back();
	node.build( this, nodeIndex, startIndex, endIndex, kdtreeMonitor );

	return nodeIndex;
}



int BackgroundMesh::KDTree::raytrace(const KDSegment &originalSeg, KDSegment &seg, float &segT, const KDTreeNode &node) const
{
	int triIndex = -1;
	if ( node.isLeaf() )
	{
		float u, v, bestT = 0.0f;
		for (int triI = node.startIndex; triI < node.endIndex; triI++)
		{
			float t;
			KDTTri face( mesh->vPosition, mesh->tris[ faceOrder[triI] ] );
			if ( face.raytrace( seg, t, u, v ) )
			{
				if ( t < bestT  ||  triIndex == -1 )
				{
					bestT = t;
					triIndex = triI;
				}
			}
		}

		if ( triIndex != -1 )
		{
			segT *= bestT;
			seg.setEnd( originalSeg.getPoint( segT ) );
		}

		return triIndex;
	}
	else
	{
		int splitAxis = node.getSplitAxis();
		float d = originalSeg.direction.v[splitAxis];
		bool bReverseOrder = d < 0.0;

		// Traverse negative subtree
		const KDTreeNode &negNode = nodes[node.getNegativeIndex()];
		const KDTreeNode &posNode = nodes[node.getPositiveIndex()];

		if ( bReverseOrder )
		{
			if ( posNode.intersects( seg ) )
			{
				int posTriIndex = raytrace( originalSeg, seg, segT, posNode );
				if ( posTriIndex != -1 )
				{
					triIndex = posTriIndex;
				}
			}

			if ( negNode.intersects( seg ) )
			{
				int negTriIndex = raytrace( originalSeg, seg, segT, negNode );
				if ( negTriIndex != -1 )
				{
					triIndex = negTriIndex;
				}
			}
		}
		else
		{
			if ( negNode.intersects( seg ) )
			{
				int negTriIndex = raytrace( originalSeg, seg, segT, negNode );
				if ( negTriIndex != -1 )
				{
					triIndex = negTriIndex;
				}
			}

			if ( posNode.intersects( seg ) )
			{
				int posTriIndex = raytrace( originalSeg, seg, segT, posNode );
				if ( posTriIndex != -1 )
				{
					triIndex = posTriIndex;
				}
			}
		}

		return triIndex;
	}
}

int BackgroundMesh::KDTree::raytrace(const KDSegment &seg, Point3f &p, float &t) const
{
	if ( nodes.size() > 0 )
	{
		if ( nodes[0].intersects( seg ) )
		{
			KDSegment clippedSeg = seg;
			t = 1.0f;
			int triIndex = raytrace( seg, clippedSeg, t, nodes[0] );
			if ( triIndex != -1 )
			{
				p = clippedSeg.b;
			}
			return triIndex;
		}
	}

	return -1;
}






/***************************************************************
				BackgroundMesh
 ***************************************************************/

BackgroundMesh::BackgroundMesh()
{
	bInitialisedGL = false;
}

BackgroundMesh::BackgroundMesh(Array<Point3f> &inVerts, Array<IndexFace> &inFaces, ProgressMonitor *convertMonitor, ProgressMonitor *kdtreeMonitor)
{
	bInitialisedGL = false;

	// Face index buffer
	IndexFace faceOrder;
	// Edge vectors
	Vector3f edges[2];
	// Normal vector
	Vector3f n;

	vNormal.resize( inVerts.size() );
	tris.reserve( inFaces.size() );

	// Copy vertex positions
	vPosition = inVerts;
	
	float numPrimsRecip = 1.0f / (float)( inFaces.size() + inVerts.size() );

	// For each face
	for (int faceI = 0; faceI < inFaces.size(); faceI++)
	{
		// Copy the face indices into @faceIndices, removing any duplicate vertices
		IndexFace &inFace = inFaces[faceI];

		faceOrder.clear();
		faceOrder.push_back( inFace[0] );

		int x = inFace[0];
		for (int faceVertexI = 1; faceVertexI < inFace.size(); faceVertexI++)
		{
			int y = inFace[faceVertexI];
			if ( y != x )
			{
				faceOrder.push_back( y );
				x = y;
			}
		}


		if ( faceOrder.size() >= 3 )
		{
			// The face is not degenerate

			// Triangulate the face, and compute the normal
			int aIndex = faceOrder[0], bIndex = faceOrder[1];
			const Point3f *a = &inVerts[aIndex];
			const Point3f *b = &inVerts[bIndex];
			edges[0] = *b - *a;
			n = Vector3f( 0.0f, 0.0f, 0.0f );
			Vector3f *u = &edges[0], *v = &edges[1];
			for (int cFaceVertexIndex = 2; cFaceVertexIndex < faceOrder.size(); cFaceVertexIndex++)
			{
				int cIndex = faceOrder[cFaceVertexIndex];
				const Point3f *c = &inVerts[cIndex];
				*v = *c - *a;

				n += u->cross( *v );

				tris.push_back( IndexTri() );
				IndexTri &tri = tris.back();
				tri.i[0] = aIndex;
				tri.i[1] = bIndex;
				tri.i[2] = cIndex;

				bIndex = cIndex;
				b = c;
				std::swap( u, v );
			}
			
			n.normalise();

			// Accumulate normals per vertex
			for (int faceVertexI = 0; faceVertexI < faceOrder.size(); faceVertexI++)
			{
				int vIndex = faceOrder[faceVertexI];
				vNormal[vIndex] += n;
			}
		}
		
		
		if ( convertMonitor != NULL )
		{
			if ( faceI % 10000  ==  0 )
			{
				convertMonitor->updateProgress( (float)faceI * numPrimsRecip );
			}
		}
	}


	// Normalise vertex normals
	float progressOffset = (float)inFaces.size() * numPrimsRecip;
	for (int vertexI = 0; vertexI < inVerts.size(); vertexI++)
	{
		vNormal[vertexI].normalise();
	
		if ( convertMonitor != NULL )
		{
			if ( vertexI % 10000  ==  0 )
			{
				convertMonitor->updateProgress( progressOffset  +  (float)vertexI * numPrimsRecip );
			}
		}
	}

	
	kdTree.initialise( this, kdtreeMonitor );
}



bool BackgroundMesh::readFromFile(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "rb" );
	
	int numPositions = 0, numNormals = 0, numTris = 0;
	
	const char *expectedHeader = "GS_BKG_MESH";
	char header[12];
	fread( header, sizeof(char), 12, file );
	header[11] = 0x00;
	
	if ( strcmp( header, expectedHeader )  !=  0 )
	{
		return false;
	}
	
	
	// Vertex positions
	vPosition.clear();
	fread( &numPositions, sizeof(int), 1, file );
	vPosition.resize( numPositions );
	fread( vPosition.begin(), sizeof(Point3f), numPositions, file );
	
	// Vertex normals
	vNormal.clear();
	fread( &numNormals, sizeof(int), 1, file );
	vNormal.resize( numNormals );
	fread( vNormal.begin(), sizeof(Vector3f), numNormals, file );
	
	// Triangles
	tris.clear();
	fread( &numTris, sizeof(int), 1, file );
	tris.resize( numTris );
	fread( tris.begin(), sizeof(IndexTri), numTris, file );
	
	// KD-Tree
	kdTree.readFromFile( this, file );
	
	fclose( file );
	
	return true;
}

void BackgroundMesh::writeToFile(std::string filename)
{
	FILE *file = fopen( filename.c_str(), "wb" );
	
	const char *header = "GS_BKG_MESH";
	fwrite( header, sizeof(char), strlen(header)+1, file );
	
	// Vertex positions
	int numPositions = vPosition.size();
	fwrite( &numPositions, sizeof(int), 1, file );
	fwrite( vPosition.begin(), sizeof(Point3f), vPosition.size(), file );
	
	// Vertex normals
	int numNormals = vNormal.size();
	fwrite( &numNormals, sizeof(int), 1, file );
	fwrite( vNormal.begin(), sizeof(Vector3f), vNormal.size(), file );
	
	// Triangles
	int numTris = tris.size();
	fwrite( &numTris, sizeof(int), 1, file );
	fwrite( tris.begin(), sizeof(IndexTri), tris.size(), file );
	
	// KD-Tree
	kdTree.writeToFile( file );
	
	fclose( file );
}




#ifdef _BACKGROUNDMESH_USE_VERTEX_BUFFERS

void BackgroundMesh::initGL()
{
	if ( !bInitialisedGL )
	{
		bInitialisedGL = true;

		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glext->glGenBuffers( NUMBUFFERS, buffers );
		
			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_POSITION] );
			glext->glBufferData( GL_ARRAY_BUFFER, sizeof( Point3f ) * vPosition.size(), vPosition.begin(), GL_STATIC_DRAW );
			glVertexPointer( 3, GL_FLOAT, 0, NULL );

			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_NORMAL] );
			glext->glBufferData( GL_ARRAY_BUFFER, sizeof( Vector3f ) * vNormal.size(), vNormal.begin(), GL_STATIC_DRAW );
			glNormalPointer( GL_FLOAT, 0, NULL );
		
			glext->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[BUFFERINDEX_INDICES] );
			glext->glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( IndexTri ) * tris.size(), tris.begin(), GL_STATIC_DRAW );
		}
	}
}


void BackgroundMesh::drawGL()
{
	if ( bInitialisedGL )
	{
		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_NORMAL_ARRAY );
		
			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_POSITION] );
			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_NORMAL] );
			glext->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[BUFFERINDEX_INDICES] );

			glDrawElements( GL_TRIANGLES, tris.size() * 3, GL_UNSIGNED_INT, 0 );

			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_NORMAL_ARRAY );
		}
	}
}


void BackgroundMesh::shutdownGL()
{
	if ( bInitialisedGL )
	{
		bInitialisedGL = false;
		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glext->glDeleteBuffers( NUMBUFFERS, buffers );
		}
	}
}

#elif defined(_BACKGROUNDMESH_USE_VERTEX_ARRAYS_AND_DISPLAY_LIST)

void BackgroundMesh::initGL()
{
	if ( !bInitialisedGL )
	{
		bInitialisedGL = true;
		
		// Generate a display list
		displayListId = glGenLists( 1 );
		
		// Setup vertex and normal arrays
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );

		glVertexPointer( 3, GL_FLOAT, 0, vPosition.begin() );
		glNormalPointer( GL_FLOAT, 0, vNormal.begin() );

		
		// Compile the display list
		glNewList( displayListId, GL_COMPILE );
		glDrawElements( GL_TRIANGLES, tris.size() * 3, GL_UNSIGNED_INT, tris.begin() );
		glEndList();


		// Shutdown vertex and normal arrays
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
	}
}


void BackgroundMesh::drawGL()
{
	if ( bInitialisedGL )
	{
		// Call the display list
		glCallList( displayListId );
	}
}


void BackgroundMesh::shutdownGL()
{
	if ( bInitialisedGL )
	{
		bInitialisedGL = false;
		
		// Delete the display list
		glDeleteLists( displayListId, 1 );
	}
}


#endif





int BackgroundMesh::raytrace(const Segment3f &seg, Point3f &p, float &t) const
{
	return kdTree.raytrace( BackgroundMesh::KDTree::KDSegment( seg.a, seg.b ), p, t );
}


boost::python::tuple BackgroundMesh::py_raytrace(const Segment3f &seg) const
{
	Point3f p;
	float t;

	int triIndex = raytrace( seg, p, t );

	if ( triIndex != -1 )
	{
		return boost::python::make_tuple( true, triIndex, p, t );
	}
	else
	{
		return boost::python::make_tuple( false, -1, Point3f(), 0.0f );
	}
}




#endif
