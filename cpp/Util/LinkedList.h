//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINKEDLIST_H__
#define LINKEDLIST_H__




/*************************************************************************
			LinkedListNode
 *************************************************************************/

template <typename ObjectType> class LinkedListNode
{
public:
	ObjectType *prev, *next;


	inline LinkedListNode() : prev( NULL ), next( NULL )
	{
	}


	inline ObjectType * getPrev()
	{
		return prev;
	}

	inline const ObjectType * getPrev() const
	{
		return prev;
	}

	inline ObjectType * getNext()
	{
		return next;
	}

	inline const ObjectType * getNext() const
	{
		return next;
	}
};


template <typename ObjectType, typename NodeFunctor> class LinkedList
{
public:
	typedef LinkedListNode<ObjectType> Node;

private:
	ObjectType *head, *tail;


public:
	inline LinkedList() : head( NULL ), tail( NULL )
	{
	}



	inline ObjectType * getHead()
	{
		return head;
	}

	inline const ObjectType * getHead() const
	{
		return head;
	}

	inline ObjectType * getTail()
	{
		return tail;
	}

	inline const ObjectType * getTail() const
	{
		return tail;
	}


	inline bool isEmpty() const
	{
		return head == NULL;
	}


	inline void append(ObjectType *o)
	{
		if ( head == NULL )
		{
			head = o;
		}

		if ( tail != NULL )
		{
			NodeFunctor::getNode( tail )->next = o;
		}

		NodeFunctor::getNode( o )->prev = tail;
		tail = o;
	}

	inline void remove(ObjectType *o)
	{
		Node *oNode = NodeFunctor::getNode( o );

		if ( oNode->prev != NULL )
		{
			NodeFunctor::getNode( oNode->prev )->next = oNode->next;
		}

		if ( oNode->next != NULL )
		{
			NodeFunctor::getNode( oNode->next )->prev = oNode->prev;
		}

		if ( head == o )
		{
			head = oNode->next;
		}

		if ( tail == o )
		{
			tail = oNode->prev;
		}

		oNode->prev = oNode->next = NULL;
	}
};


//	class NodeFunctor
//	{
//	public:
//		inline static LinkedListNode<NODETYPE> * getNode(NODETYPE *node)
//		{
//			return &node->listNode;
//		}
//	};



#endif
