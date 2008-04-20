//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJECTPAINTERFACTORY_CPP__
#define OBJECTPAINTERFACTORY_CPP__

#include <Util/gsassert.h>

#include <Painter/ObjectPainter.h>
#include <Painter/ObjectPainterFactory.h>




ObjectPainterFactory::ObjectPainterFactory()
{
}



ObjectPainter * ObjectPainterFactory::createPainter(const RTType * objectType)
{
	PainterTable::const_iterator iter = painterTable.find( objectType );

	if ( iter != painterTable.end() )
	{
		ObjectPainter *painter = iter->second->newInstanceTyped<ObjectPainter>();

		painter->setPainterFactory( this );

		return painter;
	}
	else
	{
		return NULL;
	}
}

ObjectPainter * ObjectPainterFactory::createPainterForObject(GSProduct *object)
{
	return createPainter( object->getType() );
}


ObjectPainterFactory & ObjectPainterFactory::getPainterFactory()
{
	static ObjectPainterFactory factory;
	return factory;
}



void ObjectPainterFactory::registerPainterType(const RTType *objectType, const RTType *painterType)
{
	gs_assert( !painterTable.contains( objectType ), "ObjectPainterFactory::registerPainterType(): painter already registered for type %s\n", objectType->getName().c_str() );

	painterTable.insert( objectType, painterType );
}




ObjectPainterRegistrar::ObjectPainterRegistrar(const RTType *objectType, const RTType *painterType)
{
	ObjectPainterFactory::getPainterFactory().registerPainterType( objectType, painterType );
}



#endif
