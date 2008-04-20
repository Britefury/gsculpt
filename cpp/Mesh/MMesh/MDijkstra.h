//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MDIJKSTRA_H__
#define MDIJKSTRA_H__

#include <Util/Array.h>



//
// MDijkstraNode
//

template <typename Element, typename PathSegment> class MDijkstraNode
{
public:
	double cumulativeCost;
	int heapIndex;
	Element *previous;
	PathSegment *pathSeg;

	typedef MDijkstraNode<Element, PathSegment> Self;


	inline MDijkstraNode(double cumulativeCost, Element *previous, PathSegment *pathSeg = NULL)
					: cumulativeCost( cumulativeCost ), heapIndex( -1 ), previous( previous ), pathSeg( pathSeg )
	{
	}


	inline void buildPath(Element *end, Array<PathSegment*> &path) const
	{
		Element *element = end;

		while ( element != NULL )
		{
			if ( element->getDijkstraNode()->pathSeg != NULL )
			{
				path.push_back( element->getDijkstraNode()->pathSeg );
			}
			element = element->getDijkstraNode()->previous;
		}

		path.reverse();
	}
};



//
// MDijkstraHeap
//

template <typename Element> class MDijkstraHeap
{
private:
	Array<Element*> heap;



public:
	inline MDijkstraHeap()
	{
	}


	void add(Element *element)
	{
		heap.push_back( element );
		element->getDijkstraNode()->heapIndex = heap.size() - 1;
		siftUp( heap.size() - 1 );
	}

	Element * removeMinCost()
	{
		Element *result = heap[0];
		result->getDijkstraNode()->heapIndex = -1;
		heap.front() = heap.back();
		heap.pop_back();
		siftDown( 0 );
		return result;
	}

	void costChanged(Element *element)
	{
		int heapIndex = element->getDijkstraNode()->heapIndex;
		double cost = cumulativeCost( heapIndex );


		if ( heapIndex != 0 )
		{
			if ( cost < cumulativeCost( parent( heapIndex ) ) )
			{
				siftUp( heapIndex );
				return;
			}
		}
		else
		{
			int left = leftChild( heapIndex );
			int right = rightChild( heapIndex );

			if ( left < heap.size() )
			{
				if ( cost < cumulativeCost( left ) )
				{
					siftDown( heapIndex );
					return;
				}
			}

			if ( right < heap.size() )
			{
				if ( cost < cumulativeCost( right ) )
				{
					siftDown( heapIndex );
					return;
				}
			}
		}
	}


	bool isEmpty() const
	{
		return heap.isEmpty();
	}



	int parent(int i)
	{
		return ( i - 1 ) / 2;
	}

	int leftChild(int i)
	{
		return i * 2 + 1;
	}

	int rightChild(int i)
	{
		return i * 2 + 2;
	}

	void swapElements(int i, int j)
	{
		std::swap( heap[i]->getDijkstraNode()->heapIndex, heap[j]->getDijkstraNode()->heapIndex );
		std::swap( heap[i], heap[j] );
	}

	double cumulativeCost(int i)
	{
		return heap[i]->getDijkstraNode()->cumulativeCost;
	}

	int siftUp(int i)
	{
		while ( i != 0  &&  cumulativeCost( i )  <  cumulativeCost( parent( i ) ) )
		{
			swapElements( i, parent( i ) );
			i = parent( i );
		}
		return i;
	}

	int siftDown(int i)
	{
		int c = leftChild( i );
		while ( c < ( heap.size() - 1 ) )
		{
			// Ensure that @c is the index of the least cost child if @i
			if ( cumulativeCost( c + 1 )  <  cumulativeCost( c ) )
			{
				c = c + 1;
			}

			if ( cumulativeCost( i )  >  cumulativeCost( c ) )
			{
				swapElements( i, c );
			}
			else
			{
				return i;
			}

			i = c;
			c = leftChild( i );
		}

		if ( c == ( heap.size() - 1 )  &&  ( cumulativeCost( i )  >  cumulativeCost( c ) ) )
		{
			swapElements( i, c );
			return c;
		}
		else
		{
			return i;
		}
	}
};



#endif
