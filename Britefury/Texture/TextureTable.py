##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import weakref

import os

from Britefury.Texture.Texture import Texture



class TextureTable (object):
	def __init__(self):
		self._textures = weakref.WeakValueDictionary()
		self._bLoaded = False


	def getTexture(self, path):
		normalisedPath = os.path.normpath( path )
		try:
			tex = self._textures[normalisedPath]
		except KeyError:
			tex = Texture( normalisedPath )
			self._textures[normalisedPath] = tex
			if self._bLoaded:
				tex.load()
		return tex


	def load(self):
		if not self._bLoaded:
			for tex in self._textures.values():
				tex.load()
			self._bLoaded = True

	def unload(self):
		if self._bLoaded:
			for tex in self._textures.values():
				tex.unload()
			self._bLoaded = False


globalTextureTable = TextureTable()

