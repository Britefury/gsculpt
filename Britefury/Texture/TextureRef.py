##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import ioXmlReadStringProp, ioXmlWriteStringProp, ioObjectFactoryRegister

import TextureTable



class TextureRef (object):
	def __init__(self, texture=None):
		self._texture = texture


	def __readxml__(self, xmlNode):
		path = ioXmlReadStringProp( xmlNode.property( 'path' ), '' )
		self._texture = TextureTable.globalTextureTable.getTexture( path )

	def __writexml__(self, xmlNode):
		if self._texture is not None:
			path = self._texture.path
		else:
			path = ''
		ioXmlWriteStringProp( xmlNode.property( 'path' ), path )


	def __copy__(self):
		return TextureRef( self._texture )

	def __deepcopy__(self, memo):
		return TextureRef( self._texture )


	def _p_getTexture(self):
		return self._texture


	texture = property( _p_getTexture )


ioObjectFactoryRegister( 'TextureRef', TextureRef )
