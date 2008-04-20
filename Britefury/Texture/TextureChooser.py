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

import TextureTable




def textureChooserDialog(dialogTitle, parentWindow, currentChoice=None):
	startingDirectory = userConfig.texturePath


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


	tex = None

	chooser.show()
	if chooser.run() == gtk.RESPONSE_OK:
		filename = chooser.get_filename()

		# Set the texture directory
		userConfig.texturePath = os.path.dirname( filename )

		tex = TextureTable.globalTextureTable.getTexture( filename )

	chooser.destroy()

	return tex





class TextureChooser (UIController):
	CHOICE_NONE = 0
	CHOICE_TEXTURE = 1

	textureSignal = ClassSignal()


	def __init__(self, initialChoice, parentWindow, dialogTitle):
		self._currentChoice = initialChoice
		self._parentWindow = parentWindow
		self._dialogTitle = dialogTitle

		filename = _( '<span foreground="#000040">[No texture]</span>' )
		if self._currentChoice is not None:
			filename = os.path.basename( self._currentChoice.path )

		self._popupMenu = ButtonPopupMenu( labelText=filename )
		self._popupMenu.getWidget().show()
		self._popupMenu.menuSignal.connect( self._p_onMenu )
		self._popupMenu.addMenuItem( TextureChooser.CHOICE_NONE, _( 'No texture' ) )
		self._popupMenu.addMenuItem( TextureChooser.CHOICE_TEXTURE, _( 'Choose file...' ) )



	def getWidget(self):
		return self._popupMenu.getWidget()




	def _p_onMenu(self, event, choice):
		if choice == TextureChooser.CHOICE_NONE:
			if self._currentChoice is not None:
				self._p_setCurrentChoice( None )
				self.textureSignal.emit( self.uiEvent( event.bUserEvent ), None )
		elif choice == TextureChooser.CHOICE_TEXTURE:
			tex = textureChooserDialog( self._dialogTitle, self._parentWindow, self._currentChoice )
			if tex is not self._currentChoice:
				self._p_setCurrentChoice( tex )
				self.textureSignal.emit( self.uiEvent( event.bUserEvent ), tex )


	def _p_setCurrentChoice(self, choice):
		if choice is not self._currentChoice:
			self._currentChoice = choice
			self._popupMenu.setButtonContents( labelText=self._p_computeButtonLabelText( choice ) )



	def _p_computeButtonLabelText(self, texture):
		if texture is not None:
			return os.path.basename( texture.path )
		else:
			return _( '<span foreground="#000040">[No texture]</span>' )




	def _p_getTexture(self):
		return self._currentChooice

	def _p_setTexture(self, choice):
		if choice is not self._currentChoice:
			self._p_setCurrentChoice( choice )
			self.textureSignal.emit( self.uiEvent( False ), choice )


	texture = property( _p_getTexture, _p_setTexture )



