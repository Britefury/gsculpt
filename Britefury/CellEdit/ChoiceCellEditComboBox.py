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

from Britefury.UI.ComboBox import ComboBox

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class ChoiceCellEditComboBox (BasicWidgetSingleCellEdit):
	"""Choice cell editor - combo box
	@comboBox - the combo widget"""

	__valueclass__ = object

	def __init__(self, columnSpecs=[ str ], choices=[]):
		super( ChoiceCellEditComboBox, self ).__init__()
		self.comboBox = ComboBox( columnSpecs, choices )
		self.comboBox.choiceSignal.connect( self._p_onCombo )
		self._o_setWidgetSensitivity( False )


	def append(self, choice, *row):
		self.comboBox.append( choice, *row )

	def insert(self, index, choice, *row):
		self.comboBox.insert( index, choice, *row )

	def remove(self, choice):
		self.comboBox.remove( choice )

	def clear(self):
		self.comboBox.clear()



	def show(self):
		self.comboBox.getWidget().show()

	def hide(self):
		self.comboBox.getWidget().hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.comboBox.getWidget().set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		self.comboBox.choice = value


	def _p_onCombo(self, event, choice):
		if event.bUserEvent:
			if choice is not None  or  self._cell is not None:
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

	combo1 = ChoiceCellEditComboBox()
	combo1.append( 0, 'zero' )
	combo1.append( 1, 'one' )
	combo1.append( 2, 'two' )
	combo1.show()
	combo1.attachCell( cell1 )



	layout = FormLayout()
	layout.row()  <<  combo1.comboBox
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
