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

from Britefury.UI import RadioGroup

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class ChoiceCellEditRadioGroup (BasicWidgetSingleCellEdit):
	"""Choice cell editor - radio group
	Subclass and add choices, setting attributes to the results of @addChoice()"""

	__valueclass__ = object

	def __init__(self):
		super( ChoiceCellEditRadioGroup, self ).__init__()
		self._radioGroup = RadioGroup.RadioGroup()
		self._radioGroup.radioSignal.connect( self._p_onRadio )
		self._o_setWidgetSensitivity( False )


	def addChoice(self, choice, labelText=None, gsImageFilename=None, contents=None):
		return self._radioGroup.addRadioButton( choice, labelText, gsImageFilename, contents )


	def getButtonForChoice(self, choice):
		return self._radioGroup.getButtonForChoice( choice )

	def getButtonByIndex(self, index):
		return self._radioGroup.getButtonByIndex( index )



	def show(self):
		self._radioGroup.show()

	def hide(self):
		self._radioGroup.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self._radioGroup.setSensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		self._radioGroup.currentChoice = value


	def _p_onRadio(self, event, choice):
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

	radio1 = ChoiceCellEditRadioGroup()
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
