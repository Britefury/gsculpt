//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJECTPAINTERFACTORY_H__
#define OBJECTPAINTERFACTORY_H__

#include <Util/ArrayMap.h>

#include <RTType/RTType.h>

#include <Product/GSProduct.h>



class GS_DllExport ObjectPainter;


class GS_DllExport ObjectPainterFactory
{
private:
	typedef ArrayMap<const RTType*, const RTType*> PainterTable;

	PainterTable painterTable;


private:
	ObjectPainterFactory();


public:
	ObjectPainter * createPainter(const RTType * objectType);
	ObjectPainter * createPainterForObject(GSProduct *object);


	static ObjectPainterFactory & getPainterFactory();


private:
	void registerPainterType(const RTType *objectType, const RTType *painterType);


	friend class ObjectPainterRegistrar;
};



class GS_DllExport ObjectPainterRegistrar
{
public:
	ObjectPainterRegistrar(const RTType *objectType, const RTType *painterType);
};



#define REGISTER_PAINTER(ObjectClass, PainterClass)																		\
	static ObjectPainterRegistrar __painter_##ObjectClass##_##PainterClass##_( ObjectClass::getTypeStatic(), PainterClass::getTypeStatic() );	\




#endif
