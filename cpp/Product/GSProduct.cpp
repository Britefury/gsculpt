//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCT_CPP__
#define GSPRODUCT_CPP__

#include <Util/gsassert.h>

#include <Product/GSProduct.h>



static int objectRepCount = 0;
static int productCount = 0;


/***********************************************************************************
										GSProduct::ObjectRep
 ***********************************************************************************/

 RT_IMPLEMENT_TYPE( GSProduct::ObjectRep, RTObject );

GSProduct::ObjectRep::ObjectRep()
{
	objectRepCount++;
	paint3dCount = paintUVCount = 0;
	refCount = 0;
}

GSProduct::ObjectRep::~ObjectRep()
{
	objectRepCount--;

	if ( paint3dCount > 0 )
	{
		//object rep has a number of painters; clear up
		i_paint3dEndNotify();
	}

	if ( paintUVCount > 0 )
	{
		//object rep has a number of painters; clear up
		i_paintUVEndNotify();
	}
}



GSProduct::ObjectRep * GSProduct::ObjectRep::readObjectRep(FILE *f, ObjectRepTable *repTable)
{
	ObjectRep *object = RTType::newInstanceCast<ObjectRep>( f );

	if ( object != NULL )
	{
		object->read( f, repTable );
	}

	return object;
}

GSProduct::ObjectRep * GSProduct::ObjectRep::readObjectRep(SENode &node, ObjectRepTable *repTable)
{
	ObjectRep *object = RTType::newInstanceCast<ObjectRep>( node[0] );

	if ( object != NULL )
	{
		object->read( node[1], repTable );
	}

	return object;
}

void GSProduct::ObjectRep::writeObjectRep(SENode &node, ObjectRep *rep, ObjectRepTable *repTable)
{
	rep->getType()->write( node.addList() );
	rep->write( node.addList(), repTable );
}


void GSProduct::ObjectRep::read(FILE *f, ObjectRepTable *repTable)
{
	refCount = 0;
}

void GSProduct::ObjectRep::write(FILE *f, ObjectRepTable *repTable)
{
	getType()->write( f );
}

void GSProduct::ObjectRep::read(SENode &node, ObjectRepTable *repTable)
{
	refCount = 0;
}

void GSProduct::ObjectRep::write(SENode &node, ObjectRepTable *repTable)
{
}

void GSProduct::ObjectRep::fillRepTable(ObjectRepTable *repTable)
{
}


int GSProduct::ObjectRep::estimateCost() const
{
	return sizeof(ObjectRep);
}



void GSProduct::ObjectRep::ref()
{
	refCount++;
}

void GSProduct::ObjectRep::unref()
{
	refCount--;
}


bool GSProduct::ObjectRep::isUsed() const
{
	return refCount > 0;
}

bool GSProduct::ObjectRep::isShared() const
{
	return refCount > 1;
}



void GSProduct::ObjectRep::paint3dBeginNotify()
{
	if ( paint3dCount == 0 )
	{
		i_paint3dBeginNotify();
	}

	paint3dCount++;
}

void GSProduct::ObjectRep::paint3dEndNotify()
{
	paint3dCount--;

	gs_assert( paint3dCount >= 0, "GSProduct::ObjectRep::paint3dEndNotify(): object rep %p notified of paint end, too many times\n", this );

	if ( paint3dCount == 0 )
	{
		i_paint3dEndNotify();
	}
}

void GSProduct::ObjectRep::paintUVBeginNotify()
{
	if ( paintUVCount == 0 )
	{
		i_paintUVBeginNotify();
	}

	paintUVCount++;
}

void GSProduct::ObjectRep::paintUVEndNotify()
{
	paintUVCount--;

	gs_assert( paint3dCount >= 0, "GSProduct::ObjectRep::paintUVEndNotify(): object rep %p notified of paint end, too many times\n", this );

	if ( paintUVCount == 0 )
	{
		i_paintUVEndNotify();
	}
}


void GSProduct::ObjectRep::i_paint3dBeginNotify()
{
}

void GSProduct::ObjectRep::i_paint3dEndNotify()
{
}

void GSProduct::ObjectRep::i_paintUVBeginNotify()
{
}

void GSProduct::ObjectRep::i_paintUVEndNotify()
{
}





/***********************************************************************************
									GSProduct::ObjectRepTable
 ***********************************************************************************/

GSProduct::ObjectRepTable::ObjectRepTable()
{
	idCounter = 1;
}

GSProduct::ObjectRepTable::~ObjectRepTable()
{
	clear();
}


void GSProduct::ObjectRepTable::clear()
{
	for (IDToRepTab::iterator iter = idToRep.begin(); iter != idToRep.end(); ++iter)
	{
		if ( !iter->second->isUsed() )
		{
			delete iter->second;
		}
	}

	idToRep.clear();
	repToId.clear();

	idCounter = 1;
}


void GSProduct::ObjectRepTable::read(FILE *f)
{
	int size;
	fread( &size, sizeof(int), 1, f );

	for (int i = 0; i < size; i++)
	{
		UniqueID id;
		fread( &id, sizeof(UniqueID), 1, f );
		ObjectRep *object = ObjectRep::readObjectRep( f, this );

		idToRep[id] = object;
		repToId[object] = id;
	}
}

void GSProduct::ObjectRepTable::write(FILE *f)
{
	int size = idToRep.size();
	fwrite( &size, sizeof(int), 1, f );

	for (IDToRepTab::iterator iter = idToRep.begin(); iter != idToRep.end(); ++iter)
	{
		UniqueID id = iter->first;
		ObjectRep *object = iter->second;

		fwrite( &id, sizeof(UniqueID), 1, f );
		object->write( f, this );
	}
}

void GSProduct::ObjectRepTable::read(SENode &node)
{
	std::string header;
	node[0]  >>  header;
	if ( header == "GSProduct::ObjectRepTable" )
	{
		int size = 0;
		node[1]  >>  size;

		for (int i = 0; i < size; i++)
		{
			SENode &entryNode = node[i+2];

			UniqueID id = -1;
			entryNode[0]  >>  id;

			ObjectRep *object = ObjectRep::readObjectRep( entryNode[1], this );

			idToRep[id] = object;
			repToId[object] = id;
		}
	}
}

void GSProduct::ObjectRepTable::write(SENode &node)
{
	node.addItem()  <<  "GSProduct::ObjectRepTable";
	node.addItem()  <<  (int)idToRep.size();

	for (IDToRepTab::iterator iter = idToRep.begin(); iter != idToRep.end(); ++iter)
	{
		SENode &entryNode = node.addList();
		UniqueID id = iter->first;
		ObjectRep *object = iter->second;

		entryNode.addItem()  <<  id;
		ObjectRep::writeObjectRep( entryNode.addList(), object, this );
	}
}


GSProduct::ObjectRep * GSProduct::ObjectRepTable::getObjectRep(UniqueID id)
{
	IDToRepTab::iterator iter = idToRep.find( id );

	if ( iter == idToRep.end() )
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

UniqueID GSProduct::ObjectRepTable::getObjectID(ObjectRep *rep)
{
	RepToIDTab::iterator iter = repToId.find( rep );

	if ( iter == repToId.end() )
	{
		return -1;
	}
	else
	{
		return iter->second;
	}
}

UniqueID GSProduct::ObjectRepTable::addObjectRep(ObjectRep *rep)
{
	//see if @rep is already in the tables
	RepToIDTab::iterator iter = repToId.find( rep );

	if ( iter == repToId.end() )
	{
		//no; add it

		//generate an ID
		UniqueID id = idCounter++;
		//add to table
		idToRep[id] = rep;
		repToId[rep] = id;
		//return the ID
		return id;
	}
	else
	{
		//return the ID
		return iter->second;
	}
}




/***********************************************************************************
												GSProduct
 ***********************************************************************************/

 RT_IMPLEMENT_TYPE( GSProduct, RTObject );


GSProduct::GSProduct()
{
	objectLockedFlag = true;
	paint3dFlag = paintUVFlag = false;

	productCount++;
	representation = new ObjectRep();
	refRepresentation();
}

GSProduct::GSProduct(ObjectRep *rep)
{
	objectLockedFlag = true;
	paint3dFlag = paintUVFlag = false;

	productCount++;
	representation = rep;
	refRepresentation();
}

GSProduct::GSProduct(const GSProduct &o)
{
	objectLockedFlag = true;
	paint3dFlag = paintUVFlag = false;

	productCount++;
	representation = o.representation;
	refRepresentation();
}

GSProduct::~GSProduct()
{
	gs_assert( objectLockedFlag, "GSProduct::~GSProduct(): object %p is unlocked\n", this );

	productCount--;
	unrefRepresentation();
}




std::string GSProduct::getDescription() const
{
	return "None";
}



int GSProduct::estimateCost() const
{
	const ObjectRep *rep = getReadOnlyRepresentation();
	return sizeof(GSProduct) + rep->estimateCost();
}



void GSProduct::copyFrom(const GSProduct *o)
{
	setRepresentation( o->representation->cloneTyped<ObjectRep>() );
}




GSProduct * GSProduct::readGSProduct(FILE *f, ObjectRepTable *repTable)
{
	GSProduct *object = RTType::newInstanceCast<GSProduct>( f );

	if ( object != NULL )
	{
		object->read( f, repTable );
	}

	return object;
}

GSProduct * GSProduct::readGSProduct(SENode &node, ObjectRepTable *repTable)
{
	GSProduct *object = RTType::newInstanceCast<GSProduct>( node[0] );

	if ( object != NULL )
	{
		object->read( node[1], repTable );
	}

	return object;
}

void GSProduct::writeGSProduct(SENode &node, GSProduct *product, ObjectRepTable *repTable)
{
	product->getType()->write( node.addList() );
	product->write( node.addList(), repTable );
}

GSProduct * GSProduct::readSingleGSProduct(SENode &node)
{
	ObjectRepTable repTable;

	std::string header;
	node[0]  >>  header;
	if ( header == "GSProduct-single" )
	{
		repTable.read( node[1] );
		return readGSProduct( node[2], &repTable );
	}
	else
	{
		return NULL;
	}
}

void GSProduct::writeSingleGSProduct(SENode &node, GSProduct *product)
{
	ObjectRepTable repTable;
	product->fillRepTable( &repTable );

	node.addItem()  <<  "GSProduct-single";
	repTable.write( node.addList() );
	writeGSProduct( node.addList(), product, &repTable );
}


void GSProduct::read(FILE *f, ObjectRepTable *repTable)
{
	UniqueID id;
	fread( &id, sizeof(UniqueID), 1, f );

	if ( id != -1 )
	{
		setRepresentation( repTable->getObjectRep( id ) );
	}
	else
	{
		setRepresentation( NULL );
	}
}

void GSProduct::write(FILE *f, ObjectRepTable *repTable)
{
	getType()->write( f );

	UniqueID id = repTable->getObjectID( representation );

	gs_assert( id != -1, "GSProduct::write(): object representation not found in rep table\n" );

	fwrite( &id, sizeof(UniqueID), 1, f );
}

void GSProduct::read(SENode &node, ObjectRepTable *repTable)
{
	UniqueID id = -1;
	node[0]  >>  id;

	if ( id != -1 )
	{
		setRepresentation( repTable->getObjectRep( id ) );
	}
	else
	{
		setRepresentation( NULL );
	}
}

void GSProduct::write(SENode &node, ObjectRepTable *repTable)
{
	UniqueID id = repTable->getObjectID( representation );

	gs_assert( id != -1, "GSProduct::write(): object representation not found in rep table\n" );

	node.addItem()  <<  id;
}


void GSProduct::readSimple(SENode &node)
{
	ObjectRepTable repTable;

	std::string header;
	node[0]  >>  header;
	if ( header == "GSProduct-simple" )
	{
		const RTType *readType = RTType::readType( node[1] );
		if ( readType == getType() )
		{
			repTable.read( node[2] );
			read( node[3], &repTable );
		}
	}
}

void GSProduct::writeSimple(SENode &node)
{
	ObjectRepTable repTable;
	fillRepTable( &repTable );

	node.addItem()  <<  "GSProduct-simple";
	getType()->write( node.addList() );
	repTable.write( node.addList() );
	write( node.addList(), &repTable );
}


void GSProduct::fillRepTable(ObjectRepTable *repTable)
{
	repTable->addObjectRep( representation );
	representation->fillRepTable( repTable );
}




void GSProduct::lock()
{
	if ( !objectLockedFlag )
	{
		objectLockedFlag = true;
	}
}

void GSProduct::unlock()
{
	if ( objectLockedFlag )
	{
		objectLockedFlag = false;
		unshareRepresentation();
	}
}


void GSProduct::assertUnlocked()
{
	gs_assert( !objectLockedFlag, "GSProduct::assertUnlocked(): object %p is locked\n", this );
}



void GSProduct::paint3dBeginNotify()
{
	if ( !paint3dFlag )
	{
		paint3dFlag = true;
		ObjectRep *rep = getReadOnlyRepresentation();
		rep->paint3dBeginNotify();
	}
}

void GSProduct::paint3dEndNotify()
{
	if ( paint3dFlag )
	{
		paint3dFlag = false;
		ObjectRep *rep = getReadOnlyRepresentation();
		rep->paint3dEndNotify();
	}
}

void GSProduct::paintUVBeginNotify()
{
	if ( !paintUVFlag )
	{
		paintUVFlag = true;
		ObjectRep *rep = getReadOnlyRepresentation();
		rep->paintUVBeginNotify();
	}
}

void GSProduct::paintUVEndNotify()
{
	if ( paintUVFlag )
	{
		paintUVFlag = false;
		ObjectRep *rep = getReadOnlyRepresentation();
		rep->paintUVEndNotify();
	}
}





GSProduct::ObjectRep * GSProduct::getRepresentation()
{
	assertUnlocked();

	return representation;
}

GSProduct::ObjectRep * GSProduct::getReadOnlyRepresentation()
{
	return representation;
}

const GSProduct::ObjectRep * GSProduct::getReadOnlyRepresentation() const
{
	return representation;
}


void GSProduct::applyLocalChangesToRepresentation(ObjectRep *rep)
{
}


void GSProduct::unshareRepresentation()
{
	if ( representation->isShared() )
	{
		setRepresentation( representation->cloneTyped<ObjectRep>() );
	}
	applyLocalChangesToRepresentation( representation );
}

bool GSProduct::isRepresentationShared() const
{
	return representation->isShared();
}



void GSProduct::setRepresentation(ObjectRep *newRepresentation)
{
	gs_assert( newRepresentation != NULL, "GSProduct::setRepresentation(): newRepresentation is NULL\n" );

	unrefRepresentation();
	representation = newRepresentation;
	refRepresentation();
}

void GSProduct::unrefRepresentation()
{
	if ( paint3dFlag )
	{
		//if @this is has a painter, notify of end, due to @representation
		//no longer being attached to @this
		representation->paint3dEndNotify();
	}

	if ( paintUVFlag )
	{
		//if @this is has a painter, notify of end, due to @representation
		//no longer being attached to @this
		representation->paintUVEndNotify();
	}

	representation->unref();
	if ( !representation->isUsed() )
	{
		delete representation;
	}
}

void GSProduct::refRepresentation()
{
	representation->ref();

	if ( paint3dFlag )
	{
		//if @this is has a painter, notify of begin, due to @representation
		//being attached to this
		representation->paint3dBeginNotify();
	}

	if ( paintUVFlag )
	{
		//if @this is has a painter, notify of begin, due to @representation
		//being attached to this
		representation->paintUVBeginNotify();
	}
}


#endif
