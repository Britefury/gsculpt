//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LARGESTACK_H__
#define LARGESTACK_H__

#include <Util/Array.h>


#define LARGE_STACK_INITIAL_SIZE  4




//LargeStack template class
//Implemented as a linked list of Arrays where each Array has double the
//capacity of the previoud one.

template <typename T> class LargeStack
{
private:
	class SubStack
	{
	private:
		Array<T> data;
		SubStack *prev, *next;


	public:
		SubStack(int size)
		{
			//reserve space
			data.setCapacity( size );
			prev = NULL;
			next = NULL;
		};

		//append to s
		void appendTo(SubStack *s)
		{
			s->next = this;
			prev = s;
		}


		void push(const T &element)
		{
			data.push_back( element );
		}

		void pop(T &element)
		{
			element = data.back();
			data.pop_back();
		}


		T & top()
		{
			return data.back();
		}

		const T & top() const
		{
			return data.back();
		}


		bool isFull() const
		{
			//full if the entire capacity has been used
			return data.size() == data.capacity();
		}

		bool isEmpty() const
		{
			return data.isEmpty();
		}

		bool contains(const T &element) const
		{
			return data.contains( element );
		}

		int size() const
		{
			return data.capacity();
		}


		SubStack * getPrev()
		{
			return prev;
		}


		SubStack * getNext()
		{
			return next;
		}
	};


	//@head is the head of the linked list of substacks
	//@currentTop is the substack in which the current top of stack resides
	SubStack *head, *topSubStack;
	int sz;


	//append a substack to the list
	void addSubStack()
	{
		//create the substack
		SubStack *s = new SubStack( topSubStack->size() * 2 );

		//append it
		s->appendTo( topSubStack );
		topSubStack = s;
	}


public:
	LargeStack()
	{
		//initially empty
		head = NULL;
		topSubStack = NULL;
		sz = 0;
	}

	~LargeStack()
	{
		//iterate through the list of substacks and delete each one
		SubStack *s = head;
		SubStack *next;

		while ( s != NULL )
		{
			next = s->getNext();
			delete s;
			s = next;
		}
	}


	void push(const T &element)
	{
		if ( topSubStack == NULL )
		{
			//topSubStack is NULL: this indicates that the stack is empty
			//initialise by starting at the first substack
			if ( head == NULL )
			{
				//there is no first substack; create one
				head = new SubStack( LARGE_STACK_INITIAL_SIZE );
			}
			topSubStack = head;
		}
		else if ( topSubStack->isFull() )
		{
			//the substack that contains the top of the stack is fill;
			//we need a new one

			//see if there is a substack available;
			//e.g. one emptied by pop operations
			if ( topSubStack->getNext() == NULL )
			{
				//no empty substack available; add a new one
				addSubStack();
			}
			else
			{
				//next one is available
				topSubStack = topSubStack->getNext();
			}
		}

		//push
		topSubStack->push( element );
		sz++;
	}

	void pop(T &element)
	{
		//pop into @element
		topSubStack->pop( element );
		sz--;

		//if @topSubStack points to en empty stack, the element on the top
		//of the stack resides in the previous substack
		if ( topSubStack->isEmpty() )
		{
			//go to the previous stack, if there is one
			if ( topSubStack != head )
			{
				topSubStack = topSubStack->getPrev();
			}
		}
	}


	int size() const
	{
		return sz;
	}


	T & top()
	{
		return topSubStack->top();
	}

	const T & top() const
	{
		return topSubStack->top();
	}


	bool isEmpty() const
	{
		if ( topSubStack == NULL )
		{
			//currentTop is NULL; empty stack
			return true;
		}
		else
		{
			//empty if the current top substack
			return topSubStack == head  &&  topSubStack->isEmpty();
		}
	}


	bool contains(const T &element) const
	{
		SubStack *s = head;

		while ( s != NULL )
		{
			if ( s->contains( element ) )
			{
				return true;
			}

			s = s->getNext();
		}

		return false;
	}
};



#endif
