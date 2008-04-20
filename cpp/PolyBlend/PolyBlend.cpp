//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************


/************************************************************************

 Polyline blending system is an implementation of the algorithms
 presented in 'Optimal Surface Reconstruction from Planar Contours'
 by Fuchs et el, 1997; Communications of the ACM 1977, Vol 20 #10

 ************************************************************************/
#ifndef POLYBLEND_CPP__
#define POLYBLEND_CPP__

#include <stdio.h>

#include <math.h>

#include <Math/index.h>
#include <Math/clamp.h>
#include <Math/lerp.h>

#include <Util/gsassert.h>
#include <Util/Pool.h>

#include <PolyBlend/PolyBlend.h>




/****************************************************************************
					PolyBlendGraphPoint
 A point on the polyline blending graph.
 ****************************************************************************/

class PolyBlendGraphPoint
{
public:
	int p0Index, p1Index;


	inline PolyBlendGraphPoint(int p0Index, int p1Index)
							: p0Index( p0Index ), p1Index( p1Index )
	{
	}
};







/****************************************************************************
						PolyBlendContext
 Polyline blending context class. Contains pointers to the polylines to blend
 and blending parameters.
 ****************************************************************************/

/*double triArea(const Point3 &a, const Point3 &b, const Point3 &c)
{
	return ( c - a ).cross( b - a ).length();
}*/

class PolyBlendContext
{
public:
	const BlendingPolyline *poly0, *poly1;
	const PolyBlend *params;



	//
	// Constructor
	//
	inline PolyBlendContext(const BlendingPolyline *poly0, const BlendingPolyline *poly1, const PolyBlend *params)
						: poly0( poly0 ), poly1( poly1 ), params( params )
	{
	}



/*	//
	// Compute the cost of blending the edges defined by @graphPoint0 and @graphPoint1
	//
	double computeCost(const PolyBlendGraphPoint &graphPoint0, const PolyBlendGraphPoint &graphPoint1) const
	{
		double penalty = 0.0;
		if ( graphPoint0.p0Index == graphPoint1.p0Index  or  graphPoint0.p1Index == graphPoint1.p1Index )
		{
			penalty = params->getCollapsePenalty();
		}

		const Point3 &a = poly0->at( graphPoint0.p0Index );
		const Point3 &b = poly1->at( graphPoint0.p1Index );
		const Point3 &c = poly0->at( graphPoint1.p0Index );
		const Point3 &d = poly1->at( graphPoint1.p1Index );

		return fabs( triArea( a, b, c ) + triArea( b, d, c ) + triArea( a, b, d ) + triArea( a, d, c ) ) * 0.25  +  penalty;
	}*/


	//
	// Compute the cost of blending the edges defined by @graphPoint0 and @graphPoint1
	//
	double computeCost(const PolyBlendGraphPoint &graphPoint0, const PolyBlendGraphPoint &graphPoint1) const
	{
		int x0 = graphPoint0.p0Index;
		int x1 = graphPoint1.p0Index;
		int y0 = graphPoint0.p1Index;
		int y1 = graphPoint1.p1Index;

		Vector3 u, v;
		Vector3 iVector, jVector;
		double area = 0.0;

		//there are two ways to triangulate a quadrilateral:
		//compute the average of the areas which result from the 2 ways


		if ( x0 != x1 )
		{
			u = poly0->at( x1 ) - poly0->at( x0 );
			v = poly1->at( y0 ) - poly0->at( x0 );
			iVector = u;

			area += u.cross( v ).length();
		}

		if ( y0 != y1 )
		{
			u = poly0->at( x1 ) - poly1->at( y1 );
			v = poly1->at( y0 ) - poly1->at( y1 );

			area += u.cross( v ).length();
		}


		if ( y0 != y1 )
		{
			u = poly0->at( x0 ) - poly1->at( y0 );
			v = poly1->at( y1 ) - poly1->at( y0 );
			jVector = v;

			area += u.cross( v ).length();
		}

		if ( x0 != x1 )
		{
			u = poly0->at( x0 ) - poly0->at( x1 );
			v = poly1->at( y1 ) - poly0->at( x1 );

			area += u.cross( v ).length();
		}


		area *= 0.25;

		double penalty = 0.0;


		//see if it is a triangle
		if ( x0 == x1  ||  y0 == y1 )
		{
			penalty = params->getCollapsePenalty();
		}
		else
		{
			// Project @iVector and @jVector to become perpendicular to the two matching line segments
			// from poly0 and poly1

			// Compute the mid points of the two segments
			Point3 midX = lerp( poly0->at( x0 ), poly0->at( x1 ), 0.5 );
			Point3 midY = lerp( poly1->at( y0 ), poly1->at( y1 ), 0.5 );
			// Compute the segment vector
			Vector3 xToY = midY - midX;
			double xToYLength = xToY.length();

			// Attempt to project if @xToY has sufficient length
			Vector3 iVec, jVec;
			if ( xToYLength > EPSILON )
			{
				Vector3 xToYUnit = xToY  *  ( 1.0 / xToYLength );

				iVec = iVector.projectOntoPlane( xToYUnit );
				jVec = jVector.projectOntoPlane( xToYUnit );
			}
			else
			{
				iVec = iVector;
				jVec = jVector;
			}

			// Compute cos(twist_angle)
			double cosTheta = iVec.getNormalised().dot( jVec.getNormalised() );

			double twist = ( 1.0 - cosTheta ) * 0.5;
			double inversion = clampLower( -cosTheta, 0.0 );


			penalty = twist * params->getTwistPenalty()  +  inversion * params->getInversionPenalty();
		}


		penalty *= lerp( 1.0, area, params->getAreaWeighting() );


		return area + penalty;
	}
};






class PolyBlendGraphPathNode;



/****************************************************************************
				PolyBlendGraphPathNodeRef
 A reference to a PolyBlendGraphPathNode object.
 ****************************************************************************/

class PolyBlendGraphPathNodeRef
{
private:
	PolyBlendGraphPathNode *node;

public:
	//
	// Constructors
	//
	inline PolyBlendGraphPathNodeRef() : node( NULL )
	{
	}

	inline PolyBlendGraphPathNodeRef(PolyBlendGraphPathNode *n)
								: node( n )
	{
		ref();
	}

	inline PolyBlendGraphPathNodeRef(const PolyBlendGraphPathNodeRef &n)
								: node( n.node )
	{
		ref();
	}

	inline ~PolyBlendGraphPathNodeRef()
	{
		unref();
	}


	//
	// Assigment operators
	//
	inline PolyBlendGraphPathNodeRef & operator=(const PolyBlendGraphPathNodeRef &n)
	{
		unref();
		node = n.node;
		ref();

		return *this;
	}

	inline PolyBlendGraphPathNodeRef & operator=(PolyBlendGraphPathNode *n)
	{
		unref();
		node = n;
		ref();

		return *this;
	}


	//
	// Pointer de-reference operators
	//
	inline const PolyBlendGraphPathNode * operator->() const
	{
		return node;
	}

	inline PolyBlendGraphPathNode * operator->()
	{
		return node;
	}


	//
	// Comparison operators
	//
	inline bool operator==(const PolyBlendGraphPathNode *n)
	{
		return node == n;
	}

	inline bool operator!=(const PolyBlendGraphPathNode *n)
	{
		return node != n;
	}


	//
	// Pointer access operators
	//
	inline operator const PolyBlendGraphPathNode *() const
	{
		return node;
	}

	inline operator PolyBlendGraphPathNode *()
	{
		return node;
	}


private:
	//
	// referencing and de-referencing convenience methods; implemented below the PolyBlendGraphPathNode class, as they use its methods
	//
	void ref();
	void unref();
};




/****************************************************************************
				PolyBlendGraphPathNode
 Node in a polyline blending graph.
 ****************************************************************************/

class PolyBlendGraphPathNode : public PolyBlendGraphPoint
{
public:
	double cost;
	PolyBlendGraphPathNodeRef prev;


private:
	short refCount;


	//
	// Private constructor; use create() method
	//
	inline PolyBlendGraphPathNode(int p0Index, int p1Index, const PolyBlendContext &context, PolyBlendGraphPathNode *prev)
								: PolyBlendGraphPoint( p0Index, p1Index ),
								cost( 0.0 ), prev( prev ), refCount( 0 )
	{
		if ( this->prev != NULL )
		{
			cost = this->prev->cost + context.computeCost( *this->prev, *this );
		}
	}


	//
	// Allocation methods; objects are stored in a pool
	//
	void * operator new(size_t sz);
	void operator delete(void *data);


	//
	// Increment and decrement reference counts
	//
	void ref()
	{
		refCount++;
	}

	void unref()
	{
		refCount--;

		if ( refCount == 0 )
		{
			delete this;
		}
	}


public:
	//
	// create(); construction method
	//
	inline static PolyBlendGraphPathNode * create(int p0Index, int p1Index, const PolyBlendContext &context, PolyBlendGraphPathNode *prev)
	{
		return new PolyBlendGraphPathNode( p0Index, p1Index, context, prev );
	}


	friend class PolyBlendGraphPathNodeRef;
};


//
// PolyBlendGraphPathNode pool
//
static Pool<PolyBlendGraphPathNode> polyBlendNodePool;



//
// Implementation of PolyBlendGraphPathNode allocation methods
//
void * PolyBlendGraphPathNode::operator new(size_t sz)
{
	return static_cast<void*>( polyBlendNodePool.allocateRaw() );
}

void PolyBlendGraphPathNode::operator delete(void *data)
{
	polyBlendNodePool.freeRaw( static_cast<PolyBlendGraphPathNode*>( data ) );
}


//
// PolyBlendGraphPathNodeRef::ref(); increment reference count convenience method
//
void PolyBlendGraphPathNodeRef::ref()
{
	if ( node != NULL )
	{
		node->ref();
	}
}

//
// PolyBlendGraphPathNodeRef::unref(); decrement reference count convenience method
//
void PolyBlendGraphPathNodeRef::unref()
{
	if ( node != NULL )
	{
		node->unref();
	}
}







/****************************************************************************
					PolyBlendRowNode
 Node in a row that makes up the polyline blending graph.
 ****************************************************************************/

class PolyBlendRowNode
{
public:
	PolyBlendGraphPathNodeRef path;
};



/****************************************************************************
						PolyBlendRow
 Row of PolyBlendRowNode node objects.
 ****************************************************************************/

typedef Array<PolyBlendRowNode> PolyBlendRow;




/****************************************************************************
					PolyBlendRowBounds
 Data structure representing a bounds of a subset of a row.
 ****************************************************************************/

class PolyBlendRowBounds
{
public:
	int lower, upper;


	inline PolyBlendRowBounds(int width = 0)
	{
		lower = width;
		upper = -1;
	}


	inline void addLower(int m)
	{
		lower = std::min( lower, m );
	}

	inline void addUpper(int m)
	{
		upper = std::max( upper, m );
	}
};






/****************************************************************************
				PolyBlendGraphBounds
 Data structure representing a bounds of a subset of a polyline blending
 graph.
 ****************************************************************************/

 class PolyBlendGraphBounds
 {
 private:
 	Array<PolyBlendRowBounds> bounds;

public:
	//
	// Generate the bounds delimiting a subset of a polyline blending graph. The subgraph lies in the region marked out between @minPath and @maxPath.
	// @width and @height are the width and height of the full graph
	// @relativeTo is the x-co-ordinate that the bounds should be relative to.
	//
	PolyBlendGraphBounds(PolyBlendGraphPathNode *minPath, PolyBlendGraphPathNode *maxPath, int width, int height, int relativeTo)
	{
		bounds.resize( height + 1 );
		for (int i = 0; i <= height; i++)
		{
			bounds[i] = PolyBlendRowBounds( width );
		}

		if ( minPath != NULL  &&  maxPath != NULL )
		{
			int index = height;
			int prevX = minPath->p0Index;
			int prevY = minPath->p1Index;
			int offset = 0;

			while ( minPath != NULL )
			{
				int x = minPath->p0Index;
				int y = minPath->p1Index;

				if ( x > prevX )
				{
					offset -= width;
				}
				prevX = x;
				x += offset;
				if ( y != prevY )
				{
					index--;
					prevY = y;
				}
				//printf( "%d -- %d\n", y, index );

				int unwrappedRelativeX = x + width - relativeTo;
				int clamped = clamp( unwrappedRelativeX, 0, width );
				bounds[index].addLower( clamped );
				minPath = minPath->prev;
			}

			gs_assert( index == 0, "PolyBlendGraphBounds::PolyBlendGraphBounds(): after processing minPath, index is not 0\n" );


			index = height;
			prevX = maxPath->p0Index;
			prevY = maxPath->p1Index;
			offset = 0;
			while ( maxPath != NULL )
			{
				int x = maxPath->p0Index;
				int y = maxPath->p1Index;
				if ( x > prevX )
				{
					offset -= width;
				}
				prevX = x;
				x += offset;
				if ( y != prevY )
				{
					index--;
					prevY = y;
				}

				int unwrappedRelativeX = x + width - relativeTo;
				int clamped = clamp( unwrappedRelativeX, 0, width );
				bounds[index].addUpper( clamped );
				maxPath = maxPath->prev;
			}

			gs_assert( index == 0, "PolyBlendGraphBounds::PolyBlendGraphBounds(): after processing maxPath, index is not 0\n" );
		}
	}


	//
	// Access methods
	//
	inline int size() const
	{
		return bounds.size();
	}

	inline const PolyBlendRowBounds & operator[](int i) const
	{
		return bounds[i];
	}

 	inline PolyBlendRowBounds & operator[](int i)
	{
		return bounds[i];
	}

	inline const PolyBlendRowBounds & at(int i) const
	{
		return bounds[i];
	}

 	inline PolyBlendRowBounds & at(int i)
	{
		return bounds[i];
	}
};






//
//	generateInitialRowClosed() - Generate the initial row of a polyline blending graph used to blend two closed polylines.
//

static void generateInitialRowClosed(const PolyBlendContext &context, int startX, const PolyBlendGraphBounds *bounds, PolyBlendRow &row)
{
	int width = context.poly0->size();
	row.clear();
	row.resize( width + 1 );

	int lower, upper;
	if ( bounds != NULL )
	{
		lower = bounds->at( 0 ).lower;
		upper = bounds->at( 0 ).upper;
	}
	else
	{
		lower = 0;
		upper = width;
	}

	PolyBlendGraphPathNode *pathNode = NULL;
	for (int nodeIndex = lower; nodeIndex <= upper; nodeIndex++)
	{
		int x = wrapSingleOverflow( startX + nodeIndex, width );
		pathNode = PolyBlendGraphPathNode::create( x, 0, context, pathNode );
		row[nodeIndex].path = pathNode;
	}
}



//
//	generateInitialRowOpen() - Generate the initial row of a polyline blending graph used to blend two open polylines.
//

static void generateInitialRowOpen(const PolyBlendContext &context, PolyBlendRow &row)
{
	int width = context.poly0->size();
	row.clear();
	row.resize( width );

	PolyBlendGraphPathNode *pathNode = NULL;
	for (int x = 0; x < width; x++)
	{
		pathNode = PolyBlendGraphPathNode::create( x, 0, context, pathNode );
		row[x].path = pathNode;
	}
}



//
//	generateNextRowClosed() - Generate a subsequent row of a polyline blending graph used to blend two closed polylines.
//

static void generateNextRowClosed(const PolyBlendContext &context, PolyBlendRow &prevRow, int rowIndex, int prevRowIndex, int startX, int y, int prevY,
							const PolyBlendGraphBounds *bounds, PolyBlendRow &row)
{
	int width = context.poly0->size();
	row.clear();
	row.resize( width + 1 );


	int lower, upper;
	if ( bounds != NULL )
	{
		lower = bounds->at( rowIndex ).lower;
		upper = bounds->at( rowIndex ).upper;
	}
	else
	{
		lower = 0;
		upper = width;
	}

	for (int nodeIndex = lower; nodeIndex <= upper; nodeIndex++)
	{
		int x = wrapSingleOverflow( startX + nodeIndex, width );

		double northCost = 0.0;
		double northSegCost = context.computeCost( PolyBlendGraphPoint( x, prevY ), PolyBlendGraphPoint( x, y ) );
		PolyBlendGraphPathNode *northPath = prevRow[nodeIndex].path;
		if ( northPath != NULL )
		{
			northCost = northPath->cost + northSegCost;
		}

		PolyBlendGraphPathNode *prevPath = northPath;
		double prevCost = northCost;


		if ( nodeIndex != 0 )
		{
			int prevNodeIndex = nodeIndex - 1;
			int prevX = prevIndex( x, width );

			double northWestCost = 0.0;
			double northWestSegCost = context.computeCost( PolyBlendGraphPoint( prevX, prevY ), PolyBlendGraphPoint( x, y ) );
			PolyBlendGraphPathNode *northWestPath = prevRow[prevNodeIndex].path;
			if ( northWestPath != NULL )
			{
				northWestCost = northWestPath->cost + northWestSegCost;

				if ( northWestCost < prevCost  ||  prevPath == NULL )
				{
					prevPath = northWestPath;
					prevCost = northWestCost;
				}
			}



			if ( nodeIndex != lower )
			{
				double westCost = 0.0;
				double westSegCost = context.computeCost( PolyBlendGraphPoint( prevX, y ), PolyBlendGraphPoint( x, y ) );
				PolyBlendGraphPathNode *westPath = row[prevNodeIndex].path;
				if ( westPath != NULL )
				{
					westCost = westPath->cost + westSegCost;

					if ( westCost < prevCost  ||  prevPath == NULL )
					{
						prevPath = westPath;
						prevCost = westCost;
					}
				}
			}
		}

		gs_assert( prevPath != NULL, "generateNextRowClosed(): prevPath is NULL\n" );

		row[nodeIndex].path = PolyBlendGraphPathNode::create( x, y, context, prevPath );
	}
}



//
//	generateNextRowOpen() - Generate a subsequent row of a polyline blending graph used to blend two open polylines.
//

static void generateNextRowOpen(const PolyBlendContext &context, PolyBlendRow &prevRow, int y, int prevY, PolyBlendRow &row)
{
	int width = context.poly0->size();
	row.clear();
	row.resize( width );


	for (int x = 0; x < width; x++)
	{
		double northSegCost = context.computeCost( PolyBlendGraphPoint( x, prevY ), PolyBlendGraphPoint( x, y ) );
		PolyBlendGraphPathNode *northPath = prevRow[x].path;
		double northCost = northPath->cost + northSegCost;

		PolyBlendGraphPathNode *prevPath = northPath;
		double prevCost = northCost;


		if ( x != 0 )
		{
			int prevX = x - 1;

			double northWestSegCost = context.computeCost( PolyBlendGraphPoint( prevX, prevY ), PolyBlendGraphPoint( x, y ) );
			PolyBlendGraphPathNode *northWestPath = prevRow[prevX].path;
			double northWestCost = northWestPath->cost + northWestSegCost;

			if ( northWestCost < prevCost  ||  prevPath == NULL )
			{
				prevPath = northWestPath;
				prevCost = northWestCost;
			}



			double westSegCost = context.computeCost( PolyBlendGraphPoint( prevX, y ), PolyBlendGraphPoint( x, y ) );
			PolyBlendGraphPathNode *westPath = row[prevX].path;
			double westCost = westPath->cost + westSegCost;

			if ( westCost < prevCost  ||  prevPath == NULL )
			{
				prevPath = westPath;
				prevCost = westCost;
			}
		}

		gs_assert( prevPath != NULL, "generateNextRowOpen(): prevPath is NULL\n" );

		row[x].path = PolyBlendGraphPathNode::create( x, y, context, prevPath );
	}
}




//
//	computePathClosed() - Compute the minimum cost path spanning the graph, staring at x-co-ordinate @startX
//

static PolyBlendGraphPathNodeRef computePathClosed(const PolyBlendContext &context, int startX)
{
	int height = context.poly1->size();

	PolyBlendRow row0, row1;

	generateInitialRowClosed( context, startX, NULL, row0 );

	PolyBlendRow *prevRow = &row0;
	PolyBlendRow *row = &row1;
	for (int i = 1; i < height; i++)
	{
		generateNextRowClosed( context, *prevRow, i, i - 1, startX, i, i - 1, NULL, *row );
		std::swap( prevRow, row );
	}

	generateNextRowClosed( context, *prevRow, height, height - 1, startX, 0, height - 1, NULL, *row );

	return row->back().path;
}

//
//	computePathBoundedClosed() - Compute the minimum cost path spanning the graph, staring at x-co-ordinate @startX.
//								Only nodes in the subgraph delimited by @minPath and @maxPath are searched.
//

static PolyBlendGraphPathNodeRef computePathBoundedClosed(const PolyBlendContext &context, int startX, PolyBlendGraphPathNodeRef minPath, PolyBlendGraphPathNodeRef maxPath)
{
	int width = context.poly0->size();
	int height = context.poly1->size();

	PolyBlendGraphBounds bounds( minPath, maxPath, width, height, startX );

	PolyBlendRow row0, row1;

	generateInitialRowClosed( context, startX, &bounds, row0 );

	PolyBlendRow *prevRow = &row0;
	PolyBlendRow *row = &row1;
	for (int i = 1; i < height; i++)
	{
		generateNextRowClosed( context, *prevRow, i, i - 1, startX, i, i - 1, &bounds, *row );
		std::swap( prevRow, row );
	}

	generateNextRowClosed( context, *prevRow, height, height - 1, startX, 0, height - 1, &bounds, *row );

	return row->back().path;
}

//
//	findBestPathBoundedClosed() - Find the minimum cost spanning path that spans the subgraph delimited by @minPath and @maxPath
//

static PolyBlendGraphPathNodeRef findBestPathBoundedClosed(const PolyBlendContext &context, int lowerX, int upperX,
														PolyBlendGraphPathNodeRef minPath, PolyBlendGraphPathNodeRef maxPath)
{
	int midX = ( lowerX + upperX )  /  2;

	if ( midX > lowerX )
	{
		PolyBlendGraphPathNodeRef midPath = computePathBoundedClosed( context, midX, minPath, maxPath );
		PolyBlendGraphPathNodeRef bestLower = findBestPathBoundedClosed( context, lowerX, midX, minPath, midPath );
		PolyBlendGraphPathNodeRef bestUpper = findBestPathBoundedClosed( context, midX, upperX, midPath, maxPath );
		if ( bestLower->cost < bestUpper->cost )
		{
			return bestLower;
		}
		else
		{
			return bestUpper;
		}
	}
	else
	{
		if ( minPath->cost < maxPath->cost )
		{
			return minPath;
		}
		else
		{
			return maxPath;
		}
	}
}


//
//	findBestPath() - Find the minimum cost spanning path that spans the graph
//

static PolyBlendGraphPathNodeRef findBestPathClosed(const PolyBlendContext &context)
{
	int width = context.poly0->size();


	PolyBlendGraphPathNodeRef minPath = computePathClosed( context, 0 );
	PolyBlendGraphPathNodeRef maxPath = computePathClosed( context, width - 1 );
	return findBestPathBoundedClosed( context, 0, width - 1, minPath, maxPath );
}



//
//	findBestPathOpen() - Find the minimum path to blend two open polylines
//

static PolyBlendGraphPathNodeRef findBestPathOpen(const PolyBlendContext &context)
{
	int height = context.poly1->size();

	PolyBlendRow row0, row1;

	generateInitialRowOpen( context, row0 );

	PolyBlendRow *prevRow = &row0;
	PolyBlendRow *row = &row1;
	for (int i = 1; i < height; i++)
	{
		generateNextRowOpen( context, *prevRow, i, i - 1, *row );
		std::swap( prevRow, row );
	}

	return prevRow->back().path;
}




//
//	polyBlendGraphPathToBlendList() - convert a polyline blend graph path (in linked list form) to a list of PolyBlendPoint's
//

static void polyBlendGraphPathToBlendList(PolyBlendGraphPathNodeRef path, PolyBlendPointList &blend, bool invertFlag)
{
	if ( path->prev != NULL )
	{
		polyBlendGraphPathToBlendList( path->prev, blend, invertFlag );
	}

	int x = invertFlag  ?  path->p1Index  :  path->p0Index;
	int y = invertFlag  ?  path->p0Index  :  path->p1Index;
	blend.push_back( PolyBlendPoint( x, y ) );
}



/****************************************************************************
				PolyBlend
 Polyline blending class
 ****************************************************************************/


//
//	PolyBlend::computeBlendPath() - blend two polylines
//

double PolyBlend::computeBlendPath(const BlendingPolyline &poly0, const BlendingPolyline &poly1, PolyBlendPointList &blend) const
{
	gs_assert( poly0.isClosed() == poly1.isClosed(), "PolyBlend::computeBlendPath(): both polylines must be closed, or both must be open\n" );


	// Normalise
	BlendingPolyline polyline0 = poly0;
	BlendingPolyline polyline1 = poly1;
	BlendingPolyline::scaleToUnit( polyline0, polyline1 );


	//
	// Blending algorithm for closed polylines is O( log2(M) . M . N )
	// Where M = poly0.size()  and  N = poly1.size()
	//
	// Blending algorithm for open polylines is O(M.N)
	// However, it will be slightly more efficient if poly0 and poly1 are chosen such that M is the smaller of the two
	//
	// So: ensure that poly0.size() < poly1.size()  e.g. swap if necessary

	const BlendingPolyline *p0 = NULL, *p1 = NULL;
	bool invertFlag = polyline0.size()  >  polyline1.size();

	p0 = invertFlag  ?  &polyline1  :  &polyline0;
	p1 = invertFlag  ?  &polyline0  :  &polyline1;

	// Build the PolyBlendContext structure
	PolyBlendContext context( p0, p1, this );

	// Generate the blending path
	PolyBlendGraphPathNodeRef path;

	if ( poly0.isClosed() )
	{
		//Use closed method
		path = findBestPathClosed( context );
	}
	else
	{
		//Use open method
		path = findBestPathOpen( context );
	}

	// Convert the blending path to the blend point list
	blend.clear();
	polyBlendGraphPathToBlendList( path, blend, invertFlag );

	return path->cost;
}


//
//	PolyBlend::lerpPolylines() - interpolate between two polylines
//

void PolyBlend::lerpPolylines(const BlendingPolyline &poly0, const BlendingPolyline &poly1, const PolyBlendPointList &blendPath, double t, BlendingPolyline &result)
{
	gs_assert( poly0.isClosed() == poly1.isClosed(), "PolyBlend::lerpPolylines(): both polylines must be closed, or both must be open\n" );

	result.resize( blendPath.size() );

	for (int i = 0; i < blendPath.size(); i++)
	{
		result[i] = lerp( poly0[ blendPath[i].p0Index ], poly1[ blendPath[i].p1Index ], t );
	}
}


//
//	PolyBlend::lerpPolylines() - interpolate between two polylines
//

void PolyBlend::generateTextureCoords(const BlendingPolyline &p0, const BlendingPolyline &p1, const PolyBlendPointList &blendPath, Array<double> &t0, Array<double> &t1)
{
	t0.resize( blendPath.size() );
	t1.resize( blendPath.size() );

	double l0 = 0.0, l1 = 0.0;
	t0[0] = t1[0] = 0.0;

	int blendIPrev = 0;
	for (int blendI = 1; blendI < blendPath.size(); blendI++)
	{
		int x0 = blendPath[blendIPrev].p0Index;
		int y0 = blendPath[blendIPrev].p1Index;
		int x1 = blendPath[blendI].p0Index;
		int y1 = blendPath[blendI].p1Index;

		l0 += p0[x1].distanceTo( p0[x0] );
		l1 += p1[y1].distanceTo( p1[y0] );

		t0[blendI] = l0;
		t1[blendI] = l1;

		blendIPrev = blendI;
	}

	double oneOverL0 = 1.0 / l0;
	double oneOverL1 = 1.0 / l1;

	for (int blendI = 1; blendI < blendPath.size(); blendI++)
	{
		t0[blendI] *= oneOverL0;
		t1[blendI] *= oneOverL1;
	}
}




#endif
