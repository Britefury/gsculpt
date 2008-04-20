##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Cell.LiteralCell import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import *

from Britefury.UI.UIAction import UIActionInstanceSignal

from Britefury.Manipulator.Manipulator import *

from Britefury.Event.QueuedEvent import queueEvent



class ManipulatorSwitch (Manipulator):
	def __init__(self, manipulators):
		super( ManipulatorSwitch, self ).__init__()
		self._manipulators = manipulators
		self.switchIndexCell = LiteralIntCell( 0 )
		self.switchIndexCell.changedSignal.connect( self._p_onSwitchIndexChanged )
		self._currentManipulator = self._manipulators[self.switchIndexCell.getValue()]


	def manipAttachToView(self, view):
		super( ManipulatorSwitch, self ).manipAttachToView( view )
		if self._currentManipulator is not None:
			self._currentManipulator.manipAttachToView( view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._currentManipulator is not None:
			self._currentManipulator.manipDetachFromView()
		super( ManipulatorSwitch, self ).manipDetachFromView()


	def index(self, manipulator):
		return self._manipulators.index( manipulator )


	def _o_setUISensitivity(self, bSensitive):
		pass


	def _p_onSwitchIndexChanged(self):
		if self._currentManipulator is not None:
			if self._view is not None:
				self._currentManipulator.manipDetachFromView()
			self._currentManipulator = None

		queueEvent( self._p_refreshManipulatorChoice )


	def _p_refreshManipulatorChoice(self):
		value = self.switchIndexCell.getValue()
		self._currentManipulator = self._manipulators[value]
		if self._view is not None:
			self._currentManipulator.manipAttachToView( self._view )




class ManipulatorSwitchRadioGroup (object):
	"""Manipulator switch radio group"""
	def __init__(self):
		super( ManipulatorSwitchRadioGroup, self ).__init__()
		self._manipSwitch = None
		self._cellEdit = ChoiceCellEditRadioGroup()
		self._actions = []
		self._actionToIndex = {}


	def attachManipulatorSwitch(self, switch):
		assert self._manipSwitch is None, 'manipulator switch already attached'
		self._manipSwitch = switch
		self._cellEdit.attachCell( self._manipSwitch.switchIndexCell )

	def detachManipulatorSwitch(self):
		assert self._manipSwitch is not None, 'no manipulator switch attached'
		self._cellEdit.detachCell()
		self._manipSwitch = None


	def addChoice(self, manipulator, action):
		actionInstance = action.instance( UIActionInstanceSignal )
		actionInstance.actionSignal.connect( self._p_onAction )
		index = self._manipSwitch.index( manipulator )
		self._actionToIndex[actionInstance] = index
		self._actions.append( actionInstance )
		return self._cellEdit.addChoice( index, contents=action.createButtonContentsWidget() )


	def show(self):
		self._cellEdit.show()

	def hide(self):
		self._cellEdit.hide()


	def setSensitive(self, bSensitive):
		self._cellEdit.setSensitive( bSensitive )


	def installAccelerators(self, accels):
		for action in self._actions:
			action.installAccel( accels )

	def uninstallAccelerators(self, accels):
		for action in self._actions:
			action.uninstallAccel( accels )


	def getButtonForManipulator(self, choice):
		return self._cellEdit.getButtonForChoice( choice )

	def getButtonByIndex(self, index):
		return self._cellEdit.getButtonByIndex( index )


	def _p_onAction(self, action):
		index = self._actionToIndex[action]
		self._manipSwitch.switchIndexCell.setLiteralValue( index )

