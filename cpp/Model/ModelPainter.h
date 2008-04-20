//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELPAINTER_H__
#define MODELPAINTER_H__

#include <Model/GSProductModel.h>
#include <Painter/ObjectPainter.h>


class GS_DllExport ModelPainter : public ObjectPainter
{
RT_TYPE;
public:
	ModelPainter();
	ModelPainter(GSProduct *obj);



protected:
	virtual void i_paintObject3d(Viewport3dSettings *viewportSettings, GSProduct *obj, PaintLayer layer, bool root, bool background, int reflectionCount);
	virtual void i_paintObjectUV(ViewportUVSettings *viewportSettings, GSProduct *obj, bool root);

	virtual void i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount);
	virtual void i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root);
};


#endif
