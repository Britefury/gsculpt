##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import sys

from OpenGL.GL import *

import Image

from Britefury.Math.Math import Vector2i

from Britefury.Graphics import GLUploadImage

# Win32 bug workaround; replace glDeleteTextures
if sys.platform == 'win32':
	from Britefury.bugworkarounds.bugworkarounds import glDeleteTextures



class Texture (object):
	def __init__(self, path):
		self._path = path
		self._size = None
		self._glTextureHandle = None
		self._bLoaded = False
		self._refCount = 0

	def __del__(self):
		self.unload()


	def load(self):
		if not self._bLoaded:
			textureHandle = glGenTextures( 1 )
			self._glTextureHandle = textureHandle
			try:
				image = Image.open( self._path )
			except Exception, e:
				print _( 'Error while loading %s: %s' ) % ( self._path, e )
				self._size = None
				self._bLoaded = False
			else:
				image = image.transpose( Image.FLIP_TOP_BOTTOM )
				try:
					GLUploadImage.glUploadImage( image, textureHandle )
				except ValueError:
					# Could not load
					print _( 'Error while loading %s: %s' ) % ( self._path, e )
					self._size = None
					self._bLoaded = False
				else:
					self._size = Vector2i( image.size[0], image.size[1] )
					self._bLoaded = True

	def unload(self):
		if self._bLoaded:
			glDeleteTextures( [ self._glTextureHandle ] )
			self._glTextureHandle = None
			self._size = None
			self._bLoaded = False



	def isEquivalentTo(self, tex):
		assert isinstance( tex, Texture )
		return self._path == tex._path



	def _p_getPath(self):
		return self._path

	def _p_getSize(self):
		return self._size

	def _p_getGLTextureHandle(self):
		return self._glTextureHandle


	path = property( _p_getPath )
	size = property( _p_getSize )
	glTextureHandle = property( _p_getGLTextureHandle )

