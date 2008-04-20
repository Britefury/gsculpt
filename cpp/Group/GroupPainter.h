/************************
 This program is free software; you can use it, redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as published by the
 Free Software Foundation. The full text of the GNU General Public License
 version 2 can be found in the file named 'COPYING' that accompanies this
 program. This source code is (C)copyright Geoffrey French 1999-2005.
 ************************/
#ifndef GROUPPAINTER_H__
#define GROUPPAINTER_H__

#include <Product/GSProduct.h>
#include <Group/GSProductGroup.h>

#include <Model/ModelPainter.h>

#include <Painter/ObjectPainterFactory.h>



/****************************************************************************
						GroupPainter
 ****************************************************************************/

class GS_DllExport GroupPainter : public ModelPainter
{
RT_TYPE;
private:
	/****************************************************************************
						GroupMemberPainter
	 ****************************************************************************/

	class GS_DllExport GroupMemberPainter
	{
	private:
		ObjectPainter *painter;
		GSProduct *product;

	public:
		GroupMemberPainter();
		GroupMemberPainter(const GroupMemberPainter &p);
		~GroupMemberPainter();


		GroupMemberPainter & operator=(const GroupMemberPainter &p);


		void setProduct(GSProduct *product, ObjectPainterFactory *painterFactory);

		void paint3d(Viewport3dSettings *viewportSettings, PaintLayer layer, bool background);

		friend class GroupPainter;
	};


	Array<GroupMemberPainter> memberPainters;


public:
	GroupPainter();


	virtual void setProduct(GSProduct *obj);



protected:
	virtual void i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount);
	virtual void i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root);

	void paintGroup3d(Viewport3dSettings *viewportSettings, GSProductGroup *group, PaintLayer layer, bool background, int reflectionCount);


private:
	void refreshMembers();
};


#endif
