##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.UIController import UIController




class RadioButton (UIController):
	clickedSignal = ClassSignal()


	def __init__(self, group, labelText=None, gsImageFilename=None, contents=None):
		super( RadioButton, self ).__init__()

		if group is not None:
			assert isinstance( group, RadioButton )
			group = group._button

		if labelText is not None:
			self._button = gtk.RadioButton( group, labelText )
		else:
			self._button = gtk.RadioButton( group )
			if gsImageFilename is not None:
				self._button.add( GSImage.getGSImageWidget( gsImageFilename ) )
			elif contents is not None:
				self._button.add( contents )

		self._button.connect( 'clicked', self._p_onClicked )


	def getWidget(self):
		return self._button



	def _p_getGroup(self):
		return self._button.get_group()


	def _p_onClicked(self, widget):
		self.clickedSignal.emit( self.uiEvent(), self._button.get_active() )


	def _p_getState(self):
		return self._button.get_active()

	def _p_setState(self, state):
		self._button.set_active( state )


	state = property( _p_getState, _p_setState )




class RadioGroup (UIController):
	radioSignal = ClassSignal()


	def __init__(self):
		super( RadioGroup, self ).__init__()
		self._group = None
		self._bVisible = True
		self._bSensitive = True
		self._buttons = []
		self._choiceToButton = {}
		self._buttonToChoice = {}
		self._currentChoice = None
		self._bIgnoreRadioSignals = False


	def addRadioButton(self, choice, labelText=None, gsImageFilename=None, contents=None):
		button = RadioButton( self._group, labelText, gsImageFilename, contents )
		self._group = button

		if len( self._buttons ) == 0:
			self._currentChoice = choice
			button.state = True

		self._buttons.append( button )
		self._choiceToButton[choice] = button
		self._buttonToChoice[button] = choice

		button.clickedSignal.connect( self._p_onRadioClicked )

		if self._bVisible:
			button.getWidget().show()

		if not self._bSensitive:
			button.getWidget().set_sensitive( False )

		return button


	def show(self):
		if not self._bVisible:
			for button in self._buttons:
				button.getWidget().show()
			self._bVisible = True

	def hide(self):
		if self._bVisible:
			for button in self._buttons:
				button.getWidget().hide()
			self._bVisible = False

	def setSensitive(self, bSensitive):
		if bSensitive != self._bSensitive:
			for button in self._buttons:
				button.getWidget().set_sensitive( bSensitive )
			self._bSensitive = bSensitive


	def getButtonForChoice(self, choice):
		return self._choiceToButton[choice]

	def getButtonByIndex(self, index):
		return self._buttons[index]


	def _p_onRadioClicked(self, event, bState):
		if not self._bIgnoreRadioSignals:
			if bState:
				try:
					self._currentChoice = self._buttonToChoice[event.controller]
					self.radioSignal.emit( self.uiEvent(), self._currentChoice )
				except KeyError:
					pass


	def _p_getCurrentChoice(self):
		return self._currentChoice

	def _p_setCurrentChoice(self, choice):
		if choice != self._currentChoice:
			try:
				button = self._choiceToButton[choice]
			except KeyError:
				pass
			else:
				self._bIgnoreRadioSignals = True
				button.state = True
				self._bIgnoreRadioSignals = False

			self._currentChoice = choice

			self.applicationEventsBegin()
			self.radioSignal.emit( self.uiEvent(), self._currentChoice )
			self.applicationEventsEnd()


	currentChoice = property( _p_getCurrentChoice, _p_setCurrentChoice )

