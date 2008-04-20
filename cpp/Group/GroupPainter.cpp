/************************
 This program is free software; you can use it, redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as published by the
 Free Software Foundation. The full text of the GNU General Public License
 version 2 can be found in the file named 'COPYING' that accompanies this
 program. This source code is (C)copyright Geoffrey French 1999-2005.
 ************************/
#ifndef GROUPPAINTER_CPP__
#define GROUPPAINTER_CPP__

#include <Group/GroupPainter.h>




/****************************************************************************
				GroupPainter::GroupMemberPainter
 ****************************************************************************/

GroupPainter::GroupMemberPainter::GroupMemberPainter() : painter( NULL )
{
}

GroupPainter::GroupMemberPainter::GroupMemberPainter(const GroupMemberPainter &p)
{
	if ( p.painter != NULL )
	{
		painter = p.painter->cloneTyped<ObjectPainter>();
	}
	else
	{
		painter = NULL;
	}
}

GroupPainter::GroupMemberPainter::~GroupMemberPainter()
{
	if ( painter != NULL )
	{
		delete painter;
	}
}


GroupPainter::GroupMemberPainter & GroupPainter::GroupMemberPainter::operator=(const GroupMemberPainter &p)
{
	if ( painter != NULL )
	{
		delete painter;
	}

	if ( p.painter != NULL )
	{
		painter = p.painter->cloneTyped<ObjectPainter>();
	}
	else
	{
		painter = NULL;
	}

	return *this;
}


void GroupPainter::GroupMemberPainter::setProduct(GSProduct *product, ObjectPainterFactory *painterFactory)
{
	// Delete the old painter
	if ( painter != NULL )
	{
		delete painter;
		painter = NULL;
	}

	// Get a new one
	painter = painterFactory->createPainter( product->getType() );

	if ( painter != NULL )
	{
		painter->setProduct( product );
	}

	this->product = product;
}


void GroupPainter::GroupMemberPainter::paint3d(Viewport3dSettings *viewportSettings, PaintLayer layer, bool background)
{
	if ( painter != NULL )
	{
		painter->paintObject3d( viewportSettings, layer, false, background );
	}
}






/****************************************************************************
						GroupPainter
 ****************************************************************************/

RT_IMPLEMENT_TYPE( GroupPainter, ModelPainter );

REGISTER_PAINTER( GSProductGroup, GroupPainter );


GroupPainter::GroupPainter()
{
}



void GroupPainter::setProduct(GSProduct *obj)
{
	ModelPainter::setProduct( obj );


	gs_assert_is_instance_of( obj, GSProductGroup, "GroupPainter::setProduct()" );
	GSProductGroup *group = static_cast<GSProductGroup*>( obj );


	Array<GSProductModel*> &objects = group->getObjectListForDisplay();

	memberPainters.resize( objects.size() );

	for (int i = 0; i < objects.size(); i++)
	{
		memberPainters[i].setProduct( objects[i], painterFactory );
	}
}


void GroupPainter::i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount)
{
	ModelPainter::i_paintModel3d( viewportSettings, model, layer, root, background, reflectionCount );

	if ( model != NULL )
	{
		gs_assert_is_instance_of( model, GSProductGroup, "GroupPainter::i_paintModel3d()" );

		paintGroup3d( viewportSettings, static_cast<GSProductGroup*>( model ), layer, background, reflectionCount );
	}
}

void GroupPainter::i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root)
{
}


void GroupPainter::paintGroup3d(Viewport3dSettings *viewportSettings, GSProductGroup *group, PaintLayer layer, bool background, int reflectionCount)
{
	refreshMembers();

	for (int i = 0; i < memberPainters.size(); i++)
	{
		memberPainters[i].paint3d( viewportSettings, layer, background );
	}
}


void GroupPainter::refreshMembers()
{
	GSProductGroup *group = static_cast<GSProductGroup*>( getProduct() );
	Array<GSProductModel*> &objects = group->getObjectListForDisplay();

	if ( memberPainters.size() != objects.size() )
	{
		// List size changed; rebuild
		memberPainters.resize( objects.size() );

		for (int i = 0; i < objects.size(); i++)
		{
			memberPainters[i].setProduct( objects[i], painterFactory );
		}
	}
	else
	{
		// Change any members where the product is different
		for (int i = 0; i < memberPainters.size(); i++)
		{
			if ( memberPainters[i].product != objects[i] )
			{
				memberPainters[i].setProduct( objects[i], painterFactory );
			}
		}
	}
}


#endif
