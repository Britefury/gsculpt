##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

from Britefury.Background.Model import BackgroundMesh

from Britefury.FileIO.IOXml import ioXmlReadStringProp, ioXmlWriteStringProp, ioObjectFactoryRegister



def _loadBackgroundMesh(path):
	ext = os.path.splitext( path )[1].lower()
	if ext == '.gbm':
		mesh = BackgroundMesh()
		if not mesh.readFromFile( path ):
			print 'Failed to read background mesh from ' + path
		return mesh
	else:
		return None
	
	
	
class BackgroundModelFileRef (object):
	def __init__(self, path=None):
		self._path = path
		
		if path is not None:
			self._model = _loadBackgroundMesh( os.path.normpath( path ) )
		else:
			self._model = None


	def __readxml__(self, xmlNode):
		self._path = ioXmlReadStringProp( xmlNode.property( 'path' ), '' )
		self._loadMesh()

	def __writexml__(self, xmlNode):
		if self._path is not None:
			path = self._path
		else:
			path = ''
		ioXmlWriteStringProp( xmlNode.property( 'path' ), path )


	def __copy__(self):
		return BackgroundModelFileRef( self._path )

	def __deepcopy__(self, memo):
		return BackgroundModelFileRef( self._path )


	def _getPath(self):
		return self._path
	
	def _getModel(self):
		return self._model
	
	
	def _loadMesh(self):
		if self._path is not None:
			self._model = _loadBackgroundMesh( os.path.normpath( self._path ) )
		else:
			self._model = None


	path = property( _getPath )
	model = property( _getModel )


ioObjectFactoryRegister( 'BackgroundModelFileRef', BackgroundModelFileRef )
