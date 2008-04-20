##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.NumericCellEditSlider import NumericCellEditSlider


class IntCellEditSlider (NumericCellEditSlider):
	"""Integer cell editor - slider
	@slider - the slider"""

	__valueclass__ = int

	def __init__(self, minValue, maxValue, step, page):
		super( IntCellEditSlider, self ).__init__( minValue, maxValue, step, page )


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

	slider1 = IntCellEditSlider( -100, 100, 1, 10 )
	slider1.show()
	slider1.attachCell( cell1 )


	label2 = CellEditLabelWithFn( 'cell2:' )
	label2.show()
	label2.attachCell( cell2 )

	slider2 = IntCellEditSlider( -100, 100, 1, 10 )
	slider2.show()
	slider2.attachCell( cell2 )


	label3 = CellEditLabelWithFn( 'cell3:' )
	label3.show()
	label3.attachCell( cell3 )

	slider3 = IntCellEditSlider( -100, 100, 1, 10 )
	slider3.show()
	slider3.attachCell( cell3 )


	layout = FormLayout()
	layout.row()  <<  label1.label  <<  slider1.slider
	layout.row()  <<  label2.label  <<  slider2.slider
	layout.row()  <<  label3.label  <<  slider3.slider
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
