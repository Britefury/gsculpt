##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.UIController import UIController
from Britefury.UI.ButtonPopupMenu import ButtonPopupMenu

from Britefury.gSculptConfig.gSculptUserConfig import userConfig

from Britefury.Background.Model import BackgroundMesh
from Britefury.ImportExportFilter.Obj import ObjImport

import gtk
import gobject



def backgroundModelFileChooserDialog(dialogTitle, parentWindow, currentPath=None):
	startingDirectory = userConfig.backgroundModelPath


	gbmFilter = gtk.FileFilter()
	gbmFilter.set_name( _( 'gSculpt Background Model (*.gbm)' ) )
	gbmFilter.add_pattern( '*.gbm' )

			
	# If a current choice has been provided, and its path is absolute, use it as a starting point
	bCurrentChoiceValid = False
	if currentPath is not None:
		if os.path.isabs( currentPath ):
			startingDirectory = os.path.dirname( currentPath )
			bCurrentChoiceValid = True


	# Create the file chooser dialog
	chooser = gtk.FileChooserDialog( dialogTitle, parentWindow, gtk.FILE_CHOOSER_ACTION_OPEN, ( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
	chooser.add_filter( gbmFilter )

	if bCurrentChoiceValid:
		# Absolute filename: start here
		chooser.set_filename( currentPath )
	else:
		# Start at the starting directory
		chooser.set_current_folder( startingDirectory )


	modelPath = None

	chooser.show()
	if chooser.run() == gtk.RESPONSE_OK:
		filename = chooser.get_filename()

		# Set the background model directory
		userConfig.backgroundModelPath = os.path.dirname( filename )

		modelPath = filename

	chooser.destroy()

	return modelPath




def backgroundModelFileSaveChooserDialog(dialogTitle, parentWindow, currentPath=None):
	startingDirectory = userConfig.backgroundModelPath


	gbmFilter = gtk.FileFilter()
	gbmFilter.set_name( _( 'gSculpt Background Model (*.gbm)' ) )
	gbmFilter.add_pattern( '*.gbm' )

			
	# If a current choice has been provided, and its path is absolute, use it as a starting point
	bCurrentChoiceValid = False
	if currentPath is not None:
		if os.path.isabs( currentPath ):
			startingDirectory = os.path.dirname( currentPath )
			bCurrentChoiceValid = True


	# Create the file chooser dialog
	chooser = gtk.FileChooserDialog( dialogTitle, parentWindow, gtk.FILE_CHOOSER_ACTION_SAVE, ( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
	chooser.add_filter( gbmFilter )

	if bCurrentChoiceValid:
		# Absolute filename: start here
		chooser.set_filename( currentPath )
	else:
		# Start at the starting directory
		chooser.set_current_folder( startingDirectory )


	modelPath = None

	chooser.show()
	if chooser.run() == gtk.RESPONSE_OK:
		filename = chooser.get_filename()

		# Set the background model directory
		userConfig.backgroundModelPath = os.path.dirname( filename )

		modelPath = filename

	chooser.destroy()

	return modelPath




def importObjBackgroundModelFileChooserDialog(dialogTitle, parentWindow, currentPath=None):
	startingDirectory = userConfig.backgroundModelPath


	objFilter = gtk.FileFilter()
	objFilter.set_name( _( 'Wavefront OBJ (*.obj)' ) )
	objFilter.add_pattern( '*.obj' )

			
	# If a current choice has been provided, and its path is absolute, use it as a starting point
	bCurrentChoiceValid = False
	if currentPath is not None:
		if os.path.isabs( currentPath ):
			startingDirectory = os.path.dirname( currentPath )
			bCurrentChoiceValid = True


	# Create the file chooser dialog
	chooser = gtk.FileChooserDialog( dialogTitle, parentWindow, gtk.FILE_CHOOSER_ACTION_OPEN, ( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
	chooser.add_filter( objFilter )

	if bCurrentChoiceValid:
		# Absolute filename: start here
		chooser.set_filename( currentPath )
	else:
		# Start at the starting directory
		chooser.set_current_folder( startingDirectory )


	modelPath = None

	chooser.show()
	if chooser.run() == gtk.RESPONSE_OK:
		filename = chooser.get_filename()

		modelPath = filename

	chooser.destroy()

	return modelPath



class BackgroundModelFileChooser (UIController):
	CHOICE_NONE = 0
	CHOICE_TEXTURE = 1

	modelSignal = ClassSignal()


	def __init__(self, initialChoice, parentWindow, dialogTitle):
		self._currentChoice = initialChoice
		self._parentWindow = parentWindow
		self._dialogTitle = dialogTitle

		filename = _( '<span foreground="#000040">[No model]</span>' )
		if self._currentChoice is not None:
			filename = os.path.basename( self._currentChoice.path )

		self._popupMenu = ButtonPopupMenu( labelText=filename )
		self._popupMenu.getWidget().show()
		self._popupMenu.menuSignal.connect( self._p_onMenu )
		self._popupMenu.addMenuItem( BackgroundModelFileChooser.CHOICE_NONE, _( 'No model' ) )
		self._popupMenu.addMenuItem( BackgroundModelFileChooser.CHOICE_TEXTURE, _( 'Choose file...' ) )



	def getWidget(self):
		return self._popupMenu.getWidget()




	def _p_onMenu(self, event, choice):
		if choice == BackgroundModelFileChooser.CHOICE_NONE:
			if self._currentChoice is not None:
				self._p_setCurrentChoice( None )
				self.modelSignal.emit( self.uiEvent( event.bUserEvent ), None )
		elif choice == BackgroundModelFileChooser.CHOICE_TEXTURE:
			modelPath = backgroundModelFileChooserDialog( self._dialogTitle, self._parentWindow, self._currentChoice.path   if self._currentChoice is not None   else   None )
			if modelPath is not self._currentChoice:
				self._p_setCurrentChoice( modelPath )
				self.modelSignal.emit( self.uiEvent( event.bUserEvent ), modelPath )


	def _p_setCurrentChoice(self, choice):
		if choice is not self._currentChoice:
			self._currentChoice = choice
			self._popupMenu.setButtonContents( labelText=self._p_computeButtonLabelText( choice ) )



	def _p_computeButtonLabelText(self, modelPath):
		if modelPath is not None:
			return os.path.basename( modelPath )
		else:
			return _( '<span foreground="#000040">[No model]</span>' )




	def _getModelPath(self):
		return self._currentChooice

	def _setModelPath(self, choice):
		if choice is not self._currentChoice:
			self._p_setCurrentChoice( choice )
			self.modelSignal.emit( self.uiEvent( False ), choice )


	modelPath = property( _getModelPath, _setModelPath )




	
	
	
	
	

	
	
	
def importObjAsBackgroundModelWithProgressDialog(path):
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
