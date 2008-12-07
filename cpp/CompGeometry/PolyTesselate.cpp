//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYTESSELATE_CPP__
#define POLYTESSELATE_CPP__

#include <Math/index.h>

#include <Math/BBox2.h>

#include <Util/LinkedList.h>

#include <CompGeometry/PolyTesselate.h>




static bool triangleContainsPoint(const Point2 &a, const Point2 &b, const Point2 &c, const Point2 &p, double epsilonSquared)
{
	double i = Point2::areaOfTrianglex2( a, b, p );
	double j = Point2::areaOfTrianglex2( b, c, p );

	bool positiveArea = i > -epsilonSquared  &&  j > -epsilonSquared;
	bool negativeArea = i < epsilonSquared  &&  j < epsilonSquared;

	if ( positiveArea || negativeArea )
	{
		double k = Point2::areaOfTrianglex2( c, a, p );

		return ( positiveArea  &&  k > -epsilonSquared )  ||  ( negativeArea  &&  k < epsilonSquared );
	}
	else
	{
		return false;
	}
}




/************************************************************************
	Polygon tesselation algorithm based on an improved ear
	cutting method in:

	'The Graham Scan triangulates simple polygons' (1991)
	By:
	Xianshu Kong, Hazel Everett and Godfried Toussaint,


	It was explained at the site:

	http://www.mema.ucl.ac.be/~wu/FSA2716-2002/project.html

	which was used as a reference for this implementation.

 ************************************************************************/





class TessPointNode
{
public:
	Point2 point;
	int index;
	bool convexFlag;
	TessPointNode *prev, *next;
	LinkedListNode<TessPointNode> concaveNode;


	inline TessPointNode() : index( -1 ), convexFlag( false ), prev( NULL ), next( NULL )
	{
	}

	inline TessPointNode(const Point2 &point, int index) : point( point ), index( index ), convexFlag( false ), prev( NULL ), next( NULL )
	{
	}


	bool isConvex() const
	{
		return convexFlag;
	}

	bool isConcave() const
	{
		return !convexFlag;
	}


	inline void linkToNext(TessPointNode *to)
	{
		to->prev = this;
		next = to;
	}

	inline void remove()
	{
		prev->next = next;
		next->prev = prev;
		prev = next = NULL;
	}
};



class TessPointNodeConcaveFunctor
{
public:
	static LinkedListNode<TessPointNode> * getNode(TessPointNode *t)
	{
		return &t->concaveNode;
	}
};


typedef LinkedList<TessPointNode, TessPointNodeConcaveFunctor> TessConcaveList;





static bool isTriangleConvex(const Point2 &a, const Point2 &b, const Point2 &c, bool direction, double epsilonSquared)
{
	double areax2 = Point2::areaOfTrianglex2( a, b, c );

	return direction ?  areax2 > epsilonSquared  :  areax2 < -epsilonSquared;
}

static bool isEar(TessConcaveList &concaveVertices, TessPointNode *vertex, bool direction, double epsilonSquared)
{
	if ( concaveVertices.isEmpty() )
	{
		return true;
	}
	else if ( vertex->isConvex() )
	{
		TessPointNode *prev = vertex->prev;
		TessPointNode *next = vertex->next;

		TessPointNode *n = concaveVertices.getHead();

		while ( n != NULL )
		{
			if ( n != prev  &&  n != next )
			{
				if ( triangleContainsPoint( prev->point, vertex->point, next->point, n->point, epsilonSquared ) )
				{
					return false;
				}
			}

			n = n->concaveNode.getNext();
		}


		return true;
	}
	else
	{
		return false;
	}
}

bool tesselate(const Polygon2 &poly, Tesselation &tess)
{
	bool direction = poly.computeAreaX2()  >  0.0;

	BBox2 polyBox( poly );
	double sizeForTolerence = std::max( fabs( polyBox.getLower().x ), fabs( polyBox.getUpper().x ) );
	sizeForTolerence = std::max( std::max( fabs( polyBox.getLower().y ), fabs( polyBox.getUpper().y ) ), sizeForTolerence );
	double epsilon = sizeForTolerence * EPSILON;
	double epsilonSquared = epsilon * epsilon;


	Array<TessPointNode> nodes;
	nodes.reserve( poly.size() );

	int polyPrevI = poly.size() - 1;
	for (int polyI = 0; polyI < poly.size(); polyI++)
	{
		//do not build a node if the edge polyPrevI->polyI is degenerate
		if ( poly[polyI].sqrDistanceTo( poly[polyPrevI] )  >  epsilonSquared )
		{
			nodes.push_back( TessPointNode( poly[polyI], polyI ) );
		}
		polyPrevI = polyI;
	}



	if ( nodes.size() >= 3 )
	{
		// Initialise the polygon list
		TessPointNode *polyHead = &nodes[0];
		int nodePrevI = nodes.size() - 1;
		for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
		{
			nodes[nodePrevI].linkToNext( &nodes[nodeI] );
			nodePrevI = nodeI;
		}



		// Initialise the polygon and concave lists
		TessConcaveList concaveList;
		for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
		{
			TessPointNode *node = &nodes[nodeI];
			bool convex = isTriangleConvex( node->prev->point, node->point, node->next->point, direction, epsilonSquared );
			node->convexFlag = convex;
			if ( !convex )
			{
				concaveList.append( node );
			}
		}



		// Tesselation algorithm
		TessPointNode *prev = polyHead;
		TessPointNode *current = prev->next;
		TessPointNode *next = current->next;
		while ( next != polyHead )
		{
			if ( isEar( concaveList, current, direction, epsilonSquared ) )
			{
				// vertex @current is an ear; add the triange [prev, current, next] to the tesselation
				tess.add( IndexTriangle( prev->index, current->index, next->index ) );
				// remove from the polygon
				current->remove();

				if ( next->isConcave() )
				{
					// @next is marked as concave
					if ( isTriangleConvex( prev->point, next->point, next->next->point, direction, epsilonSquared ) )
					{
						// but it is now convex due to the ear removal; update its flag with the new information and remove from the concave vertex list
						next->convexFlag = true;
						concaveList.remove( next );
					}
				}

				if ( prev->isConcave() )
				{
					// @prev is marked as concave
					if ( isTriangleConvex( prev->prev->point, prev->point, next->point, direction, epsilonSquared ) )
					{
						// but it is now convex due to the ear removal; update its flag with the new information and remove from the concave vertex list
						prev->convexFlag = true;
						concaveList.remove( prev );
					}
				}

				if ( prev == polyHead )
				{
					// @prev is the head of the list; iterate forward
					current = next;
					next = next->next;
				}
				else
				{
					// iterate @current back, and leave @next
					current = prev;
					prev = prev->prev;
				}
			}
			else
			{
				// no ear; iterate forward
				prev = current;
				current = next;
				next = next->next;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}



PolyTesselationResult checkAndTesselatePoly(const Polygon2 &poly, Tesselation &tris)
{
	tris.clear();

	if ( poly.size() < 3 )
	{
		return POLYTESSELATION_COULDNOTTESSELATE;
	}
	else if ( poly.size() == 3 )
	{
		return POLYTESSELATION_POLYISTRIANGLE;
	}

	//perform checks:
	if ( poly.isConvex() )
	{
//		printf( "polyline is convex\n" );
		return POLYTESSELATION_POLYISCONVEX;
	}

	if ( poly.computeAreaX2()  ==  0.0 )	//area != 0.0
	{
//		printf( "polyline area == 0.0\n" );
		return POLYTESSELATION_COULDNOTTESSELATE;
	}

	if ( poly.isSelfIntersecting() )	//not self intersecting
	{
//		printf( "polyline is self intersecting\n" );
		return POLYTESSELATION_COULDNOTTESSELATE;
	}

	if ( !tesselate( poly, tris ) )	//tesselate
	{
//		printf( "could not tesselate polyline\n" );
		return POLYTESSELATION_COULDNOTTESSELATE;
	}



	return POLYTESSELATION_TESSELATED;
}


#endif
