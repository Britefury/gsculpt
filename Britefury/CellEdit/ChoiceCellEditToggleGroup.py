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

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.ToggleGroup import ToggleGroup

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class ChoiceCellEditToggleGroup (BasicWidgetSingleCellEdit):
	"""Choice cell editor - radio group
	Subclass and add choices, setting attributes to the results of @addChoice()"""

	__valueclass__ = object

	def __init__(self):
		super( ChoiceCellEditToggleGroup, self ).__init__()
		self._toggleGroup = ToggleGroup()
		self._toggleGroup.choiceSignal.connect( self._p_onChoice )
		self._toggleButtons = []
		self._o_setWidgetSensitivity( False )


	def addChoice(self, choice, labelText=None, gsImageFilename=None, contents=None):
		button = ToggleButton( labelText=labelText, gsImageFilename=gsImageFilename, contents=contents )
		self._toggleGroup.addToggle( choice, button )
		self._toggleButtons.append( button )
		return button


	def getButtonForChoice(self, choice):
		return self._toggleGroup.getButtonForChoice( choice )

	def getButtonByIndex(self, index):
		return self._toggleGroup.getButtonByIndex( index )



	def show(self):
		for button in self._toggleButtons:
			button.getWidget().show()

	def hide(self):
		for button in self._toggleButtons:
			button.getWidget().hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		for button in self._toggleButtons:
			button.getWidget().set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		self._toggleGroup.choice = value


	def _p_onChoice(self, event, choice):
		if event.bUserEvent:
			assert self._cell is not None
			if self._cell.bLiteral  and  choice != self._cell.literalValue:
				self._o_blockCell( choice )
				self._cell.literalValue = choice
				self._o_unblockCell()









if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.CellEdit.CellEditLabelWithFn import CellEditLabelWithFn
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	owner = CellOwner()
	cell1 = IntCell( 0 )
	cell1.owner = owner
	owner.cellScope['cell1'] = cell1


	label1 = CellEditLabelWithFn( 'cell1:' )
	label1.show()
	label1.attachCell( cell1 )

	radio1 = ChoiceCellEditToggleGroup()
	radio1_0 = radio1.addChoice( 0, 'zero' )
	radio1_1 = radio1.addChoice( 1, 'one' )
	radio1_2 = radio1.addChoice( 2, 'two' )
	radio1.show()
	radio1.attachCell( cell1 )



	layout = FormLayout()
	layout.row()  <<  label1.label
	layout.row()  <<  radio1_0
	layout.row()  <<  radio1_1
	layout.row()  <<  radio1_2
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
