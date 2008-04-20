//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJECTPAINTER_H__
#define OBJECTPAINTER_H__

#include <RTType/RTType.h>

#include <Product/GSProduct.h>

#include <View/Viewport3dSettings.h>
#include <View/ViewportUVSettings.h>
#include <View/PaintLayer.h>

#include <Painter/ObjectPainterFactory.h>



class GS_DllExport ObjectPainter : public RTObject
{
RT_TYPE;
private:
	GSProduct *object;

protected:
	ObjectPainterFactory *painterFactory;


public:
	ObjectPainter();
	ObjectPainter(const ObjectPainter &p);


	virtual void setProduct(GSProduct *obj);
	GSProduct * getProduct();



	void paintObject3d(Viewport3dSettings *viewportSettings, PaintLayer layer, bool root, bool background);
	void paintObjectUV(ViewportUVSettings *viewportSettings, bool root);

protected:
	virtual void i_paintObject3d(Viewport3dSettings *viewportSettings, GSProduct *obj, PaintLayer layer, bool root, bool background, int reflectionCount);
	virtual void i_paintObjectUV(ViewportUVSettings *viewportSettings, GSProduct *obj, bool root);


private:
	void setPainterFactory(ObjectPainterFactory *factory);


	friend class ObjectPainterFactory;
};


#endif
