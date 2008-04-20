//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MBBTREE_CPP__
#define MBBTREE_CPP__

#include <algorithm>

#include <Math/Point2.h>

#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>

#include <Mesh/MMesh/MBBTree.h>


#define NODE_SIZE_THRESHOLD 32

#define REBUILD_COST_THRESHOLD 1.75

#define FACE_BOX_EPSILON 1.0e-6


//#define DISABLE_BBTREE_ACCELERATION





/***************************************************************
				MBBTree::BBTreeNode
 ***************************************************************/

MBBTree::BBTreeNode::BBTreeNode(MBBTree *tree, int start, int end)
							: startIndex( start ), endIndex( end ), negative( NULL ), positive( NULL )
{
	int numFacesInNode = end - start;

	if ( numFacesInNode < NODE_SIZE_THRESHOLD )
	{
		// Compute the bounding box for the faces in this node
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3 &faceBox = tree->faceBoxes[ tree->faceIndices[faceI] ];
			box.addBox( faceBox );
		}
		box.enlargeForIntersection();
	}
	else
	{
		// Compute the bounding box for the faces in this node, and compute the average
		// of the centres of the face boxes.
		Point3 centre;
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3 &faceBox = tree->faceBoxes[ tree->faceIndices[faceI] ];
			box.addBox( faceBox );
			centre.cumulativeAdd( faceBox.getCentre() );
		}
		box.enlargeForIntersection();
		centre.scale( 1.0 / (double)( end - start ) );

		// Using the average centre as a point on the split plane, compute the number of faces that would
		// end up on the negative, positive, or both sides of the plane with a given axis (X, Y, or Z)
		// used as the split plane normal.
		int negativeCount[3] = { 0, 0, 0 };
		int crossCount[3] = { 0, 0, 0 };
		int positiveCount[3] = { 0, 0, 0 };
		for (int faceI = start; faceI < end; faceI++)
		{
			const BBox3 &faceBox = tree->faceBoxes[ tree->faceIndices[faceI] ];

			for (int axisI = 0; axisI < 3; axisI++)
			{
				if ( faceBox.getLower().get( (Axis)axisI ) > centre.get( (Axis)axisI ) )
				{
					positiveCount[axisI]++;
				}
				else if ( faceBox.getUpper().get( (Axis)axisI ) < centre.get( (Axis)axisI ) )
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
		int bestAxis = 0;
		for (int axisI = 1; axisI < 3; axisI++)
		{
			if ( score[axisI] < score[bestAxis]  ||  score[bestAxis] == -1 )
			{
				bestAxis = axisI;
			}
		}

		// Ensure that we only proceed if the score is 'valid'; an invalid score indicates that splitting
		// cannot improve matters, so don't bother.
		if ( score[bestAxis] != -1 )
		{
			splitAxis = (Axis)bestAxis;

			// Re-order the face index list so that the faces the lie on the negative side of the split plane come
			// before those that lie on the positive side
			// The algorithm works from each end in towards the centre of the list, swapping elements if they
			// are on the wrong side of the 'anchor' point.
			double planeD = centre.get( splitAxis );												// @planeD is the anchor value
			int faceI = start, faceJ = end - 1;														// @faceI is the lower index, @faceJ the upper index
			double faceID = tree->faceBoxes[ tree->faceIndices[faceI] ].getCentre().get( splitAxis );			// @faceID and @faceJD are the values for @faceI and @faceJ
			double faceJD = tree->faceBoxes[ tree->faceIndices[faceJ] ].getCentre().get( splitAxis );
			int anchorIndex = 0;																	// @anchorIndex will be the anchor position
			while ( faceI < faceJ )
			{
				if ( faceID <= planeD )
				{
					// @faceI indexes a face that is on the correct side of the anchor, move on to the next
					faceI++;
					faceID = tree->faceBoxes[ tree->faceIndices[faceI] ].getCentre().get( splitAxis );
				}
				if ( faceJD >= planeD )
				{
					// @faceJ indexes a face that is on the correct side of the anchor, move on to the next
					anchorIndex = faceJ;
					faceJ--;
					faceJD = tree->faceBoxes[ tree->faceIndices[faceJ] ].getCentre().get( splitAxis );
				}
				if ( faceID > planeD  &&  faceJD < planeD )
				{
					// @faceI and @faceJ index faces that are on the wrong side of the anchor; swap them
					std::swap( tree->faceIndices[faceI], tree->faceIndices[faceJ] );
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


			// Only create subtrees if the split does result in splitting the face list into two parts, otherwise
			// you end up with infinite recursion
			if ( start != anchorIndex  &&  anchorIndex != end )
			{
				// Create subtrees
				negative = new BBTreeNode( tree, start, anchorIndex );
				positive = new BBTreeNode( tree, anchorIndex, end );
			}
		}
	}
}

MBBTree::BBTreeNode::BBTreeNode(const BBTreeNode &node)
							: startIndex( node.startIndex ), endIndex( node.endIndex ), box( node.box ), splitAxis( node.splitAxis )
{
	if ( node.negative != NULL )
	{
		negative = new BBTreeNode( *node.negative );
	}
	else
	{
		negative = NULL;
	}

	if ( node.positive != NULL )
	{
		positive = new BBTreeNode( *node.positive );
	}
	else
	{
		positive = NULL;
	}
}


MBBTree::BBTreeNode::~BBTreeNode()
{
	if ( negative != NULL )
	{
		delete negative;
	}
	if ( positive != NULL )
	{
		delete positive;
	}
}



void * MBBTree::BBTreeNode::operator new(size_t sz)
{
	return static_cast<void *>( getPool().allocateRaw() );
}

void MBBTree::BBTreeNode::operator delete(void *data)
{
	getPool().freeRaw( static_cast<MBBTree::BBTreeNode *>( data ) );
}



void MBBTree::BBTreeNode::refresh(MBBTree *tree)
{
	bool bFoundModifiedFace = false;
	for (int i = startIndex; i < endIndex; i++)
	{
		if ( tree->modifiedFaces[ tree->faceIndices[i] ] )
		{
			bFoundModifiedFace = true;
			break;
		}
	}

	if ( bFoundModifiedFace )
	{
		box = BBox3();
		for (int faceI = startIndex; faceI < endIndex; faceI++)
		{
			box.addBox( tree->faceBoxes[ tree->faceIndices[faceI] ] );
		}
		box.enlargeForIntersection();


		if ( positive != NULL )
		{
			positive->refresh( tree );
		}

		if ( negative != NULL )
		{
			negative->refresh( tree );
		}
	}
}


double MBBTree::BBTreeNode::queryCostHeuristic() const
{
	if ( isLeaf() )
	{
		return double( endIndex - startIndex );
	}
	else
	{
		double overallArea = box.getSurfaceArea();
		double positiveArea = positive->box.getSurfaceArea();
		double negativeArea = negative->box.getSurfaceArea();
		double intersectionArea = positive->box.intersection( negative->box ).getSurfaceArea();

		double overallAreaRecip = 1.0 / overallArea;

		double positiveWeight = ( positiveArea + intersectionArea ) * overallAreaRecip;
		double negativeWeight = ( negativeArea + intersectionArea ) * overallAreaRecip;

		return positive->queryCostHeuristic() * positiveWeight  +  negative->queryCostHeuristic() * negativeWeight;
	}
}




bool MBBTree::BBTreeNode::intersects(const Segment3 &seg) const
{
	return box.intersects( seg );
}

Side MBBTree::BBTreeNode::projectIntoScreenSpace(const Projection &projection, BBox2 &box2) const
{
	Side sideOfNearPlane = box.side( projection.getNearClipPlane() );

	if ( sideOfNearPlane == SIDE_NEGATIVE )
	{
		// Behind near plane; not visible
		return SIDE_NEGATIVE;
	}


	Side sideOfFarPlane = box.side( projection.getFarClipPlane() );

	if ( sideOfFarPlane == SIDE_NEGATIVE )
	{
		// Beyond far plane; not visible
		return SIDE_NEGATIVE;
	}

	if ( sideOfNearPlane == SIDE_BOTH  ||  sideOfFarPlane == SIDE_BOTH )
	{
		// Straddles near plane or far plane; use SIDE_BOTH to indicate that we should just process it
		return SIDE_BOTH;
	}


	// Project the box into 2d
	for (int i = 0; i < 8; i++)
	{
		box2.addPoint( projection.getWorldToScreenMatrix().transformHomogeneous2d( box.getPoint( i ) ) );
	}

	box2.enlargeForIntersection();

	return SIDE_POSITIVE;
}

bool MBBTree::BBTreeNode::isWithinRange(const Point2 &screenPoint, double currentSqrDist, const Projection &projection) const
{
	BBox2 box2;
	Side side = projectIntoScreenSpace( projection, box2 );
	if ( side == SIDE_NEGATIVE )
	{
		return false;
	}
	else if ( side == SIDE_BOTH )
	{
		return true;
	}
	else if ( side == SIDE_POSITIVE )
	{
		return box2.sqrDistanceTo( screenPoint )  <  currentSqrDist;
	}
	else
	{
		return true;
	}
}

bool MBBTree::BBTreeNode::intersects(const Brush &brush, const Point3 &brushPosition) const
{
	return brush.intersectsBBox( brushPosition, box );
}


bool MBBTree::BBTreeNode::isLeaf() const
{
	return negative == NULL  &&  positive == NULL;
}



Pool<MBBTree::BBTreeNode> & MBBTree::BBTreeNode::getPool()
{
	static Pool<MBBTree::BBTreeNode> nodePool;
	return nodePool;
}




/***************************************************************
				MBBTree::TraversalState
 ***************************************************************/

MBBTree::TraversalState::TraversalState() : bInitialised( false ), numFaces( 0 )
{
}




/***************************************************************
				MBBTree
 ***************************************************************/

MBBTree::MBBTree(MMesh *mesh)
				: mesh( mesh ), bHasModifiedFaces( false ), bTopologyModified( false ), initialCost( 0.0 ), root( NULL )
{
}

MBBTree::~MBBTree()
{
	if ( root != NULL )
	{
		delete root;
	}
}


void MBBTree::copyFrom(MBBTree *tree)
{
	if ( root != NULL )
	{
		delete root;
		root = NULL;
	}
	faceBoxes = tree->faceBoxes;
	faceIndices = tree->faceIndices;
	modifiedFaces = tree->modifiedFaces;
	bHasModifiedFaces = tree->bHasModifiedFaces;
	bTopologyModified = tree->bTopologyModified;
	initialCost = tree->initialCost;
	if ( tree->root != NULL )
	{
		root = new BBTreeNode( *tree->root );
	}
	else
	{
		root = NULL;
	}
}


void MBBTree::faceModified(int faceIndex)
{
	if ( faceIndex < modifiedFaces.size() )
	{
		modifiedFaces.set( faceIndex, true );
	}
	bHasModifiedFaces = true;
}

void MBBTree::topologyModified()
{
	bTopologyModified = true;
}


void MBBTree::rebuild()
{
	// Destroy the existing tree
	if ( root != NULL )
	{
		delete root;
		root = NULL;
	}

	// Initialise the lists of face bounding boxes and indices
	int numFaces = mesh->getFaces().size();

	faceBoxes.resize( numFaces );
	faceIndices.clear();
	faceIndices.reserve( numFaces );


	if ( numFaces > 0 )
	{
		for (int faceI = 0; faceI < numFaces; faceI++)
		{
			MFace *face = mesh->getFaces()[faceI];
			if ( !face->isDestroyed() )
			{
				faceBoxes[faceI] = face->computeBBox();
				faceIndices.push_back( faceI );
			}
			else
			{
				faceBoxes[faceI] = BBox3();
			}
		}

		modifiedFaces.resize( numFaces );
		modifiedFaces.zero();
		bHasModifiedFaces = false;
		bTopologyModified = false;


		// Create the tree
		root = new BBTreeNode( this, 0, faceIndices.size() );


		// Compute the initial query cost heuristic
		initialCost = root->queryCostHeuristic();
	}
	else
	{
		root = NULL;
		initialCost = 0.0;
		modifiedFaces.zero();
		bHasModifiedFaces = false;
		bTopologyModified = false;
	}
}

void MBBTree::refresh()
{
	// Get the number of faces in the mesh
	int numFaces = mesh->getFaces().size();

	if ( faceBoxes.size() != numFaces )
	{
		// If this doesn't match the size of the face box array, rebuild
		rebuild();
	}
	else
	{
		if ( numFaces > 0  &&  ( bHasModifiedFaces  ||  bTopologyModified ) )
		{
			// Attempt to refresh
			for (int faceI = 0; faceI < numFaces; faceI++)
			{
				if ( modifiedFaces[faceI]  ||  bTopologyModified )
				{
					MFace *face = mesh->getFaces()[faceI];
					if ( !face->isDestroyed() )
					{
						faceBoxes[faceI] = face->computeBBox();
					}
					else
					{
						faceBoxes[faceI] = BBox3();
					}
				}
			}


			root->refresh( this );

			// Compute a query cost heuristic
			double cost = root->queryCostHeuristic();


			// If the new cost is above the threshold, rebuild the tree
			if ( cost  >  ( initialCost * REBUILD_COST_THRESHOLD ) )
			{
				rebuild();
				// Don't need to zero @modifiedFaces or set @bHasModifiedFaces to false since this is done in rebuild()
			}
			else
			{
				modifiedFaces.zero();
				bHasModifiedFaces = false;
				bTopologyModified = false;
			}
		}
	}
}



bool MBBTree::raytrace(const Segment3 &seg, TraversalState &state, const int *&start, const int *&end) const
{
#ifdef DISABLE_BBTREE_ACCELERATION
	// Non-accelerated tracing
	if ( !state.bInitialised )
	{
		start = faceIndices.begin();
		end = faceIndices.end();
		state.bInitialised = true;
		return true;
	}
	else
	{
		return false;
	}
#endif
	// This is the a binary tree traversal algorithm modified so that it stops at each leaf node, and can be restarted with a subsequent call
	Vector3 segDir = seg.getDirection();


	// If the state is not initialised yet, initialise it
	if ( !state.bInitialised )
	{
		if ( root != NULL )
		{
			if ( root->intersects( seg ) )
			{
				state.nodeStack.push_back( TraversalState::Entry( root ) );
			}
		}

		state.bInitialised = true;
	}


	while ( state.nodeStack.size() > 0 )
	{
		TraversalState::Entry &top = state.nodeStack.back();

		if ( top.node->isLeaf() )
		{
			// The node is a leaf; process the faces
			start = faceIndices.begin() + top.node->startIndex;
			end = faceIndices.begin() + top.node->endIndex;
			// Pop the node off the stack
			state.nodeStack.pop_back();
			// Accumulate face count
			state.numFaces += ( top.node->endIndex - top.node->startIndex );
			// Return true; something to process
			return true;
		}
		else
		{
			// The node is NOT a leaf
			if ( top.stage == TraversalState::STAGE_START )
			{
				// Start stage
				// Decide on the order in which to process the child nodes.
				// Process the first child

				// Use the split axis to determine which order to process the child nodes in
				top.order = segDir.get( top.node->splitAxis )  >  0.0  ?  TraversalState::ORDER_NEG_POS  :  TraversalState::ORDER_POS_NEG;

				// Move on to second node stage
				top.stage = TraversalState::STAGE_SECOND_NODE;

				// Get the first child node
				BBTreeNode *firstNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->negative  :  top.node->positive;

				// Add it to the stack if it intersects the segment
				if ( firstNode->intersects( seg ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( firstNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_SECOND_NODE )
			{
				// Stage 1.
				// Process the second child

				// Get the second child node
				BBTreeNode *secondNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->positive  :  top.node->negative;

				top.stage = TraversalState::STAGE_FINISHED;

				// Add it to the stack if it intersects the segment
				if ( secondNode->intersects( seg ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( secondNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_FINISHED )
			{
				// Stage 2.
				// Finished

				// Pop the node off the stack
				state.nodeStack.pop_back();
			}
		}
	}

	return false;
}




bool MBBTree::findClosestScreenPoint(const Point2 &screenPoint, double currentSqrDist, const Projection &projection, TraversalState &state, const int *&start, const int *&end) const
{
	// This is the a binary tree traversal algorithm modified so that it stops at each leaf node, and can be restarted with a subsequent call


	// If the state is not initialised yet, initialise it
	if ( !state.bInitialised )
	{
		if ( root != NULL )
		{
			state.nodeStack.push_back( TraversalState::Entry( root ) );
		}
		state.bInitialised = true;
	}


	while ( state.nodeStack.size() > 0 )
	{
		TraversalState::Entry &top = state.nodeStack.back();

		if ( top.node->isLeaf() )
		{
			// The node is a leaf; process the faces
			start = faceIndices.begin() + top.node->startIndex;
			end = faceIndices.begin() + top.node->endIndex;
			// Pop the node off the stack
			state.nodeStack.pop_back();
			// Accumulate face count
			state.numFaces += ( top.node->endIndex - top.node->startIndex );
			// Return true; something to process
			return true;
		}
		else
		{
			// The node is NOT a leaf
			if ( top.stage == TraversalState::STAGE_START )
			{
				// Start stage
				// Decide on the order in which to process the child nodes.
				// Process the first child

				// Project the centres of the bounding boxes of both children to determine which child to process first
				Point3 centreNeg = top.node->negative->box.getCentre();
				Point3 centrePos = top.node->positive->box.getCentre();
				if ( centreNeg >= projection.getNearClipPlane()  &&  centrePos >= projection.getNearClipPlane() )
				{
					// Both points are beyond the near clip plane
					// Project
					Point3 centreNeg2 = projection.transformWorldToScreen( centreNeg );
					Point3 centrePos2 = projection.transformWorldToScreen( centrePos );
					// Determine their distances to @screenPoint
					double sqrDistNeg = centreNeg2.sqrDistanceTo( screenPoint );
					double sqrDistPos = centrePos2.sqrDistanceTo( screenPoint );
					// Process the closest node first
					top.order = sqrDistNeg < sqrDistPos  ?  TraversalState::ORDER_NEG_POS  :  TraversalState::ORDER_POS_NEG;
				}
				else
				{
					top.order = TraversalState::ORDER_NEG_POS;
				}

				// Move on to second node stage
				top.stage = TraversalState::STAGE_SECOND_NODE;

				// Get the first child node
				BBTreeNode *firstNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->negative  :  top.node->positive;

				// Add it to the stack if it intersects the segment
				if ( firstNode->isWithinRange( screenPoint, currentSqrDist, projection ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( firstNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_SECOND_NODE )
			{
				// Stage 1.
				// Process the second child

				// Get the second child node
				BBTreeNode *secondNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->positive  :  top.node->negative;

				top.stage = TraversalState::STAGE_FINISHED;

				// Add it to the stack if it intersects the segment
				if ( secondNode->isWithinRange( screenPoint, currentSqrDist, projection ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( secondNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_FINISHED )
			{
				// Stage 2.
				// Finished

				// Pop the node off the stack
				state.nodeStack.pop_back();
			}
		}
	}

	return false;
}




bool MBBTree::brush(const Brush &b, const Point3 &brushPosition, TraversalState &state, const int *&start, const int *&end) const
{
	// This is the a binary tree traversal algorithm modified so that it stops at each leaf node, and can be restarted with a subsequent call


	// If the state is not initialised yet, initialise it
	if ( !state.bInitialised )
	{
		if ( root != NULL )
		{
			if ( root->intersects( b, brushPosition ) )
			{
				state.nodeStack.push_back( TraversalState::Entry( root ) );
			}
		}

		state.bInitialised = true;
	}


	while ( state.nodeStack.size() > 0 )
	{
		TraversalState::Entry &top = state.nodeStack.back();

		if ( top.node->isLeaf() )
		{
			// The node is a leaf; process the faces
			start = faceIndices.begin() + top.node->startIndex;
			end = faceIndices.begin() + top.node->endIndex;
			// Pop the node off the stack
			state.nodeStack.pop_back();
			// Accumulate face count
			state.numFaces += ( top.node->endIndex - top.node->startIndex );
			// Return true; something to process
			return true;
		}
		else
		{
			// The node is NOT a leaf
			if ( top.stage == TraversalState::STAGE_START )
			{
				// Start stage
				// Decide on the order in which to process the child nodes.
				// Process the first child

				// Use the split axis to determine which order to process the child nodes in
				double sqrDistNeg = top.node->negative->box.getCentre().sqrDistanceTo( brushPosition );
				double sqrDistPos = top.node->positive->box.getCentre().sqrDistanceTo( brushPosition );
				top.order = sqrDistNeg < sqrDistPos  ?  TraversalState::ORDER_NEG_POS  :  TraversalState::ORDER_POS_NEG;

				// Move on to second node stage
				top.stage = TraversalState::STAGE_SECOND_NODE;

				// Get the first child node
				BBTreeNode *firstNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->negative  :  top.node->positive;

				// Add it to the stack if it intersects the segment
				if ( firstNode->intersects( b, brushPosition ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( firstNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_SECOND_NODE )
			{
				// Stage 1.
				// Process the second child

				// Get the second child node
				BBTreeNode *secondNode = top.order == TraversalState::ORDER_NEG_POS  ?  top.node->positive  :  top.node->negative;

				top.stage = TraversalState::STAGE_FINISHED;

				// Add it to the stack if it intersects the segment
				if ( secondNode->intersects( b, brushPosition ) )
				{
					state.nodeStack.push_back( TraversalState::Entry( secondNode ) );
				}
			}
			else if ( top.stage == TraversalState::STAGE_FINISHED )
			{
				// Stage 2.
				// Finished

				// Pop the node off the stack
				state.nodeStack.pop_back();
			}
		}
	}

	return false;
}



#endif //MBBTREE_CPP__
