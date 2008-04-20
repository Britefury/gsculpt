//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RTTYPE_CPP__
#define RTTYPE_CPP__

#include <FileIO/fileio.h>

#include <RTType/RTType.h>




//
//
//  RTObject class
//
//

RT_IMPLEMENT_ROOT_TYPE( RTObject );

RTObject::~RTObject()
{
}






//
//
//  RTObjectFactory class
//
//

RTObjectFactory::~RTObjectFactory()
{
}




//
//
//  RTType class
//
//

RTType::RTType() : superType( NULL ), factory( NULL )
{
}

RTType::RTType(const std::string &name, const RTType *super, const RTObjectFactory *fac)
				: name( name ), superType( super ), factory( fac )
{
	TypeTable &typeRegistry = getTypeRegistry();

	gs_assert( !typeRegistry.contains( name ), "RTType::RTType(): Already got a type called %s\n", name.c_str() );

	if ( superType != NULL )
	{
		allSupers = superType->allSupers;
		allSupers.insert( superType );
	}

	typeRegistry.insert( name, this );
}



const std::string & RTType::getName() const
{
	return name;
}

const RTType * RTType::getSuperType() const
{
	return superType;
}


bool RTType::isDerivedFrom(const RTType *type) const
{
	return allSupers.contains( type );
}

bool RTType::isA(const RTType *type) const
{
	if ( this == type )
	{
		return true;
	}
	else
	{
		return isDerivedFrom( type );
	}
}



const RTType * RTType::readType(FILE *f)
{
	std::string name = readString( f );
	return getTypeByName( name );
}

const RTType * RTType::readType(SENode &node)
{
	std::string name;
	node[0]  >>  name;
	return getTypeByName( name );
}


void RTType::write(FILE *f) const
{
	writeString( f, name );
}

void RTType::write(SENode &node) const
{
	node.addItem()  <<  name;
}



RTObject * RTType::newInstance() const
{
	gs_assert( factory != NULL, "RTType::newInstance(): type \"%s\" cannot be instantiated as it has no factory\n", getName().c_str() );

	if ( factory != NULL )
	{
		return factory->create();
	}
	else
	{
		return NULL;
	}
}



const RTType * RTType::getTypeByName(const std::string &name)
{
	TypeTable &typeRegistry = getTypeRegistry();
	TypeTable::const_iterator iter = typeRegistry.find( name );

	if ( iter == typeRegistry.end() )
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

RTObject * RTType::newInstance(const std::string &name)
{
	const RTType *type = getTypeByName( name );

	if ( type == NULL )
	{
		return NULL;
	}
	else
	{
		return type->newInstance();
	}
}

RTObject * RTType::newInstance(FILE *f)
{
	std::string type = readString( f );
	return newInstance( type );
}

RTObject * RTType::newInstance(SENode &node)
{
	std::string type;
	node[0]  >>  type;
	return newInstance( type );
}




RTType::TypeTable & RTType::getTypeRegistry()
{
	static TypeTable typeRegistry;

	return typeRegistry;
}






//
//
//  Primitive type declarations
//
//

RT_IMPLEMENT_PRIMITIVE_TYPE( bool );
RT_IMPLEMENT_PRIMITIVE_TYPE( int );
RT_IMPLEMENT_PRIMITIVE_TYPE( double );
RT_IMPLEMENT_PRIMITIVE_TYPE( std::string );



const RTType * getUnknownRTType()
{
	static RTType type( "unknown", NULL, NULL );
	return &type;
}




#endif
