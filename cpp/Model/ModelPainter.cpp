//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELPAINTER_CPP__
#define MODELPAINTER_CPP__

#include <PlatformSpecific/IncludeGL.h>

#include <Graphics/GLMatrix.h>

#include <Graphics/Drawing.h>

#include <View/ViewSettings.h>

#include <Model/ModelPainter.h>
#include <Model/ModelDraw.h>




RT_IMPLEMENT_TYPE( ModelPainter, ObjectPainter );


REGISTER_PAINTER( GSProductModel, ModelPainter );


ModelPainter::ModelPainter() : ObjectPainter()
{
}



void ModelPainter::i_paintObject3d(Viewport3dSettings *viewportSettings, GSProduct *obj, PaintLayer layer, bool root, bool background, int reflectionCount)
{
	ObjectPainter::i_paintObject3d( viewportSettings, obj, layer, root, background, reflectionCount );

	if ( obj != NULL )
	{
		gs_assert_is_instance_of( obj, GSProductModel, "ModelPainter::i_paintObject3d()" );

		GSProductModel *model = static_cast<GSProductModel*>( obj );


		if ( model->hasLocalTransformation() )
		{
			const Matrix4 &modelXform = model->getLocalTransformation();

			glPushMatrix();
			glMultMatrix( modelXform );
		}

		if ( model->hasMarkOverride() )
		{
			pushGraphicsMarkOverride( model->getMarkOverride() );
		}


		i_paintModel3d( viewportSettings, model, layer, root, background, reflectionCount );


		if ( model->isVirtualMirrorEnabled()  &&  root )
		{
			model->getVirtualMirrorPlane().paint( getMirrorPlaneColour(), getMirrorPlaneAlpha(), layer );


			Plane plane = model->getVirtualMirrorPlane().getPlane();
			Matrix4 reflectionMatrix = Matrix4::reflect( plane );

			glPushMatrix();
			glMultMatrix( reflectionMatrix );

			i_paintModel3d( viewportSettings, model, layer, root, background, reflectionCount + 1 );

			glPopMatrix();
		}


		if ( model->hasMarkOverride() )
		{
			popGraphicsMarkOverride();
		}

		if ( model->hasLocalTransformation() )
		{
			glPopMatrix();
		}
	}
}

void ModelPainter::i_paintObjectUV(ViewportUVSettings *viewportSettings, GSProduct *obj, bool root)
{
	ObjectPainter::i_paintObjectUV( viewportSettings, obj, root );

	if ( obj != NULL )
	{
		gs_assert_is_instance_of( obj, GSProductModel, "ModelPainter::i_paintObjectUV()" );

		GSProductModel *model = static_cast<GSProductModel*>( obj );


		i_paintModelUV( viewportSettings, model, root );
	}
}



void ModelPainter::i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount)
{
}

void ModelPainter::i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root)
{
}



#endif
