##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.NumericCellEditEntry import NumericCellEditEntry


class IntCellEditEntry (NumericCellEditEntry):
	"""Integer cell editor - entry
	@entry - the entry widget"""

	__valueclass__ = int

	def __init__(self, minValue, maxValue, step, page):
		super( IntCellEditEntry, self ).__init__( minValue, maxValue, step, page, 0 )


	@staticmethod
	def _o_convertValue(value):
		return int( value + 0.5 )




if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.CellEdit.CellEditLabelWithFn import CellEditLabelWithFn
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	owner = CellOwner()
	cell1 = IntCell( 0 )
	cell2 = IntCell( 1 )
	cell3 = IntCell( 2 )
	cell1.owner = owner
	cell2.owner = owner
	cell3.owner = owner
	owner.cellScope['cell1'] = cell1
	owner.cellScope['cell2'] = cell2
	owner.cellScope['cell3'] = cell3


	label1 = CellEditLabelWithFn( 'cell1:' )
	label1.show()
	label1.attachCell( cell1 )

	entry1 = IntCellEditEntry( -1048576, 1048576, 1, 10 )
	entry1.show()
	entry1.attachCell( cell1 )


	label2 = CellEditLabelWithFn( 'cell2:' )
	label2.show()
	label2.attachCell( cell2 )

	entry2 = IntCellEditEntry( -1048576, 1048576, 1, 10 )
	entry2.show()
	entry2.attachCell( cell2 )


	label3 = CellEditLabelWithFn( 'cell3:' )
	label3.show()
	label3.attachCell( cell3 )

	entry3 = IntCellEditEntry( -1048576, 1048576, 1, 10 )
	entry3.show()
	entry3.attachCell( cell3 )


	layout = FormLayout()
	layout.row()  <<  label1.label  <<  entry1.entry
	layout.row()  <<  label2.label  <<  entry2.entry
	layout.row()  <<  label3.label  <<  entry3.entry
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
