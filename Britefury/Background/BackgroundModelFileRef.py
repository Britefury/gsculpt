##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

from Britefury.Background.Model import BackgroundMesh
from Britefury.ImportExportFilter.Obj import ObjImport

from Britefury.FileIO.IOXml import ioXmlReadStringProp, ioXmlWriteStringProp, ioObjectFactoryRegister


import gtk
import gobject



def _loadBackgroundMesh(path):
	ext = os.path.splitext( path )[1].lower()
	if ext == '.obj':
		return ObjImport.importObjFileAsBackgroundMesh( path )
	else:
		return None
	
	
def _objImportWithProgressDialog(path):
	def _timeout():
		progress = threadMonitor.getProgress()
		
		if progress.stage == ObjImport.ObjImportProgressStage.SCAN_STRUCTURE:
			fraction = float(progress.bytesRead) / float(progress.fileSize)
			text = 'Analysing structure: %.1f%%'  %  ( fraction * 100.0, )
			scanStructureProgress.set_fraction( fraction )
			scanStructureProgress.set_text( text )
			readGeometryProgress.set_fraction( 0.0 )
			readGeometryProgress.set_text( '' )
			buildModelsProgress.set_fraction( 0.0 )
			buildModelsProgress.set_text( '' )
		elif progress.stage == ObjImport.ObjImportProgressStage.READ_GEOMETRY:
			fraction = float(progress.geometryRead) / float(progress.geometrySize)
			text = 'Reading geometry: %.1f%%'  %  ( fraction * 100.0, )
			scanStructureProgress.set_fraction( 1.0 )
			scanStructureProgress.set_text( 'Structure analysed' )
			readGeometryProgress.set_fraction( fraction )
			readGeometryProgress.set_text( text )
			buildModelsProgress.set_fraction( 0.0 )
			buildModelsProgress.set_text( '' )
		elif progress.stage == ObjImport.ObjImportProgressStage.BUILD_MODELS:
			if progress.numModels > 0:
				modelsFrac = float(progress.modelsBuilt) / float(progress.numModels)
			else:
				modelsFrac = 0.0

			if progress.modelFaces > 0:
				modelFrac = float(progress.modelFacesBuilt) / float(progress.modelFaces)
			else:
				modelFrac = 0.0
			
			if progress.numModels > 0:
				fraction = modelsFrac + modelFrac / float(progress.numModels)
			else:
				fraction = 0.0

			text = 'Building models: %.1f%%'  %  ( fraction * 100.0, )
			scanStructureProgress.set_fraction( 1.0 )
			scanStructureProgress.set_text( 'Structure analysed' )
			readGeometryProgress.set_fraction( 1.0 )
			readGeometryProgress.set_text( 'Geometry read' )
			buildModelsProgress.set_fraction( fraction )
			buildModelsProgress.set_text( text )
		elif progress.stage == ObjImport.ObjImportProgressStage.FINISHED:
			scanStructureProgress.set_fraction( 1.0 )
			scanStructureProgress.set_text( 'Structure analysed' )
			readGeometryProgress.set_fraction( 1.0 )
			readGeometryProgress.set_text( 'Geometry read' )
			buildModelsProgress.set_fraction( 1.0 )
			buildModelsProgress.set_text( 'Models built' )
			dialog.destroy()
			return False
					
		return bStillRunning
	
	
	bStillRunning = True
	threadMonitor = ObjImport.importObjFileAsBackgroundMeshThreaded( path )
	
	dialog = gtk.Dialog( 'Import OBJ Progress', None, gtk.DIALOG_MODAL, None )
	filename = os.path.split( path )[1]
	
	scanStructureProgress = gtk.ProgressBar()
	readGeometryProgress = gtk.ProgressBar()
	buildModelsProgress = gtk.ProgressBar()
	
	dialog.vbox.pack_start( gtk.Label( 'Importing ' + filename ), False, False, 25 )
	dialog.vbox.pack_start( scanStructureProgress, False, False, 0 )
	dialog.vbox.pack_start( readGeometryProgress, False, False, 0 )
	dialog.vbox.pack_start( buildModelsProgress, False, False, 0 )
	
	dialog.set_size_request( 640, 175 )
	
	gobject.timeout_add( 150, _timeout )
	
	dialog.show_all()
	dialog.run()
	bStillRunning = False
	
	return threadMonitor.getResult()
	


def _loadBackgroundMesh(path):
	ext = os.path.splitext( path )[1].lower()
	if ext == '.obj':
		return _objImportWithProgressDialog( path )
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
