##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.UIController import *




class ToggleGroup (UIController):
	choiceSignal = ClassSignal()


	def __init__(self, bAllowNone = True):
		self._currentChoice = None
		self._bAllowNone = bAllowNone
		self._bBlockIncomingSignals = False
		self._toggles = []
		self._toggleToChoice = {}
		self._choiceToToggle = {}


	def addToggle(self, choice, toggle):
		self._toggles.append( toggle )
		self._toggleToChoice[toggle] = choice
		self._choiceToToggle[choice] = toggle

		toggle._group = self

		if not self._bAllowNone  and  self._currentChoice is None:
			self.choice = choice


	def allowNone(self):
		self._bAllowNone = True

	def disallowNone(self):
		if self._currentChoice is None:
			if len( self._choiceToToggle ) > 0:
				choice, toggle = self._choiceToToggle.items()[0]
				self.choice = choice
		self._bAllowNone = True



	def _p_onToggleClicked(self, event, bState):
		if not self._bBlockIncomingSignals:
			if not event.bUserEvent:
				self.applicationEventsBegin()

			if bState:
				oldChoice = self._currentChoice

				# We are going to be changing toggle states; no need to be notified
				self._bBlockIncomingSignals = True

				# Clear other toggles
				for toggle in self._toggles:
					if toggle is not event.controller:
						toggle.state = False

				# Get the choice
				self._currentChoice = self._toggleToChoice[event.controller]

				# unblock
				self._bBlockIncomingSignals = False


				if oldChoice != self._currentChoice:
					self.choiceSignal.emit( self.uiEvent(), self._currentChoice )
			else:
				if self._bAllowNone:
					# None allowed; set the current choice to None and emit the signal
					self._currentChoice = None
					self.choiceSignal.emit( self.uiEvent(), self._currentChoice )
				else:
					# None not allowed; set the toggle back
					self._bBlockIncomingSignals = True
					event.controller.state = True
					self._bBlockIncomingSignals = False

			if not event.bUserEvent:
				self.applicationEventsEnd()



	def _p_getChoice(self):
		return self._currentChoice

	def _p_setChoice(self, choice):
		if choice != self._currentChoice:
			self._bBlockIncomingSignals = True
			for ch, toggle in self._choiceToToggle.items():
				toggle.state = ch == choice
			self._currentChoice = choice
			self._bBlockIncomingSignals = False

		self.applicationEventsBegin()
		self.choiceSignal.emit( self.uiEvent(), choice )
		self.applicationEventsEnd()


	choice = property( _p_getChoice, _p_setChoice )
