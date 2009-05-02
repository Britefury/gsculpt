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




def backgroundModelFileChooserDialog(dialogTitle, parentWindow, currentChoice=None):
	startingDirectory = userConfig.backgroundModelPath


	# If a current choice has been provided, and its path is absolute, use it as a starting point
	bCurrentChoiceValid = False
	if currentChoice is not None:
		if os.path.isabs( currentChoice.path ):
			startingDirectory = os.path.dirname( currentChoice.path )
			bCurrentChoiceValid = True


	# Create the file chooser dialog
	chooser = gtk.FileChooserDialog( dialogTitle, parentWindow, gtk.FILE_CHOOSER_ACTION_OPEN, ( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )

	if bCurrentChoiceValid:
		# Absolute filename: start here
		chooser.set_filename( currentChoice.path )
	else:
		# Start at the starting directory
		chooser.set_current_folder( startingDirectory )


	modelPath = None

	chooser.show()
	if chooser.run() == gtk.RESPONSE_OK:
		filename = chooser.get_filename()

		# Set the texture directory
		userConfig.texturePath = os.path.dirname( filename )

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
			modelPath = backgroundModelFileChooserDialog( self._dialogTitle, self._parentWindow, self._currentChoice )
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



