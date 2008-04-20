//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJECTPAINTER_CPP__
#define OBJECTPAINTER_CPP__

#include <Util/gsassert.h>

#include <Graphics/GLMatrix.h>

#include <View/ViewSettings.h>

#include <Painter/ObjectPainter.h>




RT_IMPLEMENT_TYPE( ObjectPainter, RTObject );


ObjectPainter::ObjectPainter() : object( NULL ), painterFactory( NULL )
{
}

ObjectPainter::ObjectPainter(const ObjectPainter &p) : object( p.object ), painterFactory( p.painterFactory )
{
}



void ObjectPainter::setProduct(GSProduct *obj)
{
	gs_assert( object == NULL, "ObjectPainter::setProduct(): object is not NULL\n" );
	object = obj;
}

GSProduct * ObjectPainter::getProduct()
{
	return object;
}


void ObjectPainter::paintObject3d(Viewport3dSettings *viewportSettings, PaintLayer layer, bool root, bool background)
{
	if ( object != NULL )
	{
		object->paint3dBeginNotify();

		i_paintObject3d( viewportSettings, object, layer, root, background, 0 );
	}
}

void ObjectPainter::paintObjectUV(ViewportUVSettings *viewportSettings, bool root)
{
	if ( object != NULL )
	{
		object->paintUVBeginNotify();

		i_paintObjectUV( viewportSettings, object, root );
	}
}


void ObjectPainter::i_paintObject3d(Viewport3dSettings *viewportSettings, GSProduct *obj, PaintLayer layer, bool root, bool background, int reflectionCount)
{
}

void ObjectPainter::i_paintObjectUV(ViewportUVSettings *viewportSettings, GSProduct *obj, bool root)
{
}


void ObjectPainter::setPainterFactory(ObjectPainterFactory *factory)
{
	painterFactory = factory;
}



#endif
