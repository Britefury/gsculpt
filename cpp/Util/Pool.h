//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POOL_H__
#define POOL_H__

#include <memory.h>

#include <Util/Array.h>
#include <Util/LargeStack.h>


// #define __POOLUSEMALLOC__




template <typename T> class Pool
{
private:
	class SubPool
	{
	private:
		T *data;

	public:
		SubPool *next;


	public:
		SubPool(int numItems)
				: next( NULL )
		{
			int bytes = sizeof( T )  *  numItems;
			data = reinterpret_cast<T*>( ::malloc( bytes ) );
		}

		~SubPool()
		{
			::free( reinterpret_cast<void*>( data ) );
		}


		T * getItem(int itemIndex)
		{
			return data + itemIndex;
		}
	};




	int currentSubPoolSize;
	SubPool *subPoolsHead, *subPoolsTail;
	LargeStack<T*> itemStack;




private:
	void subPoolListAppend(SubPool *p)
	{
		if ( subPoolsHead == NULL )
		{
			//empty; head and tail = node
			subPoolsHead = subPoolsTail = p;
		}
		else
		{
			//append
			subPoolsTail->next = p;
			subPoolsTail = p;
		}
	}

	void addSubPoolItems(SubPool *p, int numItems)
	{
		for (int i = numItems - 1; i >= 0; i--)
		{
			itemStack.push( p->getItem( i ) );
		}
	}

	void addSubPool(int requiredItems = 0)
	{
		//DETERMINE THE SIZE OF THE NEW SUBPOOL
		int newSubPoolSize = 0;
		if ( currentSubPoolSize == 0 )
		{
			//start at 4
			newSubPoolSize = 4;
		}
		else
		{
			newSubPoolSize = currentSubPoolSize * 2;
		}

		//ensure that the required number of blocks are made available
		newSubPoolSize = std::max( newSubPoolSize, requiredItems );

		//update current size
		currentSubPoolSize = newSubPoolSize;


		//CREATE THE SUBPOOL AND APPEND IT TO THE LIST
		//create the new subpool list node
		SubPool *p = new SubPool( newSubPoolSize );

		//append to the list
		subPoolListAppend( p );


		//ADD THE ITEMS TO THE ITEM STACK
		addSubPoolItems( p, newSubPoolSize );
	}




public:
	Pool()
	{
		currentSubPoolSize = 0;
		subPoolsHead = subPoolsTail = NULL;
	}

	~Pool()
	{
		SubPool *n = subPoolsHead;
		SubPool *next;

		//iterate through the list and delete the subpools
		while ( n != NULL )
		{
			next = n->next;
			delete n;
			n = next;
		}
	}


	T * allocateRaw()
	{
#ifdef __POOLUSEMALLOC__
		return static_cast<T*>( ::malloc( sizeof( T ) ) );
#else
		//if there are no blocks in the stack, add a new subpool
		if ( itemStack.isEmpty() )
		{
			addSubPool();
		}

		T * item;
		itemStack.pop( item );

		return item;
#endif
	}

	T * allocate()
	{
		T *item = allocateRaw();
		new( item ) T();
		return item;
	}


	void freeRaw(T *item)
	{
#ifdef __POOLUSEMALLOC__
		::free( static_cast<void*>( item ) );
#else
		itemStack.push( item );
#endif
	}

	void free(T *item)
	{
		item->~T();
		freeRaw( item );
	}


	void freeItems(const Array<T*> &items)
	{
		for (int i = 0; i < items.size(); i++)
		{
			free( items[i] );
		}
	}


	void reserve(int numItems)
	{
#ifdef __POOLUSEMALLOC__
#else
		int freeItems = itemStack.size();

		if ( freeItems < numItems )
		{
			//need extra space
			int required = numItems - freeItems;
			addSubPool( required );
		}
#endif
	}
};




#endif
