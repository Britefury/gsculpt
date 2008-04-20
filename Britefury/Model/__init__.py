##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.ProceduralCore import Product

from Britefury.Painter import Painter

from Britefury.Model.Model import GSProductModel, GSProductModelBlank, VisualPlane



def VisualPlane__copy__(self):
	return VisualPlane( self )


VisualPlane.__copy__ = VisualPlane__copy__

VisualPlane.position = property( VisualPlane._p_getPosition, VisualPlane._p_setPosition )
VisualPlane.rotation = property( VisualPlane._p_getRotation, VisualPlane._p_setRotation )
VisualPlane.size = property( VisualPlane._p_getSize, VisualPlane._p_setSize )
VisualPlane.plane = property( VisualPlane._p_getPlane, None )
VisualPlane.bVisible = property( VisualPlane._p_isVisible, VisualPlane._p_setVisible )


GSProductModel.virtualMirrorPlane = property( GSProductModel._p_getVirtualMirrorPlane, None )
GSProductModel.bVirtualMirrorEnabled = property( GSProductModel._p_isVirtualMirrorEnabled, GSProductModel._p_setVirtualMirrorEnabled )


ioObjectFactoryRegister( 'GSProductModelBlank', GSProductModelBlank )
ioObjectFactoryRegister( 'GSProductModel', GSProductModel )

