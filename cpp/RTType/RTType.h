//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RTTYPE_H__
#define RTTYPE_H__

#include <string>

#include <stdio.h>

#include <Util/ArraySet.h>
#include <Util/ArrayMap.h>
#include <Util/gsassert.h>
#include <Util/SETree.h>




//
//
//  RTType helper macros
//
//

class GS_DllExport RTType;
class GS_DllExport RTObject;




#define RT_ABSTRACT_TYPE																					\
	private:																									\
		static const RTType *RTType_internal_my_type__;															\
																											\
	public:																									\
		static const RTType * getTypeStatic();																		\
		virtual const RTType * getType() const;																	\

#define RT_TYPE																								\
		RT_ABSTRACT_TYPE																					\
		virtual RTObject * clone() const;																			\

#define RT_ABSTRACT_ROOT_TYPE																			\
		RT_ABSTRACT_TYPE																					\
		bool isOfType(const RTType *type) const;																	\
		bool isInstanceOf(const RTType *type) const;																\
		virtual RTObject * clone() const = 0;																		\

#define RT_ROOT_TYPE																						\
		RT_TYPE																							\
		bool isOfType(const RTType *type) const;																	\
		bool isInstanceOf(const RTType *type) const;																\
																											\
		template <typename T> T * cloneTyped() const																\
		{																									\
			RTObject *object = clone();																			\
																											\
			gs_assert_is_instance_of( object, T, "clone()" );															\
																											\
			return static_cast<T*>( object );																		\
		}																									\







#define RT_IMPLEMENT_GETTYPE_METHOD(identifier)																\
	const RTType * identifier::getType() const																		\
	{																										\
		return getTypeStatic();																					\
	}																										\


#define RT_IMPLEMENT_CLONE_METHOD(identifier)																\
	RTObject * identifier::clone() const																			\
	{																										\
		return new identifier( *this );																				\
	}																										\

#define RT_IMPLEMENT_ISOFTYPE_METHOD(identifier)															\
	bool identifier::isOfType(const RTType *type) const																\
	{																										\
		return getType() == type;																				\
	}																										\

#define RT_IMPLEMENT_ISINSTANCEOF_METHOD(identifier)														\
	bool identifier::isInstanceOf(const RTType *type) const															\
	{																										\
		return getType()->isA( type );																				\
	}																										\

#define RT_IMPLEMENT_LOCAL_FACTORY(identifier)																\
	class LocalObjectFactory : public RTObjectFactory																\
	{																										\
	public:																									\
		virtual RTObject * create() const																			\
		{																									\
			return new identifier();																				\
		}																									\
	};																										\

#define RT_IMPLEMENT_INITIALIZER(identifier)																	\
	const RTType * identifier::RTType_internal_my_type__ = identifier::getTypeStatic();										\








#define RT_IMPLEMENT_ABSTRACT_UNINITIALIZED_TYPE(identifier, super)											\
	RT_IMPLEMENT_GETTYPE_METHOD(identifier)																\
																											\
	const RTType * identifier::getTypeStatic()																		\
	{																										\
		static RTType type( #identifier, super::getTypeStatic(), NULL );													\
		return &type;																							\
	}																										\


#define RT_IMPLEMENT_ABSTRACT_TYPE(identifier, super)															\
	RT_IMPLEMENT_ABSTRACT_UNINITIALIZED_TYPE( identifier, super )											\
	RT_IMPLEMENT_INITIALIZER( identifier )																		\






#define RT_IMPLEMENT_ABSTRACT_ROOT_UNINITIALIZED_TYPE(identifier)											\
	RT_IMPLEMENT_GETTYPE_METHOD( identifier )																\
	RT_IMPLEMENT_ISOFTYPE_METHOD( identifier )																\
	RT_IMPLEMENT_ISINSTANCEOF_METHOD( identifier )															\
																											\
	const RTType * identifier::getTypeStatic()																		\
	{																										\
		static RTType type( #identifier, NULL, NULL );																\
		return &type;																							\
	}																										\


#define RT_IMPLEMENT_ABSTRACT_ROOT_TYPE(identifier)														\
	RT_IMPLEMENT_ABSTRACT_ROOT_UNINITIALIZED_TYPE( identifier )											\
	RT_IMPLEMENT_INITIALIZER( identifier )																		\





#define RT_IMPLEMENT_UNINITIALIZED_TYPE(identifier, super)														\
	RT_IMPLEMENT_GETTYPE_METHOD(identifier)																\
	RT_IMPLEMENT_CLONE_METHOD(identifier)																	\
																											\
	const RTType * identifier::getTypeStatic()																		\
	{																										\
		RT_IMPLEMENT_LOCAL_FACTORY( identifier )															\
		static LocalObjectFactory factory;																			\
		static RTType type( #identifier, super::getTypeStatic(), &factory );												\
		return &type;																							\
	}																										\


#define RT_IMPLEMENT_TYPE(identifier, super)																	\
	RT_IMPLEMENT_UNINITIALIZED_TYPE( identifier, super )														\
	RT_IMPLEMENT_INITIALIZER( identifier )																		\






#define RT_IMPLEMENT_ROOT_UNINITIALIZED_TYPE(identifier)														\
	RT_IMPLEMENT_GETTYPE_METHOD(identifier)																\
	RT_IMPLEMENT_CLONE_METHOD(identifier)																	\
	RT_IMPLEMENT_ISOFTYPE_METHOD(identifier)																\
	RT_IMPLEMENT_ISINSTANCEOF_METHOD(identifier)															\
																											\
	const RTType * identifier::getTypeStatic()																		\
	{																										\
		RT_IMPLEMENT_LOCAL_FACTORY( identifier )															\
		static LocalObjectFactory factory;																			\
		static RTType type( #identifier , NULL, &factory );															\
		return &type;																							\
	}																										\


#define RT_IMPLEMENT_ROOT_TYPE(identifier)																	\
	RT_IMPLEMENT_ROOT_UNINITIALIZED_TYPE( identifier )														\
	RT_IMPLEMENT_INITIALIZER( identifier )																		\










//
//
//  RTObjectFactory class
//
//

class GS_DllExport RTObjectFactory
{
public:
	virtual ~RTObjectFactory();

	virtual RTObject * create() const = 0;
};







//
//
//  RTType class
//
//

class GS_DllExport RTType
{
private:
	typedef ArrayMap<std::string ,const RTType*> TypeTable;

	std::string name;
	const RTType *superType;
	const RTObjectFactory *factory;
	ArraySet<const RTType*> allSupers;



public:
	RTType();
	RTType(const std::string &name, const RTType *superType, const RTObjectFactory *factory);


	const std::string & getName() const;
	const RTType * getSuperType() const;

	bool isDerivedFrom(const RTType *type) const;
	bool isA(const RTType *type) const;

	RTObject * newInstance() const;

	template <typename T> T * newInstanceTyped() const;


	static const RTType * readType(FILE *f);
	static const RTType * readType(SENode &node);

	void write(FILE *f) const;
	void write(SENode &node) const;

	static const RTType * getTypeByName(const std::string &name);
	static RTObject * newInstance(const std::string &name);
	static RTObject * newInstance(FILE *f);
	static RTObject * newInstance(SENode &node);


	template <typename T> static T * newInstanceCast(const std::string &name);
	template <typename T> static T * newInstanceCast(FILE *f);
	template <typename T> static T * newInstanceCast(SENode &node);

private:
	static TypeTable & getTypeRegistry();

	friend class RTTypeInitializerBase;
};





//
//
//  RTObject class			(defined here so that the RTType methods below will compile due to their referencing RTObject)
//
//

class GS_DllExport RTObject
{
public:
RT_ROOT_TYPE;
	virtual ~RTObject();
};









template <typename T> T * RTType::newInstanceTyped() const
{
	RTObject *object = newInstance();

	gs_assert_is_instance_of_type( object, T::getTypeStatic(), "RTType::newInstanceTyped()" );

	return static_cast<T*>( object );
}


template <typename T> T * RTType::newInstanceCast(const std::string &name)
{
	RTObject *object = newInstance( name );

	if ( object != NULL )
	{
		if ( object->isInstanceOf( T::getTypeStatic() ) )
		{
			return static_cast<T*>( object );
		}
		else
		{
			delete object;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

template <typename T> T * RTType::newInstanceCast(FILE *f)
{
	RTObject *object = newInstance( f );

	if ( object != NULL )
	{
		if ( object->isInstanceOf( T::getTypeStatic() ) )
		{
			return static_cast<T*>( object );
		}
		else
		{
			delete object;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

template <typename T> T * RTType::newInstanceCast(SENode &node)
{
	RTObject *object = newInstance( node );

	if ( object != NULL )
	{
		if ( object->isInstanceOf( T::getTypeStatic() ) )
		{
			return static_cast<T*>( object );
		}
		else
		{
			delete object;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

















//
//
//  Primitive type macros
//
//

#define RT_PRIMITIVE_TYPE(type)																	\
	template <> const RTType * getRTType<type>();																\


#define RT_IMPLEMENT_PRIMITIVE_TYPE(type)																\
	template <> const RTType * getRTType<type>()																\
	{																										\
		static RTType rttype( #type, NULL, NULL );																\
		return &rttype;																						\
	}																										\


//
//
//  Primitive type declarations
//
//

template <typename T> const RTType * getRTType();

RT_PRIMITIVE_TYPE( bool );
RT_PRIMITIVE_TYPE( int );
RT_PRIMITIVE_TYPE( double );
RT_PRIMITIVE_TYPE( std::string );




template <typename T> inline const RTType * getRTType(T x)
{
	return getRTType<T>();
}

GS_DllExport inline const RTType * getRTType(RTObject *o)
{
	return o->getType();
}


GS_DllExport const RTType * getUnknownRTType();




#endif
